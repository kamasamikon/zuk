/* vim:set et sw=4 sts=4: */
#ifndef __K_IM_H__
#define __K_IM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <sdlist.h>

#define RF_READONLY             0x00000001
#define RF_DISABLE              0x00000002
#define RF_PROCESS              0x00000004
#define RF_AUTOSET              0x00000008
#define RF_UNREADD              0x00000010

#define RT_STR                  (kuchar)0x01
#define RT_PTR                  (kuchar)0x02
#define RT_INT                  (kuchar)0x03

#define WT_UP                   (kuchar)0x00
#define WT_LO                   (kuchar)0x01

#define ATRSN_ADD               (kuchar)0x00            /**< Add */
#define ATRSN_REW               (kuchar)0x01            /**< Rewrite */
#define ATRSN_DEL               (kuchar)0x02            /**< Delete */
#define ATRSN_SET               (kuchar)0x03            /**< Set */
#define ATRSN_GET               (kuchar)0x04            /**< Get */
#define ATRSN_RST               (kuchar)0x05            /**< Reset */

typedef struct _KIM KIM;
typedef struct _KRtiRec KRtiRec;
typedef struct _KRtiWatch KRtiWatch;


/* AT can adjust the rec's field */
typedef kint (*ATPROC)(KIM *im, KRtiRec *rec, kuchar reason);

/* WATCH can NOT adjust the rec's field */
typedef kint (*WCHPROC)(KIM *im, KRtiRec *rec, kvoid* ua, kvoid* ub, kuchar type);

/* for quick type watch function */
#define IMWCH(f) (f)(KIM *im, KRtiRec *rec, void *ua, void *ub, kuchar type)
#define IMAT(f) (f)(KIM *im, KRtiRec *rec, kuchar reason)

struct _KRtiWatch
{
    K_dlist_entry abwchentry;   /**< queue to KRtiRec::bwchhdr/awchhdr */
    WCHPROC wch;
    kvoid* ua;                  /**< usr arg */
    kvoid* ub;                  /**< usr arg */
    kchar *forid;               /**< watch what event */
    kchar *desc;
};

#define WCH_UA(_wch_) (_wch_)->ua
#define WCH_UB(_wch_) (_wch_)->ub

struct _KRtiRec
{
    kchar *id;                  /**< identifier */
    kchar *desc;

    struct {
        kuchar type;            /**< RECTYP_XXX */
        union {
            kchar *sv;          /**< string value */
            kvoid *pv;          /**< pointer value */
            kint iv;            /**< kint value */
        } use, def;             /**< used value, set value, default value */
    } var;

    ATPROC at;                  /**< @see ATREASON */
    kvoid *val;
    kvoid *ua, *ub;             /**< XXX only for ATREASON_SET */

    K_dlist_entry entry;        /**< for queue into KIM::rechdr */

    /* watch list */
    K_dlist_entry bwchhdr;      /**< before watch header */
    K_dlist_entry awchhdr;      /**< after watch header */

    kuint flg;                  /**< RECFLG_XXX */
};

#define REC_SET(_rec_) (_rec_)->val                 /**< val set by kim_setXXX */

#define REC_CUR_STR(_rec_) (_rec_)->var.use.sv
#define REC_CUR_PTR(_rec_) (_rec_)->var.use.pv
#define REC_CUR_INT(_rec_) (_rec_)->var.use.iv

#define REC_DEF_STR(_rec_) (_rec_)->var.def.sv
#define REC_DEF_PTR(_rec_) (_rec_)->var.def.pv
#define REC_DEF_INT(_rec_) (_rec_)->var.def.iv

#define REC_UA(_rec_) (_rec_)->ua
#define REC_UB(_rec_) (_rec_)->ub

struct _KIM
{
    K_dlist_entry rechdr;

    /* watch that Not Yet connect */
    struct {
        K_dlist_entry bhdr;         /**< before watch header */
        K_dlist_entry ahdr;         /**< after watch header */
    } nywch;

    kbean lck;                      /**< lck to protect rechdr, ahdr, bhdr */
};

KRtiRec* kim_chkrec(KIM *im, const kchar *id);

kint kim_setflg(KIM *im, const kchar *id, kuint flg);
kint kim_clrflg(KIM *im, const kchar *id, kuint flg);

/*---------------------------------------------------------------------------------
 * prototype for val
 */
kint kim_addstr(KIM *im, const kchar *id, kchar *def, kint flg, ATPROC at, const kchar *desc);
kint kim_delstr(KIM *im, const kchar *id);
kint kim_setstr(KIM *im, const kchar *id, kchar *val, kvoid **ua, kvoid **ub);
kchar* kim_getstr(KIM *im, const kchar *id, kint *err);

kint kim_addptr(KIM *im, const kchar *id, kvoid *def, kint flg, ATPROC at, const kchar *desc);
kint kim_delptr(KIM *im, const kchar *id);
kint kim_setptr(KIM *im, const kchar *id, kvoid *val, kvoid **ua, kvoid **ub);
kvoid* kim_getptr(KIM *im, const kchar *id, kint *err);

kint kim_addint(KIM *im, const kchar *id, kint def, kint flg, ATPROC at, const kchar *desc);
kint kim_delint(KIM *im, const kchar *id);
kint kim_setint(KIM *im, const kchar *id, kint val, kvoid **ua, kvoid **ub);
kint kim_getint(KIM *im, const kchar *id, kint *err);

/*---------------------------------------------------------------------------------
 * prototype for watch
 */
kbean kim_addawch(KIM *im, const kchar *id, WCHPROC watch, kvoid *ua, kvoid *ub, const kchar *desc);
kint kim_delawch(KIM *im, kbean watch);

kbean kim_addbwch(KIM *im, const kchar *id, WCHPROC watch, kvoid *ua, kvoid *ub, const kchar *desc);
kint kim_delbwch(KIM *im, kbean watch);

/*---------------------------------------------------------------------------------
 * new and del
 */
KIM* kim_new(KIM *im);
kint kim_del(KIM *im);

kint kim_start(KIM *im);
kint kim_stop(KIM *im);

#ifdef __cplusplus
}
#endif

#endif /* __K_IM_H__ */

