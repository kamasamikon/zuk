/* vim:set et sw=4 sts=4: */
#ifndef __SG_SDD_H__
#define __SG_SDD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "esgfile.h"
#include "sg_com.h"

#define SDDF_DONE        0x00000001

    typedef ServiceGuideDeliveryDescriptor_t sg_sdd;

    kint sg_sdd_new(sg_sdd **sdd);
    kint sg_sdd_del(sg_sdd *sdd, kbool rmcache);
    kint sg_sdd_x2s(sg_sdd *sdd);
    kint sg_sdd_add_cache(sg_sdd *sdd);
    kint sg_sdd_del_cache(sg_sdd *sdd);
    kint sg_sdd_ld_cache(sg_sdd *sdd);

    Fragment_rec *sg_sdd_get_frag_by_tid(sg_sdd *sdd, kuint tid);

#ifdef __cplusplus
}
#endif

#endif // __SG_SDD_H__

