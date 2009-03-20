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
#include "sg_pcd.h"
#include "s2n_macro.h"

kint sg_pcd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_pcd *pcd = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, PCD, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_pcd_new(&pcd);
    pcd->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)pcd);
    kflg_set(pcd->flg, EFF_LOADING);

    if (-2 == sg_pcd_x2s(pcd)) {
        FREE_ORG_DAT(pcd);
        sg_pcd_del(pcd, ktrue);
        return -1;
    }
    FREE_ORG_DAT(pcd);

    insert_dlist_tail_entry(&root->allhdr, &pcd->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[PCD], &pcd->cls_le);
    kflg_clr(pcd->flg, EFF_LOADING);
    return 0;
}

kint sg_pcd_new(sg_pcd **pcd)
{
    if (!(*pcd)) {
        (*pcd) = (sg_pcd*)kmem_alloc(sizeof(sg_pcd));
    }
    if ((*pcd)) {
        memset((*pcd), 0, sizeof(sg_pcd));
        INIT_COMMON((*pcd));
        (*pcd)->cls = CLS_PCDA;
        return 0;
    }
    return -1;
}

kint sg_pcd_del(sg_pcd *pcd, kbool rmcache)
{
    if (!pcd) {
        return -1;
    }
    remove_dlist_entry(&pcd->all_le);
    remove_dlist_entry(&pcd->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_PCDA == pcd->cls);

    if (rmcache) {
        sg_root_rm_cache_file(pcd);
    }

    kmem_free(pcd->attr.id);
    S2N_FUN(PurchaseData, pcd);
    // DEL_COMMON(pcd);
    return 0;
}

kint sg_pcd_add_cache(sg_pcd *pcd)
{
    // save the content of the file to disk etc
    if (kflg_chk(pcd->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(pcd);
    return 0;
}

kint sg_pcd_del_cache(sg_pcd *pcd)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_pcd_ld_cache(sg_pcd *pcd)
{
    // save the content of the file to disk etc
    kflg_clr(pcd->flg, EFF_LOADING);
    return 0;
}


