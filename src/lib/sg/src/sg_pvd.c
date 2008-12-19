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
#include "sg_pvd.h"
#include "s2n_macro.h"

kint sg_pvd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_pvd *pvd = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, PVD, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_pvd_new(&pvd);
    pvd->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)pvd);
    kflg_set(pvd->flg, EFF_LOADING);

    if (-2 == sg_pvd_x2s(pvd)) {
        FREE_ORG_DAT(pvd);
        sg_pvd_del(pvd, ktrue);
        return -1;
    }
    FREE_ORG_DAT(pvd);

    insert_dlist_tail_entry(&root->allhdr, &pvd->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[PVD], &pvd->cls_le);
    kflg_clr(pvd->flg, EFF_LOADING);
    return 0;
}
kint sg_pvd_new(sg_pvd **pvd)
{
    if (!(*pvd)) {
        (*pvd) = (sg_pvd*)kmem_alloc(sizeof(sg_pvd));
    }
    if ((*pvd)) {
        memset((*pvd), 0, sizeof(sg_pvd));
        INIT_COMMON((*pvd));
        (*pvd)->cls = CLS_PVDA;
        return 0;
    }
    return -1;
}

kint sg_pvd_del(sg_pvd *pvd, kbool rmcache)
{
    if (!pvd) {
        return -1;
    }
    remove_dlist_entry(&pvd->all_le);
    remove_dlist_entry(&pvd->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_PVDA == pvd->cls);

    if (rmcache) {
        sg_root_rm_cache_file(pvd);
    }

    kmem_free(pvd->attr.id);
    S2N_FUN(PreviewData, pvd);
    // DEL_COMMON(pvd);
    return 0;
}

kint sg_pvd_add_cache(sg_pvd *pvd)
{
    // save the content of the file to disk etc
    if (kflg_chk(pvd->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(pvd);
    return 0;
}

kint sg_pvd_del_cache(sg_pvd *pvd)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_pvd_ld_cache(sg_pvd *pvd)
{
    // save the content of the file to disk etc
    kflg_clr(pvd->flg, EFF_LOADING);
    return 0;
}


