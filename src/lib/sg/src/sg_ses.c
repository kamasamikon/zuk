#include <string.h>
#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include <kstr.h>

#include "xmldoc.h"

#include <ktmr.h>

#include "sg_mgr2.h"
#include "sg_ses.h"

#include "ivisa_sg_config.h"
#include "ivisa_sg_filerev.h"

typedef kint (*SES_OBJECTLIST_CALLBACK)(sg_ses *ses, kint iHandle, kint iStatus,
        IV_OBJECT_LIST_ATTRIBUTE * pObjectList_attibute, IV_OBJECT_INFO * pInfoLisHead,
        kint iObjectNum, void * vpUserData0, void * vpUserData1, void * vpUserData2, void * vpUserData3);
typedef kint (*SES_TRANSFEROBJECT_CALLBACK)(sg_ses *ses, kint iHandle, kint iStatus,
        kuint uiObjectID, kchar* pObjectName, kchar * pObjectUrl, kint iObjectVersion,
        kuchar * pDataBuffer, kuint uiObjectLength, kchar * pSavePathName, kchar * pContentEncoding,
        void * vpUserData0, void * vpUserData1, void * vpUserData2, void * vpUserData3);

extern kchar __bdsg_base_url[256];

#define SES_MGC                 0x19771982

static kuint __g_ses_next_guid = 1394;

static kint sg_ses_get_obj_filter(sg_ses *ses);
static kint sg_ses_free_obj_filter(sg_ses *ses, kuint index);
static kint sg_ses_add_obj(sg_ses *ses, kuint index, kint hdl);
static kint sg_ses_try_done(sg_ses *ses);
static kint sg_ses_try_auto_stop(sg_ses *ses);
static kint sg_ses_try_auto_del(sg_ses *ses);

static kbool sg_ses_valid(sg_ses *ses, kuint guid)
{
    if ((!ses) || (ses->mgc != SES_MGC) || ((guid) && (ses->guid != guid))) {
        kerror(("sg_ses_valid: bad ses\n"));
        return kfalse;
    }
    return ktrue;
}
/**
 * \brief Create a session and init with \a tmode, and \a sei.
 *
 * Only setup the base structure.
 */
kint sg_ses_new(sg_ses **ses, kint tmode, kvoid *sei)
{
    kuint flg = SES_USED;
    if (!(*ses)) {
        (*ses) = (sg_ses*)kmem_alloc(sizeof(sg_ses));
        flg |= SES_DYN_MEM;
    }
    if ((*ses)) {
        memset((*ses), 0, sizeof(sg_ses));
        (*ses)->flg = flg;
        (*ses)->tmode = tmode;
        init_dlist_head(&(*ses)->le);
        if (sei) {
            if (SG_TMODE_FLUTE == tmode) {
                (*ses)->sei.alc = sei;
            } else if (SG_TMODE_MOT == tmode) {
                (*ses)->sei.mot = sei;
            } else {
                (*ses)->sei.bdsg_url = kstr_dup((kchar*)sei);
                klog(("bdsg_url:%s\n", (*ses)->sei.bdsg_url));
            }
        }
        (*ses)->mgc = SES_MGC;
        (*ses)->guid = __g_ses_next_guid++;

        klog(("ss-ses:new : %x\n", (*ses)));
        return 0;
    }
    kerror(("sg_ses_new failed\n"));
    return -1;
}

kint sg_ses_del(sg_ses *ses)
{
    klog(("ss-ses:del : %x\n", ses));
    if (!sg_ses_valid(ses, 0)) {
        return 0;
    }

    ses->mgc = 0;
    ses->guid = 0;

    if (!kflg_chk(ses->flg, SES_USED)) {
        kerror(("sg_ses_del: not valid ses\n"));
        if (kflg_chk(ses->flg, SES_DYN_MEM)) {
            kmem_free(ses);
        }
        return 0;
    }
    remove_dlist_entry(&ses->le);
    sg_ses_stop_session(ses);

    if (SG_TMODE_FLUTE == ses->tmode) {
        kmem_free(ses->sei.alc);
    } else if (SG_TMODE_MOT == ses->tmode) {
        kmem_free(ses->sei.mot);
    } else {
        kmem_free(ses->sei.bdsg_url);
    }

    if (kflg_chk(ses->flg, SES_DYN_MEM)) {
        memset(ses, 0, sizeof(sg_ses));
        kmem_free(ses);
    }
    klog(("ss-ses : del done\n"));
    return 0;
}

/**
 * \brief Make the low layer to prepare data transfer.
 *
 * For MOT and FLUTE, transfer layer will start it's own session, for BDIR
 * no additional work should be done.
 *
 * If transfer layer start successfully, mark this session to be SES_WORK.
 * If already start, will not re-start it.
 */
kint sg_ses_start_session(sg_ses *ses, kbool withDir)
{
    kint hdl;
    klog(("ss-ses:run : %x\n", ses));
    if (!sg_ses_valid(ses, 0)) {
        return -1;
    }
    if (kflg_chk(ses->flg, SES_WORK)) {
        kerror(("sg_ses_start_session: already started\n"));
        return 0;
    }

    ses->req.out = ses->req.ret = 0;
    if (SG_TMODE_FLUTE == ses->tmode) {
        hdl = Iv_FileRev_StartSession(ses->sei.alc,
                withDir ? IV_SESSION_WITHDIR_TYPE : IV_SESSION_WITHOUTDIR_TYPE);
    } else if (SG_TMODE_MOT == ses->tmode) {
        hdl = Iv_FileRev_StartSession(ses->sei.mot,
                withDir ? IV_SESSION_WITHDIR_TYPE : IV_SESSION_WITHOUTDIR_TYPE);
    } else {
        /* XXX avoid assert check */
        hdl = (kint)"hdl";
    }

    klog(("Iv_FileRev_StartSession:%x\n", hdl));
    if (hdl) {
        ses->hdl = hdl;
        kflg_set(ses->flg, SES_WORK);
        return 0;
    }

    kerror(("sg_ses_start_session: failed\n"));
    return -1;
}

/**
 * \internal
 * \brief Stop session timer callback.
 *
 * In some transfer layer, can not stop session in it's callback, so should
 * use timer, which run in timer thread, to do it.
 */
static kint sg_ses_stop_ses_tmr_cbk(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3)
{
    sg_ses *ses = ar0;
    if (!sg_ses_valid(ses, 0)) {
        return -1;
    }
    sg_ses_stop_session(ses);
    return 0;
}
/**
 * \brief Stop all transfer layer process.
 */
kint sg_ses_stop_session(sg_ses *ses)
{
    kint ret = -1;

    klog(("ss-ses:zhi : %x\n", ses));
    if (!sg_ses_valid(ses, 0)) {
        return 0;
    }
    if (!kflg_chk(ses->flg, SES_WORK)) {
        kerror(("sg_ses_stop_session: already stopped\n"));
        return 0;
    }

#if 1
    /* stop dir request */
    if (ses->filter.dir.hdl && kflg_chk(ses->filter.dir.flg, SES_WORK)) {
        ktmr_kill(ses->filter.dir.tmr);
        klog(("52 Iv_FileRev_StopSession:%x\n", ses->filter.dir.hdl));

        if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
            Iv_FileRev_StopObjectListFilter(ses->filter.dir.hdl);
        } else {
            /* FIXME */
            Http_CancelWebObject(ses->filter.dir.hdl);
        }

        klog(("ss-ses:del dir stopped : %x\n", ses));
        ses->filter.dir.hdl = 0;
    }

    /* stop obj request */
    if (ses->filter.obj.arr) {
        kuint i;
        for (i = 0; i < ses->filter.obj.cnt; i++) {
            ktmr_kill(ses->filter.obj.arr[0].tmr);
            if (ses->filter.obj.arr[i].hdl && kflg_chk(ses->filter.obj.arr[i].flg, SES_WORK)) {
                klog(("61 Iv_FileRev_StopTransferObjectFilter:%x\n", ses->filter.obj.arr[i].hdl));

                if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
                    /*
                     * XXX When a request cancelled, is the callback called with errror
                     */
                    Iv_FileRev_StopTransferObjectFilter(ses->hdl, ses->filter.obj.arr[i].hdl);
                } else {
                    /* FIXME */
                    Http_CancelWebObject(ses->filter.obj.arr[i].hdl);
                }

                klog(("ss-ses OBJ stopped:%x\n", ses->filter.obj.arr[i].hdl));
                ses->filter.obj.arr[i].hdl = 0;
            }
        }
        kmem_free(ses->filter.obj.arr);
        ses->filter.obj.arr = knil;
    }

    if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
        ret = Iv_FileRev_StopSession(ses->hdl);
    } else {
        /* XXX for bdir ses->hdl == "hdl", needn't release */
        ret = 0;
    }

    if (!ret) {
        kflg_clr(ses->flg, SES_WORK);
        ses->hdl = 0;
        klog(("sg-ses:zhi done:%x\n", ses));
        return 0;
    } else {
        kerror(("sg_ses_stop_session: Iv_FileRev_StopSession failed\n"));
        return -1;
    }
#else
    ktmr_set(0, 0, kfalse, (TMR_CBK)sg_ses_stop_ses_tmr_cbk, ses, knil, knil, knil);
    return 0;
#endif
}

static sg_fr_filter *sg_ses_find_obj_filter(sg_ses *ses, kint hdl)
{
    kuint i;
    sg_fr_filter *filter;
    if (!sg_ses_valid(ses, 0)) {
        return knil;
    }
    for (i = 0; i < ses->filter.obj.cnt; i++) {
        filter = &ses->filter.obj.arr[i];
        if (kflg_chk(filter->flg, SES_USED)) {
            if (filter->hdl == hdl) {
                return filter;
            }
        }
    }
    kerror(("sg_ses_find_obj_filter: not filter for hdl : %x\n", hdl));
    return knil;
}

static kint sg_ses_cancel_dir_callback(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3)
{
    sg_ses *ses = ar0;

    if (!sg_ses_valid(ses, 0)) {
        return -1;
    }

    if (!kflg_chk(ses->filter.dir.flg, SES_DIR)) {
        ses->req.ret++;
        klog(("Iv_FileRev_StopObjectListFilter:%x\n", ses->filter.dir.hdl));

        if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
            Iv_FileRev_StopObjectListFilter(ses->filter.dir.hdl);
        } else {
            Http_CancelWebObject(ses->filter.dir.hdl);
        }

        ses->filter.dir.hdl = 0;
        sg_ses_try_done(ses);
        sg_ses_try_auto_stop(ses);
    }
    return 0;
}

static kint sg_ses_cancel_obj_callback(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3)
{
    sg_ses *ses = ar0;
    kint index = (kint)ar1;

    if (!sg_ses_valid(ses, 0)) {
        return -1;
    }

    if (!kflg_chk(ses->filter.obj.arr[index].flg, SES_OBJ)) {
        ses->req.ret++;
        klog(("Iv_FileRev_StopTransferObjectFilter:%x, %x\n", ses->hdl, ses->filter.obj.arr[index].hdl));

        if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
            /*
             * XXX When a request cancelled, is the callback called with errror
             */
            Iv_FileRev_StopTransferObjectFilter(ses->hdl, ses->filter.obj.arr[index].hdl);
        } else {
            Http_CancelWebObject(ses->filter.obj.arr[index].hdl);
        }

        ses->filter.obj.arr[index].hdl = 0;
        sg_ses_try_done(ses);
        if (0 == sg_ses_try_auto_stop(ses)) {
            return 0;
        }
    }
    sg_ses_free_obj_filter(ses, index);

    return 0;
}

kint sg_ses_try_end_session(sg_ses *ses)
{
    if (!sg_ses_valid(ses, (kuint)0)) {
        return -1;
    }

    // if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
    sg_ses_try_done(ses);
    sg_ses_try_auto_stop(ses);
    // }
    return 0;
}

static kint sg_ses_dir_cbk(kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    kint ret;
    sg_ses *ses = (sg_ses*)vpUserData0;

    if (!sg_ses_valid(ses, (kuint)0)) {
        return -1;
    }

    klog(("ss-ses:dir_cbk : %x\n", ses));
    ktmr_kill(ses->filter.dir.tmr);
    kflg_set(ses->filter.dir.flg, SES_DIR);
    kflg_clr(ses->filter.dir.flg, SES_WORK);
    ses->req.ret++;
    ret = ((SES_OBJECTLIST_CALLBACK)ses->filter.dir.cbk)(ses, iHandle, iStatus, pObjectList_attibute, pInfoLisHead, iObjectNum,
            ses->filter.dir.usr0, ses->filter.dir.usr1, ses->filter.dir.usr2, ses->filter.dir.usr3);

    /* XXX All the transfer layer session is single request */
    return ret;
}

static kint sg_ses_obj_cbk(kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    sg_ses *ses = (sg_ses*)vpUserData0;
    kint ret, index = (kint)vpUserData2;

    if (!sg_ses_valid(ses, (kuint)vpUserData1)) {
        return -1;
    }

    klog(("ss-ses:obj_cbk : ses= %x,toi= %d\n", ses,uiObjectID));
    ktmr_kill(ses->filter.obj.arr[index].tmr);
    kflg_set(ses->filter.obj.arr[index].flg, SES_OBJ);
    kflg_clr(ses->filter.obj.arr[index].flg, SES_WORK);
    ses->req.ret++;

    klog(("befroe callback: ses = %x,out(%d), ret(%d), cbk(%x)\n",ses, ses->req.out, ses->req.ret, ses->done.cbk));
    ret = ((SES_TRANSFEROBJECT_CALLBACK)ses->filter.obj.arr[index].cbk)(ses, iHandle, iStatus,  uiObjectID, pObjectName,
            pObjectUrl, iObjectVersion, pDataBuffer, uiObjectLength, pSavePathName, pContentEncoding,
            ses->filter.obj.arr[index].usr0, ses->filter.obj.arr[index].usr1,
            ses->filter.obj.arr[index].usr2, ses->filter.obj.arr[index].usr3);

    /* XXX All the transfer layer session is single request */
    return ret;
}

kint sg_ses_get_dir(sg_ses *ses, kuint tmo, kvoid *cbk, kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3)
{
    kint rv, dirhdl;
    if (!sg_ses_valid(ses, 0)) {
        return -1;
    }
    if (!kflg_chk(ses->flg, SES_WORK)) {
        kerror(("sg_ses_get_dir: session not work\n"));
        return -1;
    }
    klog(("ss-ses:get_dir : %x\n", ses));
    if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
        kassert(ses->hdl);
    }

    ses->filter.dir.cbk = cbk;
    ses->filter.dir.usr0 = usr0;
    ses->filter.dir.usr1 = usr1;
    ses->filter.dir.usr2 = usr2;
    ses->filter.dir.usr3 = usr3;

    if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
        rv = Iv_FileRev_SetObjectListFilter(ses->hdl, sg_ses_dir_cbk, (kvoid*)ses, (kvoid*)ses->guid, knil, knil);
        dirhdl = ses->hdl;
    } else {
        kchar url[1024];
        if (strstr(ses->sei.bdsg_url, "?")) {
            sprintf(url, "%s&Type=SGDD", ses->sei.bdsg_url);
        } else {
            sprintf(url, "%s?Type=SGDD", ses->sei.bdsg_url);
        }

        dirhdl = Http_GetWebObject(ses, url, (Iv_TransferObject_CALLBACK)cbk, knil, 0x87654321,
                usr0, usr1, usr2, usr3);

        rv = IVEC_SUCCESS;
    }

    klog(("Sent command to get object list, session handle = %d\n", ses->hdl));
    if (rv) {
        kerror(("sg_ses_get_dir: Iv_FileRev_SetObjectListFilter failed : %d\n", rv));
        return -1;
    }

    klog(("Iv_FileRev_SetObjectListFilter:%x\n", ses->hdl));
    ses->filter.dir.hdl = ses->hdl;
    kflg_set(ses->filter.dir.flg, SES_WORK);
    ses->req.out++;
    if (tmo) {
        if (ses->filter.dir.tmr) {
            ktmr_kill(ses->filter.dir.tmr);
        }
        ses->filter.dir.tmr = ktmr_set(0, tmo, kfalse, sg_ses_cancel_dir_callback, ses, knil, knil, knil);
    }

    return 0;
}

kint sg_ses_get_obj(sg_ses *ses, kuint tmo, kuint objid, kuint msk, kvoid *cbk, kchar *objname,
        kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3)
{
    kint hdl, index;
    klog(("ss-ses:get_obj : %x\n", ses));
    if (!kflg_chk(ses->flg, SES_WORK)) {
        kerror(("sg_ses_get_obj: session not work\n"));
        return -1;
    }
    index = sg_ses_get_obj_filter(ses);

    ses->filter.obj.arr[index].cbk = cbk;
    ses->filter.obj.arr[index].usr0 = usr0;
    ses->filter.obj.arr[index].usr1 = usr1;
    ses->filter.obj.arr[index].usr2 = usr2;
    ses->filter.obj.arr[index].usr3 = usr3;

    klog(("sg_ses_get_obj: set filter: ses =%x, objid = %d, objname = %s\n", ses,objid,objname));
    if ((ses->tmode == SG_TMODE_MOT) || (ses->tmode == SG_TMODE_FLUTE)) {
        hdl = Iv_FileRev_SetTransferObjectFilter(ses->hdl, objid, msk, sg_ses_obj_cbk, objname,
                knil, (kvoid*)ses, (kvoid*)ses->guid, (kvoid*)index, knil);

    } else {
        kchar url[IV_PATH_MAX_SIZE];
        if (strstr(ses->sei.bdsg_url, "?")) {
            sprintf(url, "%s&Type=TOI&TOI=%d", ses->sei.bdsg_url, objid);
        } else {
            sprintf(url, "%s?Type=TOI&TOI=%d", ses->sei.bdsg_url, objid);
        }

        hdl = Http_GetWebObject(ses, url, cbk, knil, 0x87654321,
                usr0, usr1, usr2, usr3);
    }

    if (!hdl) {
        kerror(("sg_ses_get_obj: Iv_FileRev_SetTransferObjectFilter failed for oid: %d\n", objid));
        sg_ses_free_obj_filter(ses, index);
        return -1;
    }

    klog(("Iv_FileRev_SetTransferObjectFilter(end):ses=%x,handle %x\n", ses, hdl));
    ses->req.out++;
    if (tmo) {
        if (ses->filter.obj.arr[index].tmr) {
            ktmr_kill(ses->filter.obj.arr[index].tmr);
        }
        ses->filter.obj.arr[index].tmr = ktmr_set(0, tmo, kfalse, sg_ses_cancel_obj_callback,
                (kvoid*)ses, (kvoid*)index, (kvoid*)knil, (kvoid*)knil);
    }
    sg_ses_add_obj(ses, index, hdl);
    return 0;
}

static kint sg_ses_get_obj_filter(sg_ses *ses)
{
    kuint i, found = kfalse;
    kvoid *arr;
    for (i = 0; i < ses->filter.obj.cnt; i++) {
        if (!kflg_chk(ses->filter.obj.arr[i].flg, SES_USED)) {
            ses->filter.obj.arr[i].flg = SES_USED;
            return i;
        }
    }
    ses->filter.obj.cnt += 20;
    arr = kmem_alloz(ses->filter.obj.cnt * sizeof(sg_fr_filter));
    if (ses->filter.obj.arr) {
        memcpy(arr, ses->filter.obj.arr, (ses->filter.obj.cnt - 20) * sizeof(sg_fr_filter));
        kmem_free(ses->filter.obj.arr);
    }
    ses->filter.obj.arr = arr;

    ses->filter.obj.arr[i].flg = SES_USED;
    return i;
}

static kint sg_ses_free_obj_filter(sg_ses *ses, kuint index)
{
    kassert(ses->filter.obj.cnt > index);
    kflg_clr(ses->filter.obj.arr[index].flg, SES_USED);
    ses->filter.obj.arr[index].hdl = 0;
    return 0;
}

static kint sg_ses_add_obj(sg_ses *ses, kuint index, kint hdl)
{
    kassert(ses->filter.obj.cnt > index);
    kassert(kflg_chk(ses->filter.obj.arr[index].flg, SES_USED));
    kflg_set(ses->filter.obj.arr[index].flg, SES_WORK);
    ses->filter.obj.arr[index].hdl = hdl;
    return 0;
}

static kint sg_ses_del_obj(sg_ses *ses, kuint index)
{
    kassert(ses->filter.obj.cnt > index);
    kassert(kflg_chk(ses->filter.obj.arr[index].flg, SES_USED | SES_WORK));
    kflg_clr(ses->filter.obj.arr[index].flg, SES_WORK);
    ses->filter.obj.arr[index].hdl = 0;
    return 0;
}

static kint sg_ses_try_done(sg_ses *ses)
{
    klog(("sg_ses_try_call_done:ses= %x,out(%d), ret(%d), cbk(%x)\n", ses,ses->req.out, ses->req.ret, ses->done.cbk));
    if ((ses->req.out == ses->req.ret) && (ses->done.cbk)) {
        ses->done.cbk(ses->done.usr0, ses->done.usr1, ses->done.usr2, ses->done.usr3);
    }
    return 0;
}
static kint sg_ses_try_auto_stop(sg_ses *ses)
{
    if (!kflg_chk(ses->flg, SES_AUTO_STOP)) {
        return -1;
    }
    if (ses->req.out == ses->req.ret) {
        if (sg_ses_stop_session(ses)) {
            return -1;
        }
        return sg_ses_try_auto_del(ses);
    }
    return -1;
}

static kint sg_ses_try_auto_del(sg_ses *ses)
{
    if (!kflg_chk(ses->flg, SES_AUTO_DEL)) {
        return -1;
    }
    if (!kflg_chk(ses->flg, SES_WORK)) {
        return sg_ses_del(ses);
    }
    return -1;
}

kint sg_ses_set_done_cbk(sg_ses *ses, SES_DONE_CBK cbk, kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3)
{
    ses->done.cbk = cbk;
    ses->done.usr0 = usr0;
    ses->done.usr1 = usr1;
    ses->done.usr2 = usr2;
    ses->done.usr3 = usr3;
    return 0;
}

