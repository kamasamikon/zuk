
#ifndef __ESGSES_H__
#define __ESGSES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ivisa_sg_filerev.h"

#define SES_USED        0x00000001      // idle, this item is not used,
#define SES_FAIL        0x00000002
#define SES_TOUT        0x00000004
#define SES_WORK        0x00000008      // working
#define SES_DIR            0x00000020        // DIR CALLBACK called
#define SES_OBJ            0x00000040        // OBJ CALLBACK called
#define SES_DYN_MEM        0x00000080
#define SES_AUTO_STOP    0x00000100        // Auto Stop the session when package is received
#define SES_AUTO_DEL    0x00000200        // Auto del the session when package is received

    typedef struct _sg_fr_filter {
        kuint flg;
        kint hdl;
        kbean tmr;
        kvoid *cbk;
        kvoid *usr0, *usr1, *usr2, *usr3;

        // kvoid *file;    // received file
    } sg_fr_filter;

    /**
     * @name sg_ses_entry_info
     * @brief FLUTE MOT and B-DIR SG can be exist at same time. So defined as structure
     */
    typedef struct _sg_ses_entry_info {
        IV_FLUTE_ALC_SESSION_ENTRYINFO *alc;
        IV_DAB_SESSION_ENTRYINFO *mot;
        kchar *bdsg_url;
    } sg_ses_entry_info;

    // XXX called before SES stop
    // XXX called after SES callback return
    struct _sg_ses;
    typedef kint (*SES_DONE_CBK)(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);

    typedef struct _sg_ses {
        kuint mgc;
        kuint guid;

        K_dlist_entry le;

        kint hdl;
        kuint flg;

        kint tmode;

        struct {
            SES_DONE_CBK cbk;
            kvoid *usr0, *usr1, *usr2, *usr3;
        } done;

        sg_ses_entry_info sei;

        struct {
            kushort out, ret;
            kushort dir_out;
            kushort dir_ret;
            kushort obj_out;
            kushort obj_ret;
        } req;

        struct {
            sg_fr_filter dir;

            struct {
                kuint cnt;
                sg_fr_filter *arr;
            } obj;
        } filter;
    } sg_ses;

    /////////////////////////////////////////////////////////////////////////

    kint sg_ses_new(sg_ses **ses, kint prot, kvoid *sei);
    kint sg_ses_del(sg_ses *ses);

    kint sg_ses_start_session(sg_ses *ses, kbool withDir);
    kint sg_ses_stop_session(sg_ses *ses);

    kint sg_ses_get_dir(sg_ses *ses, kuint tmo, kvoid *cbk, kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3);
    kint sg_ses_get_obj(sg_ses *ses, kuint tmo, kuint objid, kuint msk, kvoid *cbk, kchar *objname,
            kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3);

    kint sg_ses_set_done_cbk(sg_ses *ses, SES_DONE_CBK cbk, kvoid *usr0, kvoid *usr1, kvoid *usr2, kvoid *usr3);


#ifdef __cplusplus
}
#endif

#endif /* __ESGSES_H__ */


