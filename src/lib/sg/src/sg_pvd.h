
#ifndef __SG_PVD_H__
#define __SG_PVD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"


typedef PreviewData_t sg_pvd;

//kint sg_pvd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_pvd_new(sg_pvd **pvd);
kint sg_pvd_del(sg_pvd *pvd, kbool rmcache);
kint sg_pvd_add_cache(sg_pvd *pvd);
kint sg_pvd_del_cache(sg_pvd *pvd);
kint sg_pvd_ld_cache(sg_pvd *pvd);

#ifdef __cplusplus
}
#endif

#endif // __SG_PVD_H__




