#include <stdio.h>
#include <string.h>
#include "ksupport.h"
#include <kflg.h>
#include <kdbg.h>

#include "sg_root.h"
#include "sg_mgr2.h"
#include "sg_bs.h"
#include <ktsk.h>
#include <kmem.h>
#include <ksyn.h>

#include "zlib.h"

#include "ipaddr.h"

extern char __dir_delimeter;

static kint sg_root_sgdd_dir_callback(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3);
static kint sg_root_sgdd_obj_callback(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3);

static kint om_quit(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_timer(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_parse_sgdu(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_load_from_cache(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_save_to_cache(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_stop(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_abort_sync(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_call_evt_callback(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kint om_start_scan(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_abort_scan(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kint om_sgdd_dir(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_sgdd_obj(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
extern kint om_sgdu_obj(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kint om_freq_changed(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_abort_iad(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

kint om_GetServiceDetailInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_GetServicePreviewInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_GetServiceAccessInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_GetProgramList(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_GetProgramDescription(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_GetCurrentInteractivity(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

kint om_mos_file(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_start_iad_search_for_frequency(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_start_iad_search(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

kint om_iad_dir_cbk(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
kint om_iad_obj_cbk(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kint sg_root_cancel_sync_session(sg_root *rt);

/**
 * @brief Make the cache directory for the SG root.
 * Every SG root has a cache directory to save received fragment.
 * The name of the directory is hex format string of the provider ID
 * for the SG root with fix string length 8.
 *
 * @param rt What directory created for.
 *
 * @return none
 */
static kvoid sg_root_mk_cache_dir(sg_root *rt)
{
    kchar fn[IV_PATH_MAX_SIZE];

    sprintf(fn, "%s%c%08x", rt->mgr->env->data_dir, __dir_delimeter, rt->pvdr->id);
    kvfs_mkdir(fn);

    sprintf(fn, "%s%c%08x%cfragments", rt->mgr->env->data_dir, __dir_delimeter, rt->pvdr->id, __dir_delimeter);
    kvfs_mkdir(fn);
}

/**
 * @brief new structure, startup the working thread.\n And from now on, the
 * ESG Root is driven by message.
 *
 * @pre Must called after sg_mgr_new
 *
 * @param rt
 * the caller may maintain the memory for a root, if the rt is set.
 * sg_root_new will not allocate memory for it.
 *
 * @param mgr
 * Pointer to SG manager.
 *
 * @param pid
 * the Provider ID
 *
 * @retval 0 for success,
 * @retval -1 for error
 *
 * @see ::sg_root_del ::sg_mgr_new
 */
kint sg_root_new(sg_root **rt, sg_mgr *mgr, struct _K_pvdr *pvdr)
{
    K_dlist_entry *hdr, *entry;
    kuint flg = ERF_USED, i;

    K_freq *fblk;
    K_pvdr_acc *acc;

    kbean tworker;

    if (!(*rt)) {
        (*rt) = (sg_root*)kmem_alloc(sizeof(sg_root));
        flg |= ERF_DYN_MEM;
    }
    if ((*rt)) {
        memset((*rt), 0, sizeof(sg_root));

        /* FILL the BASE INFO with bsinfo */
        (*rt)->flg = flg;
        (*rt)->pvdr = pvdr;
        (*rt)->mgr = mgr;

        tworker = (*rt)->worker_thread = ktsk_new("sgroot", knil, (kvoid*)(*rt), knil, knil, knil);
        if (!tworker) {
            return -1;
        }

        /* system msg slot */
        kmsg_slot_set(tworker, KMSG_QUIT, om_quit);
        kmsg_slot_set(tworker, KMSG_TIMER, om_timer);

        /* sg root msg slot */
        kmsg_slot_set(tworker, KRTM_SGDU_PARSE, om_parse_sgdu);
        kmsg_slot_set(tworker, KRTM_STOP, om_stop);
        kmsg_slot_set(tworker, KRTM_LOAD_FROM_CACHE, om_load_from_cache);
        kmsg_slot_set(tworker, KRTM_SAVE_TO_CACHE, om_save_to_cache);
        kmsg_slot_set(tworker, KRTM_ABORT_SYNC, om_abort_sync);
        kmsg_slot_set(tworker, KRTM_CALL_EVT_CBK, om_call_evt_callback);

        /* For Scan */
        kmsg_slot_set(tworker, KRTM_START_SCAN, om_start_scan);
        kmsg_slot_set(tworker, KRTM_ABORT_SCAN, om_abort_scan);

        /* Session message */
        kmsg_slot_set(tworker, KRTM_SGDD_DIR, om_sgdd_dir);
        kmsg_slot_set(tworker, KRTM_SGDD_OBJ, om_sgdd_obj);
        kmsg_slot_set(tworker, KRTM_SGDU_OBJ, om_sgdu_obj);

        /* interactivity related message */
        kmsg_slot_set(tworker, KRTM_MOS_FILE, om_mos_file);
        kmsg_slot_set(tworker, KRTM_IAD_SEARCH_FOR_FREQ, om_start_iad_search_for_frequency);
        kmsg_slot_set(tworker, KRTM_IAD_SEARCH, om_start_iad_search);
        kmsg_slot_set(tworker, KRTM_IAD_ABORT, om_abort_iad);

        kmsg_slot_set(tworker, KRTM_IAD_DIR, om_iad_dir_cbk);
        kmsg_slot_set(tworker, KRTM_IAD_OBJ, om_iad_obj_cbk);

        /* frequency switch */
        kmsg_slot_set(tworker, KRTM_FREQ_CHG, om_freq_changed);

        /* ulapi msg slot */
        kmsg_slot_set(tworker, KRTM_GET_SERVICE_DETAIL_INFO, om_GetServiceDetailInfo);
        kmsg_slot_set(tworker, KRTM_GET_SERVICE_PREVIEW_INFO, om_GetServicePreviewInfo);
        kmsg_slot_set(tworker, KRTM_GET_SERVICE_ACCESS_INFO, om_GetServiceAccessInfo);
        kmsg_slot_set(tworker, KRTM_GET_PROGRAM_LIST, om_GetProgramList);
        kmsg_slot_set(tworker, KRTM_GET_PROG_DESC, om_GetProgramDescription);
        kmsg_slot_set(tworker, KRTM_GET_CURRENT_INTERACTIVITY, om_GetCurrentInteractivity);

        /* make cache dir */
        sg_root_mk_cache_dir(*rt);

        init_dlist_head(&(*rt)->frqhdr);

        /* scan pvdr->acchdr, create K_freq for it */
        hdr = &pvdr->acchdr;
        entry = hdr->next;
        while (entry != hdr) {
            acc = FIELD_TO_STRUCTURE(entry, K_pvdr_acc, le);
            entry = entry->next;

            sg_root_frequency_add(*rt, acc);
        }

        /* init all the list header for fragments */
        init_dlist_head(&(*rt)->allhdr);

        /* try to cleanup the OOD fragement every five minutes */
        (*rt)->gc_tmr = ktmr_set(tworker, 5 * 60 * 1000, ktrue, knil, (kvoid*)(*rt), (kvoid*)KRTM_OOD, 0, 0);

        return 0;
    }
    kerror(("sg_root_new: failed\n"));
    return -1;
}

/**
 * @brief undo what the ::sg_root_new do.
 *
 * @param rt SG root to destruct.
 * @param rmcache also remove the data cached in disk.
 *
 * @retval 0 OK
 * @retval -1 error
 *
 * @see ::sg_root_new
 */
kint sg_root_del(sg_root *rt, kbool rmcache)
{
    if (!kflg_chk(rt->flg, ERF_USED)) {
        kerror(("sg_root_del: unused, quit\n"));
        return -1;
    }

    /* FIXME: will use the return value from kmsg_send */
    /* XXX do not access anything in rt, because the rt is freed when message return */
    kmsg_send(rt->worker_thread, KMSG_QUIT, (kvoid*)rt, (kvoid*)rmcache, 0, 0);
    return 0;
}

/**
 * @brief called for each frequency directory
 */
static kint sg_root_cache_load_for_freq(sg_root *rt, kchar *dir, kuint freq, kbool force)
{
    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff;
    ef_cache_rec *reclst, *rec;
    kint flen, reccnt, i;
    kchar fn[IV_PATH_MAX_SIZE];
    kbean fp;

    K_freq *fblk = sg_root_freq_blk_find(rt, freq);

    if (!force && kflg_chk(rt->flg, ERF_CAC_LOAD)) {
        kerror(("om_load_from_cache: already loaded\n"));
        return 0;
    }

    sprintf(fn, "%s%csgcache.dat", dir, __dir_delimeter);

    fp = kvfs_open(fn, "rb", 0);
    if (!fp) {
        kerror(("Could not open sgcache : %s\n", fn));
        return -1;
    }

    flen = kvfs_length(fp);
    if ((flen <= 0) || (flen % sizeof(ef_cache_rec) != 0)) {
        kerror(("FileSize(%d) is not zhengshubei of ef_cache_rec(%d)\n", flen, sizeof(ef_cache_rec)));
        kvfs_close(fp);
        return -1;
    }

    reclst = knil;
    kvfs_seek(fp, 0, SEEK_SET);
    i = kvfs_read(fp, &reclst, &flen);
    if (flen != i) {
        kerror(("read file : %s failed, should read : %d bytes, only %d returned\n", fn, flen, i));
        kmem_free(reclst);
        kvfs_close(fp);
        return -1;
    }

    kflg_set(rt->flg, ERF_LOADING);

    reccnt = flen / sizeof(ef_cache_rec);
    for (i = 0; i < reccnt; i++) {
        rec = &reclst[i];

        if (rec->attr.validTo && rec->attr.validTo <= curtime + 2) {
            sg_root_del_cache_rec(rt, rec);
            continue;
        }

        switch (rec->cls) {
            case CLS_SGDU:
                sg_sdu_new_from_cache(rt, fblk, rec);
                break;

            case CLS_SGDD:
                sg_sdd_new_from_cache(rt, fblk, rec);
                break;

            case CLS_SCHE:
                sg_sch_new_from_cache(rt, fblk, rec);
                break;

            case CLS_PCCH:
                sg_pcc_new_from_cache(rt, fblk, rec);
                break;

            case CLS_IADA:
                sg_iad_new_from_cache(rt, fblk, rec);
                break;

            case CLS_COTT:
                sg_ctt_new_from_cache(rt, fblk, rec);
                break;

            case CLS_PCDA:
                sg_pcd_new_from_cache(rt, fblk, rec);
                break;

            case CLS_PCIT:
                sg_pci_new_from_cache(rt, fblk, rec);
                break;

            case CLS_PVDA:
                sg_pvd_new_from_cache(rt, fblk, rec);
                break;

            case CLS_SERV:
                sg_ser_new_from_cache(rt, fblk, rec);
                break;

            case CLS_ACCE:
                sg_acc_new_from_cache(rt, fblk, rec);
                break;

            default:
                break;
        }
    }

    kmem_free(reclst);
    kvfs_close(fp);

    kflg_clr(rt->flg, ERF_LOADING);
    kflg_set(rt->flg, ERF_CAC_LOAD);

    return 0;
}

/**
 * @brief load the fragment cached in disk into memory.
 *
 * @param a_tsk Task ID for the SG root.
 * @param a_msg KRTM_LOAD_FROM_CACHE.
 * @param a_ar0 Current SG root.
 * @param a_ar1 Force load, if true load anyway, else skip if already loaded.
 * @param a_ar2 not used.
 * @param a_ar3 not used.
 *
 * @param a_rsn Call reason.
 *
 * @retval 0 OK
 * @retval -1 NG
 */
static kint om_load_from_cache(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kbool force = (kbool)a_ar1;

    KVFS_FINDDATA finf;
    kbean file = 0;

    kchar fullpath[IV_PATH_MAX_SIZE];   /* for findfirst *.* */
    kchar temppath[IV_PATH_MAX_SIZE];   /* for sub directory */

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if (!force && kflg_chk(rt->flg, ERF_CAC_LOAD)) {
        kerror(("om_load_from_cache: already loaded\n"));
        return 0;
    }

    kflg_set(rt->flg, ERF_LOADING);
    /* walk data_dir, each dir be a provider */

    sprintf(fullpath, "%s%c%08x", rt->mgr->env->data_dir, __dir_delimeter,
            rt->pvdr->id);

    file = kvfs_findfirst(fullpath, &finf);
    if (file) {
        do {
            if (0 != strcmp(finf.name, ".") && 0 != strcmp(finf.name, "..")) {

                if (KVFS_A_SUBDIR & finf.attrib) {
                    sprintf(temppath, "%s%c%08x%c%s", rt->mgr->env->data_dir, __dir_delimeter, rt->pvdr->id, __dir_delimeter, finf.name);

                    /* OK, found a frequency */
                    klog(("Load cache for BS: Found : %s\n", temppath));
                    sg_root_cache_load_for_freq(rt, temppath, strtoul(finf.name, 0, 16), force);
                }
            }
        } while (-1 != kvfs_findnext(file, &finf));

        kvfs_findclose(file);
    }

    kflg_clr(rt->flg, ERF_LOADING);
    kflg_set(rt->flg, ERF_CAC_LOAD);

    return 0;
}
kint sg_root_load_from_cache(sg_root *rt, kbool force)
{
    kmsg_send(rt->worker_thread, KRTM_LOAD_FROM_CACHE, (kvoid*)rt, (kvoid*)force, 0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param a_tsk
 * @param a_msg
 * @param a_ar0
 * @param a_ar1
 * @param a_ar2
 * @param a_ar3
 * @param a_rsn
 *
 * @return
 */
static kint om_stop(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kbool rmcache = (kbool)a_ar1;

    kuint i;

    K_dlist_entry *entry, *hdr;
    sg_ses *ses;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    /* stop iinfo */
    sg_root_abort_iad(rt);

    /* stop transition */
    sg_root_abort_sync(rt);

    /* clear flag */
    sg_root_clr_auto_update_flg(rt);

    /* stop cleanup OOD data */
    ktmr_kill(rt->gc_tmr);

    /* stop auto cleanup timer */
    ktmr_kill(rt->up_tmr);

    kmem_free(rt->curser);

    /* first, delete all the session to prevent new data */
    /* FIXME: delete sub fblk */
    kerror(("\n\n\n\n\ncall sg_root_freq_del\n\n\n\n\n"));
    //sg_root_freq_del(rt);

    /* free all the fragments */
    sg_root_cleanup(rt, rmcache);

    /* ensure all the list is empty */
    kassert(is_dlist_empty(&rt->allhdr));

    return 0;
}
kint sg_root_stop(sg_root *rt, kbool rmcache)
{
    kmsg_send(rt->worker_thread, KRTM_STOP, (kvoid*)rt, (kvoid*)rmcache, 0, 0);
    return 0;
}


/**
 * @brief Issue command to check and try to receive new version data.
 * will, and always use timer to impletement this.
 * auto start sync after delay %d milliseconds.
 * XXX Why used timer? Because Sync function is called in ::om_timer.
 *
 * @param rt SG root.
 * @param delay delay time in milliseconds to start KRTM_START_SYNC command.
 *
 * @return 0
 */
kint sg_root_start_sync(sg_root *rt, kuint delay)
{
    if (kflg_chk(rt->flg, ERF_AUTO_SYNC) && !kflg_chk(rt->flg, ERF_NOW_SYNC)) {
        ktmr_kill(rt->up_tmr);
        rt->up_tmr = ktmr_set(rt->worker_thread, delay, kfalse, knil, (kvoid*)rt, (kvoid*)KRTM_START_SYNC, 0, 0);
    }
    return 0;
}

/**
 * @brief
 */
static kint om_start_scan(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_pvdr *pvdr = (K_pvdr*)a_ar0;
    kuint *scan_started = (kuint*)a_ar3;
    sg_scan *scan;

    K_dlist_entry *fb_entry, *fb_hdr;
    K_freq *fblk;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    scan = pvdr->root->mgr->scan;

    if (kflg_chk(pvdr->root->flg, ERF_SCAN)) {
        kerror(("already scan, ignore\n"));
        *scan_started = 0;
    }

    /* reset the statistics variables */
    fb_hdr = &pvdr->root->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

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

    kflg_set(pvdr->root->flg, ERF_SCAN);

    sg_root_frequency_switch(pvdr->root, scan->scan.frequency);

    sg_root_load_from_cache(pvdr->root, kfalse);
    sg_root_set_auto_update_flg(pvdr->root);

    if (!sg_root_get_sdd_dir(pvdr->root)) {
        /* one scan issued */
        *scan_started = 1;
    } else {
        *scan_started = 0;
    }
    return 0;
}

static kint om_abort_scan(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if (!kflg_chk(rt->flg, ERF_SCAN)) {
        kerror(("not scan, ignore\n"));
        return 0;
    }

    /* auto quit the scanning state */
    kflg_clr(rt->flg, ERF_SCAN);

    kflg_clr(rt->flg, ERF_NOW_SYNC);
    sg_root_cancel_sync_session(rt);

    STR_SET(rt->curser, knil);

    /* When search done, clear current frequency, will stop auto update */
    sg_root_frequency_switch(rt, 0);

    rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)IVEC_INPROGRESS,
            (kvoid*)rt->mgr->scan->scan.stat.all, (kvoid*)rt->mgr->scan->scan.stat.cur, (kvoid*)rt->mgr->scan->scan.userdata);

    kmsg_post(rt->mgr->scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)rt->mgr->scan, (kvoid*)-1, 0, 0);

    return 0;
}

static kint om_abort_iad(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_dlist_entry *fb_entry, *fb_hdr, *entry, *qhdr;
    K_freq *fblk;
    sg_root *rt = (sg_root*)a_ar0;
    sg_ser *ser;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        qhdr = &fblk->fhdrs[SER];
        entry = qhdr->next;
        while (entry != qhdr) {
            ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
            entry = entry->next;

            kflg_clr(ser->flg, SER_IAD_TIMER);
            ktmr_kill(ser->iad_probe_tmr);
            ser->iad_probe_tmr = knil;
        }
    }

    if (rt->freq.current) {
        sg_ses_stop_session(&rt->freq.current->data_ses.sdd);
    }

    return 0;
}

kint sg_root_abort_iad(sg_root *rt)
{
    kmsg_send(rt->worker_thread, KRTM_IAD_ABORT, (kvoid*)rt, knil, knil, knil);
    return 0;
}

static kint om_call_evt_callback(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    K_freq *fblk;
    sg_ser *ser;
    sg_ctt *ctt;
    sg_acc *acc;
    sg_sch *sch;
    sg_pci *pci;
    sg_pcd *pcd;
    sg_pcc *pcc;

    K_dlist_entry *fb_entry, *fb_hdr, *entry, *qhdr;
    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    /* decide which service should be callbacked */
    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        qhdr = &fblk->fhdrs[SER];
        entry = qhdr->next;
        while (entry != qhdr) {
            ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
            entry = entry->next;

            if (kflg_chk(ser->flg, EFF_INDICATE)) {
                kflg_clr(ser->flg, EFF_INDICATE);
                kflg_set(ser->flg, SER_CHG_SER);
            }
        }

        qhdr = &fblk->fhdrs[CTT];
        entry = qhdr->next;
        while (entry != qhdr) {
            ctt = FIELD_TO_STRUCTURE(entry, sg_ctt, cls_le);
            entry = entry->next;

            if (kflg_chk(ctt->flg, EFF_INDICATE)) {
                kflg_clr(ctt->flg, EFF_INDICATE);
                ser = sg_root_find_file(rt, SER, knil, ctt->arr->ServiceReference->arr->attr.idRef);
                if (ser) {
                    kflg_set(ser->flg, SER_CHG_PRG);
                }
            }
        }

        qhdr = &fblk->fhdrs[SCH];
        entry = qhdr->next;
        while (entry != qhdr) {
            sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
            entry = entry->next;

            if (kflg_chk(sch->flg, EFF_INDICATE)) {
                kflg_clr(sch->flg, EFF_INDICATE);
                ser = knil;
                if (sch->arr->ServiceReference) {
                    ser = sg_root_find_file(rt, SER, knil, sch->arr->ServiceReference->arr->attr.idRef);
                } else if (sch->arr->ContentReference) {
                    ctt = sg_root_find_file(rt, CTT, knil, sch->arr->ContentReference->arr->attr.idRef);
                    if (ctt) {
                        ser = sg_root_find_file(rt, SER, knil, ctt->arr->ServiceReference->arr->attr.idRef);
                    }
                }
                if (ser) {
                    kflg_set(ser->flg, SER_CHG_PRG);
                }
            }
        }

        qhdr = &fblk->fhdrs[ACC];
        entry = qhdr->next;
        while (entry != qhdr) {
            acc = FIELD_TO_STRUCTURE(entry, sg_acc, cls_le);
            entry = entry->next;

            if (kflg_chk(acc->flg, EFF_INDICATE)) {
                kflg_clr(acc->flg, EFF_INDICATE);
                ser = knil;
                if (acc->arr->ServiceReference) {
                    ser = sg_root_find_file(rt, SER, knil, acc->arr->ServiceReference->arr->attr.idRef);
                } else if (acc->arr->ScheduleReference) {
                    sch = sg_root_find_file(rt, SCH, knil, acc->arr->ScheduleReference->arr->attr.idRef);
                    if (sch) {
                        if (sch->arr->ServiceReference) {
                            ser = sg_root_find_file(rt, SER, knil, sch->arr->ServiceReference->arr->attr.idRef);
                        } else if (sch->arr->ContentReference) {
                            ctt = sg_root_find_file(rt, CTT, knil, sch->arr->ContentReference->arr->attr.idRef);
                            if (ctt) {
                                ser = sg_root_find_file(rt, SER, knil, ctt->arr->ServiceReference->arr->attr.idRef);
                            }
                        }
                    }
                }
                if (ser) {
                    kflg_set(ser->flg, SER_CHG_ACC);
                }
            }
        }

        qhdr = &fblk->fhdrs[PCI];
        entry = qhdr->next;
        while (entry != qhdr) {
            pci = FIELD_TO_STRUCTURE(entry, sg_pci, cls_le);
            entry = entry->next;

            if (kflg_chk(pci->flg, EFF_INDICATE)) {
                kflg_clr(pci->flg, EFF_INDICATE);
                ser = knil;
                if (pci->arr->ServiceReference) {
                    ser = sg_root_find_file(rt, SER, knil, pci->arr->ServiceReference->arr->attr.idRef);
                } else if (pci->arr->ContentReference) {
                    ctt = sg_root_find_file(rt, CTT, knil, pci->arr->ContentReference->arr->attr.idRef);
                    if (ctt) {
                        ser = sg_root_find_file(rt, SER, knil, ctt->arr->ServiceReference->arr->attr.idRef);
                    }
                } else if (pci->arr->ScheduleReference) {
                    sch = sg_root_find_file(rt, SCH, knil, pci->arr->ScheduleReference->arr->attr.idRef);
                    if (sch) {
                        if (sch->arr->ServiceReference) {
                            ser = sg_root_find_file(rt, SER, knil, sch->arr->ServiceReference->arr->attr.idRef);
                        } else if (sch->arr->ContentReference) {
                            ctt = sg_root_find_file(rt, CTT, knil, sch->arr->ContentReference->arr->attr.idRef);
                            if (ctt) {
                                ser = sg_root_find_file(rt, SER, knil, ctt->arr->ServiceReference->arr->attr.idRef);
                            }
                        }
                    }
                }
                if (ser) {
                    kflg_set(ser->flg, SER_CHG_PCS);
                }
            }
        }
    }

    /* call evtCallback */
    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        qhdr = &fblk->fhdrs[SER];
        entry = qhdr->next;
        while (entry != qhdr) {
            ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
            entry = entry->next;

            if (kflg_chk(ser->flg, SER_CHG_SER)) {
                kflg_clr(ser->flg, SER_CHG_SER | SER_CHG_PRG | SER_CHG_ACC | SER_CHG_PCS);
                rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SERVICE_UPDATE, ser->attr.id, knil, knil, knil);
            }
            if (kflg_chk(ser->flg, SER_CHG_PRG)) {
                kflg_clr(ser->flg, SER_CHG_PRG);
                rt->mgr->conf.evtCallback(IV_SG_CALLBACK_PROGRAM_UPDATE, ser->attr.id, knil, knil, knil);
            }
            if (kflg_chk(ser->flg, SER_CHG_ACC)) {
                kflg_clr(ser->flg, SER_CHG_ACC);
                rt->mgr->conf.evtCallback(IV_SG_CALLBACK_ACCESS_UPDATE, ser->attr.id, knil, knil, knil);
            }
            if (kflg_chk(ser->flg, SER_CHG_PCS)) {
                kflg_clr(ser->flg, SER_CHG_PCS);
                rt->mgr->conf.evtCallback(IV_SG_CALLBACK_PURCHASE_UPDATE, ser->attr.id, knil, knil, knil);
            }
        }
    }

    return 0;
}

static kint om_abort_sync(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    kflg_clr(rt->flg, ERF_NOW_SYNC);
    return sg_root_cancel_sync_session(rt);
}

/**
 * @brief Issue command to check and try to receive new version data.
 * will, and always use timer to impletement this.
 * auto start sync after delay %d milliseconds.
 * XXX Why used timer? Because Sync function is called in ::om_timer.
 *
 * @param rt SG root.
 * @param delay delay time in milliseconds to start KRTM_START_SYNC command.
 *
 * @return 0
 */
kint sg_root_abort_sync(sg_root *rt)
{
    kmsg_send(rt->worker_thread, KRTM_ABORT_SYNC, (kvoid*)rt, knil, knil, knil);
    return 0;
}

static kint sg_root_cancel_sync_session(sg_root *rt)
{
    K_dlist_entry *entry, *hdr;
    sg_ses *ses;

    if (!rt->freq.current) {
        kerror(("No current freq, which sync to be cancelled??\n"));
        return -1;
    }

    /* XXX Only stop the session, Not delete it */
    sg_ses_stop_session(&rt->freq.current->data_ses.sdd);

    hdr = &rt->freq.current->data_ses.subhdr;
    entry = hdr->next;
    while (entry != hdr) {
        ses = FIELD_TO_STRUCTURE(entry, sg_ses, le);
        entry = entry->next;

        // sg_ses_stop_session(ses);
        klog(("sg_root_cancel_sync_session:call sg_ses_del,ses=%x\n ",ses));
        sg_ses_del(ses);
    }
    return 0;
}

/**
 * @brief Set flag to indicate when frequency is OK, auto start auto sync.
 *
 * @param rt SG root.
 *
 * @retval 0 successfully started
 * @retval -1 Do nothing
 */
kint sg_root_set_auto_update_flg(sg_root *rt)
{
    kflg_set(rt->flg, ERF_AUTO_SYNC);
    return 0;
}
kint sg_root_clr_auto_update_flg(sg_root *rt)
{
    kflg_clr(rt->flg, ERF_AUTO_SYNC);
    ktmr_kill(rt->up_tmr);
    return 0;
}

/**
 * @brief try and delete all the fragments which out of data.
 *
 * @param rt SG root.
 *
 * @return
 */
static kint om_cleanup_ood(sg_root *rt)
{
    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff;
    K_dlist_entry *entry, *hdr;
    sg_zbd *zbd;

    klog(("Garbage Collecting ...\n"));
    klog(("Checking ALL the file to determine which file is out of data\n"));

    // ONLY Checking allhdr is OK
    hdr = &rt->allhdr;
    entry = hdr->next;
    while (entry != hdr) {
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, all_le);
        entry = entry->next;

        /* OOD within 5 second will be deleted too */
        if (kflg_chk(zbd->flg, EFF_SYNC_DEL) ||
                (zbd->attr.validTo && zbd->attr.validTo <= curtime + 5)) {

            remove_dlist_entry(&zbd->cls_le);
            remove_dlist_entry(&zbd->all_le);

            // do right thing for right class
            sg_delete_file(zbd, ktrue);
        }
    }
    return 0;
}

static kint om_cleanup_cache(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }
    return 0;
}
kint sg_root_cleanup_cache(sg_root *rt)
{
    // FIXME remove missing files
    return 0;
}

/**
 * @brief Delete the SG root.
 *
 * @param a_tsk
 * @param a_msg
 * @param a_ar0
 * @param a_ar1
 * @param a_ar2
 * @param a_ar3
 * @param a_rsn
 *
 * @return
 */
static kint om_quit(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kbool rmcache = (kbool)a_ar1;
    K_freq *fblk;
    sg_ses *ses;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    klog(("quiting...\n"));

    if (!kflg_chk(rt->flg, ERF_USED)) {
        kerror(("sg_root_del: unused, quit\n"));
        return -1;
    }

    ktmr_killall(a_tsk);

    if (rmcache) {
        sg_root_cleanup_cache(rt);
    } else {
        sg_root_save_to_cache(rt);
    }

    sg_root_stop(rt, rmcache);

    /* free sub acc */
    while (!is_dlist_empty(&rt->frqhdr)) {
        fblk = FIELD_TO_STRUCTURE(rt->frqhdr.next, K_freq, le);
        remove_dlist_entry(&fblk->le);

        /* stop session for this */
        sg_ses_del(&fblk->data_ses.sdd);

        while (!is_dlist_empty(&fblk->data_ses.subhdr)) {
            ses = FIELD_TO_STRUCTURE(fblk->data_ses.subhdr.next, sg_ses, le);

            remove_dlist_entry(&ses->le);
            klog(("om_quit(in while):call sg_ses_del,ses=%x\n ",ses));
            sg_ses_del(ses);
        }

        /* free this */
        kmem_free(fblk);
    }

    if (kflg_chk(rt->flg, ERF_DYN_MEM)) {
        kmem_free(rt);
    }

    return 0;
}

/**
 * @brief Process periodically work, OOD and RESYNC.
 *
 * @param a_tsk
 * @param a_msg
 * @param a_ar0
 * @param a_ar1
 * @param a_ar2
 * @param a_ar3
 * @param a_rsn
 *
 * @return
 */
static kint om_timer(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kuint cmd = (kuint)a_ar1, arg = (kuint)a_ar2;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if (cmd == KRTM_OOD) {
        // check and delete OOD files
        klog(("om_timer: Try to delete OOD fragments\n"));
        om_cleanup_ood(rt);
    }
    if (cmd == KRTM_START_SYNC) {
        if (arg) {
            kflg_clr(rt->flg, ERF_NOW_SYNC);
            sg_root_cancel_sync_session(rt);
        } else {
            klog(("om_timer: Try to reSync fragments, autoSync:%s\n", kflg_chk(rt->flg, ERF_AUTO_SYNC) ? "ktrue" : "kfalse"));

            sg_root_cancel_sync_session(rt);
            sg_root_get_sdd_dir(rt);
        }
    }

    return 0;
}

kint sg_root_apply_delete(sg_root *rt)
{
    klog(("delete the fragments marked as deleted\n"));
    return 0;
}

static kint om_parse_sgdu(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    // in fact, only SDU file need this
    sg_root *rt = (sg_root*)a_ar0;
    K_freq *fblk;
    sg_sdu *sdu;
    K_dlist_entry *fb_entry, *fb_hdr, *entry, *qhdr;
    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        qhdr = &fblk->fhdrs[SDU];
        entry = qhdr->next;
        while (entry != qhdr) {
            sdu = FIELD_TO_STRUCTURE(entry, sg_sdu, cls_le);
            entry = entry->next;

            if (kflg_chk(sdu->flg, EFF_SYNC_DEL) ||
                    (sdu->attr.validTo && sdu->attr.validTo <= curtime)) {
                /* OOD delete it, do right thing for right class */
                remove_dlist_entry(&sdu->cls_le);
                remove_dlist_entry(&sdu->all_le);
                sg_delete_file((sg_zbd*)sdu, ktrue);
            } else if (kflg_chk(sdu->flg, EFF_DATAOK)) {
                /* parse the data */
                sg_sdu_parse(sdu);

                if (fblk->stat.sdu_set == fblk->stat.sdu_parsed) {
                    /* call evtCallback to uppler layer */
                    sg_root_call_evt_callback(rt);

                    /* delete all the fragments marked as deleted */
                    /* XXX called after sg_root_call_evt_callback */
                    sg_root_apply_delete(rt);

                    /* save the fragments to disk */
                    sg_root_save_to_cache(rt);
                }
            }
        }
    }

    return 0;
}


// Only cleanup the memory cached fragments, if rmcache set,
// disk cache will be cleanup too.
kint sg_root_cleanup(sg_root *rt, kbool rmcache)
{
    sg_zbd *zbd;
    kint i;
    K_dlist_entry *entry, *hdr;

    klog(("delfile:%s\n", "START"));
    hdr = &rt->allhdr;
    while (!is_dlist_empty(hdr)) {
        entry = remove_dlist_head_entry(hdr);
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, all_le);
        klog(("delfile:%08x, %s\n", zbd->cls, zbd->attr.id));
        remove_dlist_entry(&zbd->cls_le);
        remove_dlist_entry(&zbd->all_le);
        sg_delete_file(zbd, rmcache);
    }

    klog(("delfile:%s\n", "DONE"));
    return 0;
}

static kint sg_root_save_to_cache_for_freq(sg_root *rt, K_freq *fblk)
{
    K_dlist_entry *entry, *hdr;
    sg_zbd *zbd;
    kbool found;
    kchar fn[IV_PATH_MAX_SIZE];
    kbean fp;

    kuint curtime = ksys_ntp_time() + 20 + rt->mgr->env->time_diff;

    ef_cache_rec *reclst, *rec;
    kint reccnt = 0, i = 0;
    kuint cls, ret;

    sprintf(fn, "%s%c%08x%c%08x%csgcache.dat", rt->mgr->env->data_dir, __dir_delimeter,
            rt->pvdr->id, __dir_delimeter, fblk->acc->frequency, __dir_delimeter);

    fp = kvfs_open(fn, "wb+", 0);
    if (!fp) {
        kerror(("Could not open file : %s\n", fn));
        return -1;
    }

    hdr = &rt->allhdr;
    entry = hdr->next;
    while (entry != hdr) {
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, all_le);
        entry = entry->next;

        // skip OOD file
        if (kflg_chk(zbd->flg, EFF_SYNC_DEL) ||
                (zbd->attr.validTo && (zbd->attr.validTo <= curtime))) {
            continue;
        }

        switch (zbd->cls) {
            case CLS_SGDU:
            case CLS_SGDD:
            case CLS_SCHE:
            case CLS_PCCH:
            case CLS_IADA:
            case CLS_COTT:
            case CLS_PCDA:
            case CLS_PCIT:
            case CLS_PVDA:
            case CLS_SERV:
            case CLS_ACCE:
                break;
            default:
                continue;
        }

        reccnt++;
    }

    if (reccnt <= 0) {
        kerror(("No file should be saved into disk\n"));
        kvfs_close(fp);
        return -1;
    }

    reclst = (ef_cache_rec*)kmem_alloc(sizeof(ef_cache_rec) * reccnt);
    if (!reclst) {
        kerror(("Can not allocate memory for saving file : %s\n", fn));
        kvfs_close(fp);
        return -1;
    }

    rec = &reclst[0];
    entry = hdr->next;
    while (entry != hdr) {
        zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, all_le);
        entry = entry->next;

        cls = zbd->cls;

        // skip OOD file
        if (kflg_chk(zbd->flg, EFF_SYNC_DEL) ||
                (zbd->attr.validTo && (zbd->attr.validTo <= curtime))) {
            sg_root_rm_cache_file(zbd);
            continue;
        }

        if (!zbd->attr.id) {
            klog(("noname file\n"));
        }

        switch (zbd->cls) {
            case CLS_SGDU:
                sg_root_zbd_to_rec(rt, (sg_sdu*)zbd, rec);
                break;

            case CLS_SGDD:
                sg_root_zbd_to_rec(rt, (sg_sdd*)zbd, rec);
                break;

            case CLS_SCHE:
                sg_root_zbd_to_rec(rt, (sg_sch*)zbd, rec);
                break;

            case CLS_PCCH:
                sg_root_zbd_to_rec(rt, (sg_pcc*)zbd, rec);
                break;

            case CLS_IADA:
                sg_root_zbd_to_rec(rt, (sg_iad*)zbd, rec);
                break;

            case CLS_COTT:
                sg_root_zbd_to_rec(rt, (sg_ctt*)zbd, rec);
                break;

            case CLS_PCDA:
                sg_root_zbd_to_rec(rt, (sg_pcd*)zbd, rec);
                break;

            case CLS_PCIT:
                sg_root_zbd_to_rec(rt, (sg_pci*)zbd, rec);
                break;

            case CLS_PVDA:
                sg_root_zbd_to_rec(rt, (sg_pvd*)zbd, rec);
                break;

            case CLS_SERV:
                sg_root_zbd_to_rec(rt, (sg_ser*)zbd, rec);
                break;

            case CLS_ACCE:
                sg_root_zbd_to_rec(rt, (sg_acc*)zbd, rec);
                break;

            default:
                continue;
        }

        rec++;
        i++;

        kassert(i <= reccnt);
    }

    ret = kvfs_write(fp, reclst, sizeof(ef_cache_rec) * reccnt);
    if (ret != sizeof(ef_cache_rec) * reccnt) {
        kerror(("Write file : %s, should write %d byte, %d returned\n", fn, sizeof(ef_cache_rec) * reccnt, ret));
    }

    kmem_free(reclst);
    kvfs_close(fp);
    return 0;
}

/**
 * @brief
 *
 * @param a_tsk
 * @param a_msg
 * @param a_ar0
 * @param a_ar1
 * @param a_ar2
 * @param a_ar3
 * @param a_rsn
 *
 * @return
 */
static kint om_save_to_cache(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    K_dlist_entry *fb_entry, *fb_hdr;
    K_freq *fblk;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        sg_root_save_to_cache_for_freq(rt, fblk);
    }
    return 0;
}
/**
 * @brief
 *
 * @param rt
 *
 * @return
 */
kint sg_root_save_to_cache(sg_root *rt)
{
    return kmsg_send(rt->worker_thread, KRTM_SAVE_TO_CACHE, rt, 0, 0, 0);
}

/**
 * @brief
 *
 * @param rt
 *
 * @return
 */
kint sg_root_call_evt_callback(sg_root *rt)
{
    return kmsg_send(rt->worker_thread, KRTM_CALL_EVT_CBK, rt, 0, 0, 0);
}


/**
 * @brief Start get SGDD directory.
 * When SGDD directory gotten, it will automatically receive the followed data
 *
 * @param rt SG root.
 *
 * @return 0 for success, -1 for error
 *
 * @warning Only receive data according to current frequency
 */
kint sg_root_get_sdd_dir(sg_root *rt)
{
    sg_ses *ses;
    K_freq *fblk = rt->freq.current;
    kchar curmode;
    kuint i, timeout;
    kbool scanning;
    K_pvdr_acc *acc;

    IV_FLUTE_ALC_SESSION_ENTRYINFO *flute_sei;
    IV_DAB_SESSION_ENTRYINFO *mot_sei;
    if (0 == fblk) {
        kerror(("SG Manager:: Current service not set, can not receive data.\n"));
        return -1;
    }

    acc = fblk->acc;

    if (kflg_chk(rt->flg, ERF_SCAN)) {
        scanning = ktrue;
        curmode = rt->mgr->scan->scan.tmode;
    } else {
        scanning = kfalse;
        curmode = acc->tmode[acc->tindex];
    }

    if (curmode == SG_TMODE_LAST) {
        /* reset tmode index */
        acc->tindex = 0;
        curmode = acc->tmode[acc->tindex];
    }

    while (curmode != SG_TMODE_LAST) {

        ses = &fblk->data_ses.sdd;
        switch (curmode) {
            case SG_TMODE_BDIR:
                kassert(acc->bdir);
                if (1) {
                    kchar url[1024];
                    if (strstr(acc->bdir->url, "?")) {
                        sprintf(url, "%s&NID=%s&TSID=%s&", acc->bdir->url, acc->bdir->nid, acc->bdir->tsid);
                    } else {
                        sprintf(url, "%s?NID=%s&TSID=%s&", acc->bdir->url, acc->bdir->nid, acc->bdir->tsid);
                    }
                    sg_ses_new(&ses, curmode, url);
                }
                break;
            case SG_TMODE_MOT:
                kassert(acc->mot);
                mot_sei = kmem_alloc(sizeof(IV_DAB_SESSION_ENTRYINFO));
                memcpy(mot_sei, &acc->mot->dab, sizeof(IV_DAB_SESSION_ENTRYINFO));
                sg_ses_new(&ses, curmode, mot_sei);
                break;
            case SG_TMODE_FLUTE:
                kassert(acc->flute);
                flute_sei = kmem_alloc(sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO));
                memcpy(flute_sei, &acc->flute->alc, sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO));
                sg_ses_new(&ses, curmode, flute_sei);
                break;
            default:
                kassert(!"bad tmode");
                return -1;
        }
        kflg_set(ses->flg, SES_AUTO_STOP);

        if (sg_ses_start_session(&fblk->data_ses.sdd, ktrue)) {
            klog(("sg_ses_start_session, goto try next\n"));

            klog(("sg_root_get_sdd_dir 1:call sg_ses_del,ses=%x\n ",ses));
            sg_ses_del(ses);

            if (scanning) {
                return -1;
            }

            curmode = acc->tmode[++acc->tindex];
            continue;
        }

        if (scanning) {
            timeout = rt->mgr->scan->scan.timeout * 3 / 4;
        } else {
            timeout = rt->mgr->conf.autoUpdateTime * 1000;
        }

        /* a dir working on a certain frequency */
        if (sg_ses_get_dir(&fblk->data_ses.sdd, timeout, (kvoid*)sg_root_sgdd_dir_callback, rt, (kvoid*)acc->frequency, knil, knil)) {
            klog(("sg_ses_get_dir, goto try next\n"));

            klog(("sg_root_get_sdd_dir 2:call sg_ses_del,ses=%x\n ",ses));
            sg_ses_del(ses);

            if (scanning) {
                return -1;
            }

            curmode = acc->tmode[++acc->tindex];
            continue;
        }

        /* OK, all the operation is successfully returned */

        kflg_set(rt->flg, ERF_NOW_SYNC);
        fblk->stat.dir_set++;
        return 0;
    }

    curmode = acc->tmode[acc->tindex];
    if (curmode == SG_TMODE_LAST) {
        /* reset tmode index */
        acc->tindex = 0;
        curmode = acc->tmode[acc->tindex];
    }

    return -1;
}

typedef struct _K_MB_SGDD_DIR {
    sg_ses *ses;
    kint iHandle;
    kint iStatus;
    IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute;
    IV_OBJECT_INFO *pInfoLisHead;
    kint iObjectNum;

    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} K_MB_SGDD_DIR;

/**
 * @brief
 *
 * @param iHandle
 * @param iStatus
 * @param pObjectList_attibute
 * @param pInfoLisHead
 * @param iObjectNum
 * @param vpUserData0
 * @param vpUserData1
 * @param vpUserData2
 * @param vpUserData3
 *
 * @return
 */
kint sg_root_sgdd_dir_callback(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3)
{
    K_MB_SGDD_DIR dir;
    sg_root *rt = (sg_root*)vpUserData0;

    dir.ses = ses;
    dir.iHandle = iHandle;
    dir.iStatus = iStatus;
    dir.pObjectList_attibute = pObjectList_attibute;
    dir.pInfoLisHead = pInfoLisHead;
    dir.iObjectNum = iObjectNum;

    dir.vpUserData0 = vpUserData0;
    dir.vpUserData1 = vpUserData1;
    dir.vpUserData2 = vpUserData2;
    dir.vpUserData3 = vpUserData3;

    kmsg_send(rt->worker_thread, KRTM_SGDD_DIR, &dir, 0, 0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param a_tsk
 * @param a_msg
 * @param a_ar0
 * @param a_ar1
 * @param a_ar2
 * @param a_ar3
 * @param a_rsn
 *
 * @return
 */
static kint om_sgdd_dir(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MB_SGDD_DIR *dir = (K_MB_SGDD_DIR*)a_ar0;

    sg_ses *ses = dir->ses;
    kint iHandle = dir->iHandle;
    kint iStatus = dir->iStatus;
    IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute = dir->pObjectList_attibute;
    IV_OBJECT_INFO *pInfoLisHead = dir->pInfoLisHead;
    kint iObjectNum = dir->iObjectNum;

    kvoid *vpUserData0 = dir->vpUserData0;
    kvoid *vpUserData1 = dir->vpUserData1;
    kvoid *vpUserData2 = dir->vpUserData2;
    kvoid *vpUserData3 = dir->vpUserData3;

    kint handle, i, same, shift;
    sg_root *rt = (sg_root*)vpUserData0;
    kuint frequency = (kuint)vpUserData1, sescnt = 0;
    IV_OBJECT_INFO *ivoi;
    sg_fdt *sgdd_dir = knil;
    sg_sdd *sdd;
    K_dlist_entry *entry, *hdr;

    K_freq *fblk;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    fblk = sg_root_freq_blk_find(rt, frequency);

    klog((">>>sg_root_sgdd_dir_callback:rt(%x), ses(%x)\n", rt,ses));

    if (kflg_chk(rt->flg, ERF_AUTO_SYNC)) {
        ktmr_kill(rt->up_tmr);
        rt->up_tmr = ktmr_set(rt->worker_thread, rt->mgr->conf.autoUpdateTime * 1000, ktrue, knil, rt, (kvoid*)KRTM_START_SYNC, 0, 0);
    }

    if (!fblk) {
        sg_ses_try_end_session(ses);
        return -1;
    }

    if (0 != iStatus) {
        kfatal(("sg_root_sgdd_dir_callback:callback failed with status :%d\n", iStatus));

        /*
         * XXX If current tmode timeout, it means the Transport layer has problem,
         * So, use next tmode next time.
         */
        if (!kflg_chk(rt->flg, ERF_SCAN)) {
            fblk->acc->tindex++;

            if (SG_TMODE_LAST == fblk->acc->tmode[fblk->acc->tindex]) {
                /* reset tmode index */
                fblk->acc->tindex = 0;
            }
        }

        /* When scanning quit immediately */
        if ((-2 == iStatus) && kflg_chk(rt->flg, ERF_SCAN)) {

            rt->mgr->scan->scan_ng++;

            /* auto quit the scanning state */
            kflg_clr(rt->flg, ERF_SCAN);

            STR_SET(rt->curser, knil);

            /* When search done, clear current frequency, will stop auto update */
            sg_root_frequency_switch(rt, 0);

            rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)IVEC_INPROGRESS,
                    (kvoid*)rt->mgr->scan->scan.stat.all, (kvoid*)rt->mgr->scan->scan.stat.cur, (kvoid*)rt->mgr->scan->scan.userdata);

            kmsg_post(rt->mgr->scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)rt->mgr->scan, (kvoid*)-1, 0, 0);
        }

        sg_ses_try_end_session(ses);
        return -1;
    }

    fblk->stat.dir_ret++;

    /* SGDD Dir Only update */
    sgdd_dir = sg_root_get_dir_by_freq(rt, frequency);
    if (!sgdd_dir) {
        if (sg_fdt_new(&sgdd_dir)) {
            kerror(("sg_root_sgdd_dir_callback: sg_fdt_new failed\n"));
            sg_ses_try_end_session(ses);
            return -1;
        }

        /* frequency act as id for fdt */
        sgdd_dir->frequency = frequency;
        sgdd_dir->rt = rt;
        insert_dlist_tail_entry(&rt->allhdr, &sgdd_dir->all_le);
        insert_dlist_tail_entry(&fblk->fhdrs[FDT], &sgdd_dir->cls_le);
    }
    if (pObjectList_attibute) {
        sgdd_dir->olattr.uiExpiresTime = pObjectList_attibute->uiExpiresTime;
        sgdd_dir->olattr.uiObjectListVersion = pObjectList_attibute->uiObjectListVersion;
        sgdd_dir->olattr.iComplete_flag = pObjectList_attibute->iComplete_flag;
        sgdd_dir->olattr.iFullFDT_flag = pObjectList_attibute->iFullFDT_flag;
    }

    /*
     * First, remove unused SGDD ITEM from old/saved list
     *
     * A: If a sgdd for this frequency not list in this directory, this
     * means the SGDD is disppear from the world, so, these sgdd should
     * be deleted from system.
     *
     * B: XXX If a sgdd with same name in system, but not in current frequency,
     * it also should be delete. Because all the sub fragments's frequency
     * field also should be adjust.
     * NOTE: Another way is to change all the sub frequency to the current frequency
     */
    hdr = &fblk->fhdrs[SDD];
    entry = hdr->next;
    while (entry != hdr) {
        sdd = FIELD_TO_STRUCTURE(entry, sg_sdd, cls_le);
        entry = entry->next;

        for (i = 0; i < iObjectNum; i++) {
            ivoi = pInfoLisHead + i;
            if ((sdd->frequency == frequency) && streq(ivoi->pObjectName, sdd->objinf.pObjectName)) {
                klog(("SGDD(%s) at freq(%d) already exist\n", ivoi->pObjectName, frequency));
                break;
            }
        }

        /*
         * OK, the old item not exist in current/new list,
         * this means it should not be used any longer, just delete it
         */
        if (i == iObjectNum) {
            klog(("SGDD(%s) not in new list, remove it\n", sdd->objinf.pObjectName));
            sg_sdd_free_sub_sdu(sdd);
            sg_sdd_del(sdd, ktrue);
        }
    }

    /*
     * Then, add new sgdd to current list
     *
     * Loop all the dir item in list, if not in current list, then it must be a new one.
     */
    for (i = 0; i < iObjectNum; i++) {
        ivoi = pInfoLisHead + i;

        same = kfalse;
        entry = hdr->next;
        while (entry != hdr) {
            sdd = FIELD_TO_STRUCTURE(entry, sg_sdd, cls_le);
            if (streq(ivoi->pObjectName, sdd->objinf.pObjectName)) {

                /*
                 * XXX oi->iObjectVersion can be not exist
                 * and uiObjectID ALWAYS change if version changed
                 */
                if (ivoi->uiObjectID == sdd->objinf.uiObjectID) {
                    klog(("Same SGDD(%s): OID is %d\n", ivoi->pObjectName, ivoi->uiObjectID));
                    same = ktrue;
                } else {
                    /* OK, the new OI is changed, update the old with new param */
                    klog(("changed SGDD(%s): OID is %d, old OID:%d\n", ivoi->pObjectName, ivoi->uiObjectID, sdd->objinf.uiObjectID));
                    sdd->objinf.uiObjectID = ivoi->uiObjectID;
                }
                /* same, skip it */
                sdd->objinf.iObjectVersion = ivoi->iObjectVersion;
                sdd->objinf.iObjectVersionLength = ivoi->iObjectVersionLength;
                sdd->objinf.uiContentLength = ivoi->uiContentLength;
                break;
            }
            entry = entry->next;
        }

        /* this sdd is not exist in saved list, append this */
        if (entry == hdr) {
            klog(("SGDD List item not exist in old list, append it. Name:%s, OID:%d\n", ivoi->pObjectName, ivoi->uiObjectID));
            sdd = knil;
            sg_sdd_new(&sdd);
            sdd->rt = rt;
            sdd->tid = ivoi->uiObjectID;
            sdd->frequency = frequency;

            sdd->objinf.pObjectName = kstr_dup(ivoi->pObjectName);
            sdd->objinf.uiObjectID = ivoi->uiObjectID;
            sdd->objinf.iObjectVersion = ivoi->iObjectVersion;
            sdd->objinf.iObjectVersionLength = ivoi->iObjectVersionLength;
            sdd->objinf.uiContentLength = ivoi->uiContentLength;

            sg_sdd_save_extinfo(sdd);

            insert_dlist_tail_entry(&fblk->fhdrs[SDD], &sdd->cls_le);
            insert_dlist_tail_entry(&rt->allhdr, &sdd->all_le);
        }

        if (same) {
            /*
             * `same` mean the version not changed.
             *
             * Continue to check the SGDD for unreceived sgdu
             * if there are some SGDUs not received, start session to get the SGDU
             */
            klog(("SGDD %s with OID %d is already exist checking for lost fragments\n", sdd->objinf.pObjectName, sdd->objinf.uiObjectID));
            if (!sg_sdd_try_receive_lost_file(sdd)) {
                /* OK, sg_sdd_try_receive_lost_file send request to receive lost files, so continue to next loop */
                continue;
            }
        }

        if (ivoi->iObjectVersionLength >= 0) {
            shift = ivoi->iObjectVersionLength;
        }

        /* frequency information can be get form sdd */
        if (!sg_ses_get_obj(ses,4000000, ivoi->uiObjectID, 0xFFFFFFFF << shift,
                    (kvoid*)sg_root_sgdd_obj_callback, knil, rt, ses, sdd, knil)) {
            fblk->stat.sdd_set++;
            sescnt++;
        }
    }
    klog(("<<<sg_root_sgdd_dir_callback:rt(%x), ses(%x)\n", rt,ses));

    /* All the fragments already received in previous scan, end this immediately */
    if ((0 == sescnt) && kflg_chk(rt->flg, ERF_SCAN)) {

        /* auto quit the scanning state */
        kflg_clr(rt->flg, ERF_SCAN);

        STR_SET(rt->curser, knil);

        /* When search done, clear current frequency, will stop auto update */
        sg_root_frequency_switch(rt, 0);

        rt->mgr->conf.evtCallback(IV_SG_CALLBACK_SCAN, (kvoid*)IVEC_INPROGRESS,
                (kvoid*)rt->mgr->scan->scan.stat.all, (kvoid*)rt->mgr->scan->scan.stat.cur, (kvoid*)rt->mgr->scan->scan.userdata);

        kmsg_post(rt->mgr->scan->worker_thread, SCAN_CHANGE_CNT, (kvoid*)rt->mgr->scan, (kvoid*)-1, 0, 0);
    }

    sg_ses_try_end_session(ses);
    return 0;
}

typedef struct _K_MB_SGDD_OBJ {
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
} K_MB_SGDD_OBJ;

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
static kint sg_root_sgdd_obj_callback(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3)
{
    K_MB_SGDD_OBJ obj;
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

    return kmsg_send(rt->worker_thread, KRTM_SGDD_OBJ, &obj, 0, 0, 0);
}

static kint om_sgdd_obj(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MB_SGDD_OBJ *obj = (K_MB_SGDD_OBJ*)a_ar0;

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

    sg_root *rt = (sg_root*)vpUserData0;
    sg_ses *rootses = (sg_ses*)vpUserData1, *sgduses = knil;
    sg_sdd *sdd = (sg_sdd*)vpUserData2;
    sg_sdd *new_ver_sdd = knil;
    K_dlist_entry *entry, *hdr;

    kchar *ob = knil;
    kint ol = 0, i, j, k, shift;

    kuchar *gunzip_buf = pDataBuffer, *gunzip_buf_end = pDataBuffer + uiObjectLength;

    klog((">>>sg_root_sgdd_obj_callback:rt(%x), ses(%x)\n", rt,rootses));

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    if (iStatus) {
        kfatal(("sg_root_sgdd_obj_callback:callback failed with status :%d\n", iStatus));
        sg_ses_try_end_session(ses);
        return -1;
    }

    if (!rt->freq.current) {
        kerror(("no current frequency\n"));
        sg_ses_try_end_session(ses);
        return -1;
    }

    rt->freq.current->stat.sdd_ret++;

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
            kerror(("sg_root_sgdd_obj_callback:unzip file failed\n"));
            sg_ses_try_end_session(ses);
            return -1;
        }
    } else {
        ob = kmem_alloc(uiObjectLength);
        memcpy(ob, pDataBuffer, uiObjectLength);
        ol = uiObjectLength;
    }

    kassert(sdd);

    sg_sdd_new(&new_ver_sdd);
    new_ver_sdd->rt = rt;
    new_ver_sdd->tid = uiObjectID;
    new_ver_sdd->frequency = sdd->frequency;
    new_ver_sdd->dat.buf = ob;
    new_ver_sdd->dat.len = ol;
    if (-2 == sg_sdd_x2s(new_ver_sdd)) {
        // FIXME: old_sdd:(id == 2) cache_file = sdd00000002
        // cur sdd:(id == 2)
        // if cur_sdd is bad, the sg_sdd_del will delete the old SDD's cache data
        FREE_ORG_DAT(new_ver_sdd);
        sg_sdd_del(new_ver_sdd, ktrue);
        sg_ses_try_end_session(ses);
        return -1;
    }
    FREE_ORG_DAT(new_ver_sdd);

    if (new_ver_sdd->arr->NotificationReception) {
        // save this Notification int rt
        //STR_SET(rt->curNotification.description, new_ver_sdd->arr->NotificationReception->arr->Description->arr->val.Description);
        //STR_SET(rt->curNotification.requenstURL, new_ver_sdd->arr->NotificationReception->arr->RequestURL->arr->val.RequestURL);
        rt->mgr->conf.evtCallback(IV_SG_CALLBACK_NOTIFICATION, knil, knil, knil, knil);
    }

    /* First, Remove all OOD data from old sgdd */
    sg_sdd_remove_ood_fragment(sdd, new_ver_sdd);

    /* OK, All the OOD data is removed, Should use new SGDD to overwrite the OLD sgdd */
    sg_sdd_chk_update(sdd, new_ver_sdd);
    sg_sdd_del(new_ver_sdd, kfalse);

    klog(("<<<sg_root_sgdd_obj_callback:rt(%x), ses(%x)\n", rt,rootses));
    sg_ses_try_end_session(ses);
    return 0;
}

kint sg_delete_file(sg_zbd *zbd, kbool rmcache)
{
    switch (zbd->cls) {
        case CLS_SGDD:
            sg_sdd_del((sg_sdd*)zbd, rmcache);
            break;

        case CLS_SGDU:
            sg_sdu_del((sg_sdu*)zbd, rmcache);
            break;

        case CLS_SERV:
            sg_ser_del((sg_ser*)zbd, rmcache);
            break;

        case CLS_SCHE:
            sg_sch_del((sg_sch*)zbd, rmcache);
            break;

        case CLS_COTT:
            sg_ctt_del((sg_ctt*)zbd, rmcache);
            break;

        case CLS_PCIT:
            sg_pci_del((sg_pci*)zbd, rmcache);
            break;

        case CLS_PCDA:
            sg_pcd_del((sg_pcd*)zbd, rmcache);
            break;

        case CLS_PCCH:
            sg_pcc_del((sg_pcc*)zbd, rmcache);
            break;

        case CLS_PVDA:
            sg_pvd_del((sg_pvd*)zbd, rmcache);
            break;

        case CLS_DIRE:
            sg_fdt_del((sg_fdt*)zbd, rmcache);
            break;

        case CLS_IADA:
            sg_iad_del((sg_iad*)zbd, rmcache);
            break;

        case CLS_ACCE:
            sg_acc_del((sg_acc*)zbd, rmcache);
            break;

        case CLS_SDPF:
            sg_sdp_del((sg_sdp*)zbd, rmcache);
            break;

        default:
            kerror(("bad file class%08x\n", zbd->cls));
            break;
    }
    return 0;
}




/**
 * @defgroup SearchFunctions
 * @brief Search the fragments by id or idref.
 *
 * @{
 */

sg_ctt **sg_root_get_ctt_by_ser(sg_root *rt, kchar *serid)
{
    kuint cnt = 0, idx, i;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_ctt *ctt, **cttlst;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[CTT];
        entry = hdr->next;
        while (entry != hdr) {
            kint srcnt;
            ServiceReference_rec *sr_rec;
            ctt = FIELD_TO_STRUCTURE(entry, sg_ctt, cls_le);
            entry = entry->next;

            if ((!ctt->arr) || (!ctt->arr->ServiceReference)) {
                continue;
            }

            sr_rec = ctt->arr->ServiceReference->arr;
            for (i = 0; i < ctt->arr->ServiceReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, serid)) {
                    cnt++;
                }
            }
        }
    }

    if (!cnt) {
        return knil;
    }

    cttlst = (sg_ctt**)kmem_alloc((cnt + 1) * sizeof(sg_ctt*));
    cttlst[cnt] = knil;
    idx = 0;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[CTT];
        entry = hdr->next;

        while (entry != hdr) {
            kint srcnt;
            ServiceReference_rec *sr_rec;
            ctt = FIELD_TO_STRUCTURE(entry, sg_ctt, cls_le);
            entry = entry->next;

            if (!ctt->arr->ServiceReference) {
                continue;
            }

            sr_rec = ctt->arr->ServiceReference->arr;
            for (i = 0; i < ctt->arr->ServiceReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, serid)) {
                    cttlst[idx++] = ctt;
                }
            }
        }
    }

    return cttlst;
}


sg_acc **sg_root_get_acc_by_sch(sg_root *rt, kchar *schid)
{
    kuint cnt = 0, idx, i;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_acc *acc, **acclst;


    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[ACC];
        entry = hdr->next;
        while (entry != hdr) {
            kint srcnt;
            ScheduleReference_rec *sr_rec;
            acc = FIELD_TO_STRUCTURE(entry, sg_acc, cls_le);
            entry = entry->next;

            if (!acc->arr->ScheduleReference) {
                continue;
            }

            sr_rec = acc->arr->ScheduleReference->arr;
            for (i = 0; i < acc->arr->ScheduleReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, schid)) {
                    cnt++;
                }
            }
        }
    }

    if (!cnt) {
        return knil;
    }

    acclst = (sg_acc**)kmem_alloc((cnt + 1) * sizeof(sg_acc*));
    acclst[cnt] = knil;
    idx = 0;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[ACC];
        entry = hdr->next;
        while (entry != hdr) {

            kint srcnt;
            ScheduleReference_rec *sr_rec;
            acc = FIELD_TO_STRUCTURE(entry, sg_acc, cls_le);
            entry = entry->next;

            if (!acc->arr->ScheduleReference) {
                continue;
            }

            sr_rec = acc->arr->ScheduleReference->arr;
            for (i = 0; i < acc->arr->ScheduleReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, schid)) {
                    acclst[idx++] = acc;
                }
            }
        }
    }

    return acclst;
}


sg_acc **sg_root_get_acc_by_ser(sg_root *rt, kchar *serid)
{
    kuint cnt = 0, idx, i;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_acc *acc, **acclst;


    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[ACC];
        entry = hdr->next;
        while (entry != hdr) {
            kint srcnt;
            ServiceReference_rec *sr_rec;
            acc = FIELD_TO_STRUCTURE(entry, sg_acc, cls_le);
            entry = entry->next;

            if (!acc->arr->ServiceReference) {
                continue;
            }

            sr_rec = acc->arr->ServiceReference->arr;
            for (i = 0; i < acc->arr->ServiceReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, serid)) {
                    cnt++;
                }
            }
        }
    }

    if (!cnt) {
        return knil;
    }

    acclst = (sg_acc**)kmem_alloc((cnt + 1) * sizeof(sg_acc*));
    acclst[cnt] = knil;
    idx = 0;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[ACC];
        entry = hdr->next;
        while (entry != hdr) {

            kint srcnt;
            ServiceReference_rec *sr_rec;
            acc = FIELD_TO_STRUCTURE(entry, sg_acc, cls_le);
            entry = entry->next;

            if (!acc->arr->ServiceReference) {
                continue;
            }

            sr_rec = acc->arr->ServiceReference->arr;
            for (i = 0; i < acc->arr->ServiceReference->ecnt; i++) {
                if (streq(sr_rec[i].attr.idRef, serid)) {
                    acclst[idx++] = acc;
                }
            }
        }
    }

    return acclst;
}
sg_sdu *sg_root_get_sdu_by_id(sg_root *rt, kuint tid)
{
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_sdu *sdu;


    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[SDU];
        entry = hdr->next;
        while (entry != hdr) {
            sdu = FIELD_TO_STRUCTURE(entry, sg_sdu, cls_le);
            entry = entry->next;

            if (sdu->tid == tid) {
                return sdu;
            }
        }
    }
    return knil;
}
/** @} */


/**
 * @brief Search SG root by Fragment Transport ID.
 *
 * @param rt SG root.
 * @param ftid Fragment Transport ID.
 *
 * @return sg_zbd* for success, knil for fail.
 */
sg_zbd *sg_root_find_file_by_ftid(sg_root *rt, kuint ftid)
{
    sg_zbd *zbd;
    kint i;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;


    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        for (i = SER; i < TOP; i++) {
            hdr = &fblk->fhdrs[i];
            entry = hdr->next;
            while (entry != hdr) {
                zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, cls_le);
                entry = entry->next;

                if (zbd->tid == ftid) {
                    return zbd;
                }
            }
        }
    }
    return knil;
}

/**
 * @brief Find fragment specified by ID or ftid.
 *
 * @param rt SG root.
 * @param cls specified the class queue will search. if set to be -1, search all
 * @param ftid Fragment Transport ID.
 * @param id fragment ID.
 *
 * @return sg_zbd* for success, knil for fail.
 */
sg_zbd *sg_root_find_file(sg_root *rt, kint cls, kuint *ftid, kchar *id)
{
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_zbd *zbd;

    if (cls >= TOP) {
        kerror(("sg_root_find_file:bad cls:%d\n", cls));
        return knil;
    }

    if (-1 == cls) {
        hdr = &rt->allhdr;
        entry = hdr->next;
        while (entry != hdr) {
            zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, all_le);
            entry = entry->next;

            if (ftid && zbd->tid == (*ftid)) {
                return zbd;
            } else if (id && streq(id, zbd->attr.id)) {
                return zbd;
            }
        }
        return knil;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[cls];
        entry = hdr->next;
        while (entry != hdr) {
            zbd = FIELD_TO_STRUCTURE(entry, sg_zbd, cls_le);
            entry = entry->next;

            if (ftid && zbd->tid == (*ftid)) {
                return zbd;
            } else if (id && streq(id, zbd->attr.id)) {
                return zbd;
            }
        }
    }
    return knil;
}

kint sg_root_del_cache_rec(sg_root *rt, ef_cache_rec *rec)
{
    kchar path[IV_PATH_MAX_SIZE];
    sg_root_get_cache_rec_item_path(rt, rec, path);
    kvfs_remove(path);
    return 0;
}

kbool sg_root_chk_cache_rec(sg_root *rt, ef_cache_rec *rec)
{
    kchar path[IV_PATH_MAX_SIZE];
    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff;
    sg_root_get_cache_rec_item_path(rt, rec, path);
    if (!kvfs_exist(path)) {
        return kfalse;
    }
    return ktrue;
}

kint sg_root_get_cache_rec_dat(sg_root *rt, ef_cache_rec *rec, kchar **data, kint *datalen)
{
    kchar *dat = knil, path[IV_PATH_MAX_SIZE];
    kbean file;
    kint datlen = 0x7FFFFFFF;
    sg_root_get_cache_rec_item_path(rt, rec, path);

    file = kvfs_open(path, "rb", 0);
    if (!file) {
        kerror(("sg_root_get_cache_rec_dat: fopen fail: %s\n", path));
        return -1;
    }
    kvfs_read(file, &dat, &datlen);
    kvfs_close(file);

    *data = dat;
    *datalen = datlen;
    return 0;
}

kint sg_root_zbd_to_rec(sg_root *rt, sg_zbd *zbd, ef_cache_rec *rec)
{
    rec->flg = zbd->flg;
    rec->cls = zbd->cls;
    rec->tid = zbd->tid;
    rec->freq = zbd->frequency;

    rec->attr.validFrom = zbd->attr.validFrom;
    rec->attr.validTo = zbd->attr.validTo;
    rec->attr.version = zbd->attr.version;
    return 0;
}

kvoid sg_root_rec_to_zbd(sg_root *rt, ef_cache_rec *rec, sg_zbd *zbd)
{
    zbd->flg = rec->flg;
    zbd->cls = rec->cls;
    zbd->tid = rec->tid;
    zbd->frequency = rec->freq;

    zbd->attr.validFrom = rec->attr.validFrom;
    zbd->attr.validTo = rec->attr.validTo;
    zbd->attr.version = rec->attr.version;

    sg_root_get_cache_rec_dat(rt, rec, &zbd->dat.buf, &zbd->dat.len);
}

kint sg_root_get_cache_rec_item_path(sg_root *rt, ef_cache_rec *rec, kchar *path)
{
    kchar *prefix;

    switch (rec->cls) {
        case CLS_SGDD:
            prefix = "sdd";
            break;

        case CLS_SGDU:
            prefix = "sdu";
            break;

        case CLS_SERV:
            prefix = "ser";
            break;

        case CLS_SCHE:
            prefix = "sch";
            break;

        case CLS_COTT:
            prefix = "ctt";
            break;

        case CLS_PCIT:
            prefix = "pci";
            break;

        case CLS_PCDA:
            prefix = "pcd";
            break;

        case CLS_PCCH:
            prefix = "pcc";
            break;

        case CLS_PVDA:
            prefix = "pvd";
            break;

        case CLS_DIRE:
            prefix = "fdt";
            break;

        case CLS_IADA:
            prefix = "iad";
            break;

        case CLS_ACCE:
            prefix = "acc";
            break;

        case CLS_SDPF:
            prefix = "sdp";
            break;

        default:
            return -1;
    }

    sprintf(path, "%s%c%08x%cfragments%c%s%08x", rt->mgr->env->data_dir, __dir_delimeter,
            rt->pvdr->id, __dir_delimeter, __dir_delimeter, prefix, rec->tid);

    return 0;
}

kint sg_root_get_zbd_path(sg_zbd *zbd, kchar *path)
{
    kchar *prefix;

    switch (zbd->cls) {
        case CLS_SGDD:
            prefix = "sdd";
            break;

        case CLS_SGDU:
            prefix = "sdu";
            break;

        case CLS_SERV:
            prefix = "ser";
            break;

        case CLS_SCHE:
            prefix = "sch";
            break;

        case CLS_COTT:
            prefix = "ctt";
            break;

        case CLS_PCIT:
            prefix = "pci";
            break;

        case CLS_PCDA:
            prefix = "pcd";
            break;

        case CLS_PCCH:
            prefix = "pcc";
            break;

        case CLS_PVDA:
            prefix = "pvd";
            break;

        case CLS_DIRE:
            prefix = "fdt";
            break;

        case CLS_IADA:
            prefix = "iad";
            break;

        case CLS_ACCE:
            prefix = "acc";
            break;

        case CLS_SDPF:
            prefix = "sdp";
            break;

        default:
            return -1;
    }

    sprintf(path, "%s%c%08x%cfragments%c%s%08x", zbd->rt->mgr->env->data_dir, __dir_delimeter,
            zbd->rt->pvdr->id, __dir_delimeter, __dir_delimeter, prefix, zbd->tid);

    return 0;
}

kint sg_root_rm_cache_file(sg_zbd *zbd)
{
    kchar path[IV_PATH_MAX_SIZE] = { 0 };
    sg_root_get_zbd_path(zbd, path);
    klog(("sg_root_rm_cache_file:%s\n", path));
    kvfs_remove(path);
    return 0;
}

kint sg_root_save_cache_file(sg_zbd *zbd)
{
    kchar path[IV_PATH_MAX_SIZE] = { 0 };
    sg_root_get_zbd_path(zbd, path);
    klog(("sg_root_save_to_cache:%s\n", path));
    ksal_file_save(path, zbd->dat.buf, zbd->dat.len);
    return 0;
}

sg_fdt* sg_root_get_dir_by_freq(sg_root *rt, kuint freq)
{
    sg_fdt *fdt;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[FDT];
        entry = hdr->next;
        while (entry != hdr) {
            fdt = FIELD_TO_STRUCTURE(entry, sg_fdt, cls_le);
            entry = entry->next;

            if (fdt->frequency == freq) {
                return fdt;
            }
        }
    }
    return knil;
}

K_freq *sg_root_freq_blk_find(sg_root *rt, kuint frequency)
{
    K_dlist_entry *fb_entry, *fb_hdr;
    K_freq *fblk;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        if (fblk->acc->frequency == frequency) {
            return fblk;
        }
    }
    return knil;
}

/**
 * @brief Stop or Start transition after frequency changed
 */
static kint om_freq_changed(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kuint oldFreq = (kuint)a_ar1;
    kuint newFreq = (kuint)a_ar2;

    K_dlist_entry *fb_entry, *fb_hdr;
    K_freq *fblk;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    kassert(newFreq);

    if (oldFreq == newFreq) {
        return -1;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        if (fblk->acc->frequency == newFreq) {
            /*
             * now the SG working on a sertain frequency,
             * try to receive the iad info in background
             * for all the sg root
             */
            klog(("iad : will search for frequency %d\n", newFreq));
            sg_start_iad_search_for_frequency(rt, newFreq, kfalse);
        }

        if (fblk->acc->frequency == oldFreq) {
            klog(("iad : stop search for frequency %d\n", oldFreq));

            /* stop auto sync on old frequency */
            sg_root_abort_sync(rt);

            /* stop idata transition on old frequency */
            sg_root_abort_iad(rt);
        }
    }
    return 0;
}

/**
 * @brief called when frequency change
 *
 * @param frequency, frequency switch to, 0 means not current frequency
 */
K_freq* sg_root_frequency_switch(sg_root *rt, kuint frequency)
{
    K_dlist_entry *fb_entry, *fb_hdr;
    K_freq *fblk;
    kuint curFrequency = rt->curFrequency;

    rt->curFrequency = frequency;

    if (rt->freq.current && rt->freq.current->acc->frequency == frequency) {
        return rt->freq.current;
    }

    if (frequency == 0) {
        rt->freq.current = knil;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        if (fblk->acc->frequency == frequency) {
            rt->freq.current = fblk;

            /* When frequency changed, should stop data transition not in current frequency */
            kmsg_post(rt->worker_thread, KRTM_FREQ_CHG, rt, (kvoid*)curFrequency, (kvoid*)frequency, knil);
            return fblk;
        }
    }
    return knil;
}
kint sg_root_frequency_add(sg_root *rt, K_pvdr_acc *acc)
{
    kint i;
    K_freq *fblk;
    K_dlist_entry *fb_entry, *fb_hdr;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;
        if (fblk->acc == acc) {
            /* if the pvdr_acc already in list, do not add twice */
            return 0;
        }
    }

    fblk = (K_freq*)kmem_alloz(sizeof(K_freq));
    fblk->acc = acc;

    insert_dlist_tail_entry(&rt->frqhdr, &fblk->le);

    for (i = 0; i < TOP; i++) {
        init_dlist_head(&(fblk->fhdrs[i]));
    }

    init_dlist_head(&fblk->data_ses.subhdr);

    return 0;
}

