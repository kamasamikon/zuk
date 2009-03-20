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
#include "sg_pci.h"
#include "s2n_macro.h"

kint sg_pci_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_pci *pci = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, PCI, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_pci_new(&pci);
    pci->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)pci);
    kflg_set(pci->flg, EFF_LOADING);

    if (-2 == sg_pci_x2s(pci)) {
        FREE_ORG_DAT(pci);
        sg_pci_del(pci, ktrue);
        return -1;
    }
    FREE_ORG_DAT(pci);

    insert_dlist_tail_entry(&root->allhdr, &pci->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[PCI], &pci->cls_le);
    kflg_clr(pci->flg, EFF_LOADING);
    return 0;
}
kint sg_pci_new(sg_pci **pci)
{
    if (!(*pci)) {
        (*pci) = (sg_pci*)kmem_alloc(sizeof(sg_pci));
    }
    if ((*pci)) {
        memset((*pci), 0, sizeof(sg_pci));
        INIT_COMMON((*pci));
        (*pci)->cls = CLS_PCIT;
        return 0;
    }
    return -1;
}

kint sg_pci_del(sg_pci *pci, kbool rmcache)
{
    if (!pci) {
        return -1;
    }
    remove_dlist_entry(&pci->all_le);
    remove_dlist_entry(&pci->cls_le);

    // 只有调用合适的删除函数才能保证正确
    // 释放所有的资源。
    kassert(CLS_PCIT == pci->cls);

    if (rmcache) {
        sg_root_rm_cache_file(pci);
    }

    kmem_free(pci->attr.id);
    S2N_FUN(PurchaseItem, pci);
    // DEL_COMMON(pci);
    return 0;
}

kint sg_pci_add_cache(sg_pci *pci)
{
    // save the content of the file to disk etc
    if (kflg_chk(pci->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(pci);
    return 0;
}

kint sg_pci_del_cache(sg_pci *pci)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_pci_ld_cache(sg_pci *pci)
{
    // save the content of the file to disk etc
    kflg_clr(pci->flg, EFF_LOADING);
    return 0;
}



