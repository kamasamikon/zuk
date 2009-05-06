/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ktypes.h>

#include <kim.h>

#include <ksyn.h>
#include <kdbg.h>
#include <kmem.h>
#include <kstr.h>

static KRtiRec* addrec(KIM *im, const kchar *id, kuchar type, kvoid *def, kint flg, ATPROC at, const kchar *desc);
static kint delrec(KIM *im, KRtiRec *rec);
static kint setrec(KIM *im, KRtiRec *rec, kvoid *val, kvoid **ua, kvoid **ub);
static kvoid* getrec(KIM *im, KRtiRec *rec, kint *err);

static kbean addwch(KIM *im, const kchar *id, WCHPROC watch, kuchar type, kvoid *ua, kvoid *ub, const kchar *desc);
static kint delwch(KIM *im, kbean watch);

/*---------------------------------------------------------------------------------
 * check
 */
KRtiRec* kim_chkrec(KIM *im, const kchar *id)
{
    KRtiRec *val;
    K_dlist_entry *entry;
    entry = im->rechdr.next;
    while (entry != &im->rechdr) {
        val = FIELD_TO_STRUCTURE(entry, KRtiRec, entry);
        if (0 == strcmp(val->id, id))
            return val;
        entry = entry->next;
    }
    return knil;
}

/*---------------------------------------------------------------------------------
 * Operation on flg
 */
kint kim_setflg(KIM *im, const kchar *id, kuint flg)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        rec->flg |= flg;
        return 0;
    } else
        kerror(("KIM: kim_setflg: not found record : %s\n", id));
    return -1;
}
kint kim_clrflg(KIM *im, const kchar *id, kuint flg)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        rec->flg &= ~flg;
        return 0;
    } else
        kerror(("KIM: kim_clrflg: not found record : %s\n", id));
    return -1;
}

/*---------------------------------------------------------------------------------
 * Add Value to RTI
 */
static KRtiRec* addrec(KIM *im, const kchar *id, kuchar type, kvoid *def, kint flg, ATPROC at, const kchar *desc)
{
    KRtiRec *rec;
    KRtiWatch *wch;
    K_dlist_entry *wchentry;

    ksyn_lck_get(im->lck);

    rec = kim_chkrec(im, id);
    if (rec)
        delrec(im, rec);

    rec = (KRtiRec*)kmem_alloz(sizeof(KRtiRec));
    if (rec) {

        rec->id = kstr_dup(id);
        rec->desc = desc ? kstr_dup(desc) : knil;
        rec->var.type = type;
        rec->flg = flg;
        rec->at = at;

        init_dlist_head(&rec->awchhdr);
        init_dlist_head(&rec->bwchhdr);
        init_dlist_head(&rec->entry);

        switch (type) {
            case RT_STR:
                if (def)
                    rec->var.def.sv = kstr_dup((kchar*)def);
                break;
            case RT_PTR:
                rec->var.def.pv = (kvoid*)def;
                break;
            case RT_INT:
                rec->var.def.iv = (kint)def;
                break;
            default:
                delrec(im, rec);
                ksyn_lck_rel(im->lck);
                return knil;
        }

        /* XXX queue to im */
        insert_dlist_tail_entry(&im->rechdr, &rec->entry);

        /* XXX process the NY watches */
        wchentry = im->nywch.ahdr.next;
        while (wchentry != &im->nywch.ahdr) {
            wch = FIELD_TO_STRUCTURE(wchentry, KRtiWatch, abwchentry);
            wchentry = wchentry->next;
            if (0 == strcmp(wch->forid, id)) {
                remove_dlist_entry(&wch->abwchentry);
                insert_dlist_tail_entry(&rec->awchhdr, &wch->abwchentry);
            }
        }
        wchentry = im->nywch.bhdr.next;
        while (wchentry != &im->nywch.bhdr) {
            wch = FIELD_TO_STRUCTURE(wchentry, KRtiWatch, abwchentry);
            wchentry = wchentry->next;
            if (0 == strcmp(wch->forid, id)) {
                remove_dlist_entry(&wch->abwchentry);
                insert_dlist_tail_entry(&rec->bwchhdr, &wch->abwchentry);
            }
        }

        if (RF_AUTOSET & flg)
            setrec(im, rec, (kvoid*)def, knil, knil);
    }

    ksyn_lck_rel(im->lck);
    return rec;
}
kint kim_addstr(KIM *im, const kchar *id, kchar *def, kint flg, ATPROC at, const kchar *desc)
{
    KRtiRec *rec = addrec(im, id, RT_STR, (kvoid*)def, flg, at, desc);
    return rec ? 0 : -1;
}
kint kim_addptr(KIM *im, const kchar *id, kvoid *def, kint flg, ATPROC at, const kchar *desc)
{
    KRtiRec *rec = addrec(im, id, RT_PTR, (kvoid*)def, flg, at, desc);
    return rec ? 0 : -1;
}
kint kim_addint(KIM *im, const kchar *id, kint def, kint flg, ATPROC at, const kchar *desc)
{
    KRtiRec *rec = addrec(im, id, RT_INT, (kvoid*)def, flg, at, desc);
    return rec ? 0 : -1;
}

/*---------------------------------------------------------------------------------
 * Remove Value from RTI
 */
static kint delrec(KIM *im, KRtiRec *rec)
{
    KRtiWatch *wch;
    K_dlist_entry *wchentry;

    ksyn_lck_get(im->lck);

    remove_dlist_entry(&rec->entry);
    switch (rec->var.type) {
        case RT_STR:
            kmem_free_s(rec->var.use.sv);
            kmem_free_s(rec->var.def.sv);
            break;
        case RT_PTR:
        case RT_INT:
            break;
        default:
            kerror(("BAD RTIREC Type : %d for %s\n", rec->var.type, rec->id));
            break;
    }

    /* del watch list */
    wchentry = rec->awchhdr.next;
    while (wchentry != &rec->awchhdr) {
        wch = FIELD_TO_STRUCTURE(wchentry, KRtiWatch, abwchentry);
        wchentry = wchentry->next;

        remove_dlist_entry(&wch->abwchentry);
        insert_dlist_tail_entry(&im->nywch.ahdr, &wch->abwchentry);
    }
    wchentry = rec->bwchhdr.next;
    while (wchentry != &rec->bwchhdr) {
        wch = FIELD_TO_STRUCTURE(wchentry, KRtiWatch, abwchentry);
        wchentry = wchentry->next;

        remove_dlist_entry(&wch->abwchentry);
        insert_dlist_tail_entry(&im->nywch.bhdr, &wch->abwchentry);
    }

    kmem_free(rec->id);
    kmem_free_s(rec->desc);
    kmem_free(rec);

    ksyn_lck_rel(im->lck);
    return 0;
}
kint kim_delstr(KIM *im, const kchar *id)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_STR == rec->var.type);
        return delrec(im, rec);
    } else
        kerror(("KIM: kim_delstr: not found record : %s\n", id));
    return -1;
}
kint kim_delptr(KIM *im, const kchar *id)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_PTR == rec->var.type);
        return delrec(im, rec);
    } else
        kerror(("KIM: kim_delptr: not found record : %s\n", id));
    return -1;
}
kint kim_delint(KIM *im, const kchar *id)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_INT == rec->var.type);
        return delrec(im, rec);
    } else
        kerror(("KIM: kim_delint: not found record : %s\n", id));
    return -1;
}

/*---------------------------------------------------------------------------------
 * Set value
 */
static kint setrec(KIM *im, KRtiRec *rec, kvoid *val, kvoid **ua, kvoid **ub)
{
    K_dlist_entry *entry;
    KRtiWatch *watch;

    ksyn_lck_get(im->lck);

    /* for disable recorder skip */
    if (RF_DISABLE & rec->flg) {
        klog(("set disable recorder, ignored\n"));
        ksyn_lck_rel(im->lck);
        return 0;
    }

    rec->val = val;
    rec->ua = ua;
    rec->ub = ub;

    /* upper filter */
    entry = rec->bwchhdr.next;
    while (entry != &rec->bwchhdr) {
        watch = FIELD_TO_STRUCTURE(entry, KRtiWatch, abwchentry);
        if (watch->wch)
            watch->wch(im, rec, watch->ua, watch->ub, WT_UP);
        entry = entry->next;
    }

    if (rec->at) {
        rec->flg |= RF_PROCESS;
        rec->at(im, rec, ATRSN_SET);
        rec->flg &= ~RF_PROCESS;
    }

    switch (rec->var.type) {
        case RT_STR:
            kmem_free_s(rec->var.use.sv);
            if (val)
                rec->var.use.sv = kstr_dup((kchar*)val);
            else
                rec->var.use.sv = knil;
            break;
        case RT_PTR:
            rec->var.use.pv = (kvoid*)val;
            break;
        case RT_INT:
            rec->var.use.iv = (kint)val;
            break;
        default:
            break;
    }

    /* lower filter */
    entry = rec->awchhdr.next;
    while (entry != &rec->awchhdr) {
        watch = FIELD_TO_STRUCTURE(entry, KRtiWatch, abwchentry);
        if (watch->wch)
            watch->wch(im, rec, watch->ua, watch->ub, WT_LO);
        entry = entry->next;
    }

    ksyn_lck_rel(im->lck);
    return 0;
}
kint kim_setstr(KIM *im, const kchar *id, kchar *val, kvoid **ua, kvoid **ub)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_STR == rec->var.type);
        return setrec(im, rec, (kvoid*)val, ua, ub);
    } else
        kerror(("KIM: kim_setstr: not found record : %s\n", id));
    return -1;
}
kint kim_setptr(KIM *im, const kchar *id, kvoid *val, kvoid **ua, kvoid **ub)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_PTR == rec->var.type);
        return setrec(im, rec, (kvoid*)val, ua, ub);
    } else
        kerror(("KIM: kim_setptr: not found record : %s\n", id));
    return -1;
}
kint kim_setint(KIM *im, const kchar *id, kint val, kvoid **ua, kvoid **ub)
{
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec) {
        assert(RT_INT == rec->var.type);
        return setrec(im, rec, (kvoid*)val, ua, ub);
    } else
        kerror(("KIM: kim_setint: not found record : %s\n", id));
    return -1;
}

/*---------------------------------------------------------------------------------
 * Get value
 */
static kvoid* getrec(KIM *im, KRtiRec *rec, kint *err)
{
    kvoid* ret = knil;

    ksyn_lck_get(im->lck);

    *err = 0;
    switch (rec->var.type) {
        case RT_STR:
            ret = (kvoid*)rec->var.use.sv;
            break;
        case RT_PTR:
            ret = (kvoid*)rec->var.use.pv;
            break;
        case RT_INT:
            ret = (kvoid*)rec->var.use.iv;
            break;
        default:
            kerror(("KIM: getrec: BAD type for %s\n", rec->id));
            *err = 1;
            break;
    }

    ksyn_lck_rel(im->lck);

    return ret;
}
kchar* kim_getstr(KIM *im, const kchar *id, kint *err)
{
    kint ec = -1;
    kvoid *ret = knil;
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec)
        if (RT_STR == rec->var.type)
            ret = getrec(im, rec, &ec);
        else
            kerror(("KIM: kim_getstr: not found record : %s\n", id));
    if (err)
        *err = ec;
    return (kchar*)ret;
}
kvoid* kim_getptr(KIM *im, const kchar *id, kint *err)
{
    kint ec = -1;
    kvoid *ret = knil;
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec)
        if (RT_PTR == rec->var.type)
            ret = getrec(im, rec, &ec);
        else
            kerror(("KIM: kim_getptr: not found record : %s\n", id));
    if (err)
        *err = ec;
    return (kvoid*)ret;
}
kint kim_getint(KIM *im, const kchar *id, kint *err)
{
    kint ec = -1;
    kvoid *ret = knil;
    KRtiRec *rec = kim_chkrec(im, id);
    if (rec)
        if (RT_INT == rec->var.type)
            ret = getrec(im, rec, &ec);
        else
            kerror(("KIM: kim_getint: not found record : %s\n", id));
    if (err)
        *err = ec;
    return (kint)ret;
}

/*---------------------------------------------------------------------------------
 * Add watch
 */
static kbean addwch(KIM *im, const kchar *id, WCHPROC watch, kuchar type, kvoid *ua, kvoid *ub, const kchar *desc)
{
    KRtiWatch *wch = (KRtiWatch*)kmem_alloz(sizeof(KRtiWatch));

    ksyn_lck_get(im->lck);

    if (wch) {
        KRtiRec *rec = kim_chkrec(im, id);
        init_dlist_head(&wch->abwchentry);
        wch->wch = watch;
        wch->desc = desc ? kstr_dup(desc) : knil;
        wch->forid = kstr_dup(id);
        wch->ua = ua;
        wch->ub = ub;

        if (rec) {
            if (WT_LO == type)
                insert_dlist_tail_entry(&rec->awchhdr, &wch->abwchentry);
            else
                insert_dlist_tail_entry(&rec->bwchhdr, &wch->abwchentry);
        } else {
            /*
             * some watch may be added before the im item added
             * so, pending these watch till the im item add
             */
            if (WT_LO == type)
                insert_dlist_tail_entry(&im->nywch.ahdr, &wch->abwchentry);
            else
                insert_dlist_tail_entry(&im->nywch.bhdr, &wch->abwchentry);
        }
    }

    ksyn_lck_rel(im->lck);

    return (kbean)wch;
}
kbean kim_addawch(KIM *im, const kchar *id, WCHPROC watch, kvoid *ua, kvoid *ub, const kchar *desc)
{
    return addwch(im, id, watch, WT_LO, ua, ub, desc);
}
kbean kim_addbwch(KIM *im, const kchar *id, WCHPROC watch, kvoid *ua, kvoid *ub, const kchar *desc)
{
    return addwch(im, id, watch, WT_UP, ua, ub, desc);
}

/*---------------------------------------------------------------------------------
 * Del watch
 */
static kint delwch(KIM *im, kbean watch)
{
    KRtiWatch *wch = (KRtiWatch*)watch;
    kint ret = -1;

    ksyn_lck_get(im->lck);

    if (wch) {
        kmem_free(wch->forid);
        kmem_free_s(wch->desc);
        remove_dlist_entry(&wch->abwchentry);
        kmem_free(wch);
        ret = 0;
    }

    ksyn_lck_rel(im->lck);

    return ret;
}
kint kim_delawch(KIM *im, kbean watch)
{
    return delwch(im, watch);
}
kint kim_delbwch(KIM *im, kbean watch)
{
    return delwch(im, watch);
}

/*---------------------------------------------------------------------------------
 * new and del
 */
KIM* kim_new(KIM *im)
{
    if (!im)
        im = (KIM*)kmem_alloc(sizeof(KIM));
    if (im) {
        init_dlist_head(&im->rechdr);
        init_dlist_head(&im->nywch.bhdr);
        init_dlist_head(&im->nywch.ahdr);
        im->lck = ksyn_lck_new();
    }
    return im;
}
kint kim_del(KIM *im)
{
    KRtiRec *rec;
    KRtiWatch *wch;

    klog(("\n\n\n\nkim_del:%x\n", im));

    if (im) {
        ksyn_lck_get(im->lck);
        while (!is_dlist_empty(&im->nywch.ahdr)) {
            wch = FIELD_TO_STRUCTURE(im->nywch.ahdr.next, KRtiWatch, abwchentry);
            remove_dlist_entry(&wch->abwchentry);

            klog(("kim_del::nywach.ahdr: %s\n", wch->forid));

            kmem_free(wch->forid);
            kmem_free_s(wch->desc);
            kmem_free(wch);
        }
        while (!is_dlist_empty(&im->nywch.bhdr)) {
            wch = FIELD_TO_STRUCTURE(im->nywch.bhdr.next, KRtiWatch, abwchentry);
            remove_dlist_entry(&wch->abwchentry);

            klog(("kim_del::nywach.bhdr: %s\n", wch->forid));

            kmem_free(wch->forid);
            kmem_free_s(wch->desc);
            kmem_free(wch);
        }

        while (!is_dlist_empty(&im->rechdr)) {
            rec = FIELD_TO_STRUCTURE(im->rechdr.next, KRtiRec, entry);
            remove_dlist_entry(&rec->entry);

            klog(("kim_del::rechdr: %s\n", rec->id));

            while (!is_dlist_empty(&rec->bwchhdr)) {
                wch = FIELD_TO_STRUCTURE(rec->bwchhdr.next, KRtiWatch, abwchentry);
                remove_dlist_entry(&wch->abwchentry);

                klog(("kim_del::rechdr.bhdr: %s:%s\n", rec->id, wch->forid));

                kmem_free(wch->forid);
                kmem_free_s(wch->desc);
                kmem_free(wch);
            }
            while (!is_dlist_empty(&rec->awchhdr)) {
                wch = FIELD_TO_STRUCTURE(rec->awchhdr.next, KRtiWatch, abwchentry);
                remove_dlist_entry(&wch->abwchentry);

                klog(("kim_del::rechdr.ahdr: %s:%s\n", rec->id, wch->forid));

                kmem_free(wch->forid);
                kmem_free_s(wch->desc);
                kmem_free(wch);
            }

            kmem_free(rec->id);
            kmem_free_s(rec->desc);

            if (rec->var.type == RT_STR) {
                kmem_free_s(rec->var.use.sv);
                kmem_free_s(rec->var.def.sv);
            }

            kmem_free(rec);
        }
        ksyn_lck_rel(im->lck);

        assert(is_dlist_empty(&im->nywch.bhdr));
        assert(is_dlist_empty(&im->nywch.ahdr));

        ksyn_lck_del(im->lck);

        kmem_free(im);
        return 0;
    }
    return -1;
}

kint kim_start(KIM *im)
{
    klog(("kim_start: %x\n", im));
    return 0;
}
kint kim_stop(KIM *im)
{
    klog(("kim_stop: %x\n", im));
    return 0;
}

