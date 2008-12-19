
#ifndef __SG_SDP_H__
#define __SG_SDP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

typedef struct _sg_sdp {
    EF_COMMON;
} sg_sdp;

//kint sg_sdp_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_sdp_new(sg_sdp **sdp);
kint sg_sdp_del(sg_sdp *sdp, kbool rmcache);
kint sg_sdp_add_cache(sg_sdp *sdp);
kint sg_sdp_del_cache(sg_sdp *sdp);
kint sg_sdp_ld_cache(sg_sdp *sdp);
kint sg_sdp_getval(sg_sdp *sdp, kchar *type, kchar *value, kint index);
kint sdp_getval(kchar *buf, kuint len, kchar *type, kchar *value, kint index);

#ifdef __cplusplus
}
#endif

#endif // __SG_SDP_H__

