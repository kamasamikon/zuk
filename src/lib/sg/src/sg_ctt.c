#include <string.h>
#include <stdio.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include "ksupport.h"
#include <kflg.h>

#include <kmem.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_ctt.h"
#include "s2n_macro.h"

kint sg_ctt_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_ctt *ctt = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, CTT, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_ctt_new(&ctt);
    ctt->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)ctt);
    kflg_set(ctt->flg, EFF_LOADING);

    if (-2 == sg_ctt_x2s(ctt)) {
        FREE_ORG_DAT(ctt);
        sg_ctt_del(ctt, ktrue);
        return -1;
    }
    FREE_ORG_DAT(ctt);

    insert_dlist_tail_entry(&root->allhdr, &ctt->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[CTT], &ctt->cls_le);
    kflg_clr(ctt->flg, EFF_LOADING);
    return 0;
}
kint sg_ctt_new(sg_ctt **ctt)
{
    if (!(*ctt)) {
        (*ctt) = (sg_ctt*)kmem_alloc(sizeof(sg_ctt));
    }
    if ((*ctt)) {
        memset((*ctt), 0, sizeof(sg_ctt));
        INIT_COMMON((*ctt));
        (*ctt)->cls = CLS_COTT;
        return 0;
    }
    return -1;
}

kint sg_ctt_del(sg_ctt *ctt, kbool rmcache)
{
    if (!ctt) {
        return -1;
    }
    remove_dlist_entry(&ctt->all_le);
    remove_dlist_entry(&ctt->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_COTT == ctt->cls);

    if (rmcache) {
        sg_root_rm_cache_file(ctt);
    }

    kmem_free(ctt->attr.id);
    S2N_FUN(Content, ctt);
    // DEL_COMMON(ctt);
    return 0;
}

kint sg_ctt_add_cache(sg_ctt *ctt)
{
    // save the content of the file to disk etc
    if (kflg_chk(ctt->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(ctt);
    return 0;
}

kint sg_ctt_del_cache(sg_ctt *ctt)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_ctt_ld_cache(sg_ctt *ctt)
{
    // save the content of the file to disk etc
    kflg_clr(ctt->flg, EFF_LOADING);
    return 0;
}
