#ifndef __SG_ACC_H__
#define __SG_ACC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

typedef Access_t sg_acc;

//kint sg_acc_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_acc_new(sg_acc **acc);
kint sg_acc_del(sg_acc *acc, kbool rmcache);
kint sg_acc_add_cache(sg_acc *acc);
kint sg_acc_del_cache(sg_acc *acc);
kint sg_acc_ld_cache(sg_acc *acc);

#ifdef __cplusplus
}
#endif

#endif /* __SG_ACC_H__ */

