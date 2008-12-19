#ifndef __SG_SCH_H__
#define __SG_SCH_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "esgfile.h"
#include "sg_com.h"

typedef Schedule_t sg_sch;

//kint sg_sch_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_sch_new(sg_sch **sch);
kint sg_sch_del(sg_sch *sch, kbool rmcache);
kint sg_sch_add_cache(sg_sch *sch);
kint sg_sch_del_cache(sg_sch *sch);
kint sg_sch_ld_cache(sg_sch *sch);

#ifdef __cplusplus
}
#endif

#endif // __SG_SCH_H__

