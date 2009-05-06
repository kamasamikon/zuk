/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <ksal.h>

#include <kflg.h>
#include <kdbg.h>

#include <ktmr.h>
#include <ktsk.h>
#include <kmem.h>
#include <ksyn.h>

/**
 * internal used structue
 */
typedef struct _ktmr {
    /**
     * Queue item, queue the global timer item queue.
     */
    K_dlist_entry ent;

    kuint flg;

    /**
     * target task, if set, KMSG_TIMER will be send to this task.
     */
    kbean tsk;

    /**
     * the timer, in millisecond this timer be processed.
     */
    kuint proctime;

    /**
     * base time or start time for each time the timer is set.
     * When ktmr::time + ktmr::elapse larger than current time
     * in millisecond, the timer is elapse and been processed.
     */
    kuint time;

    /**
     * define the time gap in millisecond to elapse time.
     */
    kuint elapse;

    /**
     * If the use do not want the timer be processed in task,
     * a timer callback can be set. If cbk set, always call
     * the callback even if the ktmr::tsk is set.
     */
    kint (*cbk)(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3);

    /**
     * user parameters used for ktmr::cbk or for KMSG_TIMER message.
     */
    kvoid *ar0, *ar1, *ar2, *ar3;
} ktmr;

static kint __g_tmr_init_cnt = 0;

volatile kbool __g_tmr_stop;
volatile kbool __g_tmr_stopped;

volatile kint __g_tmr_wait_tmo;
static kbean __g_tmr_wait_sema;

static K_dlist_entry __g_tmr_hdr;
static kbean __g_tmr_tsk;

static kbean __g_tmr_qlck;

/**
 * @brief
 *
 * @return
 */
static kvoid ktmr_task(kvoid)
{
    K_dlist_entry *list_entry;
    kuint cur_time;
    ktmr *tmr;
    kint ret, tmp;

    while (kfalse == __g_tmr_stop) {
        tmp = __g_tmr_wait_tmo;
        ret = ksyn_sem_get(__g_tmr_wait_sema, __g_tmr_wait_tmo);

        if (kfalse != __g_tmr_stop) {
            break;
        }

        cur_time = ksal_get_tick();

        ksyn_lck_get(__g_tmr_qlck);

try_again:
        __g_tmr_wait_tmo = 0x7FFFFFFF;

        list_entry = __g_tmr_hdr.next;
        while ((list_entry != &__g_tmr_hdr) && (kfalse == __g_tmr_stop)) {
            tmr = FIELD_TO_STRUCTURE(list_entry, ktmr, ent);
            list_entry = list_entry->next;

            if (tmr->proctime == cur_time) {
                /* the timer has been callback or messaged */
                tmr->proctime = 0;

                if (!kflg_chk(tmr->flg, TMF_LOOP)) {
                    remove_dlist_entry(&tmr->ent);
                    init_dlist_head(&tmr->ent);
                    kmem_free(tmr);

                    /* er, this timer is deleted, so do not go to calculate the wait time */
                    continue;
                } else {
                    tmr->time = cur_time;
                }
            } else if ((tmr->time + tmr->elapse) <= cur_time) {

                tmr->proctime = cur_time;
                ksyn_lck_rel(__g_tmr_qlck);
                if (tmr->cbk) {
                    tmr->cbk(tmr->ar0, tmr->ar1, tmr->ar2, tmr->ar3);
                } else {
                    kmsg_post(tmr->tsk, KMSG_TIMER, tmr->ar0, tmr->ar1, tmr->ar2, tmr->ar3);
                }
                ksyn_lck_get(__g_tmr_qlck);
                goto try_again;
            }

            if ((kuint)__g_tmr_wait_tmo > (tmr->time + tmr->elapse - cur_time)) {
                __g_tmr_wait_tmo = tmr->time + tmr->elapse - cur_time;
            }
        }
        ksyn_lck_rel(__g_tmr_qlck);
    }

    __g_tmr_stopped = ktrue;

    ksal_tsk_exit();
}

/**
 * @brief
 *
 * @param kvoid
 *
 * @return
 */
kint ktmr_init(kvoid)
{
    if (__g_tmr_init_cnt++) {
        kerror(("ktmr_init, ref:%d\n", __g_tmr_init_cnt));
        return 0;
    }

    __g_tmr_stop = kfalse;
    __g_tmr_stopped = kfalse;

    __g_tmr_wait_tmo = 0x7FFFFFFF;
    __g_tmr_wait_sema = ksyn_sem_new(0);
    kassert(__g_tmr_wait_sema);

    init_dlist_head(&__g_tmr_hdr);

    __g_tmr_qlck = ksyn_lck_new();
    kassert(__g_tmr_qlck);

    __g_tmr_tsk = ksal_tsk_new(ktmr_task, 0, 0, knil);
    kassert(__g_tmr_tsk);

    return 0;
}

/**
 * @brief
 *
 * @param kvoid
 *
 * @return
 */
kint ktmr_final(kvoid)
{
    klog(("ktmr_final, ref:%d\n", __g_tmr_init_cnt));

    if (0 < --__g_tmr_init_cnt) {
        return 0;
    }

    ktmr_killall(knil);

    __g_tmr_stop = ktrue;
    ksyn_sem_rel(__g_tmr_wait_sema);
    while (kfalse == __g_tmr_stopped) {
        ksal_tsk_sleep(100);
    }

    if (0 != __g_tmr_wait_sema) {
        ksyn_sem_del(__g_tmr_wait_sema);
        __g_tmr_wait_sema = 0;
    }

    if (__g_tmr_qlck) {
        ksyn_lck_del(__g_tmr_qlck);
    }

    return 0;
}

/**
 * @brief
 *
 * @param a_tsk
 * @param a_elapse
 * @param a_loop
 * @param a_cbk
 * @param ar0
 * @param ar1
 * @param ar2
 * @param ar3
 *
 * @return
 */
kbean ktmr_set(kbean a_tsk, kuint a_elapse, kbool a_loop,
        kint (*a_cbk)(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3),
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    kuint flg = 0;
    ktmr *tmr;

    if (kfalse != __g_tmr_stop) {
        kerror(("ktmr_set: module unload, exit!\n"));
        return knil;
    }

    tmr = kmem_alloc(sizeof(ktmr));
    if (!tmr) {
        kerror(("ktmr_set: bad memory\n"));
        return knil;
    }

    if (a_loop) {
        flg |= TMF_LOOP;
    }
    tmr->flg = flg;
    tmr->tsk = a_tsk;
    tmr->proctime = 0;
    tmr->time = ksal_get_tick();
    tmr->elapse = a_elapse;
    tmr->cbk = a_cbk;
    tmr->ar0 = a_ar0;
    tmr->ar1 = a_ar1;
    tmr->ar2 = a_ar2;
    tmr->ar3 = a_ar3;

    ksyn_lck_get(__g_tmr_qlck);
    insert_dlist_tail_entry(&__g_tmr_hdr, &tmr->ent);
    ksyn_lck_rel(__g_tmr_qlck);
    ksyn_sem_rel(__g_tmr_wait_sema);

    return (kbean)tmr;
}

/*
 * kill all the timer according to a_tsk.
 * if knil == a_tsk, kill all for all task
 */
/**
 * @brief Kill all the timer belong to some task.
 *
 * @warning Some caller set resources which will be freed in the timer callback,
 * so, if directly freed the timer, the resources should be freed in timer callback will
 * be leaked.
 *
 * @param a_tsk Specific a ktsk bean, if set to be zero, means all the task.
 * @warning KTSK_ANY to replace zero.
 *
 * @return none
 */
kvoid ktmr_killall(kbean a_tsk)
{
    ktmr *tmr = knil;
    K_dlist_entry *list_entry;

    if (kfalse != __g_tmr_stop) {
        kerror(("ktmr_killall: module unload, exit!\n"));
        return;
    }

    ksyn_lck_get(__g_tmr_qlck);
    list_entry = __g_tmr_hdr.next;
    while (list_entry != &__g_tmr_hdr) {
        tmr = FIELD_TO_STRUCTURE(list_entry, ktmr, ent);
        list_entry = list_entry->next;

        if ((!a_tsk) || (a_tsk == tmr->tsk)) {
            remove_dlist_entry(&tmr->ent);
            init_dlist_head(&tmr->ent);

            kmem_free(tmr);
        }
    }
    ksyn_lck_rel(__g_tmr_qlck);
}

/**
 * @brief
 *
 * @param a_tmr
 *
 * @return
 */
kbool ktmr_kill(kbean a_tmr)
{
    ktmr *tmptmr = knil;
    K_dlist_entry *list_entry;

    if (kfalse != __g_tmr_stop) {
        kerror(("ktmr_kill: module unload, exit!\n"));
        return kfalse;
    }

    ksyn_lck_get(__g_tmr_qlck);
    list_entry = __g_tmr_hdr.next;
    while (list_entry != &__g_tmr_hdr) {
        tmptmr = FIELD_TO_STRUCTURE(list_entry, ktmr, ent);
        list_entry = list_entry->next;

        if ((kbean)tmptmr == a_tmr) {

            remove_dlist_entry(&tmptmr->ent);
            init_dlist_head(&tmptmr->ent);

            kmem_free(tmptmr);

            ksyn_lck_rel(__g_tmr_qlck);
            return ktrue;
        }
    }

    ksyn_lck_rel(__g_tmr_qlck);
    return kfalse;
}

