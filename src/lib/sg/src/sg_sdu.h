#ifndef __SG_SDU_H__
#define __SG_SDU_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

#define SDUF_PARSED        0x80000000

    typedef struct _sg_sdu {
        EF_COMMON;

        kuint sdd_tid;
    } sg_sdu;

    kint sg_sdu_new(sg_sdu **sdu);
    kint sg_sdu_del(sg_sdu *sdu, kbool rmcache);
    kint sg_sdu_x2s(sg_sdu *sdu);
    kint sg_sdu_add_cache(sg_sdu *sdu);
    kint sg_sdu_del_cache(sg_sdu *sdu);
    kint sg_sdu_ld_cache(sg_sdu *sdu);
    kint sg_sdu_parse(sg_sdu *sdu);


#ifdef __cplusplus
}
#endif

#endif // __SG_SDU_H__

