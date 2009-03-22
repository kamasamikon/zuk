/* vim:set et sw=4 sts=4: */

#ifndef __SG_IAD_H__
#define __SG_IAD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

typedef InteractivityData_t sg_iad;

//kint sg_iad_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_iad_new(sg_iad **iad);
kint sg_iad_del(sg_iad *iad, kbool rmcache);
kint sg_iad_add_cache(sg_iad *iad);
kint sg_iad_del_cache(sg_iad *iad);
kint sg_iad_ld_cache(sg_iad *iad);

#ifdef __cplusplus
}
#endif

#endif // __SG_IAD_H__

