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
#include "sg_acc.h"
#include "s2n_macro.h"

kint sg_acc_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_acc *acc = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, ACC, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_acc_new(&acc);
    acc->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)acc);
    kflg_set(acc->flg, EFF_LOADING);

    if (-2 == sg_acc_x2s(acc)) {
        FREE_ORG_DAT(acc);
        sg_acc_del(acc, ktrue);
        return -1;
    }
    FREE_ORG_DAT(acc);

    insert_dlist_tail_entry(&root->allhdr, &acc->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[ACC], &acc->cls_le);
    return 0;
}
kint sg_acc_new(sg_acc **acc)
{
    if (!(*acc)) {
        (*acc) = (sg_acc*)kmem_alloc(sizeof(sg_acc));
    }
    if ((*acc)) {
        memset((*acc), 0, sizeof(sg_acc));
        INIT_COMMON((*acc));
        (*acc)->cls = CLS_ACCE;
        return 0;
    }
    return -1;
}

kint sg_acc_del(sg_acc *acc, kbool rmcache)
{
    if (!acc) {
        return -1;
    }
    remove_dlist_entry(&acc->all_le);
    remove_dlist_entry(&acc->cls_le);

    kassert(CLS_ACCE == acc->cls);

    if (rmcache) {
        sg_root_rm_cache_file(acc);
    }

    kmem_free(acc->attr.id);
    S2N_FUN(Access, acc);
    // DEL_COMMON(acc);
    return 0;
}

kint sg_acc_add_cache(sg_acc *acc)
{
    // save the content of the file to disk etc
    if (kflg_chk(acc->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(acc);
    return 0;
}
kint sg_acc_del_cache(sg_acc *acc)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_acc_ld_cache(sg_acc *acc)
{
    // save the content of the file to disk etc
    kflg_clr(acc->flg, ERF_LOADING);
    return 0;
}


