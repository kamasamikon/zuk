#ifndef __SG_FDT_H__
#define __SG_FDT_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

#include "ivisa_sg_filerev.h"

    // the Original structure has some junk info
    typedef struct {
        kuint uiExpiresTime;
        kuint uiObjectListVersion;
        kint  iComplete_flag;
        kint iFullFDT_flag;
    } sg_fdt_obj_list_attr;

    typedef struct _sg_fdt {
        EF_COMMON;

        sg_fdt_obj_list_attr olattr;
        K_dlist_entry sddhdr;
    } sg_fdt;

    kint sg_fdt_new(sg_fdt **fdt);
    kint sg_fdt_del(sg_fdt *fdt, kbool rmcache);
    kint sg_fdt_x2s(sg_fdt *fdt);
    kint sg_fdt_add_cache(sg_fdt *fdt);
    kint sg_fdt_del_cache(sg_fdt *fdt);
    kint sg_fdt_ld_cache(sg_fdt *fdt);


#ifdef __cplusplus
}
#endif

#endif /* __SG_FDT_H__ */

