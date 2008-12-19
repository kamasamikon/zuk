#ifndef __ESGFILE_H__
#define __ESGFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <sdlist.h>

#define EFF_USED            0x00100000
#define EFF_DATAOK          0x00200000      /**< data part of the fragment has been gotten */
#define EFF_PROCOK          0x00400000      /**< the fragment has beed processed */
#define EFF_LOADING         0x00800000      /**< loading frome cache */
#define EFF_INDICATE        0x01000000      /**< evtCallback should be called */
#define EFF_SYNC_DEL        0x02000000      /**< delete after sync */

#define BAD_VER             0x00000000

#define EF_COMMON \
    /* XXX EVERY FILE SHOULD HAS THESE ATTRIBUTES */ \
    K_dlist_entry all_le;                   /**< @see ::sg_root::allhdr */ \
    K_dlist_entry cls_le;                   /**< @see ::sg_root::fhdrs */ \
    struct _sg_root *rt; \
    kuint flg; \
    kuint cls;                              /**< @see ::sg_root.h::_FT */\
    kuint tid;                              /**< Fragment::transportid, SDD::TOI */ \
    kuint frequency;                        /**< every Fragment works in a certain frequency */ \
    struct { kint len; kchar *buf; } dat; \
    struct { \
        kchar *id; \
        kuint validFrom; \
        kuint validTo; \
        kuint version; \
    } attr

#define INIT_COMMON(f) \
    init_dlist_head(&((f)->all_le)); \
    init_dlist_head(&((f)->cls_le)); \
    (f)->rt = knil; \
    (f)->flg = EFF_USED; \
    (f)->attr.version = BAD_VER; \
    (f)->attr.validFrom = 0; \
    (f)->attr.validTo = 0;

#define DEL_COMMON(zbd) \
    kmem_free(zbd->attr.id); \
    kmem_free(zbd)

#define FREE_ORG_DAT(zbd) \
    kmem_free(zbd->dat.buf); \
    zbd->dat.buf = knil; \
    zbd->dat.len = 0

    typedef struct _ef_cache_rec {
        kuint flg;
        kuint cls;
        kuint tid;                      /**< Transport ID, do same work as attr.id, if ver change, it should change */
        kuint freq;                     /**< every Fragment works in a certain frequency */
        struct {
            kuint validFrom;
            kuint validTo;
            kuint version;
        } attr;
    } ef_cache_rec;

#define CLS_SCHE        0x11111111
#define CLS_PCCH        0x22222222
#define CLS_IADA        0x33333333
#define CLS_COTT        0x44444444
#define CLS_PCDA        0x55555555
#define CLS_PCIT        0x66666666
#define CLS_PVDA        0x77777777
#define CLS_SERV        0x88888888
#define CLS_ACCE        0x99999999
#define CLS_DIRE        0xAAAAAAAA
#define CLS_IMDA        0xBBBBBBBB
#define CLS_SGDD        0xCCCCCCCC
#define CLS_SGDU        0xDDDDDDDD
#define CLS_SDPF        0xEEEEEEEE

#ifdef __cplusplus
}
#endif

#endif /* __ESGFILE_H__ */

