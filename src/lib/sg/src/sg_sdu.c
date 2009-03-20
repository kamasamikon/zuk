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

#include "xmldoc.h"
#include "sg_root.h"
#include "sg_mgr2.h"

#include "sg_sdu.h"

#include "sg_zbd.h"

#include "sg_ser.h"
#include "sg_ctt.h"
#include "sg_sch.h"
#include "sg_acc.h"
#include "sg_pci.h"
#include "sg_pcd.h"
#include "sg_pcc.h"
#include "sg_pvd.h"
#include "sg_iad.h"
#include "sg_sdp.h"

kint sg_sdu_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_sdu *sdu = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, SDU, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_sdu_new(&sdu);
    sdu->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)sdu);
    kflg_set(sdu->flg, EFF_LOADING);

    FREE_ORG_DAT(sdu);

    insert_dlist_tail_entry(&root->allhdr, &sdu->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SDU], &sdu->cls_le);
    kflg_clr(sdu->flg, EFF_LOADING);
    return 0;
}

kint sg_sdu_new(sg_sdu **sdu)
{
    if (!(*sdu)) {
        (*sdu) = (sg_sdu*)kmem_alloc(sizeof(sg_sdu));
    }
    if ((*sdu)) {
        memset((*sdu), 0, sizeof(sg_sdu));
        INIT_COMMON((*sdu));
        (*sdu)->cls = CLS_SGDU;
        return 0;
    }
    return -1;
}

kint sg_sdu_free_sub_fragments(sg_sdu *sdu)
{
    sg_zbd *zbd;
    kchar *ob;
    kuint ol, fragcnt, ftid, i;

    if ((!sdu) || (!sdu->dat.buf) || (!sdu->dat.len)) {
        return -1;
    }

    ob = sdu->dat.buf;
    ol = sdu->dat.len;

    fragcnt = sgdu_hdr_get_n_o_service_guide_fragments(ob);

    for (i = 0; i < fragcnt; i++) {
        ftid  = sgdu_hdr_get_fragmentTransportID(ob, i);

        if (zbd = sg_root_find_file(sdu->rt, -1, &ftid, knil)) {
            sg_delete_file(zbd, ktrue);
        }
    }
    return 0;
}

kint sg_sdu_del(sg_sdu *sdu, kbool rmcache)
{
    if (!sdu) {
        return -1;
    }
    remove_dlist_entry(&sdu->all_le);
    remove_dlist_entry(&sdu->cls_le);

    kassert(CLS_SGDU == sdu->cls);

    if (rmcache) {
        sg_root_rm_cache_file(sdu);
    }

    if (sdu->dat.buf) {
        kmem_free(sdu->dat.buf);
    }

    kmem_free(sdu);
    return 0;
}

kint sg_sdu_add_cache(sg_sdu *sdu)
{
    // save the content of the file to disk etc
    if (kflg_chk(sdu->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(sdu);
    return 0;
}
kint sg_sdu_del_cache(sg_sdu *sdu)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_sdu_ld_cache(sg_sdu *sdu)
{
    // save the content of the file to disk etc
    kflg_clr(sdu->flg, EFF_LOADING);
    return 0;
}
kint sg_sdu_x2s(sg_sdu *sdu)
{
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!sdu) {
        kerror(("No sdu input...\n"));
        return -1;
    }
    if ((!sdu->dat.buf) || (0 == sdu->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_sdu_add_cache(sdu);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, sdu->dat.buf, sdu->dat.len);

    if (!(node = xmldoc_gotoNode(doc, "RROOOOTT", 0))) {
        return -1;
    }

    kflg_set(sdu->flg, EFF_PROCOK);    // indicate the data not been processed
    return 0;
}

// peek the buffer from the queue, and parse the sgdu
// call this in the task
//
// a, parse sgdu
// b, generate fragment file
// c, parse the file
// d, will cost bunch of time
static kint __g_fragment_cnt = 0;
static kint __g_fragment_sdp_cnt = 0;
sg_zbd *sg_root_find_file(sg_root *root, kint cls, kuint *ftid, kchar *id);
kint sg_sdu_parse(sg_sdu *sdu)
{
    sg_root *rt = sdu->rt;

    kuint curtime = ksys_ntp_time() + sdu->rt->mgr->env->time_diff;

    sg_sdd *sdd;

    kuint i, cnt, id, ofs, hdrlen;
    K_dlist_entry *entry, *hdr;

    kchar *ob = sdu->dat.buf, *pladdr, *fragaddr, *fileaddr, *fragid;
    kuint ol = sdu->dat.len, fragcnt, extofs, fragofs, validFrom, validTo, ftid, fver;
    kuchar fragenc;

    K_freq *fblk = sg_root_freq_blk_find(rt, sdu->frequency);

    kvoid *sdu_hdr;

    sg_zbd *zbd;
    sg_ser *ser;
    sg_ctt *ctt;
    sg_sch *sch;
    sg_acc *acc;
    sg_pci *pci;
    sg_pcd *pcd;
    sg_pcc *pcc;
    sg_pvd *pvd;
    sg_iad *iad;
    sg_sdp *sdp;

    kint new_ser_cnt = 0;
    kint new_ctt_cnt = 0;
    kint new_sch_cnt = 0;
    kint new_acc_cnt = 0;
    kint new_pci_cnt = 0;
    kint new_pcd_cnt = 0;
    kint new_pcc_cnt = 0;
    kint new_pvd_cnt = 0;
    kint new_iad_cnt = 0;

    hdr = &fblk->fhdrs[SDD];
    entry = hdr->next;
    while (entry != hdr) {
        sdd = FIELD_TO_STRUCTURE(entry, sg_sdd, cls_le);

        if (sdd->tid == sdu->sdd_tid) {
            break;
        }

        entry = entry->next;
    }

    if (entry == hdr) {
        kassert(!"Can not found the SGDD");
        fblk->stat.sdu_parsed++;
        return -1;
    }

    klog(("sg_sdu_parse:%d\n", sdu->tid));

    if (kflg_chk(rt->flg, ERF_SCAN) && (0 == fblk->stat.sdu_parsed)) {
        if (!kflg_chk(rt->pvdr->flg, BSF_VALID)) {
            sg_mgr_single_provider_save(rt->mgr, rt->pvdr, kfalse);
        }
        sg_mgr_single_provider_freq_save(rt->mgr, rt->pvdr, rt->freq.current->acc);
    }

    extofs = sgdu_hdr_get_extension_offset(ob);
    fragcnt = sgdu_hdr_get_n_o_service_guide_fragments(ob);
    pladdr = sgdu_payload_get_start_addr(ob);

    __g_fragment_cnt += fragcnt;
    klog(("total fragment cnt : %d\n", __g_fragment_cnt));
    //klog(("hdr_get_extension_offset:%x\n", extofs));
    klog(("hdr_get_n_o_service_guide_fragments:%x\n", fragcnt));
    for (i = 0; i < fragcnt; i++) {
        ftid  = sgdu_hdr_get_fragmentTransportID(ob, i);
        //klog(("hdr_get_fragmentTransportID:%x\n", ftid));
        fver = sgdu_hdr_get_fragmentVersion(ob, i);
        //klog(("hdr_get_fragmentVersion:%x\n", fver));

        fragofs = sgdu_hdr_get_fragmentOffset(ob, i);

        //klog(("hdr_get_fragmentOffset:%x\n", fragofs + pladdr));

        // (fragofs + pladdr) is the real memory address for the ith fragment
        fragenc = sgdu_payload_get_fragmentEncoding(fragofs + pladdr);

        //klog(("fragenc fragenc : %d\n", fragenc));
        switch (fragenc) {
            case 0:
                // this is XML
                fileaddr = sgdu_payload_get_0_XMLFragment(fragofs + pladdr);
                switch (sgdu_payload_get_0_fragmentType(fragofs + pladdr)) {
                    case 0:        // undef
                        kerror(("sg_sdu_parse: fragmentType is zero for XML file!\n"));
                        break;
                    case 1:        // Service
                        ser = knil;
                        if (zbd = sg_root_find_file(rt, SER, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                        }
                        sg_ser_new(&ser);
                        if (zbd) {
                            ser->cur_idir = ((sg_ser*)zbd)->cur_idir;
                            ser->iad_probe_tmr = ((sg_ser*)zbd)->iad_probe_tmr;
                            sg_delete_file(zbd, ktrue);
                        }
                        ser->frequency = sdu->frequency;
                        ser->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &ser->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[SER], &ser->cls_le);
                        ser->rt = rt;
                        ser->frequency = sdu->frequency;

                        ser->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            ser->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            ser->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_ser_x2s(ser)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, ser->cls, curtime, ser->attr.validFrom, ser->attr.validTo));
                            sg_ser_del(ser, ktrue);
                            break;
                        }
                        new_ser_cnt++;
                        kflg_set(ser->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(ser->flg, EFF_INDICATE);
                        }
                        break;
                    case 2:        // Content
                        ctt = knil;
                        if (zbd = sg_root_find_file(rt, CTT, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_ctt_new(&ctt);
                        ctt->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &ctt->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[CTT], &ctt->cls_le);
                        ctt->rt = rt;
                        ctt->frequency = sdu->frequency;

                        ctt->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            ctt->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            ctt->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_ctt_x2s(ctt)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, ctt->cls, curtime, ctt->attr.validFrom, ctt->attr.validTo));
                            sg_ctt_del(ctt, ktrue);
                            break;
                        }
                        new_ctt_cnt++;
                        kflg_set(ctt->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(ctt->flg, EFF_INDICATE);
                        }
                        break;
                    case 3:        // Schedule
                        sch = knil;
                        if (zbd = sg_root_find_file(rt, SCH, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        klog(("SCH ftid:%d, %x\n", ftid, ftid));
                        sg_sch_new(&sch);
                        sch->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &sch->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[SCH], &sch->cls_le);
                        sch->rt = rt;
                        sch->frequency = sdu->frequency;

                        sch->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            sch->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            sch->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_sch_x2s(sch)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, sch->cls, curtime, sch->attr.validFrom, sch->attr.validTo));
                            sg_sch_del(sch, ktrue);
                            break;
                        }
                        new_sch_cnt++;
                        kflg_set(sch->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(sch->flg, EFF_INDICATE);
                        }
                        break;
                    case 4:        // Access
                        acc = knil;
                        if (zbd = sg_root_find_file(rt, ACC, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_acc_new(&acc);
                        acc->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &acc->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[ACC], &acc->cls_le);
                        acc->rt = rt;
                        acc->frequency = sdu->frequency;

                        acc->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            acc->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            acc->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_acc_x2s(acc)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, acc->cls, curtime, acc->attr.validFrom, acc->attr.validTo));
                            sg_acc_del(acc, ktrue);
                            break;
                        }
                        new_acc_cnt++;
                        kflg_set(acc->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(acc->flg, EFF_INDICATE);
                        }
                        break;
                    case 5:        // PurchaseItem
                        pci = knil;
                        if (zbd = sg_root_find_file(rt, PCI, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_pci_new(&pci);
                        pci->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &pci->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[PCI], &pci->cls_le);
                        pci->rt = rt;
                        pci->frequency = sdu->frequency;

                        pci->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            pci->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            pci->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_pci_x2s(pci)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, pci->cls, curtime, pci->attr.validFrom, pci->attr.validTo));
                            sg_pci_del(pci, ktrue);
                            break;
                        }
                        new_pci_cnt++;
                        kflg_set(pci->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(pci->flg, EFF_INDICATE);
                        }
                        break;
                    case 6:        // PurchaseData
                        pcd = knil;
                        if (zbd = sg_root_find_file(rt, PCD, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_pcd_new(&pcd);
                        pcd->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &pcd->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[PCD], &pcd->cls_le);
                        pcd->rt = rt;
                        pcd->frequency = sdu->frequency;

                        pcd->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            pcd->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            pcd->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_pcd_x2s(pcd)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, pcd->cls, curtime, pcd->attr.validFrom, pcd->attr.validTo));
                            sg_pcd_del(pcd, ktrue);
                            break;
                        }
                        new_pcd_cnt++;
                        kflg_set(pcd->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(pcd->flg, EFF_INDICATE);
                        }
                        break;
                    case 7:        // PurchaseChannel
                        pcc = knil;
                        if (zbd = sg_root_find_file(rt, PCC, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_pcc_new(&pcc);
                        pcc->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &pcc->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[PCC], &pcc->cls_le);
                        pcc->rt = rt;
                        pcc->frequency = sdu->frequency;

                        pcc->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            pcc->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            pcc->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_pcc_x2s(pcc)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, pcc->cls, curtime, pcc->attr.validFrom, pcc->attr.validTo));
                            sg_pcc_del(pcc, ktrue);
                            break;
                        }
                        new_pcc_cnt++;
                        kflg_set(pcc->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(pcc->flg, EFF_INDICATE);
                        }
                        break;
                    case 8:        // PreviewData
                        pvd = knil;
                        if (zbd = sg_root_find_file(rt, PVD, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_pvd_new(&pvd);
                        pvd->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &pvd->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[PVD], &pvd->cls_le);
                        pvd->rt = rt;
                        pvd->frequency = sdu->frequency;

                        pvd->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            pvd->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            pvd->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_pvd_x2s(pvd)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, pvd->cls, curtime, pvd->attr.validFrom, pvd->attr.validTo));
                            sg_pvd_del(pvd, ktrue);
                            break;
                        }
                        new_pvd_cnt++;
                        kflg_set(pvd->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(pvd->flg, EFF_INDICATE);
                        }
                        break;
                    case 9:        // InteractivityData
                        iad = knil;
                        if (zbd = sg_root_find_file(rt, IAD, &ftid, knil)) {
                            if (zbd->attr.version == fver) {
                                klog(("same frag exist: %d\n", ftid));
                                break;
                            }
                            sg_delete_file(zbd, ktrue);
                        }
                        sg_iad_new(&iad);
                        iad->tid = ftid;
                        insert_dlist_tail_entry(&rt->allhdr, &iad->all_le);
                        insert_dlist_tail_entry(&fblk->fhdrs[IAD], &iad->cls_le);
                        iad->rt = rt;
                        iad->frequency = sdu->frequency;


                        iad->dat.buf = fileaddr;
                        if (i == (fragcnt - 1)) {
                            iad->dat.len = ol - (fileaddr - ob) - sgdu_payload_XML_hdrlen;
                        } else {
                            iad->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - sgdu_payload_XML_hdrlen;
                        }
                        if (-2 == sg_iad_x2s(iad)) {
                            klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                        ftid, iad->cls, curtime, iad->attr.validFrom, iad->attr.validTo));
                            sg_iad_del(iad, ktrue);
                            break;
                        }
                        new_iad_cnt++;
                        kflg_set(iad->flg, EFF_PROCOK);
                        if (!kflg_chk(rt->flg, ERF_SCAN)) {
                            kflg_set(iad->flg, EFF_INDICATE);
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 1:
                // this is SDP
                fileaddr = sgdu_payload_get_1_SDPfragment(fragofs + pladdr);
                validFrom = sgdu_payload_get_1_validFrom(fragofs + pladdr);
                validTo = sgdu_payload_get_1_validTo(fragofs + pladdr);
                if (validTo && (validTo <= curtime + 2)) {
                    kerror(("sg_sdu_parse: sdp ood\n"));
                    break;
                }
                fragid = sgdu_payload_get_1_fragmentID(fragofs + pladdr);
                __g_fragment_sdp_cnt++;
                klog(("sdp file count : %d\n", __g_fragment_sdp_cnt));
                sdp = knil;
                if (zbd = sg_root_find_file(rt, SDP, &ftid, knil)) {
                    if (zbd->attr.version == fver) {
                        klog(("same frag exist: %d\n", ftid));
                        break;
                    }
                    sg_delete_file(zbd, ktrue);
                }
                sg_sdp_new(&sdp);
                sdp->tid = ftid;
                if (1) {
                    Fragment_rec *rec = sg_sdd_get_frag_by_tid(sdd, ftid);
                    if (rec) {
                        STR_SET(sdp->attr.id, rec->attr.id);
                    }
                }
                insert_dlist_tail_entry(&rt->allhdr, &sdp->all_le);
                insert_dlist_tail_entry(&fblk->fhdrs[SDP], &sdp->cls_le);
                sdp->rt = rt;
                sdp->frequency = sdu->frequency;

                sdp->dat.buf = fileaddr;
                if (i == (fragcnt - 1)) {
                    sdp->dat.len = ol - (fileaddr - ob) - strlen(sgdu_payload_get_1_fragmentID(fragofs + pladdr)) - 10;
                } else {
                    sdp->dat.len = sgdu_hdr_get_fragmentOffset(ob, i + 1) - fragofs - strlen(sgdu_payload_get_1_fragmentID(fragofs + pladdr)) - 10;
                }

                if (-2 == sg_sdp_x2s(sdp)) {
                    klog(("Out of Date, delete (tid:%u, cls:%x, ct:%u, st:%u, et:%u)\n",
                                ftid, sdp->cls, curtime, sdp->attr.validFrom, sdp->attr.validTo));
                    sg_sdp_del(sdp, ktrue);
                }
                kflg_set(sdp->flg, EFF_PROCOK);
                break;
            case 2:
                // this is USBD
#if 0    //
                fileaddr = sgdu_payload_get_2_USBDfragment(fragofs + pladdr);
                validFrom = sgdu_payload_get_2_validFrom(fragofs + pladdr);
                validTo = sgdu_payload_get_2_validTo(fragofs + pladdr);
                fragid = sgdu_payload_get_2_fragmentID(fragofs + pladdr);
#endif
                break;
            case 3:
                // this is ADP
#if 0    //
                fileaddr = sgdu_payload_get_3_ADPfragment(fragofs + pladdr);
                validFrom = sgdu_payload_get_3_validFrom(fragofs + pladdr);
                validTo = sgdu_payload_get_3_validTo(fragofs + pladdr);
                fragid = sgdu_payload_get_3_fragmentID(fragofs + pladdr);
#endif
                break;
            default:
                break;
        }
    }
    kflg_clr(sdu->flg, EFF_DATAOK);
    kflg_set(sdu->flg, EFF_PROCOK);

    // Free the unused part of SGDU, only need to save the index/header part
    sdu_hdr = ksal_mem_alloc(pladdr - ob);
    memcpy(sdu_hdr, ob, pladdr - ob);
    ksal_mem_free(sdu->dat.buf);
    sdu->dat.buf = sdu_hdr;
    sdu->dat.len = pladdr - ob;

    sg_sdu_add_cache(sdu);
    kflg_set(sdu->flg, SDUF_PARSED);

    fblk->stat.sdu_parsed++;

    fblk->stat.new_ser_cnt += new_ser_cnt;
    fblk->stat.new_ctt_cnt += new_ctt_cnt;
    fblk->stat.new_sch_cnt += new_sch_cnt;
    fblk->stat.new_acc_cnt += new_acc_cnt;
    fblk->stat.new_pci_cnt += new_pci_cnt;
    fblk->stat.new_pcd_cnt += new_pcd_cnt;
    fblk->stat.new_pcc_cnt += new_pcc_cnt;
    fblk->stat.new_pvd_cnt += new_pvd_cnt;
    fblk->stat.new_iad_cnt += new_iad_cnt;

    // FIXME: what if the transsion interrupted when channel change or scan abort?
    if (fblk->stat.sdu_set == fblk->stat.sdu_parsed) {
        klog(("sdu_set=sdu_parsed= %d:so,rev requirment sent successly SG file revcevd and ana finished. So forced sg scan end\n",fblk->stat.sdu_set));

        if (!kflg_chk(rt->flg, ERF_SCAN)) {
            if (fblk->stat.new_sch_cnt) {
                sg_start_iad_search_for_frequency(rt, fblk->acc->frequency, ktrue);
            }
            if (fblk->stat.new_pvd_cnt) {
                klog(("%d PreviewData update\n", fblk->stat.new_pvd_cnt));
            }
            if (fblk->stat.new_iad_cnt) {
                //klog(("%d InteractivityData update\n", fblk->stat.new_iad_cnt));
                //sg_start_iad_search_for_frequency(rt, fblk->acc->frequency, ktrue);
            }
        }

        // reset the statistics variables
        fblk->stat.new_ser_cnt = 0;
        fblk->stat.new_ctt_cnt = 0;
        fblk->stat.new_sch_cnt = 0;
        fblk->stat.new_acc_cnt = 0;
        fblk->stat.new_pci_cnt = 0;
        fblk->stat.new_pcd_cnt = 0;
        fblk->stat.new_pcc_cnt = 0;
        fblk->stat.new_pvd_cnt = 0;
        fblk->stat.new_iad_cnt = 0;

        fblk->stat.sdu_parsed = 0;
        fblk->stat.sdu_ret = 0;
        fblk->stat.sdu_set = 0;
    }

    if (kflg_chk(rt->flg, ERF_SCAN)) {
        /*
         * Here, only process the IVEC_INPROGRESS message.
         * Because other scan may be queued after this, so, do not call
         * IV_SG_CALLBACK_SCAN with IVEC_SUCCESS which will tell user to end the scan
         * before the queued scan not process. The only place to call IV_SG_CALLBACK_SCAN
         * with IVEC_SUCCESS is sg_scan_complete();
         */
        rt->mgr->scan->scan.stat.cur += fragcnt;

        if (fblk->stat.sdu_set == fblk->stat.sdu_parsed) {
            //forced sg scan to end when SGDU that has send rev cmd successly are recvied and parsed
            //in this case,lost SGDU cannot received now and will be makeup in later update
            rt->mgr->scan->scan.stat.all = rt->mgr->scan->scan.stat.cur;
        }

        if (rt->mgr->scan->scan.stat.all == rt->mgr->scan->scan.stat.cur) {

            /* auto quit the scanning state */
            kflg_clr(rt->flg, ERF_SCAN);

            STR_SET(rt->curser, knil);

            /* When search done, clear current frequency, will stop auto update */
            sg_root_frequency_switch(rt, 0);

            kmsg_post(rt->mgr->scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)rt->mgr->scan, (kvoid*)-1, 0, 0);
        }else{
            klog(("recved:%d, found:%d\n", rt->mgr->scan->scan.stat.cur, rt->mgr->scan->scan.stat.all));
            rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)IVEC_INPROGRESS,
                    (kvoid*)rt->mgr->scan->scan.stat.all, (kvoid*)rt->mgr->scan->scan.stat.cur, (kvoid*)rt->mgr->scan->scan.userdata);
        }
    }

    return 0;
}


