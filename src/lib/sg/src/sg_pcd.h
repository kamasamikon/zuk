/* vim:set et sw=4 sts=4: */

#ifndef __SG_PCD_H__
#define __SG_PCD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"


typedef PurchaseData_t sg_pcd;

//kint sg_pcd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_pcd_new(sg_pcd **pcd);
kint sg_pcd_del(sg_pcd *pcd, kbool rmcache);
kint sg_pcd_add_cache(sg_pcd *pcd);
kint sg_pcd_del_cache(sg_pcd *pcd);
kint sg_pcd_ld_cache(sg_pcd *pcd);

#ifdef __cplusplus
}
#endif

#endif // __SG_PCD_H__




