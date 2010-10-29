/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#include <ksal.h>

#include <sdlist.h>
#include <kdbg.h>

#include <kmem.h>
#include <ksyn.h>
#include <ktsk.h>

/*
 * internal structure, should not public to caller
 */
typedef struct _sync_lock {
	kbean sema;

	struct {
		kbean tsk;
		kint ref;
	} owner;
} sync_lock;

/*
 * initial state for a lock is unlocked
 */
kbean ksyn_lck_new(kvoid)
{
	sync_lock *lck = (sync_lock *) kmem_alloz(sizeof(sync_lock));
	if (lck) {
		lck->sema = ksal_sema_new(1);
	}
	return (kbean) lck;
}

kint ksyn_lck_del(kbean a_lck)
{
	sync_lock *lck = (sync_lock *) a_lck;
	if (lck && lck->sema) {
		ksal_sema_del(lck->sema);
	}
	ksal_mem_free(lck);
	return 0;
}

kvoid ksyn_lck_get(kbean a_lck)
{
	sync_lock *lck = (sync_lock *) a_lck;
	kbean curtsk = ksal_tsk_cur();
	kassert(curtsk);

	if (lck && (lck->owner.tsk == curtsk)) {
		lck->owner.ref++;
		return;
	}
	ksal_sema_get(lck->sema, -1);
	if (lck->owner.tsk == curtsk) {
		lck->owner.ref++;
	} else {
		lck->owner.tsk = curtsk;
		lck->owner.ref = 1;
	}
}

kvoid ksyn_lck_rel(kbean a_lck)
{
	sync_lock *lck = (sync_lock *) a_lck;
	kbean curtsk = ksal_tsk_cur();
	kassert(curtsk);

	if (lck && (lck->owner.tsk == curtsk)) {
		lck->owner.ref--;
		if (0 == lck->owner.ref) {
			lck->owner.tsk = 0;
			ksal_sema_rel(lck->sema);
		}
	} else {
		kassert(!"SHIT!! unlock not ownered lock");
	}
}

kbean ksyn_sem_new(kint a_initval)
{
	return ksal_sema_new(a_initval);
}

kint ksyn_sem_del(kbean a_sema)
{
	return ksal_sema_del(a_sema);
}

kint ksyn_sem_get(kbean a_sema, kint a_timeout)
{
	return ksal_sema_get(a_sema, a_timeout);
}

kint ksyn_sem_rel(kbean a_sema)
{
	return ksal_sema_rel(a_sema);
}
