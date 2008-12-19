#include <string.h>
#include <stdio.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>

#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_iad.h"
#include "s2n_macro.h"

kint sg_iad_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_iad *iad = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, IAD, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_iad_new(&iad);
    iad->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)iad);
    kflg_set(iad->flg, EFF_LOADING);

    if (-2 == sg_iad_x2s(iad)) {
        FREE_ORG_DAT(iad);
        sg_iad_del(iad, ktrue);
        return -1;
    }
    FREE_ORG_DAT(iad);

    insert_dlist_tail_entry(&root->allhdr, &iad->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[IAD], &iad->cls_le);
    kflg_clr(iad->flg, EFF_LOADING);
    return 0;
}
kint sg_iad_new(sg_iad **iad)
{
    if (!(*iad)) {
        (*iad) = (sg_iad*)kmem_alloc(sizeof(sg_iad));
    }
    if ((*iad)) {
        memset((*iad), 0, sizeof(sg_iad));
        INIT_COMMON((*iad));
        (*iad)->cls = CLS_IADA;
        return 0;
    }
    return -1;
}

kint sg_iad_del(sg_iad *iad, kbool rmcache)
{
    if (!iad) {
        return -1;
    }
    remove_dlist_entry(&iad->all_le);
    remove_dlist_entry(&iad->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_IADA == iad->cls);

    if (rmcache) {
        sg_root_rm_cache_file(iad);
    }

    kmem_free(iad->attr.id);
    S2N_FUN(InteractivityData, iad);
    // DEL_COMMON(iad);
    return 0;
}

kint sg_iad_add_cache(sg_iad *iad)
{
    // save the content of the file to disk etc
    if (kflg_chk(iad->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(iad);
    return 0;
}

kint sg_iad_del_cache(sg_iad *iad)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_iad_ld_cache(sg_iad *iad)
{
    // save the content of the file to disk etc
    kflg_clr(iad->flg, EFF_LOADING);
    return 0;
}


