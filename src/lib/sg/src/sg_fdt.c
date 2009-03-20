/* vim:set et sw=4 sts=4: */
#include <string.h>
#include <stdio.h>
#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>

#include "xmldoc.h"

#include "sg_root.h"

#include "sg_fdt.h"
#include "s2n_macro.h"

kint sg_fdt_new(sg_fdt **fdt)
{
    if (!(*fdt)) {
        (*fdt) = (sg_fdt*)kmem_alloc(sizeof(sg_fdt));
    }
    if ((*fdt)) {
        memset((*fdt), 0, sizeof(sg_fdt));
        INIT_COMMON((*fdt));
        (*fdt)->cls = CLS_DIRE;

        return 0;
    }
    return -1;
}

kint sg_fdt_del(sg_fdt *fdt, kbool rmcache)
{
    if (!fdt) {
        return -1;
    }
    remove_dlist_entry(&fdt->all_le);
    remove_dlist_entry(&fdt->cls_le);

    kassert(CLS_DIRE == fdt->cls);

    if (rmcache) {
        sg_root_rm_cache_file(fdt);
    }

    kmem_free(fdt);
    return 0;
}

kint sg_fdt_add_cache(sg_fdt *fdt)
{
    // save the content of the file to disk etc
    // do nothing for fdt
    if (kflg_chk(fdt->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(fdt);
    return 0;
}

kint sg_fdt_del_cache(sg_fdt *fdt)
{
    // save the content of the file to disk etc
    // do nothing for fdt
    return 0;
}

kint sg_fdt_ld_cache(sg_fdt *fdt)
{
    // save the content of the file to disk etc
    // do nothing for fdt
    kflg_clr(fdt->flg, EFF_LOADING);
    return 0;
}

