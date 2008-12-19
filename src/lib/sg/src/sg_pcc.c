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
#include "sg_pcc.h"
#include "s2n_macro.h"

kint sg_pcc_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_pcc *pcc = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, PCC, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_pcc_new(&pcc);
    pcc->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)pcc);
    kflg_set(pcc->flg, EFF_LOADING);

    if (-2 == sg_pcc_x2s(pcc)) {
        FREE_ORG_DAT(pcc);
        sg_pcc_del(pcc, ktrue);
        return -1;
    }
    FREE_ORG_DAT(pcc);

    insert_dlist_tail_entry(&root->allhdr, &pcc->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[PCC], &pcc->cls_le);
    kflg_clr(pcc->flg, EFF_LOADING);
    return 0;
}
kint sg_pcc_new(sg_pcc **pcc)
{
    if (!(*pcc)) {
        (*pcc) = (sg_pcc*)kmem_alloc(sizeof(sg_pcc));
    }
    if ((*pcc)) {
        memset((*pcc), 0, sizeof(sg_pcc));
        INIT_COMMON((*pcc));
        (*pcc)->cls = CLS_PCCH;
        return 0;
    }
    return -1;
}

kint sg_pcc_del(sg_pcc *pcc, kbool rmcache)
{
    if (!pcc) {
        return -1;
    }
    remove_dlist_entry(&pcc->all_le);
    remove_dlist_entry(&pcc->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_PCCH == pcc->cls);

    if (rmcache) {
        sg_root_rm_cache_file(pcc);
    }

    kmem_free(pcc->attr.id);
    S2N_FUN(PurchaseChannel, pcc);
    // DEL_COMMON(pcc);
    return 0;
}

kint sg_pcc_add_cache(sg_pcc *pcc)
{
    // save the content of the file to disk etc
    if (kflg_chk(pcc->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(pcc);
    return 0;
}

kint sg_pcc_del_cache(sg_pcc *pcc)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_pcc_ld_cache(sg_pcc *pcc)
{
    // save the content of the file to disk etc
    kflg_clr(pcc->flg, EFF_LOADING);
    return 0;
}


