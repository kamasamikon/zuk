/* vim:set et sw=4 sts=4: */
#include <string.h>
#include <stdio.h>
#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include <ktsk.h>
#include <ksyn.h>

#include "ipaddr.h"

#include "xmldoc.h"

#include "sg_sdd.h"
#include "sg_sdu.h"
#include "s2n_macro.h"

#include "sg_mgr2.h"
#include "sg_root.h"

#include "sg_ser.h"
#include "sg_sch.h"
#include "sg_ctt.h"
#include "sg_acc.h"
#include "sg_pci.h"
#include "sg_pcd.h"
#include "sg_pcc.h"
#include "sg_iad.h"
#include "sg_pvd.h"
#include "sg_sdp.h"

kint sg_sdd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_sdd *sdd = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, SDD, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_sdd_new(&sdd);
    sdd->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)sdd);
    kflg_set(sdd->flg, EFF_LOADING);

    if (-2 == sg_sdd_x2s(sdd)) {
        FREE_ORG_DAT(sdd);
        sg_sdd_del(sdd, ktrue);
        return -1;
    }
    FREE_ORG_DAT(sdd);

    sg_sdd_load_extinfo(sdd);

    insert_dlist_tail_entry(&root->allhdr, &sdd->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SDD], &sdd->cls_le);
    kflg_clr(sdd->flg, EFF_LOADING);
    return 0;
}


kint sg_sdd_new(sg_sdd **sdd)
{
    if (!(*sdd)) {
        (*sdd) = (sg_sdd*)kmem_alloc(sizeof(sg_sdd));
    }
    if ((*sdd)) {
        memset((*sdd), 0, sizeof(sg_sdd));
        INIT_COMMON((*sdd));
        (*sdd)->cls = CLS_SGDD;

        init_dlist_head(&(*sdd)->sgdu_ctx_hdr);

        init_dlist_head(&(*sdd)->dir_le);

        return 0;
    }
    return -1;
}

kint sg_sdd_free_sub_sdu(sg_sdd *sdd)
{
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    sg_sdu *sdu;
    K_freq *fblk;

    if (!sdd) {
        return -1;
    }

    fb_hdr = &sdd->rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        // Remove sub sgdus
        hdr = &fblk->fhdrs[SDU];
        entry = hdr->next;
        while (entry != hdr) {
            sdu = FIELD_TO_STRUCTURE(entry, sg_sdu, cls_le);
            entry = entry->next;

            if (sdu->sdd_tid == sdd->tid) {
                sg_sdu_free_sub_fragments(sdu);
            }
        }
    }
    return 0;
}

kint sg_sdd_del(sg_sdd *sdd, kbool rmcache)
{
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    sg_sdu *sdu;
    K_freq *fblk;

    if (!sdd) {
        return -1;
    }
    remove_dlist_entry(&sdd->all_le);
    remove_dlist_entry(&sdd->cls_le);
    remove_dlist_entry(&sdd->dir_le);

    kmem_free(sdd->objinf.pObjectName);

    fb_hdr = &sdd->rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;


        // Remove sub sgdus
        hdr = &fblk->fhdrs[SDU];
        entry = hdr->next;
        while (entry != hdr) {
            sdu = FIELD_TO_STRUCTURE(entry, sg_sdu, cls_le);
            entry = entry->next;

            if (sdu->sdd_tid == sdd->tid) {
                sg_sdu_del(sdu, rmcache);
            }
        }
    }

    kassert(CLS_SGDD == sdd->cls);

    if (rmcache) {
        sg_root_rm_cache_file(sdd);
    }

    kmem_free(sdd->attr.id);
    S2N_FUN(ServiceGuideDeliveryDescriptor, sdd);
    // DEL_COMMON(sdd);
    return 0;
}

struct objinf {
    kchar pObjectName[256];
    kuint uiObjectID;
    kint iObjectVersion;
    kint iObjectVersionLength;
    kuint uiContentLength;
} ;

kint sg_sdd_save_extinfo(sg_sdd *sdd)
{
    struct objinf oi;
    kchar oifn[128];

    sg_root_get_zbd_path(sdd, oifn);
    strcat(oifn, "_oi");

    if (sdd->objinf.pObjectName) {
        strcpy(oi.pObjectName, sdd->objinf.pObjectName);
    } else {
        oi.pObjectName[0] = '\0';
    }
    oi.uiObjectID = sdd->objinf.uiObjectID;
    oi.iObjectVersion = sdd->objinf.iObjectVersion;
    oi.iObjectVersionLength = sdd->objinf.iObjectVersionLength;
    oi.uiContentLength = sdd->objinf.uiContentLength;

    ksal_file_save(oifn, (kvoid*)&oi, sizeof(oi));

    return 0;
}

kint sg_sdd_load_extinfo(sg_sdd *sdd)
{
    kint hfile;

    struct objinf oi, *poi = &oi;
    kchar oifn[128];
    kint buflen = sizeof(oifn);

    sg_root_get_zbd_path(sdd, oifn);
    strcat(oifn, "_oi");

    hfile = kvfs_open(oifn, "rb", 0);
    if (!hfile) {
        return -1;
    }

    kvfs_read(hfile, &poi, &buflen);
    kvfs_close(hfile);

    sdd->objinf.pObjectName = kstr_dup(oi.pObjectName);
    sdd->objinf.uiObjectID = oi.uiObjectID;
    sdd->objinf.iObjectVersion = oi.iObjectVersion;
    sdd->objinf.iObjectVersionLength = oi.iObjectVersionLength;
    sdd->objinf.uiContentLength = oi.uiContentLength;

    return 0;
}


kint sg_sdd_add_cache(sg_sdd *sdd)
{
    // save the content of the file to disk etc
    if (kflg_chk(sdd->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(sdd);
    return 0;
}

kint sg_sdd_del_cache(sg_sdd *sdd)
{
    // save the content of the file to disk etc
    return 0;
}

/////////////////////
#if 0
typedef struct _sg_sdd {
    EF_COMMON;

    // le for sg_sgdd_ent::sgdd_ctx_hdr
    K_dlist_entry dir_le;

    // hdr for sub sg_sgdu_ent
    K_dlist_entry sgdu_ctx_hdr;

    struct {
        kchar *pObjectName;
        kuint uiObjectID;
        kint iObjectVersion;
        kint iObjectVersionLength;
        kuint uiContentLength;
    } objinf;

    kuint ecnt;
    ServiceGuideDeliveryDescriptor_rec *arr;
} ServiceGuideDeliveryDescriptor_t;
#endif
/////////////////////

kint sg_sdd_ld_cache(sg_sdd *sdd)
{
    // save the content of the file to disk etc
    kflg_clr(sdd->flg, EFF_LOADING);

    return 0;
}

typedef struct _K_MB_SGDU_OBJ {
    sg_ses *ses;
    kint iHandle;
    kint iStatus;
    kuint uiObjectID;
    kchar*pObjectName;
    kchar *pObjectUrl;
    kint iObjectVersion;
    kuchar *pDataBuffer;
    kuint uiObjectLength;
    kchar *pSavePathName;
    kchar *pContentEncoding;

    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} K_MB_SGDU_OBJ;

/**
 * @brief Lower Layer call this when a SGDD object gotten.
 *
 * @param iHandle
 * @param iStatus
 * @param uiObjectID
 * @param pObjectName
 * @param pObjectUrl
 * @param iObjectVersion
 * @param pDataBuffer
 * @param uiObjectLength
 * @param pSavePathName
 * @param pContentEncoding
 * @param vpUserData0
 * @param vpUserData1
 * @param vpUserData2
 * @param vpUserData3
 *
 * @return
 */
static kint sgducbk(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3)
{
    K_MB_SGDU_OBJ obj;
    sg_root *rt = (sg_root*)vpUserData0;

    obj.ses = ses;
    obj.iHandle = iHandle;
    obj.iStatus = iStatus;
    obj.uiObjectID = uiObjectID;
    obj.pObjectName = pObjectName;
    obj.pObjectUrl = pObjectUrl;
    obj.iObjectVersion = iObjectVersion;
    obj.pDataBuffer = pDataBuffer;
    obj.uiObjectLength = uiObjectLength;
    obj.pSavePathName = pSavePathName;
    obj.pContentEncoding = pContentEncoding;

    obj.vpUserData0 = vpUserData0;
    obj.vpUserData1 = vpUserData1;
    obj.vpUserData2 = vpUserData2;
    obj.vpUserData3 = vpUserData3;

    return kmsg_send(rt->worker_thread, KRTM_SGDU_OBJ, &obj, 0, 0, 0);
}

kint om_sgdu_obj(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MB_SGDU_OBJ *obj = (K_MB_SGDU_OBJ*)a_ar0;

    sg_ses *ses = obj->ses;
    kint iHandle = obj->iHandle;
    kint iStatus = obj->iStatus;
    kuint uiObjectID = obj->uiObjectID;
    kchar*pObjectName = obj->pObjectName;
    kchar *pObjectUrl = obj->pObjectUrl;
    kint iObjectVersion = obj->iObjectVersion;
    kuchar *pDataBuffer = obj->pDataBuffer;
    kuint uiObjectLength = obj->uiObjectLength;
    kchar *pSavePathName = obj->pSavePathName;
    kchar *pContentEncoding = obj->pContentEncoding;

    kvoid *vpUserData0 = obj->vpUserData0;
    kvoid *vpUserData1 = obj->vpUserData1;
    kvoid *vpUserData2 = obj->vpUserData2;
    kvoid *vpUserData3 = obj->vpUserData3;

    kint i, ofs, ol;
    sg_root *rt = (sg_root*)vpUserData0;
    sg_ses *sgduses = (sg_ses*)vpUserData1;
    sg_sdd *sdd = (sg_sdd*)vpUserData2;
    kuint *gottenMarkAddr = (kuint*)vpUserData3;
    kchar *ob = knil;
    sg_sdu *sdu = knil;
    static kint tobjcbk_called_cnt = 0;

    K_freq *fblk;

    kuchar *gunzip_buf = pDataBuffer, *gunzip_buf_end = pDataBuffer + uiObjectLength;
    kulong uncomprLen;

    tobjcbk_called_cnt++;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    klog(("sgducbk:iStatus:%02d, objid:%08d, ud:%08x, %d\n", iStatus, (kint)uiObjectID, vpUserData1, tobjcbk_called_cnt));
    if (0 != iStatus) {
        kfatal(("sgducbk:callback failed with status :%d\n", iStatus));
        sg_ses_try_end_session(ses);
        return -1;
    }

    fblk = sg_root_freq_blk_find(rt, sdd->frequency);
    if (!fblk) {
        kerror(("Can not find frequency\n"));
        sg_ses_try_end_session(ses);
        return -1;
    }

    fblk->stat.sdu_ret++;

    //pContentEncoding = kstr_dup("gzip");
    // the data should be duplicated for later used
    if (pContentEncoding && streq(pContentEncoding, "gzip")) {
        kint ret, mem_inc_step = 0;
        kuchar *uncompr;

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
        ob = (kchar*)kmem_alloc(uiObjectLength);
        memcpy(ob, pDataBuffer, uiObjectLength);
        ol = uiObjectLength;
    }

    sg_sdu_new(&sdu);
    sdu->frequency = sdd->frequency;
    sdu->sdd_tid = sdd->tid;
    sdu->tid = uiObjectID;
    sdu->rt = rt;
    sdu->dat.buf = ob;
    sdu->dat.len = ol;
    kflg_set(sdu->flg, EFF_DATAOK);    // indicate the data not been processed
    insert_dlist_tail_entry(&rt->allhdr, &sdu->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SDU], &sdu->cls_le);

    //sg_ses_stop_session(sgduses);
    // kflg_set( to be ESF_STOP

    kmsg_post(rt->worker_thread, KRTM_SGDU_PARSE, rt, knil, knil, knil);
    sg_ses_try_end_session(ses);
    return 0;
}

// if old_sdd not in sdd remove it
kbool sg_sdd_chk_ood_fragment(sg_sdd *sdd, Fragment_rec *frag_rec)
{
    kint i, j, k;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0, shift;

    if ((!sdd->arr) || (!sdd->arr->DescriptorEntry)) {
        return ktrue;
    }

    de_cnt = sdd->arr->DescriptorEntry->ecnt;
    de_arr = sdd->arr->DescriptorEntry->arr;

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;

        for (j = 0; j < sgdu_cnt; j++) {
            if (!sgdu_arr[j].Fragment) {
                continue;
            }
            frag_cnt = sgdu_arr[j].Fragment->ecnt;
            frag_arr = sgdu_arr[j].Fragment->arr;
            for (k = 0; k < frag_cnt; k++) {
                if (frag_rec->attr.transportID == frag_arr[k].attr.transportID) {
                    // in sdd list, not a ood fragment
                    return kfalse;
                }
            }
        }
    }
    // not in sdd list, this is a ood fragment
    return ktrue;
}

// if old_sdd not in sdd, that is a ood fragment
kint sg_sdd_remove_ood_fragment(sg_sdd *old_sdd, sg_sdd *sdd)
{
    kint i, j, k;
    sg_root *rt = old_sdd->rt;
    sg_ses *sgduses;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0, shift;

    K_dlist_entry *entry, *hdr;

    kuchar sdu_arr[100];
    kint sdu_cnt;

    //xxx_Remove not used SDGU file
    if (!old_sdd->arr || !old_sdd->arr->DescriptorEntry) {
        return -1;
    }

    de_cnt = old_sdd->arr->DescriptorEntry->ecnt;
    de_arr = old_sdd->arr->DescriptorEntry->arr;

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        if (!de_arr[i].Transport) {
            continue;
        }
        sdu_cnt = 0;
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;

        for (j = 0; j < sgdu_cnt; j++) {

            if (!sgdu_arr[j].Fragment) {
                continue;
            }

            frag_cnt = sgdu_arr[j].Fragment->ecnt;
            frag_arr = sgdu_arr[j].Fragment->arr;

            for (k = 0; k < frag_cnt; k++) {
                if (sg_sdd_chk_ood_fragment(sdd, &frag_arr[k])) {
                    sdu_arr[sdu_cnt++] = j;
                    break;
                }
            }
        }

        if (sdu_cnt) {
            kint objhandle,    seshandle;

            // issue command
            tp_arr = de_arr[i].Transport->arr;
            tp_cnt = de_arr[i].Transport->ecnt;

            // Only receive the TOI list int sgdu_arr[sdu_arr[k]].
            for (k = 0; k < sdu_cnt; k++) {
                // remove all the sgdu marked in sdu_arr

                sg_sdu *sdu = sg_root_get_sdu_by_id(rt, sgdu_arr[sdu_arr[k]].attr.transportObjectID);
                if (sdu) {
                    sg_sdu_free_sub_fragments(sdu);
                }
            }
        }
    }
    return 0;
}


// return true if need update
kbool sg_sdd_chk_fragment_for_update(sg_sdd *sdd, Fragment_rec *frag_rec)
{
    kint i, j, k;
    sg_root *rt = sdd->rt;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0;

    sg_sdu *sdu;

    if ((!sdd->arr) || (!sdd->arr->DescriptorEntry)) {
        return ktrue;
    }

    de_cnt = sdd->arr->DescriptorEntry->ecnt;
    de_arr = sdd->arr->DescriptorEntry->arr;

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;

        for (j = 0; j < sgdu_cnt; j++) {
            if (!sgdu_arr[j].Fragment) {
                continue;
            }
            sdu = sg_root_get_sdu_by_id(rt, sgdu_arr[i].attr.transportObjectID);
            if (sdu && kflg_chk(sdu->flg, SDUF_PARSED)) {
                return ktrue;
            }
            frag_cnt = sgdu_arr[j].Fragment->ecnt;
            frag_arr = sgdu_arr[j].Fragment->arr;
            for (k = 0; k < frag_cnt; k++) {
                if (frag_rec->attr.transportID == frag_arr[k].attr.transportID) {
                    if (frag_rec->attr.version !=  frag_arr[k].attr.version) {
                        return ktrue;
                    } else {
                        return kfalse;
                    }
                }
            }
        }
    }
    return ktrue;
}

// cover the sdd with new_ver_sdd's data part
kint sg_sdd_chk_update(sg_sdd *old_sdd, sg_sdd *sdd)
{
    kint i, j, k;
    sg_root *rt = old_sdd->rt;
    sg_ses *sgduses;
    // sg_sdd *old_sdd;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0, shift;

    K_freq *fblk = sg_root_freq_blk_find(rt, sdd->frequency);

    kuchar sdu_arr[100];
    kint sdu_cnt;

    //xxx_Remove not used SDGU file
    if (!sdd->arr || !sdd->arr->DescriptorEntry) {
        return -1;
    }
#if 0
    // find the saved / old SGDD
    hdr = &rt->fhdrs[SDD];
    entry = hdr->next;
    while (entry != hdr) {
        old_sdd = FIELD_TO_STRUCTURE(entry, sg_sdd, cls_le);

        if ((sdd != old_sdd) && streq(sdd->attr.id, old_sdd->attr.id)) {
            remove_dlist_entry(&old_sdd->cls_le);
            remove_dlist_entry(&old_sdd->all_le);
            // sg_sdd_del(old_sdd, ktrue);
            break;
        }

        entry = entry->next;
    }
    if (entry == hdr) {
        old_sdd = knil;
    }
#endif

    de_cnt = sdd->arr->DescriptorEntry->ecnt;
    de_arr = sdd->arr->DescriptorEntry->arr;
    klog(("SGDD PARSE: DescriptorEntry num = %d\n",de_cnt));

    //for rescan
    if (kflg_chk(rt->flg, ERF_SCAN)) {
        rt->mgr->scan->scan.stat.all = rt->mgr->scan->scan.stat.cur=0;
    }

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        if (!de_arr[i].Transport) {
            continue;
        }
        sdu_cnt = 0;
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;
        klog(("SGDD PARSE: DescriptorEntry %d ,SGDU_cnt=%d\n",i,sgdu_cnt));

        for (j = 0; j < sgdu_cnt; j++) {
            // ȷ���Ƿ�����ˣ������ڸ����������ո�SGDU
            // Ҳ���ǰ��������ŵ�sgu_arr��

            if (!sgdu_arr[j].Fragment) {
                continue;
            }

            frag_cnt = sgdu_arr[j].Fragment->ecnt;
            frag_arr = sgdu_arr[j].Fragment->arr;

            if (kflg_chk(rt->flg, ERF_SCAN)) {
                rt->mgr->scan->scan.stat.all += frag_cnt;
            }

            if (!old_sdd) {
                sdu_arr[sdu_cnt++] = j;
                continue;
            }

            for (k = 0; k < frag_cnt; k++) {
                if (sg_sdd_chk_fragment_for_update(old_sdd, &frag_arr[k])) {
                    sdu_arr[sdu_cnt++] = j;
                    break;
                }
            }
        }

        klog(("SGDD PARSE: DescriptorEntry %d 's %d  SGDU changed\n",i,sdu_cnt));
        if (sdu_cnt) {
            kint objhandle,    seshandle;

            // issue command
            tp_arr = de_arr[i].Transport->arr;
            tp_cnt = de_arr[i].Transport->ecnt;

            //////////////////////////////////////////////////////////////////////////////

            if (SG_TMODE_FLUTE == fblk->acc->tmode[fblk->acc->tindex]) {
                if (rt->pvdr->protocol != SG_PROT_DAB) {
                    IV_FLUTE_ALC_SESSION_ENTRYINFO *alc = (IV_FLUTE_ALC_SESSION_ENTRYINFO*)kmem_alloz(sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO));
                    alc->uiTSI = de_arr[i].Transport->arr->attr.transmissionSessionID;

                    alc->iSSM_flag = 0;
                    alc->iAlcChannelNum = 1;
                    alc->pAlcChannelHead[0].u16TargetPort = de_arr[i].Transport->arr->attr.port;

                    if (strstr(de_arr[i].Transport->arr->attr.ipAddress, ".")) {
                        alc->pAlcChannelHead[0].target_address.iIPv4_flag = ktrue;
                        ipv4_scan(de_arr[i].Transport->arr->attr.ipAddress, (kuchar*)alc->pAlcChannelHead[0].target_address.ubIP);
                    } else {
                        sgduses->sei.alc->pAlcChannelHead[0].target_address.iIPv4_flag = kfalse;
                        ipv6_scan(de_arr[i].Transport->arr->attr.ipAddress, (kuchar*)alc->pAlcChannelHead[0].target_address.ubIP);
                    }

                    sgduses = knil;
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_FLUTEALC, alc);
                    kflg_set(sgduses->flg, SES_AUTO_STOP | SES_AUTO_DEL);
                    klog(("SGDD PARSE:DescriptorEntry %d 's  session para ok\n",i));
                }
            } else if (SG_TMODE_MOT == fblk->acc->tmode[fblk->acc->tindex]) {
                if (rt->pvdr->protocol == SG_PROT_DAB) {
                    //xxx_How to fill the followed field
                    IV_DAB_SESSION_ENTRYINFO *mot = (IV_DAB_SESSION_ENTRYINFO*)ksal_mem_alloc(sizeof(IV_DAB_SESSION_ENTRYINFO));
                    mot->eID = de_arr[i].Transport->arr->attr.eID;
                    mot->mode = de_arr[i].Transport->arr->attr.TransportMode;
                    mot->serviceComponentID = de_arr[i].Transport->arr->ServiceComponentID->arr->val.ServiceComponentID;

                    sgduses = knil;
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_MOT, mot);

                    kflg_set(sgduses->flg, SES_AUTO_STOP | SES_AUTO_DEL);
                }
            } else if (SG_TMODE_BDIR == fblk->acc->tmode[fblk->acc->tindex]) {
                sgduses = knil;
                if (1) {
                    kchar url[1024];
                    if (strstr(rt->freq.current->acc->bdir->url, "?")) {
                        sprintf(url, "%s&NID=%s&TSID=%s&", rt->freq.current->acc->bdir->url,
                                rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
                    } else {
                        sprintf(url, "%s?NID=%s&TSID=%s&", rt->freq.current->acc->bdir->url,
                                rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
                    }
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_BDIR, url);
                }
            } else {
                kerror(("Unknown file tmode: %d\n", fblk->acc->tmode[fblk->acc->tindex]));
                kassert(!"Unknown file tmode");
            }


            if(sg_ses_start_session(sgduses, kfalse)){
                sg_ses_del(sgduses);
                klog(("sg_sdd_chk_update: sg_ses_start_session ses=%x,failed\n",sgduses));
                continue;
            }

            // not root session, so queue to subhdr
            insert_dlist_tail_entry(&rt->freq.current->data_ses.subhdr, &sgduses->le);

            //////////////////////////////////////////////////////////////////////////////

            // versionIDLength
            if (rt->pvdr->protocol != SG_PROT_DAB) {
                if (tp_arr[0].attr.versionIDLength >= 0) {
                    shift = tp_arr[0].attr.versionIDLength;
                }
            }
            if (rt->pvdr->protocol == SG_PROT_DAB) {
                if (tp_arr[0].attr.versionIDLength >= 0) {
                    shift = tp_arr[0].attr.versionIDLength;
                }
            }
            // Only receive the TOI list int sgdu_arr[sdu_arr[k]].
            klog(("SGDD PARSE: session check ok try to send cmd to receive the  SGDU changed\n"));
            for (k = 0; k < sdu_cnt; k++) {
                if (SG_TMODE_MOT == fblk->acc->tmode[fblk->acc->tindex]) {
                    if (!sg_ses_get_obj(sgduses, 8000000, 0, 0xFFFFFFFF << shift, (kvoid*)sgducbk,
                                sgdu_arr[sdu_arr[k]].attr.contentLocation, rt, sgduses, old_sdd, knil)) {
                        fblk->stat.sdu_set++;
                    }
                } else {
                    klog(("SGDD PARSE: SGDU to received num(sdu_set) %d\n",fblk->stat.sdu_set));
                    if (!sg_ses_get_obj(sgduses, 9000000, sgdu_arr[sdu_arr[k]].attr.transportObjectID,
                                0xFFFFFFFF  << shift, (kvoid*)sgducbk, knil, rt, sgduses, old_sdd, knil)) {
                        fblk->stat.sdu_set++;
                    }
                }
            }
        } else {
            if (kflg_chk(rt->flg, ERF_SCAN)) {
                rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SERVICE_UPDATE, knil, knil, knil, knil);
            }
        }
    }
#if 0
    if (old_sdd) {
        sg_sdd_del(old_sdd, ktrue);
    }
#endif
    old_sdd->ecnt = sdd->ecnt;
    old_sdd->arr = sdd->arr;
    sdd->ecnt = 0;
    sdd->arr = knil;
    klog(("SGDD PARSE: end\n"));
    return 0;
}

Fragment_rec *sg_sdd_get_frag_by_tid(sg_sdd *sdd, kuint tid)
{
    kint i, j, k;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0, shift;

    if (!sdd->arr->DescriptorEntry) {
        return knil;
    }

    de_cnt = sdd->arr->DescriptorEntry->ecnt;
    de_arr = sdd->arr->DescriptorEntry->arr;

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;

        for (j = 0; j < sgdu_cnt; j++) {
            if (!sgdu_arr[j].Fragment) {
                continue;
            }
            frag_cnt = sgdu_arr[j].Fragment->ecnt;
            frag_arr = sgdu_arr[j].Fragment->arr;
            for (k = 0; k < frag_cnt; k++) {
                if (tid == frag_arr[k].attr.transportID) {
                    return &frag_arr[k];
                }
            }
        }
    }
    return knil;
}

/**
 * @brief return count to receive
 */
kint sg_sdd_try_receive_lost_file(sg_sdd *sdd)
{
    kint i, j, k;
    kint objhandle,    seshandle;

    sg_root *rt = sdd->rt;
    sg_ses *sgduses;
    sg_sdu *sdu;

    DescriptorEntry_rec *de_arr;
    kint de_cnt = 0;
    Transport_rec *tp_arr;
    kint tp_cnt = 0;
    ServiceGuideDeliveryUnit_rec *sgdu_arr;
    kint sgdu_cnt = 0;
    Fragment_rec *frag_arr;
    kint frag_cnt = 0, shift;

    kuchar sdu_arr[100];
    kint sdu_cnt;

    K_freq *fblk = rt->freq.current;

    if (!sdd || !sdd->arr || !sdd->arr->DescriptorEntry) {
        kerror(("sg_sdd_try_receive_lost_file: not content\n"));
        return 0;
    }

    de_cnt = sdd->arr->DescriptorEntry->ecnt;
    de_arr = sdd->arr->DescriptorEntry->arr;

    if (kflg_chk(rt->flg, ERF_SCAN)) {
        rt->mgr->scan->scan.stat.all = rt->mgr->scan->scan.stat.cur=0;
    }

    for (i = 0; i < de_cnt; i++) {
        if (!de_arr[i].ServiceGuideDeliveryUnit) {
            continue;
        }
        sdu_cnt = 0;
        sgdu_cnt = de_arr[i].ServiceGuideDeliveryUnit->ecnt;
        sgdu_arr = de_arr[i].ServiceGuideDeliveryUnit->arr;

        for (j = 0; j < sgdu_cnt; j++) {
            if (!sgdu_arr[j].Fragment) {
                continue;
            }
            sdu = sg_root_get_sdu_by_id(rt, sgdu_arr[j].attr.transportObjectID);
            if (sdu && kflg_chk(sdu->flg, SDUF_PARSED)) {
                klog(("\nsg_sdd_try_receive_lost_file: SGDU already gotten: TOI%d, loc:%s\n\n",
                            sgdu_arr[j].attr.transportObjectID,
                            sgdu_arr[j].attr.contentLocation));

                /* for scan */
                /*
                 * XXX when BF mode, the F will inc stat.xxx, that will make the return fragcnt
                 * is doubled
                 */

                if (kflg_chk(rt->flg, ERF_SCAN)) {
                    rt->mgr->scan->scan.stat.all += sgdu_arr[j].Fragment->ecnt;
                    rt->mgr->scan->scan.stat.cur += sgdu_arr[j].Fragment->ecnt;

                    klog(("recved:%d, found:%d\n", rt->mgr->scan->scan.stat.cur, rt->mgr->scan->scan.stat.all));
                    rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)IVEC_INPROGRESS,
                            (kvoid*)rt->mgr->scan->scan.stat.all, (kvoid*)rt->mgr->scan->scan.stat.cur, (kvoid*)rt->mgr->scan->scan.userdata);
                }


                continue;
            }
            sdu_arr[sdu_cnt++] = j;
        }

        if (sdu_cnt) {
            klog(("!!!%d SGDU lost, try to got!!!,first toi %u\n",sdu_cnt,sgdu_arr[sdu_arr[0]].attr.transportObjectID));
            // issue command
            tp_arr = de_arr[i].Transport->arr;
            tp_cnt = de_arr[i].Transport->ecnt;

            //////////////////////////////////////////////////////////////////////////////

            if (SG_TMODE_FLUTE == fblk->acc->tmode[fblk->acc->tindex]) {
                if (rt->pvdr->protocol != SG_PROT_DAB) {
                    IV_FLUTE_ALC_SESSION_ENTRYINFO *alc = (IV_FLUTE_ALC_SESSION_ENTRYINFO*)kmem_alloz(sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO));
                    alc->uiTSI = de_arr[i].Transport->arr->attr.transmissionSessionID;

                    alc->iSSM_flag = 0;
                    alc->iAlcChannelNum = 1;
                    alc->pAlcChannelHead[0].u16TargetPort = de_arr[i].Transport->arr->attr.port;

                    if (strstr(de_arr[i].Transport->arr->attr.ipAddress, ".")) {
                        alc->pAlcChannelHead[0].target_address.iIPv4_flag = ktrue;
                        ipv4_scan(de_arr[i].Transport->arr->attr.ipAddress, (kuchar*)alc->pAlcChannelHead[0].target_address.ubIP);
                    } else {
                        sgduses->sei.alc->pAlcChannelHead[0].target_address.iIPv4_flag = kfalse;
                        ipv6_scan(de_arr[i].Transport->arr->attr.ipAddress, (kuchar*)alc->pAlcChannelHead[0].target_address.ubIP);
                    }

                    sgduses = knil;
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_FLUTEALC, alc);
                    kflg_set(sgduses->flg, SES_AUTO_STOP | SES_AUTO_DEL);
                }
            } else if (SG_TMODE_MOT == fblk->acc->tmode[fblk->acc->tindex]) {
                if (rt->pvdr->protocol == SG_PROT_DAB) {
                    //xxx_How to fill the followed field
                    IV_DAB_SESSION_ENTRYINFO *mot = (IV_DAB_SESSION_ENTRYINFO*)ksal_mem_alloc(sizeof(IV_DAB_SESSION_ENTRYINFO));
                    mot->eID = de_arr[i].Transport->arr->attr.eID;
                    mot->mode = de_arr[i].Transport->arr->attr.TransportMode;
                    mot->serviceComponentID = de_arr[i].Transport->arr->ServiceComponentID->arr->val.ServiceComponentID;

                    sgduses = knil;
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_MOT, mot);

                    kflg_set(sgduses->flg, SES_AUTO_STOP | SES_AUTO_DEL);
                }
            } else if (SG_TMODE_BDIR == fblk->acc->tmode[fblk->acc->tindex]) {
                sgduses = knil;
                if (1) {
                    kchar url[1024];
                    if (strstr(rt->freq.current->acc->bdir->url, "?")) {
                        sprintf(url, "%s&NID=%s&TSID=%s&", rt->freq.current->acc->bdir->url,
                                rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
                    } else {
                        sprintf(url, "%s?NID=%s&TSID=%s&", rt->freq.current->acc->bdir->url,
                                rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
                    }
                    sg_ses_new(&sgduses, IV_FILEREV_PROTOCOL_BDIR, url);
                }
            } else {
                kerror(("Unknown file tmode: %d\n", fblk->acc->tmode[fblk->acc->tindex]));
                kassert(!"Unknown file tmode");
            }

            if(sg_ses_start_session(sgduses, kfalse)){
                sg_ses_del(sgduses);
                klog(("sg_sdd_chk_update: sg_ses_start_session ses=%x,failed\n",sgduses));
                continue;
            }

            // not root session, so queue to subhdr
            insert_dlist_tail_entry(&rt->freq.current->data_ses.subhdr, &sgduses->le);

            //////////////////////////////////////////////////////////////////////////////

            // versionIDLength
            if (rt->pvdr->protocol != SG_PROT_DAB) {
                if (tp_arr[0].attr.versionIDLength >= 0) {
                    shift = tp_arr[0].attr.versionIDLength;
                }
            }
            if (rt->pvdr->protocol != SG_PROT_DAB) {
                if (tp_arr[0].attr.versionIDLength >= 0) {
                    shift = tp_arr[0].attr.versionIDLength;
                }
            }
            for (k = 0; k < frag_cnt; k++) {
                klog(("\nsg_sdd_try_receive_lost_file: SGDU should be get : TOI:%d, loc:%s\n\n",
                            sgdu_arr[sdu_arr[k]].attr.transportObjectID,
                            sgdu_arr[sdu_arr[k]].attr.contentLocation));
            }

            // Only receive the TOI list int sgdu_arr[sdu_arr[k]].
            for (k = 0; k < sdu_cnt; k++) {
                if (SG_TMODE_MOT == fblk->acc->tmode[fblk->acc->tindex]) {
                    if (!sg_ses_get_obj(sgduses, 9600000, 0, 0xFFFFFFFF << shift, (kvoid*)sgducbk,
                                sgdu_arr[sdu_arr[k]].attr.contentLocation, rt, sgduses, sdd, knil)) {
                        rt->freq.current->stat.sdu_set++;
                    }
                } else {
                    if (!sg_ses_get_obj(sgduses, 9500000, sgdu_arr[sdu_arr[k]].attr.transportObjectID,
                                0xFFFFFFFF  << shift, (kvoid*)sgducbk, knil, rt, sgduses, sdd, knil)) {
                        rt->freq.current->stat.sdu_set++;
                    }
                }
            }
        }
    }

    return 0;
}

