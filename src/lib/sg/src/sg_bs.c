/* vim:set et sw=4 sts=4: */
#include <string.h>
#include <stdio.h>

#include "ksupport.h"
#include <kflg.h>
#include <kdbg.h>

#include "sg_bs.h"
#include "sg_ses.h"
#include "sg_mgr2.h"
#include <ktsk.h>
#include <kmem.h>
#include <kstr.h>

#include "ipaddr.h"

#include "sg_root.h"
#include "sg_ses.h"
#include "sg_zbd.h"

#include "xmldoc.h"

#include "x2s_macro.h"

#include "ivisa_sg_comm.h"

static kint sg_scan_dir_cbk(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3);
static kint nnn_sg_scan_load_from_heaven(sg_scan *scan, sg_mgr *mgr, kuint protocol, kchar tmode);

static kint sg_scan_setup_env(sg_scan *scan, kuint tmode);

/**
 * \name ESGAccessDescriptor_macro
 * \{
 */
#define MK_INT08(p) (*((kuchar*)p + 0))
#define MK_INT16(p) ((*((kuchar*)p + 0) << 8) + ((*((kuchar*)p + 1)) << 0))
#define MK_INT24(p) ((*((kuchar*)p + 0) << 16) + ((*((kuchar*)p + 1)) << 8) + ((*((kuchar*)p + 2)) << 0))
#define MK_INT32(p) ((*((kuchar*)p + 0) << 24) + ((*((kuchar*)p + 1)) << 16) + ((*((kuchar*)p + 2)) << 8) + ((*((kuchar*)p + 3)) << 0))

/** Get number of Entries */
#define BS_BIN_GET_ENT_CNT(rptr) MK_INT16(rptr)

/** Get Entry version */
#define BS_BIN_GET_EE_VER(sa) MK_INT08(sa)

/** Get Entry length */
#define BS_BIN_GET_EE_LEN(sa) MK_INT08(sa + 1)

/** Get Entry MultipleStreamTransport */
#define BS_BIN_GET_EE_MST(sa) (MK_INT08(sa + 2) & 0x01)

/** Get Entry IPV6 */
#define BS_BIN_GET_EE_IPV6(sa) (MK_INT08(sa + 2) & 0x02)

/** Get Entry ProviderID */
#define BS_BIN_GET_EE_ID(sa) MK_INT16(sa + 3)

/** Get Entry source ip address */
#define BS_BIN_GET_SRC_IPA(sa, ipv6) (sa + 5)

/** Get Entry dest ip address */
#define BS_BIN_GET_DST_IPA(sa, ipv6) (sa + 5 + (ipv6 ? 16 : 4))

/** Get Entry Port */
#define BS_BIN_GET_PORT(sa, ipv6) MK_INT16(sa + 5 + (ipv6 ? 32 : 8))

/** Get Entry TSI */
#define BS_BIN_GET_TSI(sa, ipv6) MK_INT16(sa + 5 + (ipv6 ? 32 : 8) + 2)

/** \} */

/** sg_scan_del */
static kint om_quit(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

/** for bootstrap stage */
static kint om_scan_bootstrap_start(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_scan_bootstrap_done(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kint om_scan_change_cnt(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kvoid sg_scan_fragments_free(sg_scan *scan);
static kint sg_scan_bsstage_done_cbk(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);

static kint sg_scan_call_callback(sg_scan *scan);
static kint sg_scan_complete(sg_scan *scan);

/**
 * \brief construct a boot strap.
 * XXX ??? only for broadcast
 *
 * \param scan user applied memory for sg_scan structure
 * \param mgr SG manager
 *
 * \return 0 for success, other for error
 */
kint sg_scan_new(sg_scan **scan, sg_mgr *mgr)
{
    kuint flg = BSF_USED, i;
    if (!(*scan)) {
        (*scan) = (sg_scan*)kmem_alloc(sizeof(sg_scan));
        flg |= BSF_DYN_MEM;
    }
    if ((*scan)) {
        memset((*scan), 0, sizeof(sg_scan));
        for (i = 0; i < XBS_TOP; i++) {
            init_dlist_head(&(*scan)->fhdrs[i]);
        }
        (*scan)->flg = flg;
        (*scan)->mgr = mgr;
        (*scan)->scan.bdir_socket = -1;

        (*scan)->worker_thread = ktsk_new("sg scan", knil, 0, 0, (kvoid*)(*scan), knil, knil, knil);
        if (!(*scan)->worker_thread) {
            kerror(("Can not create task for boot strap\n"));
            if (kflg_chk((*scan)->flg, BSF_DYN_MEM)) {
                kmem_free(scan);
            }
            return -1;
        }

        /* system msg slot */
        kmsg_slot_set((*scan)->worker_thread, KMSG_QUIT, om_quit);

        /* sg scan msg slot */
        kmsg_slot_set((*scan)->worker_thread, SCAN_BOOTSTRAP_START, om_scan_bootstrap_start);
        kmsg_slot_set((*scan)->worker_thread, SCAN_BOOTSTRAP_DONE, om_scan_bootstrap_done);
        kmsg_slot_set((*scan)->worker_thread, SCAN_CHANGE_CNT, om_scan_change_cnt);

        return 0;
    }
    kerror(("sg_scan_new: no mem\n"));
    return -1;
}

/* kill all the scan before quit */
kint sg_scan_del(sg_scan *scan)
{
    kmsg_send(scan->worker_thread, KMSG_QUIT, (kvoid*)scan, 0, 0, 0);
    return 0;
}

/**
 * \brief Cancel pending request, will interrupt current operation.
 *
 * This ONLY cancel current transfer.
 */
static kint scan_cancel(sg_scan *scan)
{
    if (scan->scan.tmode == SG_TMODE_FLUTE) {
        sg_ses_del(&scan->ses);
    } else if (scan->scan.tmode == SG_TMODE_MOT) {
        sg_ses_del(&scan->ses);
    } else {
        hget_free_socket(scan->scan.bdir_socket);
        scan->scan.bdir_socket = -1;
    }

    return 0;
}

/**
 * \brief In the specifical time, the boot strap can not be gotten.
 *
 * User set timeout for channel scan, this timeout will be split, the first quarter
 * use for boot strap. When boot strap time out, cancel the transfer, and goto next.
 */
kint tmr_cbk_bootstrap_stage_timeout(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    kbean tsk = (sg_scan*)a_ar0;
    sg_scan *scan = (sg_scan*)a_ar1;

    /* XXX Some request will block the task, cancel it */
    scan_cancel(scan);
    kmsg_post(tsk, SCAN_BOOTSTRAP_DONE, a_ar1, knil, knil, (kvoid*)1);
    return 0;
}

/**
 * \brief start session to receive boot strap information.
 *
 * If return failed, SCAN_BOOTSTRAP_DONE will be called.
 *
 * \param scan scan;
 * \param tmode transfer mode, can be MOT or FLUTE
 * \param timeout timeout only for receiving boot strap part
 *
 * \return 0 for success, other for error.
 */
static kint sg_scan_bsstage_start_for_broadcast(sg_scan *scan, kuint tmode, kuint timeout)
{
    sg_ses *ses = &scan->ses;
    sg_ses_new(&ses, tmode, knil);
    kflg_set(ses->flg, SES_AUTO_DEL | SES_AUTO_STOP);

    /* fill parameter to get the boot strap, it will set sei of ses */
    sg_scan_setup_env(scan, tmode);

    if (sg_ses_start_session(ses, ktrue)) {
        sg_ses_del(ses);
        kerror(("sg_scan_start: can not start session, goto try default\n"));
        return -1;
    }

    if (sg_ses_get_dir(ses, 0, (kvoid*)sg_scan_dir_cbk, scan, ses, knil, knil)) {
        sg_ses_del(ses);
        kerror(("sg_scan_start: get directory failed, goto try default\n"));
        return -1;
    } else {
        /* Cancel the boot strap receive when time out */
        scan->tmr_bootstrap_timeout = ktmr_set(scan->worker_thread, timeout, kfalse,
                tmr_cbk_bootstrap_stage_timeout, (kvoid*)scan->worker_thread, (kvoid*)scan, 0, 0);

        /* When all the data done, tell main task to process the data */
        sg_ses_set_done_cbk(ses, (SES_DONE_CBK)sg_scan_bsstage_done_cbk,
                (kvoid*)scan->worker_thread, (kvoid*)scan, knil, 0);

        return 0;
    }
}

/**
 * \brief start session to receive boot strap information
 *
 * Because iv hack some in b-dir sg, it's do not follow Dir and Obj order, so,
 * directly call hget but sg_ses to get data.
 *
 * When data received, save it in scan->fhdrs[BS_XML];
 */
static kint sg_scan_bsstage_start_for_bdir(sg_scan *scan, kuint tmode, kuint timeout)
{
    sg_zbd *zbd;
    kchar realurl[2048];

    kchar *datbuf = knil, *hdrbuf = knil, *ptr;
    kint datlen = -1, hdrlen = 0, ret = PGEC_ERROR;

    ptr = scan->mgr->env->bdsg_url;
    if (!ptr[0]) {
        klog(("URL for B-DIR SG not set, use default\n"));
        ptr = "http://192.168.1.102/ESG/Portal.ashx";
    }
    STR_SET(scan->scan.bdir_url, ptr);

    sprintf(realurl, "%s", scan->scan.bdir_url);

    klog(("sg_scan_bsstage_start_for_bdir: url: %s\n", realurl));

#ifdef USE_TWO_WAY_SG
    /* Cancel the boot strap receive when time out */
    scan->tmr_bootstrap_timeout = ktmr_set(scan->worker_thread, timeout, kfalse,
            tmr_cbk_bootstrap_stage_timeout, (kvoid*)scan->worker_thread, (kvoid*)scan, 0, 0);

    ret = hget(realurl, knil, kfalse, knil, &datbuf, &datlen, &hdrbuf, &hdrlen, &scan->scan.bdir_socket);
    if ((PGEC_SUCCESS == ret) && datbuf && datlen > 0) {

        ktmr_kill(scan->tmr_bootstrap_timeout);

        /* queue to xml fhdrs */
        zbd = knil;
        sg_zbd_new(&zbd);
        zbd->dat.buf = datbuf;
        zbd->dat.len = datlen;
        insert_dlist_tail_entry(&scan->fhdrs[BS_XML], &zbd->cls_le);

        /* all success, send done message */
        klog(("sg_scan_bsstage_done_cbk\n"));
        kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_DONE, (kvoid*)scan, knil, knil, knil);

        /* return success */
        return 0;
    }
    ktmr_kill(scan->tmr_bootstrap_timeout);
#else
    klog(("\n\nsg_scan_bsstage_start_for_bdir: USE_TWO_WAY_SG not defined\n\n"));
#endif
    kerror(("sg_scan_bsstage_start_for_bdir failed\n"));
    return -1;
}

static kint om_scan_change_cnt(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_scan *scan = (sg_scan*)a_ar0;
    kint step = (kint)a_ar1;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    scan->now_scan += step;

    /*
     * XXX must called when scan_called, to avoid some scan quick return
     * and make now_scan back to zero
     */
    if (scan->all_scan_issued && (0 == scan->now_scan)) {
        sg_scan_complete(scan);
    }

    return 0;
}

/**
 * \brief start transfer for a scan group
 * - Read the <b>next</b> scan group information
 * - if null, means all the scan done, call evtCallback(IV_SG_CALLBACK_SCAN, IVEC_SUCCESS, ...);
 * - if !null, peak the next and start session to receive the data for it
 */
static kint om_scan_bootstrap_start(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_scan *scan = (sg_scan*)a_ar0;

    kint i, j;

    kchar curtmode;
    kuint timeout;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    curtmode = scan->scan.tmode;
    timeout = scan->scan.timeout;

    klog(("om_scan_bootstrap_start: mode:%d, timeout:%d\n", curtmode, timeout));

    /* First of all, kill the previous boot strap stage timeout timer */
    ktmr_kill(scan->tmr_bootstrap_timeout);
    scan->tmr_bootstrap_timeout = knil;

    if (curtmode == SG_TMODE_FLUTE) {
        /* only 1 / 4 of total timeout used for boot strap stage */
        if (sg_scan_bsstage_start_for_broadcast(scan, SG_TMODE_FLUTE, timeout / 4)) {
            /*
             * fail, try next!
             * No need to construct a default ESGAccessDescriptor, because start failed, means
             * transfer layer error.
             */
            kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_DONE, (kvoid*)scan, knil, knil, knil);
        }
    } else if (curtmode == SG_TMODE_MOT) {
        /* only 1 / 4 of total timeout used for boot strap stage */
        if (sg_scan_bsstage_start_for_broadcast(scan, SG_TMODE_MOT, timeout / 4)) {
            /*
             * fail, try next!
             * No need to construct a default ESGAccessDescriptor, because start failed, means
             * transfer layer error.
             */
            kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_DONE, (kvoid*)scan, knil, knil, knil);
        }
    } else if (curtmode == SG_TMODE_BDIR) {
        /* only 1 / 4 of total timeout used for boot strap stage */
        if (sg_scan_bsstage_start_for_bdir(scan, SG_TMODE_BDIR, timeout / 4)) {
            kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_DONE, (kvoid*)scan, knil, knil, knil);
        }
    } else {
        /* bad tmode, directly try next */
        kassert(!"non-supported trans mode!");
        kerror(("non-supported trans mode %x or %c", curtmode, curtmode));
        sg_scan_complete(scan);
    }
    return 0;
}

/**
 * \brief Get the BootStrap information.
 * User can be set multi transfer mode at same time, so, must new multi session to get the data
 *
 * Push the operation in thread
 * 1, om_scan_bootstrap_start - om_bs_bsstage_stage_done in session new
 * 2, om_scan_bootstrap_done - parse current, if has next, start next, goto 1, if hasn't next end
 */
kint sg_scan_start(sg_scan *scan)
{
    /* call om_bs_bsstage_stage_start */
    kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_START, scan, 0, 0, 0);
    return 0;
}

/**
 * \brief When this is called, all the data should be successfully received
 */
static kint sg_scan_bsstage_done_cbk(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    klog(("sg_scan_bsstage_done_cbk\n"));
    kmsg_post(a_ar0, SCAN_BOOTSTRAP_DONE, a_ar1, a_ar2, 0, 0);
    return 0;
}

/**
 * \brief destroy the sg_scan, indirectly called by sg_scan_del
 */
static kint om_quit(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_scan *scan = (sg_scan*)a_ar0;
    /* check if all the file received, if yes, parse them */

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if (!kflg_chk(scan->flg, BSF_USED)) {
        kerror(("sg_scan_del: unused, quit\n"));
        return -1;
    }

    sg_scan_call_callback(scan);

    /* detach it */
    scan->mgr->scan = knil;

    /* XXX for each session in scan->seshdr, free it */

    /* XXX drop the data */
    kmem_free_z(scan->scan.bdir_url);

    if (kflg_chk(scan->flg, BSF_DYN_MEM)) {
        kmem_free(scan);
    }
    return 0;
}
static kint tmr_cbk_data_timeout(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    sg_root *rt = (sg_root*)a_ar0;
    kmsg_post(rt->worker_thread, KRTM_ABORT_SCAN, rt, 0, 0, 0);
    return 0;
}

static kint om_scan_bootstrap_done_for_flute(kbean a_tsk, kuint a_msg,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_dlist_entry *tmp_entry, *tmp_hdr, *xml_entry, *xml_hdr, *bin_entry, *bin_hdr;

    sg_scan *scan = (sg_scan*)a_ar0;

    KXmlDoc *xml_doc, *tmp_doc, *doc;
    KXmlNode *tmp_node, *alc_node, *sp_node, *pvdr_node, *ci_node, *node;
    KXmlAttr *tmp_attr, *attr;

    sg_zbd *tmp_zbd, *xml_zbd, *bin_zbd;
    K_pvdr *pvdr;
    K_pvdr_acc *acc;

    kuint found, pid, scan_cnt = 0, cnt, i, ProviderID, noEnt, j, entVer, entLen, ipv6, port, tsi, frequency;
    kchar *sa, *srcipaddr, *dstipaddr, *tmp_buf, val[128];

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if ((kint)a_ar3 == 1) {
        /* this is called by time out, should load a default to try receive */
        klog(("Called by timeout!\n"));
    }

    frequency = scan->scan.frequency;

    /*
     * I, Process boot strap broadcast part
     * for all the provider in XML, try to find the access info in Bin file
     */

#if 0
    /*
     * <ESGProviderDiscovery xmlns="urn:dvb:ipdc:esgbs:2005">
     *     <ServiceProvider format="urn:nokia:oai:sg:2005">
     *         <ProviderURI>www.rantanplan.com</ProviderURI>
     *         <ProviderName>Asterix</ProviderName>
     *         <ProviderLogo>auv_TitleMediaType</ProviderLogo>
     *         <ProviderInformationURL>auv_anyURI</ProviderInformationURL>
     *         <PrivateAuxiliaryData>auv_anyType</PrivateAuxiliaryData>
     *         <ProviderID>3344</ProviderID>
     *     </ServiceProvider>
     * </ESGProviderDiscovery>
     */
    /*
     * ESG_Access_Descriptor {
     *     n_o_ESGEntries                            ## 16
     *     for (i = 0; i < n_o_ESGEntries; i++) {
     *         ESGEntry[i]
     *     }
     * }
     * ESGEntry {
     *     ESGEntryVersion                           ## 8
     *     ESGEntryLength                            ## 8+
     *     MultipleStreamTransport                   ## 1
     *     IPVersion6                                ## 1
     *     Reserved                                  ## 6
     *     ProviderID                                ## 16
     *     if (IPVersion6) {
     *         SourceIPAddress                       ## 128
     *         DestinationIPAddress                  ## 128
     *     } else {
     *         SourceIPAddress                       ## 32
     *         DestinationIPAddress                  ## 32
     *     }
     *     Port                                      ## 16
     *     TSI                                       ## 16
     * }
     */
#endif

    found = 0;

try_with_default_bs_param:

    /* B, try to load broadcast boot strap */
    xml_hdr = &scan->fhdrs[BS_XML];
    xml_entry = xml_hdr->next;
    while (xml_entry != xml_hdr) {
        xml_zbd = FIELD_TO_STRUCTURE(xml_entry, sg_zbd, cls_le);
        xml_entry = xml_entry->next;

        xml_doc = xmldoc_new(knil);
        xmldoc_parse(xml_doc, xml_zbd->dat.buf, xml_zbd->dat.len);
        sg_zbd_del(xml_zbd);

        /*
         * XXX We, have to, make such hypothesis that for FLUTE and MOT
         * XXX ONLY Access part of boot strap if different.
         */

        /* first goto the ESGProviderDiscovery element */
        xmldoc_gotoNode(xml_doc, "ESGProviderDiscovery", 0);

        /* Loop for each ServiceProvider */
        cnt = xmldoc_getNodeCnt(xml_doc, "ServiceProvider");
        for (i = 0; i < cnt; i++) {
            sp_node = xmldoc_gotoNode(xml_doc, "ServiceProvider", i);

            /* search the bin file to find an access entry with same provider id */
            tmp_node = xmldoc_getNode(xml_doc, "ProviderID", 0);
            if (!tmp_node) {
                kerror(("no ProviderID, bad xml\n"));
                continue;
            }

            ProviderID = strtoul(tmp_node->text, 0, 10);

            /*
             * XXX because FLUTE and MOT has different Access defination,
             * so the follow statement should be seperately implemented for
             * MOT and FLUTE. But now only FLUTE part here.
             */

            /* match ProviderID in Bin file */
            bin_hdr = &scan->fhdrs[BS_BIN];
            bin_entry = bin_hdr->next;
            while (bin_entry != bin_hdr) {
                bin_zbd = FIELD_TO_STRUCTURE(bin_entry, sg_zbd, cls_le);
                bin_entry = bin_entry->next;

                /* walk the Access part to find item for current ProviderID */
                noEnt = BS_BIN_GET_ENT_CNT(bin_zbd->dat.buf);
                sa = bin_zbd->dat.buf + 2;
                for (j = 0; j < noEnt; j++) {
                    entLen = BS_BIN_GET_EE_LEN(sa);
                    if (ProviderID == BS_BIN_GET_EE_ID(sa)) {
                        entVer = BS_BIN_GET_EE_VER(sa);
                        ipv6 = BS_BIN_GET_EE_IPV6(sa);
                        srcipaddr = BS_BIN_GET_SRC_IPA(sa, ipv6);
                        dstipaddr = BS_BIN_GET_DST_IPA(sa, ipv6);
                        port = BS_BIN_GET_PORT(sa, ipv6);
                        tsi = BS_BIN_GET_TSI(sa, ipv6);
                        break;
                    }
                    sa += entLen + 2;
                }

                if ((!noEnt) || (j == noEnt)) {
                    kerror(("can not find providerID in bin file, continue to next loop\n"));
                    sg_zbd_del(bin_zbd);
                    continue;
                }

                /* FOUND!!!, insert this provider into sg_scan::provhdr as K_pvdr */
                found++;
                /*
                 * I, find or create the Provider K_pvdr and save it
                 */

                pvdr = sg_mgr_find_provider(scan->mgr, ProviderID);
                if (!pvdr) {
                    pvdr = (K_pvdr*)kmem_alloz(sizeof(K_pvdr));
                    kassert(pvdr);
                    if (!pvdr) {
                        kerror(("no memory for K_pvdr\n"));
                        sg_zbd_del(bin_zbd);
                        continue;
                    }

                    pvdr->protocol = scan->scan.protocol;

                    init_dlist_head(&pvdr->le);
                    pvdr->id = ProviderID;
                    init_dlist_head(&pvdr->acchdr);

                    /* fill the data of K_pvdr */
                    pvdr->doc = xmldoc_new(knil);
                    pvdr_node = xmlnode_new(knil, "pvdr", knil);
                    xmldoc_addNode(pvdr->doc, pvdr_node, knil);

                    sprintf(val, "%d", ProviderID);
                    xmlnode_set_attr_value(pvdr_node, "id", val);

                    sprintf(val, "%d", 0);
                    xmlnode_set_attr_value(pvdr_node, "flg", val);

                    sprintf(val, "%d", pvdr->protocol);
                    xmlnode_set_attr_value(pvdr_node, "protocol", val);

                    if (tmp_node = xmldoc_getNode(xml_doc, "ProviderName", 0)) {
                        xmlnode_set_attr_value(pvdr_node, "name", tmp_node->text ? tmp_node->text : "");
                    }

                    if (tmp_node = xmldoc_getNode(xml_doc, "ProviderURI", 0)) {
                        xmlnode_set_attr_value(pvdr_node, "uri", tmp_node->text ? tmp_node->text : "");
                    }

                    if (tmp_node = xmldoc_getNode(xml_doc, "ProviderInformationURL", 0)) {
                        xmlnode_set_attr_value(pvdr_node, "infuri", tmp_node->text ? tmp_node->text : "");
                    }

                    if (tmp_node = xmldoc_getNode(xml_doc, "PrivateAuxiliaryData", 0)) {
                        xmlnode_set_attr_value(pvdr_node, "auxdat", tmp_node->text ? tmp_node->text : "");
                    }

                    if (tmp_node = xmldoc_getNode(xml_doc, "ProviderLogo", 0)) {
                        xmlnode_set_attr_value(pvdr_node, "logo", tmp_node->text ? tmp_node->text : "");
                    }

                    /* sg_mgr_single_provider_save do not queue, queue first */
                    insert_dlist_tail_entry(&scan->mgr->provhdr, &pvdr->le);

                    /* __pvdr.xml done, save it */
                    /* sg_mgr_single_provider_save(scan->mgr, pvdr); */
                } else {
                    /* FIXME : shall I overwrite the old paramters with new's */
                    kassert(pvdr->protocol == scan->scan.protocol);
                }

                /*
                 * II, merge or create K_pvdr_acc for found K_pvdr
                 */

                /*
                 * II.1 find K_pvdr_acc if not found, add it
                 */
                acc = sg_mgr_pvdr_acc_find(scan->mgr, pvdr, frequency);
                if (acc) {
                    int idx, idx2;
                    if (acc->flute) {
                        /* FIXME : should repair the flute? */
                        kerror(("K_pvdr_acc for Provider %d at frequency %d with flute already exist\n", pvdr->id, frequency));
                        sg_zbd_del(bin_zbd);
                        // continue;
                    }
                    /* move the found to last */
                    for (idx = 0; idx < 8; idx++) {
                        if (SG_TMODE_FLUTE == acc->tmode[idx]) {
                            for (; idx < 8; idx++) {
                                acc->tmode[idx] = acc->tmode[idx + 1];
                                if (SG_TMODE_LAST == acc->tmode[idx]) {
                                    acc->tmode[idx] = SG_TMODE_FLUTE;
                                    acc->tmode[idx + 1] = SG_TMODE_LAST;
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    acc = (K_pvdr_acc*)kmem_alloz(sizeof(K_pvdr_acc));
                    if (!acc) {
                        kerror(("no memory\n"));
                        sg_zbd_del(bin_zbd);
                        continue;
                    }

                    init_dlist_head(&acc->le);
                    acc->pvdr = pvdr;
                    acc->frequency = frequency;
                    acc->protocol = scan->scan.protocol;
                    memset(acc->tmode, SG_TMODE_LAST, sizeof(kchar) * 8);

                    acc->tmode[0] = SG_TMODE_FLUTE;
                    acc->tindex = 0;
                }

                /*
                 * II.2 create flute_acc
                 */
                if (!acc->flute) {
                    acc->flute = (K_flute_acc*)kmem_alloz(sizeof(K_flute_acc));
                }
                acc->flute->hdr.tmode = SG_TMODE_FLUTE;
                acc->flute->hdr.pvdr_acc = acc;

                acc->flute->alc.frequency = frequency;
                acc->flute->alc.iAlcChannelNum = 1;
                acc->flute->alc.iSSM_flag = 1;

                acc->flute->alc.source_address.iIPv4_flag = !ipv6;
                if (ipv6) {
                    for (i = 0; i < 16; i++) {
                        acc->flute->alc.source_address.ubIP[i] = srcipaddr[i];
                    }
                } else {
                    for (i = 0; i < 4; i++) {
                        acc->flute->alc.source_address.ubIP[i] = srcipaddr[i];
                    }
                }

                acc->flute->alc.pAlcChannelHead[0].u16TargetPort = port;
                acc->flute->alc.pAlcChannelHead[0].target_address.iIPv4_flag = !ipv6;
                if (ipv6) {
                    for (i = 0; i < 16; i++) {
                        acc->flute->alc.pAlcChannelHead[0].target_address.ubIP[i] = dstipaddr[i];
                    }
                } else {
                    for (i = 0; i < 4; i++) {
                        acc->flute->alc.pAlcChannelHead[0].target_address.ubIP[i] = dstipaddr[i];
                    }
                }

                acc->flute->alc.uiTSI = tsi;

                remove_dlist_entry(&acc->le);
                insert_dlist_tail_entry(&pvdr->acchdr, &acc->le);

                /* save K_pvdr_acc */
                /* sg_mgr_single_provider_freq_save(scan->mgr, pvdr, acc); */

                /*
                 * III, try to receive data
                 */
                /* for all the Provider in current Frequency, start to search */
                if (!pvdr->root) {
                    sg_root_new(&pvdr->root, scan->mgr, pvdr);
                }
                if (pvdr->root) {
                    kuint scan_started;

                    sg_root_frequency_add(pvdr->root, acc);

                    ktmr_kill(pvdr->root->tmr_scan_data_timeout);
                    kmsg_send(pvdr->root->worker_thread, KRTM_START_SCAN, pvdr, knil, knil, &scan_started);
                    if (scan_started) {
                        /* one scan issued */
                        pvdr->root->tmr_scan_data_timeout = ktmr_set(pvdr->root->worker_thread,
                                scan->scan.timeout * 3 / 4, kfalse, tmr_cbk_data_timeout, (kvoid*)pvdr->root, 0, 0, 0);
                        scan_cnt++;
                        scan->scan_ok++;
                    } else {
                        scan->scan_ng++;
                    }
                } else {
                    kerror(("sg_root_new failed\n"));
                }
            }
        }

        xmldoc_del(xml_doc);
    }

    if (!found) {
        /*
         * if not found any valid provider in fhdrs, first should try load the default.
         * And the default BootStrap data must be valid, which cause the found should not
         * be zero, so, the next loop, this block should be skipped.
         */

        /* first free data in fhdrs */
        sg_scan_fragments_free(scan);

        /* build a default XML and BIN file, then insert to fhdrs */
        nnn_sg_scan_load_from_heaven(scan, scan->mgr, scan->scan.protocol, SG_TMODE_FLUTE);

        /* try with the default */
        goto try_with_default_bs_param;
    }

    if (!scan_cnt) {
        /* FIXME : no scan, delete scan */
        sg_scan_complete(scan);
    } else {
        kmsg_post(scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)scan, (kvoid*)scan_cnt, 0, 0);
    }
    return 0;
}

static kint om_scan_bootstrap_done_for_bdir(kbean a_tsk, kuint a_msg,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_dlist_entry *tmp_entry, *tmp_hdr, *xml_entry, *xml_hdr, *bin_entry, *bin_hdr;

    sg_scan *scan = (sg_scan*)a_ar0;

    KXmlDoc *xml_doc, *tmp_doc;
    KXmlNode *tmp_node, *alc_node, *sp_node, *pvdr_node, *ci_node, *node;
    KXmlAttr *tmp_attr, *attr;

    sg_zbd *tmp_zbd, *xml_zbd;
    K_pvdr *pvdr;
    K_pvdr_acc *acc;

    kuint pid, scan_cnt = 0, cnt, i, ProviderID, noEnt, j, entVer, entLen, ipv6, port, tsi, frequency, tscnt;
    kchar *sa, *srcipaddr, *dstipaddr, *tmp_buf, *val, buf[128], *tsid, *nid;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    frequency = scan->scan.frequency;

    if ((kint)a_ar3 == 1) {
        /* this is called by time out, should load a default to try receive */
        klog(("Called by timeout!\n"));
    }

    /*
     * I, Process boot strap broadcast part
     * for all the provider in XML, try to find the access info in Bin file
     */

    /*
     * II, Process boot strap broadcast part
     * Then, get B-Dir SG BootStrap, and combine to broadcast one
     */

    if (is_dlist_empty(&scan->fhdrs[BS_XML])) {
        kerror(("Can not found data in BS_XML for bdir\n"));
        sg_scan_complete(scan);
        return -1;
    }

    xml_zbd = FIELD_TO_STRUCTURE(scan->fhdrs[BS_XML].next, sg_zbd, cls_le);

    if (!xml_zbd->dat.buf || !xml_zbd->dat.len) {
        kerror(("http_get_data failed\n"));
        sg_zbd_del(xml_zbd);
        sg_scan_complete(scan);
        return -1;
    }
    xml_doc = xmldoc_new(knil);
    xmldoc_parse(xml_doc, xml_zbd->dat.buf, xml_zbd->dat.len);

    klog(("sg_scan_bsstage_start_for_bdir: data : %s\n", xml_zbd->dat.buf));

    sg_zbd_del(xml_zbd);

#if 0
    /*
     * <ESG Ver="1.00">
     *     <Networks>
     *         <Network NID="string" NName="string" NDes="str" NType="0 or 1" AURL="mirror URL">
     *             <TS TSID="29" Frequency="number" Provider="same flute pvdrid" TSName="str" TSDes="str" AURL="0"/>
     *             <TS TSID="29" Frequency="number" Provider="same flute pvdrid" TSName="str" TSDes="str" AURL="0"/>
     *         </Network>
     *     </Networks>
     * </ESG>
     */
#endif

    xmldoc_gotoNode(xml_doc, "ESG", 0);
    xmldoc_gotoNode(xml_doc, "Networks", 0);
    cnt = xmldoc_getNodeCnt(xml_doc, "Network");
    for (i = 0; i < cnt; i++) {
        node = xmldoc_gotoNode(xml_doc, "Network", i);

        /*
         * FIXME: walk all the networks and peek the provider belong to current frequency
         */
        nid = xmlnode_get_attr_value(node, "NID");

        /* if no Provider set, make that Frequency belong to default Provider */
        ProviderID = 0xFFFF;

        tscnt = xmldoc_getNodeCnt(xml_doc, "TS");
        for (j = 0; j < tscnt; j++) {
            node = xmldoc_getNode(xml_doc, "TS", j);

            val = xmlnode_get_attr_value(node, "Frequency");
            if (val && frequency == strtoul(val, 0, 10)) {

                val = xmlnode_get_attr_value(node, "Provider");
                if (val) {
                    ProviderID = strtoul(val, 0, 10);
                }

                val = xmlnode_get_attr_value(node, "TSID");
                tsid = kstr_dup(val);

                break;
            }
        }
        if (j == tscnt) {
            /* goto "Network" node */
            node = xmldoc_gotoNode(xml_doc, "..", 0);
            continue;
        }

        pvdr = sg_mgr_find_provider(scan->mgr, ProviderID);
        if (!pvdr) {
            pvdr = (K_pvdr*)kmem_alloz(sizeof(K_pvdr));
            kassert(pvdr);
            if (pvdr) {

                pvdr->protocol = scan->scan.protocol;

                init_dlist_head(&pvdr->le);
                pvdr->flg = 0;
                pvdr->id = ProviderID;
                init_dlist_head(&pvdr->acchdr);
                pvdr->root = knil;

                /*
                 * FIXME:
                 * B-DIR should has same parameter as flute to describe a provider, but now,
                 * schema for Networks is not same as flute. so current can not save __pvdr.xml correctly
                 */
                /* fill the data of K_pvdr */
                pvdr->doc = xmldoc_new(knil);
                pvdr_node = xmlnode_new(knil, "pvdr", knil);
                xmldoc_addNode(pvdr->doc, pvdr_node, knil);

                sprintf(buf, "%d", ProviderID);
                xmlnode_set_attr_value(pvdr_node, "id", buf);

                sprintf(buf, "%d", 0);
                xmlnode_set_attr_value(pvdr_node, "flg", buf);

                sprintf(buf, "%d", pvdr->protocol);
                xmlnode_set_attr_value(pvdr_node, "protocol", buf);

                if (tmp_node = xmldoc_getNode(xml_doc, "ProviderName", 0)) {
                    xmlnode_set_attr_value(pvdr_node, "name", tmp_node->text ? tmp_node->text : "");
                }

                if (tmp_node = xmldoc_getNode(xml_doc, "ProviderURI", 0)) {
                    xmlnode_set_attr_value(pvdr_node, "uri", tmp_node->text ? tmp_node->text : "");
                }

                if (tmp_node = xmldoc_getNode(xml_doc, "ProviderInformationURL", 0)) {
                    xmlnode_set_attr_value(pvdr_node, "infuri", tmp_node->text ? tmp_node->text : "");
                }

                if (tmp_node = xmldoc_getNode(xml_doc, "PrivateAuxiliaryData", 0)) {
                    xmlnode_set_attr_value(pvdr_node, "auxdat", tmp_node->text ? tmp_node->text : "");
                }

                if (tmp_node = xmldoc_getNode(xml_doc, "ProviderLogo", 0)) {
                    xmlnode_set_attr_value(pvdr_node, "logo", tmp_node->text ? tmp_node->text : "");
                }

                /* sg_mgr_single_provider_save do not queue, queue first */
                insert_dlist_tail_entry(&scan->mgr->provhdr, &pvdr->le);

                /* __pvdr.xml done, save it */
                /* sg_mgr_single_provider_save(scan->mgr, pvdr); */

            } else {
                kerror(("no memory for pvdr\n"));
                /* goto "Networks" node */
                node = xmldoc_gotoNode(xml_doc, "..", 0);
                continue;
            }
        } else {
            kassert(pvdr->protocol == scan->scan.protocol);
        }

        /*
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * XXX XXX XXX XXX XXX XXX XXX XXX XXX
         * about base information about bdir provider, it should provider same information as flute mode, include name, uri, infuri, logo, auxdat etc
         */

        /*
         * II, merge or create K_pvdr_acc for found K_pvdr
         */

        /*
         * II.1 find K_pvdr_acc if not found, add it
         */
        acc = sg_mgr_pvdr_acc_find(scan->mgr, pvdr, frequency);
        if (acc) {
            int idx;
            if (acc->bdir) {
                kerror(("K_pvdr_acc for Provider %d at frequency %d with flute already exist\n", pvdr->id, frequency));
                //continue;
            }
            /* move the found to last */
            for (idx = 0; idx < 8; idx++) {
                if (SG_TMODE_BDIR == acc->tmode[idx]) {
                    for (; idx < 8; idx++) {
                        acc->tmode[idx] = acc->tmode[idx + 1];
                        if (SG_TMODE_LAST == acc->tmode[idx]) {
                            acc->tmode[idx] = SG_TMODE_BDIR;
                            acc->tmode[idx + 1] = SG_TMODE_LAST;
                            break;
                        }
                    }
                }
            }
        } else {
            acc = (K_pvdr_acc*)kmem_alloz(sizeof(K_pvdr_acc));
            if (!acc) {
                kerror(("no memory\n"));
                continue;
            }

            init_dlist_head(&acc->le);
            acc->pvdr = pvdr;
            acc->frequency = frequency;
            acc->protocol = scan->scan.protocol;
            memset(acc->tmode, SG_TMODE_LAST, sizeof(kchar) * 8);

            acc->tmode[0] = SG_TMODE_BDIR;
            acc->tindex = 0;
        }

        /*
         * II.2 create flute_acc
         */
        if (!acc->bdir) {
            acc->bdir = (K_bdir_acc*)kmem_alloz(sizeof(K_bdir_acc));
        }
        acc->bdir->hdr.pvdr_acc = acc;
        acc->bdir->hdr.tmode = SG_TMODE_BDIR;

        acc->bdir->frequency = frequency;

        STR_SET(acc->bdir->url, scan->mgr->env->bdsg_url);
        STR_SET(acc->bdir->nid, nid);
        acc->bdir->tsid = tsid;
        acc->bdir->nname = knil;

        /*
         * II.2.1 queue the acc
         */
        remove_dlist_entry(&acc->le);
        insert_dlist_tail_entry(&pvdr->acchdr, &acc->le);

        /*
         * II.2.2 save K_pvdr_acc
         */
        /* sg_mgr_single_provider_freq_save(scan->mgr, pvdr, acc); */

        /*
         * III, try to receive data
         */
        /* for all the Provider in current Frequency, start to search */
        if (!pvdr->root) {
            sg_root_new(&pvdr->root, scan->mgr, pvdr);
        }
        if (pvdr->root) {
            kuint scan_started;

            sg_root_frequency_add(pvdr->root, acc);

            ktmr_kill(pvdr->root->tmr_scan_data_timeout);
            kmsg_send(pvdr->root->worker_thread, KRTM_START_SCAN, pvdr, knil, knil, &scan_started);
            if (scan_started) {
                /* one scan issued */
                pvdr->root->tmr_scan_data_timeout = ktmr_set(pvdr->root->worker_thread,
                        scan->scan.timeout * 3 / 4, kfalse, tmr_cbk_data_timeout, (kvoid*)pvdr->root, 0, 0, 0);
                scan_cnt++;
                scan->scan_ok++;
            } else {
                scan->scan_ng++;
            }
        } else {
            kerror(("sg_root_new failed\n"));
        }

        /* goto "Networks" node */
        node = xmldoc_gotoNode(xml_doc, "..", 0);
    }

    /* XXX called before SCAN_BOOTSTRAP_START, because SCAN_BOOTSTRAP_START can cause scan task exit */
    xmldoc_del(xml_doc);

    if (!scan_cnt) {
        /* FIXME : no scan, delete scan */
        sg_scan_complete(scan);
    } else {
        kmsg_post(scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)scan, (kvoid*)scan_cnt, 0, 0);
    }

    return 0;
}
static kint om_scan_bootstrap_done_for_mot(kbean a_tsk, kuint a_msg,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_dlist_entry *tmp_entry, *tmp_hdr, *xml_entry, *xml_hdr, *bin_entry, *bin_hdr;

    sg_scan *scan = (sg_scan*)a_ar0;

    KXmlDoc *xml_doc, *tmp_doc, *doc;
    KXmlNode *tmp_node, *alc_node, *sp_node, *pvdr_node, *ci_node, *node;
    KXmlAttr *tmp_attr, *attr;

    sg_zbd *tmp_zbd, *xml_zbd, *bin_zbd;
    K_pvdr *pvdr;
    K_pvdr_acc *acc;

    kuint pid, scan_cnt = 0, cnt, i, ProviderID, noEnt, j, entVer, entLen, ipv6, port, tsi, frequency, nid, tscnt;
    kchar *sa, *srcipaddr, *dstipaddr, *tmp_buf, *val;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if ((kint)a_ar3 == 1) {
        /* this is called by time out, should load a default to try receive */
        klog(("Called by timeout!\n"));
    }

    /*
     * I, Process boot strap broadcast part
     * for all the provider in XML, try to find the access info in Bin file
     */

    /* A, can not get boot strap try to build a default one */
    if (((kint)a_ar3 == 1) && is_dlist_empty(&scan->fhdrs[BS_XML]) || is_dlist_empty(&scan->fhdrs[BS_BIN])) {

        /* first free data in fhdrs */
        sg_scan_fragments_free(scan);

        /* build a default K_pvdr according to env:;protocol, and insert */
        nnn_sg_scan_load_from_heaven(scan, scan->mgr, scan->scan.protocol, SG_TMODE_MOT);
    }

    /* B, try to load broadcast boot strap */
    xml_hdr = &scan->fhdrs[BS_XML];
    xml_entry = xml_hdr->next;
    while (xml_entry != xml_hdr) {
        xml_zbd = FIELD_TO_STRUCTURE(xml_entry, sg_zbd, cls_le);
        xml_entry = xml_entry->next;

        xml_doc = xmldoc_new(knil);
        xmldoc_parse(xml_doc, xml_zbd->dat.buf, xml_zbd->dat.len);
        sg_zbd_del(&xml_zbd);

        /* TODO: add code here */
        kerror(("BootStarp for MOT is not implemented"));
        kassert(!"BootStarp for MOT is not implemented");

        if (!sg_root_get_sdd_dir(pvdr->root)) {
            /* one scan issued */
            scan_cnt++;
        }
    }

    if (!scan_cnt) {
        /* FIXME : no scan, delete scan */
        sg_scan_complete(scan);
    } else {
        kmsg_post(scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)scan, (kvoid*)scan_cnt, 0, 0);
    }

    return 0;
}

/**
 * \brief Called when to all the needed boot strap data is done.
 * - Check status, if time out,
 * - if fhdrs, parse them, generate K_pvdr then queue them to scan->provhdr
 * - Try to get http SG, compare scan->provhdr, and insert the data to same provider
 * - for all the provider be found currently, start sg_root to get the data
 */
static kint om_scan_bootstrap_done(kbean a_tsk, kuint a_msg,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_scan *scan = (sg_scan*)a_ar0;
    kchar curtmode;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    curtmode = scan->scan.tmode;

    if (curtmode == SG_TMODE_FLUTE) {
        om_scan_bootstrap_done_for_flute(a_tsk, a_msg, a_ar0, a_ar1, a_ar2, a_ar3, a_rsn);
    } else if (curtmode == SG_TMODE_MOT) {
        om_scan_bootstrap_done_for_mot(a_tsk, a_msg, a_ar0, a_ar1, a_ar2, a_ar3, a_rsn);
    } else if (curtmode == SG_TMODE_BDIR) {
        om_scan_bootstrap_done_for_bdir(a_tsk, a_msg, a_ar0, a_ar1, a_ar2, a_ar3, a_rsn);
    } else {
        kassert(!"bad tmode");
    }

    /*
     * After all the data Process, delete the unused XML and BIN file
     */
    sg_scan_fragments_free(scan);

    /*
     * XXX change scan count with zero, this called om_scan_change_cnt called
     * and try to call ::sg_scan_complete, if not scan issued, ::sg_scan_complete will called
     */
    scan->all_scan_issued = 1;
    kmsg_post(scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)scan, (kvoid*)0, 0, 0);
    return 0;
}

static kvoid sg_scan_fragments_free(sg_scan *scan)
{
    K_dlist_entry *entry;
    sg_zbd *zbd;

    while (!is_dlist_empty(&scan->fhdrs[BS_XML])) {
        entry = remove_dlist_head_entry(&scan->fhdrs[BS_XML]);
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, cls_le);

        sg_zbd_del(zbd);
    }
    while (!is_dlist_empty(&scan->fhdrs[BS_BIN])) {
        entry = remove_dlist_head_entry(&scan->fhdrs[BS_BIN]);
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, cls_le);

        sg_zbd_del(zbd);
    }
}

static kint sg_scan_obj_cbk(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    sg_scan *scan = (sg_scan*)vpUserData0;

    kchar *ob = knil;
    kint ol = 0, i, j, k, shift, type = (kint)vpUserData2;

    sg_zbd *zbd;

    kuchar *gunzip_buf = pDataBuffer, *gunzip_buf_end = pDataBuffer + uiObjectLength;

    kassert(ses == (sg_ses*)vpUserData1);

    if (iStatus) {
        kfatal(("sgducbk:callback failed with status :%d\n", iStatus));
        sg_ses_try_end_session(ses);
        return -1;
    }

    // XXX Only really changed SDD issue the request
    // So, skip the new version checking

    if (pContentEncoding && streq(pContentEncoding, "gzip")) {
        kint ret, mem_inc_step = 0;
        kuchar *uncompr;
        kulong uncomprLen;

try_with_more_mem:
        uncomprLen = kspt_gzip_orglen(pDataBuffer, uiObjectLength) + mem_inc_step;
        kassert(uncomprLen);
        uncompr = kmem_alloc(uncomprLen);
        kassert(uncompr);

        ret = zei_uncompress(uncompr, &uncomprLen, gunzip_buf, gunzip_buf_end - gunzip_buf, knil);
        if (0 == ret) {
            ob = (kchar*)uncompr;
            ol = uncomprLen;
        } else if (-5 == ret) {
            kmem_free(uncompr);
            mem_inc_step += 1024;
            goto try_with_more_mem;
        } else {
            sg_ses_try_end_session(ses);
            return -1;
        }
    } else {
        ob = (kchar*)pDataBuffer;
        ol = uiObjectLength;
    }

    /* save the file scan fhdrs */
    zbd = knil;
    sg_zbd_new(&zbd);
    zbd->dat.buf = ob;
    zbd->dat.len = ol;
    if (type == 1) {
        /* XML */
        insert_dlist_tail_entry(&scan->fhdrs[BS_XML], &zbd->cls_le);
    } else {
        /* Access */
        insert_dlist_tail_entry(&scan->fhdrs[BS_BIN], &zbd->cls_le);
    }

    sg_ses_try_end_session(ses);
    return 0;
}

static kint sg_scan_dir_cbk(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    kint i, shift, type;
    IV_OBJECT_INFO *ivoi;
    sg_scan *scan = (sg_scan*)vpUserData0;

    kassert(ses == (sg_ses*)vpUserData1);

    if (iStatus != 0 || iObjectNum == 0) {
        kerror(("sg_scan_dir_cbk: bad status(%d) or no object(%d)!\n", iStatus, iObjectNum));

        /* end boot strap stage, simulate a time out */
        kmsg_post(scan->worker_thread, SCAN_BOOTSTRAP_DONE, (kvoid*)scan, knil, knil, (kvoid*)1);
        sg_ses_try_end_session(ses);
        return 0;
    }

    for (i = 0; i < iObjectNum; i++) {
        ivoi = pInfoLisHead + i;

        if (ivoi->iObjectVersionLength >= 0) {
            shift = ivoi->iObjectVersionLength;
        }

        if (strstr(ivoi->pContentEncoding, "xml")) {
            /* this is XML part */
            type = 1;
        } else {
            /* Access part */
            type = 0;
        }

        sg_ses_get_obj(ses, 2000000, ivoi->uiObjectID, 0xFFFFFFFF << shift,
                (kvoid*)sg_scan_obj_cbk, knil, (kvoid*)scan, (kvoid*)ses, (kvoid*)type, knil);
    }
    sg_ses_try_end_session(ses);
    return 0;
}

/**
 * \brief protocol can be ORed, how?
 */
static kint sg_scan_setup_env(sg_scan *scan, kuint tmode)
{
    sg_ses *ses = &scan->ses;
    IV_ALC_CHANNEL_INFO *aci;
    kchar *ptr;

    IV_FLUTE_ALC_SESSION_ENTRYINFO *sei_alc;
    IV_DAB_SESSION_ENTRYINFO *sei_mot;

    if (SG_TMODE_FLUTE == tmode) {
        sei_alc = kmem_alloz(sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO));
        ses->sei.alc = sei_alc;

        aci = sei_alc->pAlcChannelHead;
        ses->sei.alc->iAlcChannelNum = 1;
        ses->sei.alc->uiTSI = 1;

        aci->u16TargetPort = (kushort)scan->mgr->env->bs_port;

        ptr = scan->mgr->env->bs_ipa;
        if (!ptr[0]) {
            /* if IP address for flute boot strap not set, reset the ip address and port to default */
            ptr = "224.0.23.14";
            aci->u16TargetPort = 9214;
        }
        if (strstr(ptr, ".")) {
            aci->target_address.iIPv4_flag = ktrue;
            ipv4_scan(ptr, (kuchar*)aci->target_address.ubIP);
        } else {
            aci->target_address.iIPv4_flag = kfalse;
            ipv6_scan(ptr, (kuchar*)aci->target_address.ubIP);
        }
    } else if (SG_TMODE_MOT == tmode) {
        sei_mot = kmem_alloz(sizeof(IV_DAB_SESSION_ENTRYINFO));
        ses->sei.mot = sei_mot;

        sei_mot->eID = IV_DAB_BS_DEF_EID;
        sei_mot->mode = IV_DAB_MOT_DIRECTORY;
        sei_mot->serviceComponentID = IV_DAB_BS_DEF_SCID;
    } else if (SG_TMODE_BDIR == tmode) {
        /* bdir do not get boot strap info via sg session */
    } else {
        return -1;
    }
    return 0;
}


/**
 * \brief build a default provider information
 * According to current environment(user's scan parameter, such as sg_env::bs_port etc)
 * decide what kind of, FLUTE or MOT, SG should be generated.
 *
 * \warning Only the broadcast default data generated here.
 * \pre Only called when scan->fhdrs all empty.
 */
static kint nnn_sg_scan_load_from_heaven_for_flute(sg_scan *scan, sg_mgr *mgr, kuint protocol, kchar tmode)
{
    /*
     * <ESGProviderDiscovery xmlns="urn:dvb:ipdc:esgbs:2005">
     *     <ServiceProvider format="urn:nokia:oai:sg:2005">
     *         <ProviderURI>www.rantanplan.com</ProviderURI>
     *         <ProviderName>Asterix</ProviderName>
     *         <ProviderLogo>auv_TitleMediaType</ProviderLogo>
     *         <ProviderInformationURL>auv_anyURI</ProviderInformationURL>
     *         <PrivateAuxiliaryData>auv_anyType</PrivateAuxiliaryData>
     *         <ProviderID>3344</ProviderID>
     *     </ServiceProvider>
     * </ESGProviderDiscovery>
     */
    kchar xmlpart[] = "\
                       <ESGProviderDiscovery xmlns=\"urn:dvb:ipdc:esgbs:2005\">\
                       <ServiceProvider format=\"urn:nokia:oai:sg:2005\">\
                       <ProviderURI></ProviderURI>\
                       <ProviderName>default flute provider</ProviderName>\
                       <ProviderLogo></ProviderLogo>\
                       <ProviderInformationURL></ProviderInformationURL>\
                       <PrivateAuxiliaryData></PrivateAuxiliaryData>\
                       <ProviderID>65535</ProviderID>\
                       </ServiceProvider>\
                       </ESGProviderDiscovery>";

    /*
     * ESG_Access_Descriptor {
     *     n_o_ESGEntries                            ## 16
     *     for (i = 0; i < n_o_ESGEntries; i++) {
     *         ESGEntry[i]
     *     }
     * }
     * ESGEntry {
     *     ESGEntryVersion                           ## 8
     *     ESGEntryLength                            ## 8+
     *     MultipleStreamTransport                   ## 1
     *     IPVersion6                                ## 1
     *     Reserved                                  ## 6
     *     ProviderID                                ## 16
     *     if (IPVersion6) {
     *         SourceIPAddress                       ## 128
     *         DestinationIPAddress                  ## 128
     *     } else {
     *         SourceIPAddress                       ## 32
     *         DestinationIPAddress                  ## 32
     *     }
     *     Port                                      ## 16
     *     TSI                                       ## 16
     * }
     */
    kuchar binpart[] = {
        0x00, 0x01,                                 /* 16   : n_o_ESGEntries = 1 */
        0x00,                                       /* 8    : ESGEntryVersion  = 0 */
        0x0F,                                       /* 8    : ESGEntryLength  = 0 */
        0x00,                                       /* 8    : MST = false, IVP6 = false, RES */
        0xFF, 0xFF,                                 /* 16   : ProviderID = -1 */
        127, 0, 0, 1,                               /* 32   : SourceIPAddress */
        //192, 168, 1, 215,                            /* 32   : DestinationIPAddress */
        //224, 0, 23, 165,                            /* 32   : DestinationIPAddress */
        234, 6, 6, 5,
        0x0F, 0xFA,                                 /* 16   : Port = 4090 */
        0x00, 0x01                                  /* 16   : TSI = 1 */
    };

    sg_zbd *zbd;

    /* add xml file */
    zbd = knil;
    sg_zbd_new(&zbd);
    zbd->dat.buf = kstr_dup(xmlpart);
    zbd->dat.len = strlen(xmlpart);
    insert_dlist_tail_entry(&scan->fhdrs[BS_XML], &zbd->cls_le);

    /* add bin file */
    zbd = knil;
    sg_zbd_new(&zbd);
    zbd->dat.len = sizeof(binpart);
    zbd->dat.buf = kmem_alloc(zbd->dat.len);
    memcpy(zbd->dat.buf, binpart, zbd->dat.len);
    insert_dlist_tail_entry(&scan->fhdrs[BS_BIN], &zbd->cls_le);

    return 0;
}

/**
 * \brief build a default provider information
 * According to current environment(user's scan parameter, such as sg_env::bs_port etc)
 * decide what kind of, FLUTE or MOT, SG should be generated.
 *
 * \warning Only the broadcast default data generated here.
 * \pre Only called when scan->fhdrs all empty.
 */
static kint nnn_sg_scan_load_from_heaven_for_mot(sg_scan *scan, sg_mgr *mgr, kuint protocol, kchar tmode)
{
    kassert(!"add code her");

    return 0;
}

/**
 * \brief build a default provider information
 * According to current environment(user's scan parameter, such as sg_env::bs_port etc)
 * decide what kind of, FLUTE or MOT, SG should be generated.
 *
 * \warning Only the broadcast default data generated here.
 * \pre Only called when scan->fhdrs all empty.
 */
static kint nnn_sg_scan_load_from_heaven(sg_scan *scan, sg_mgr *mgr, kuint protocol, kchar tmode)
{
    if (tmode == SG_TMODE_FLUTE) {
        nnn_sg_scan_load_from_heaven_for_flute(scan, mgr, protocol, tmode);
    } else if (tmode == SG_TMODE_MOT) {
        nnn_sg_scan_load_from_heaven_for_mot(scan, mgr, protocol, tmode);
    } else {
        return -1;
    }

    return 0;
}

/**
 * \brief Call evtCallback and detach scan from sg_mgr
 */
static kint sg_scan_call_callback(sg_scan *scan)
{
    kuint status;
    sg_mgr *mgr;

    if (!scan) {
        return -1;
    }

    mgr = scan->mgr;
    mgr->scan = knil;

    klog(("All the scan done, call scan done callback!\n"));

    /*
     * XXX : Only one status can be returned to user, scan success or scan failed,
     * if any provider scan success that means all scan sucess. If all failed, the scan failed
     */
    if ((scan->scan_ok + scan->scan_ng) && (scan->scan_ok)) {
        status = IVEC_SUCCESS;
    } else {
        status = IVEC_FAILURE;
    }

    mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)status,
            (kvoid*)scan->scan.stat.all, (kvoid*)scan->scan.stat.cur, (kvoid*)scan->scan.userdata);

    klog(("Scan done callback called :%x, delete scan structure!\n", status));
    return 0;
}

/**
 * \brief Force complete scan process.
 *
 * If not actualy complete, abort scan.
 */
static kint sg_scan_complete(sg_scan *scan)
{
    kmsg_post(scan->worker_thread, KMSG_QUIT, (kvoid*)scan, 0, 0, 0);
    return 0;
}

