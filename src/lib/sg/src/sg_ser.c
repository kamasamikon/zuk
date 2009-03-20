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
#include "sg_ser.h"
#include "s2n_macro.h"

kint sg_ser_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_ser *ser = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, SER, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_ser_new(&ser);
    ser->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)ser);
    kflg_clr(ser->flg, SER_IAD_TIMER|SER_IAD);
    kflg_set(ser->flg, EFF_LOADING);

    ser->frequency = rec->freq;

    if (-2 == sg_ser_x2s(ser)) {
        FREE_ORG_DAT(ser);
        sg_ser_del(ser, ktrue);
        return -1;
    }
    FREE_ORG_DAT(ser);

    insert_dlist_tail_entry(&root->allhdr, &ser->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SER], &ser->cls_le);
    kflg_clr(ser->flg, EFF_LOADING);
    return 0;
}
kint sg_ser_new(sg_ser **ser)
{
    if (!(*ser)) {
        (*ser) = (sg_ser*)kmem_alloc(sizeof(sg_ser));
    }
    if ((*ser)) {
        memset((*ser), 0, sizeof(sg_ser));
        INIT_COMMON((*ser));
        (*ser)->cls = CLS_SERV;
        return 0;
    }
    return -1;
}

kint sg_ser_del(sg_ser *ser, kbool rmcache)
{
    if (!ser) {
        return -1;
    }

    if(ser->cur_idir){
        sg_imd_free_idir(ser->cur_idir);
    }

    remove_dlist_entry(&ser->all_le);
    remove_dlist_entry(&ser->cls_le);

    if (ser->iad_probe_tmr) {
        ktmr_kill(ser->iad_probe_tmr);
    }

    kassert(CLS_SERV == ser->cls);

    if (rmcache) {
        sg_root_rm_cache_file(ser);
    }

    kmem_free(ser->attr.id);
    S2N_FUN(Service, ser);
    // DEL_COMMON(ser);
    return 0;
}

kint sg_ser_add_cache(sg_ser *ser)
{
    // save the content of the file to disk etc
    if (kflg_chk(ser->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(ser);
    return 0;
}
kint sg_ser_del_cache(sg_ser *ser)
{
    // save the content of the file to disk etc
    return 0;
}

kint sg_ser_ld_cache(sg_ser *ser)
{
    // save the content of the file to disk etc
    kflg_clr(ser->flg, EFF_LOADING);
    return 0;
}

