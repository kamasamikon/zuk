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
#include "sg_sch.h"
#include "s2n_macro.h"

kint sg_sch_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_sch *sch = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, SCH, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_sch_new(&sch);
    sch->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)sch);
    kflg_set(sch->flg, EFF_LOADING);

    if (-2 == sg_sch_x2s(sch)) {
        FREE_ORG_DAT(sch);
        sg_sch_del(sch, ktrue);
        return -1;
    }
    FREE_ORG_DAT(sch);

    insert_dlist_tail_entry(&root->allhdr, &sch->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SCH], &sch->cls_le);
    kflg_clr(sch->flg, EFF_LOADING);
    return 0;
}
kint sg_sch_new(sg_sch **sch)
{
    if (!(*sch)) {
        (*sch) = (sg_sch*)kmem_alloc(sizeof(sg_sch));
    }
    if ((*sch)) {
        memset((*sch), 0, sizeof(sg_sch));
        INIT_COMMON((*sch));
        (*sch)->cls = CLS_SCHE;
        return 0;
    }
    return -1;
}

kint sg_sch_del(sg_sch *sch, kbool rmcache)
{
    if (!sch) {
        return -1;
    }
    remove_dlist_entry(&sch->all_le);
    remove_dlist_entry(&sch->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_SCHE == sch->cls);

    if (rmcache) {
        sg_root_rm_cache_file(sch);
    }

    kmem_free(sch->attr.id);
    S2N_FUN(Schedule, sch);
    // DEL_COMMON(sch);
    return 0;
}

kint sg_sch_add_cache(sg_sch *sch)
{
    // save the content of the file to disk etc
    if (kflg_chk(sch->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(sch);
    return 0;
}

kint sg_sch_del_cache(sg_sch *sch)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_sch_ld_cache(sg_sch *sch)
{
    // save the content of the file to disk etc
    kflg_clr(sch->flg, EFF_LOADING);
    return 0;
}


