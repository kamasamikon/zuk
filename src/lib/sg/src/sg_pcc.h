
#ifndef __sg_pcc_H__
#define __sg_pcc_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

typedef PurchaseChannel_t sg_pcc;

//kint sg_pcc_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_pcc_new(sg_pcc **pcc);
kint sg_pcc_del(sg_pcc *pcc, kbool rmcache);
kint sg_pcc_add_cache(sg_pcc *pcc);
kint sg_pcc_del_cache(sg_pcc *pcc);
kint sg_pcc_ld_cache(sg_pcc *pcc);

#ifdef __cplusplus
}
#endif

#endif // __sg_pcc_H__

