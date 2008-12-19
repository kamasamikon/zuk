#ifndef __SG_CTT_H__
#define __SG_CTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "esgfile.h"
#include "sg_com.h"

typedef Content_t sg_ctt;

#ifdef __cplusplus
}
#endif

//kint sg_ctt_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_ctt_new(sg_ctt **ctt);
kint sg_ctt_del(sg_ctt *ctt, kbool rmcache);
kint sg_ctt_add_cache(sg_ctt *ctt);
kint sg_ctt_del_cache(sg_ctt *ctt);
kint sg_ctt_ld_cache(sg_ctt *ctt);

#endif // __SG_CTT_H__

