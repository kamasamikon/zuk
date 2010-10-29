/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#define DBG_HIDE_CUR_LOG

#include <string.h>

#include <ktypes.h>
#include <sdlist.h>

#include <kdbg.h>

#include <ktsk.h>
#include <kmem.h>
#include <ksyn.h>
#include <kstr.h>

#include <kflg.h>

/*---------------------------------------------------------------------------------
 * internal struecture, only used in current file.
 */

/**
 * @defgroup kmsg
 * @{
 */
typedef struct _kmsg_slot {
	kuint msg;			    /**< message */
	KMSG_DISP disp;		    /**< dispatch routine for msg */
} kmsg_slot;

typedef struct _kmsg {
	K_dlist_entry ent;		    /**< used for queue into ktsk::msg_qhdr */
	kuint serial;		    /**< uniq id, auto inc when each new msg */
	kuint flg;			    /**< DYNMEM, POST, */

	kuint msg;
	kvoid *ur0, *ur1, *ur2, *ur3;
} kmsg;
/** @} */

#define KTF_DYNMEM      0x00000001
#define KTF_QUITING     0x00000002

typedef struct _ktsk {
	K_dlist_entry ent;		    /**< queue to global task header: ::__g_tsk_hdr */

	kuint next_serial;		    /**< msg guid for next msg */

	kuint flg;

	/*
	 * user use tskid instead of kbean struecture pointer to
	 * identify a tsk, this tsk will auto increament when a
	 * tsk is created.
	 */
	kbean tskid;

	/**
	 * @name User data and User callback
	 * @{
	 */
	kchar *name;
	KTSK_PROC mainproc;
	kvoid *ur0, *ur1, *ur2, *ur3;
	/** @} */

	kbean msg_wait_sema;
	kbean msg_qlck;
	K_dlist_entry msg_qhdr;

	kbean msg_send_wait_sema;

	/**
	 * Current processing message, when the task running  it can not be NULL.
	 * When message is sent, the message should not saved as curmsg.
	 */
	kmsg *curmsg;

	/**
	 * @name Message Slot
	 * @brief User registered dispatch routines for message.\n
	 * Slot_arr will grow when nor room for new dispatch.
	 * @{
	 */
	kuint slot_cnt;		    /**< sizeof slot_arr array */
	kmsg_slot *slot_arr;	    /**< pointer to array save message and dispatch map */
	/** @} */
} ktsk;

static kbool ktsk_valid(kbean a_tsk);
static kvoid ktsk_free_res(kbean a_tsk);
static kbool kmsg_push(kbean a_tsk, kmsg *a_msg);
static kvoid kmsg_windup(kbean a_tsk);
static kbool kmsg_cleanup(kbean a_tsk);

/**
 * @brief ktsk::slot_arr increament step.\n
 * When call ::kmsg_slot_set, it make the count of message
 * dispatch function exceed ktsk::slot_cnt. Then the
 * ktsk::slot_arr arrary should be reallocated to hold
 * more message dispatch map. In each time of reallocated,
 * the new size will be increase from previous ktsk::slot_cnt.
 */
#define SLOT_GROW_STEP 8

/** All task list header which maintain all the task created by ::ktsk_new */
static K_dlist_entry __g_tsk_hdr = { &__g_tsk_hdr, &__g_tsk_hdr };

/**
 * @brief Append or replace message dispatch routine for message.
 * @warning Only one dispatch for a message.
 *
 * @param a_tsk Target task.
 * @param a_msg Message ID, can not be KMSG_ANY and KMSG_NULL.
 * @param a_disp Dispatch routine for a_msg.
 *
 * @return Return the position the new dispatch append or replace.
 */
kint kmsg_slot_set(kbean a_tsk, kuint a_msg, KMSG_DISP a_disp)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kvoid *arr;
	kuint i, j;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_slot_set: bad task\n"));
		return -1;
	}

	if (KMSG_ANY == a_msg || KMSG_NULL == a_msg) {
		kerror(("kmsg_slot_set: can not set slot for KMSG_NULL or KMSG_ANY\n"));
		return -1;
	}

	ksyn_lck_get(tsk->msg_qlck);
	for (i = 0; i < tsk->slot_cnt; i++)
		if (tsk->slot_arr[i].msg == a_msg) {
			tsk->slot_arr[i].disp = a_disp;
			goto found;
		}

	for (i = 0; i < tsk->slot_cnt; i++)
		if (tsk->slot_arr[i].msg == KMSG_NULL) {
			tsk->slot_arr[i].msg = a_msg;
			tsk->slot_arr[i].disp = a_disp;
			goto found;
		}

	tsk->slot_cnt += SLOT_GROW_STEP;

	arr = kmem_alloz(tsk->slot_cnt * sizeof(kmsg_slot));
	if (arr) {
		kvoid *oldarr = tsk->slot_arr;
		memcpy(arr, tsk->slot_arr, (tsk->slot_cnt - SLOT_GROW_STEP) * sizeof(kmsg_slot));
		tsk->slot_arr = arr;
		for (j = tsk->slot_cnt - SLOT_GROW_STEP; j < tsk->slot_cnt; j++) {
			tsk->slot_arr[j].msg = KMSG_NULL;
			tsk->slot_arr[j].disp = knil;
		}
		kmem_free(oldarr);
	}

found:
	tsk->slot_arr[i].msg = a_msg;
	tsk->slot_arr[i].disp = a_disp;
	ksyn_lck_rel(tsk->msg_qlck);
	return i;
}

/**
 * @brief Return the dispatch for a_msg.
 *
 * @param a_tsk Target task.
 * @param a_msg Message ID.
 * @param a_disp Dispatch routine for a_msg.
 *
 * @return Position of the dispatch found for a_msg, -1 if not found.
 */
kint kmsg_slot_get(kbean a_tsk, kuint a_msg, KMSG_DISP *a_disp)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kuint i;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_slot_get: bad task\n"));
		return -1;
	}

	ksyn_lck_get(tsk->msg_qlck);
	for (i = 0; i < tsk->slot_cnt; i++)
		if (tsk->slot_arr[i].msg == a_msg) {
			*a_disp = tsk->slot_arr[i].disp;
			ksyn_lck_rel(tsk->msg_qlck);
			return i;
		}

	ksyn_lck_rel(tsk->msg_qlck);
	return -1;
}

/**
 * @brief Delete a message dispatch routine.
 * If a_msg == KMSG_ANY, all the message dispatch will be deleted.
 *
 * @param a_tsk Target task.
 * @param a_msg The message for which the dispatch will be deleted.
 *
 * @return 0 for success, -1 for error.
 */
kint kmsg_slot_del(kbean a_tsk, kuint a_msg)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kuint i;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_slot_del: bad task\n"));
		return -1;
	}

	ksyn_lck_get(tsk->msg_qlck);
	if (KMSG_ANY == a_msg) {
		/* delete all the slots */
		for (i = 0; i < tsk->slot_cnt; i++) {
			tsk->slot_arr[i].msg = KMSG_NULL;
			tsk->slot_arr[i].disp = knil;
		}
		ksyn_lck_rel(tsk->msg_qlck);

		return 0;
	}
	for (i = 0; i < tsk->slot_cnt; i++)
		if (tsk->slot_arr[i].msg == a_msg) {
			tsk->slot_arr[i].msg = KMSG_NULL;
			tsk->slot_arr[i].disp = knil;
			ksyn_lck_rel(tsk->msg_qlck);
			return 0;
		}

	ksyn_lck_rel(tsk->msg_qlck);
	return -1;
}

/**
 * @brief Return user data saved when ktsk_new.
 *
 * @param a_tsk Target task.
 * @param a_ur0 ktsk::ur0;
 * @param a_ur1 ktsk::ur1;
 * @param a_ur2 ktsk::ur2;
 * @param a_ur3 ktsk::ur3;
 *
 * @return 0 for success, -1 for error.
 *
 * @see ::ktsk_new
 */
kint ktsk_get_ars(kbean a_tsk, kvoid **a_ur0, kvoid **a_ur1, kvoid **a_ur2, kvoid **a_ur3)
{
	ktsk *tsk = (ktsk *) a_tsk;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_slot_get: bad task\n"));
		return -1;
	}
	if (a_ur0)
		*a_ur0 = tsk->ur0;
	if (a_ur1)
		*a_ur1 = tsk->ur1;
	if (a_ur2)
		*a_ur2 = tsk->ur2;
	if (a_ur3)
		*a_ur3 = tsk->ur3;
	return 0;
}

/**
 * @brief Check if a task exist for given a_tsk.
 * a valid task is the task still in task queue
 * even if the task is quiting
 *
 * @param a_tsk Target task.
 *
 * @return ktrue for valid task, kfalse for invalid task.
 */
static kbool ktsk_valid(kbean a_tsk)
{
	ktsk *tsk = (ktsk *) a_tsk;
	ktsk *tmptsk;
	K_dlist_entry *entry;

	if (!a_tsk)
		return kfalse;

	if (KTSK_ALL == a_tsk)
		return ktrue;

	entry = __g_tsk_hdr.next;
	while (entry != &__g_tsk_hdr) {
		tmptsk = FIELD_TO_STRUCTURE(entry, ktsk, ent);
		entry = entry->next;

		if (tsk == tmptsk)
			return ktrue;
	}
	return kfalse;
}

/**
 * @brief suspend current task for a_ms milliseconds.
 *
 * @param a_ms milliseconds to sleep.
 *
 * @return none.
 */
kvoid ktsk_sleep(kuint a_ms)
{
	ksal_tsk_sleep(a_ms);
}

/**
 * @brief Return the current ktsk.
 *
 * @param kvoid
 *
 * @return Current ktsk the caller run.
 * @retval knil When the caller task not created by ::ktsk_new or caller not in task.
 */
kbean ktsk_cur(kvoid)
{
	ktsk *tsk;
	K_dlist_entry *ent;
	kbean sys_tsk_cur = ksal_tsk_cur();

	ent = __g_tsk_hdr.next;
	while (ent != &__g_tsk_hdr) {
		tsk = FIELD_TO_STRUCTURE(ent, ktsk, ent);
		ent = ent->next;

		if (tsk->tskid == sys_tsk_cur)
			return (kbean) tsk;
	}
	return (kbean) knil;
}

/**
 * @brief check if the specified task quiting
 *
 * @param a_tsk the task id
 *
 * @return kture for still running, kfalse for quiting or error
 */
kbool ktsk_running(kbean a_tsk)
{
	ktsk *tsk = (ktsk *) a_tsk;
	if (kflg_chk(tsk->flg, KTF_QUITING))
		return kfalse;
	return ktrue;
}

/*
 * in some platform the task main can not start will any parameter,
 * which make the tsk parameter can be a invalid value
 */
/**
 * @brief Main proc for OS layer task or thread.\n
 * Set in ::ktsk_new as inner use.\n
 * In different platform, process routine for OS layer task or thread may
 * has different prototype. To work with all this occassion, this routine
 * do not used any parameters.
 *
 * @warning This function is the last for this task, when this function
 * returns, the task or thread will be destroyed by OS layer.
 *
 * @return none.
 */
static kvoid ktsk_def_proc(kvoid)
{
	ktsk *tsk = knil;

	while (!tsk) {
		tsk = (kbean) ktsk_cur();
		ksal_tsk_sleep(50);
	}

	/* when call the user task main, the task uid must have been gotten */
	klog(("into ktsk_def_proc, tskid:%x, name:%s proc:%x\n", tsk->tskid, tsk->name, tsk->mainproc));

	if (tsk->mainproc)
		tsk->mainproc(tsk->ur0, tsk->ur1, tsk->ur2, tsk->ur3);
	else {
		kuint msg;
		kvoid *ur0, *ur1, *ur2, *ur3;

		while (kmsg_peek((kbean) tsk, &msg, &ur0, &ur1, &ur2, &ur3)) {
			/*
			 * loop till quit message has been processed
			 */

			klog(("will disp: %s, %x\n", tsk->name, msg));
			kmsg_disp((kbean) tsk, msg, ur0, ur1, ur2, ur3, KMDR_NORMAL);
			klog(("end disp: %s, %x\n", tsk->name, msg));

			/* windup for the current message set by kmsg_peek */
			kmsg_windup((kbean) tsk);
			klog(("windup done for current message\n"));
		}
	}

	klog(("goto free all the queued message, %s\n", tsk->name));
	kmsg_cleanup((kbean) tsk);

	klog(("goto delete all the registered message dispatch, %s\n", tsk->name));
	kmsg_slot_del((kbean) tsk, KMSG_ANY);

	klog(("leave ktsk_def_proc, name: %s, tskid:%x, proc:%x\n", tsk->name, tsk->tskid, tsk->mainproc));

	/*
	 * free the resource, MUST called before ksal_tsk_exit
	 */
	ktsk_free_res((kbean) tsk);

	klog(("task resource freed, the tast vanished from the world! Bye\n"));

	/* the last statement in this task */
	ksal_tsk_exit();
}

/**
 * @brief Create a new task or thread.
 *
 * @param a_name name of the task, optional.
 * @param a_mainproc User defined process function, if knil, the task run in message driven mode.
 * @param a_ur0 User data.
 * @param a_ur1 User data.
 * @param a_ur2 User data.
 * @param a_ur3 User data.
 *
 * @return Identifer to created task, knil for error.
 */

kbean ktsk_new(const kchar *a_name, KTSK_PROC a_mainproc, kint a_prio,
		kint a_stack_size, kvoid *a_ur0, kvoid *a_ur1, kvoid *a_ur2, kvoid *a_ur3)
{
	ktsk *tsk = (ktsk *) kmem_alloz(sizeof(ktsk));
	if (!tsk) {
		kerror(("ktsk_new: no memory\n"));
		return knil;
	}

	tsk->name = kstr_dup(a_name);
	tsk->mainproc = a_mainproc;
	tsk->ur0 = a_ur0;
	tsk->ur1 = a_ur1;
	tsk->ur2 = a_ur2;
	tsk->ur3 = a_ur3;

	tsk->next_serial = 1;

	/* initial state is non-signaled */
	tsk->msg_wait_sema = ksyn_sem_new(0);
	kassert(tsk->msg_wait_sema);

	tsk->msg_qlck = ksyn_lck_new();
	kassert(tsk->msg_qlck);

	init_dlist_head(&tsk->msg_qhdr);

	tsk->msg_send_wait_sema = ksyn_sem_new(0);
	kassert(tsk->msg_send_wait_sema);

	tsk->curmsg = knil;

	tsk->slot_cnt = 0;
	tsk->slot_arr = knil;

	tsk->tskid = ksal_tsk_new(ktsk_def_proc, a_prio, a_stack_size, tsk);
	kassert(tsk->tskid);

	insert_dlist_tail_entry(&__g_tsk_hdr, &tsk->ent);
	return (kbean) tsk;
}

/**
 * @brief Delete a task or thread created by ::ktsk_new
 *
 * @param a_tsk Target task.
 *
 * @return 0 for success, -1 for error.
 */
kint ktsk_del(kbean a_tsk)
{
	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("ktsk_del: bad task\n"));
		return -1;
	}

	return kmsg_send(a_tsk, KMSG_QUIT, 0, 0, 0, 0);
}

/*
 * destroy the ktsk structure
 */
/**
 * @brief Free task's resouces, include ktsk structure itself.
 * @warning Only called before OS layer task quit, do not touch
 * ktsk for a_tsk after this function returns.
 *
 * @param a_tsk Target task.
 *
 * @return none.
 */
static kvoid ktsk_free_res(kbean a_tsk)
{
	ktsk *tsk = (ktsk *) a_tsk;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("ktsk_free_res: bad task\n"));
		return;
	}

	kassert(kflg_chk(tsk->flg, KTF_QUITING) && "Only called after quiting");
	kassert(is_dlist_empty(&tsk->msg_qhdr)
			&& "Only called when message queue emptied");

	remove_dlist_entry(&tsk->ent);

	if (tsk->msg_wait_sema)
		ksyn_sem_del(tsk->msg_wait_sema);

	if (tsk->msg_qlck)
		ksyn_lck_del(tsk->msg_qlck);

	if (tsk->msg_send_wait_sema)
		ksyn_sem_del(tsk->msg_send_wait_sema);

	/** the slot_arr's dispatch must all be deregistered already */
	kmem_free_zs(tsk->slot_arr);

	kmem_free_zs(tsk->name);
	kmem_free(tsk);
}

/*
 * Can not send msg to KTSK_ALL
 * return 0 for OK, -1 for ERROR
 */
/**
 * @brief Send message to a task and wait till the message been processed.
 *
 * If the target task is KTSK_ALL, the message will be broadcast to all the
 * task in __g_tsk_hdr. And the message will be processed in turn, when all
 * the message for all the task been processed, this function returns.
 *
 * If caller need the return value by message's dispatch, it must return in
 * a_arX or else, because ::kmsg_send return value only indicate the state
 * of queueing the message.
 *
 * @param a_tsk Target task.
 * @param a_msg Message.
 * @param a_ur0 User data used for message dispatch function.
 * @param a_ur1 User data used for message dispatch function.
 * @param a_ur2 User data used for message dispatch function.
 * @param a_ur3 User data used for message dispatch function.
 *
 * @return 0 for success, -1 for error.
 */
kint kmsg_send(kbean a_tsk, kuint a_msg, kvoid *a_ur0, kvoid *a_ur1, kvoid *a_ur2, kvoid *a_ur3)
{
	ktsk *tsk = (ktsk *) a_tsk, *tmptsk;
	kbean curtsk = ksal_tsk_cur();
	K_dlist_entry *ent;
	kmsg msg;

	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_send: bad task\n"));
		return -1;
	}

	if (KTSK_ALL == a_tsk) {
		/*
		 * broadcast the message to all task
		 */

		/* loop for all the task, except current task */
		ent = __g_tsk_hdr.next;
		while (ent != &__g_tsk_hdr) {
			tmptsk = FIELD_TO_STRUCTURE(ent, ktsk, ent);
			ent = ent->next;

			if (curtsk != tmptsk->tskid)
				kmsg_send(tmptsk, a_msg, a_ur0, a_ur1, a_ur2, a_ur3);
			else
				/* save current task */
				tsk = tmptsk;
		}
		/* now for current task */
		kmsg_send(tsk, a_msg, a_ur0, a_ur1, a_ur2, a_ur3);

	} else if (curtsk == tsk->tskid) {
		/*
		 * inplace message
		 */

		if (kflg_chk(tsk->flg, KTF_QUITING)) {
			klog(("task quiting, abort\n"));
			return -1;
		}

		if (KMSG_QUIT == a_msg)
			kmsg_windup((kbean) a_tsk);

		/*
		 * XXX if caller is the same tsk, do not queue it.
		 *
		 * If the message queued, it will block the task itself.
		 * So, all the send message to current task will convert to
		 * direct call. This makes that tsk->curmsg is not modified
		 * and kmsg_peek is not called after kmsg_disp.
		 */
		kmsg_disp((kbean) a_tsk, a_msg, a_ur0, a_ur1, a_ur2, a_ur3, KMDR_NORMAL);

		if (KMSG_QUIT == a_msg) {
			/* special for Quit message, clean all the pending message and quit */

			kflg_set(tsk->flg, KTF_QUITING);

			klog(("goto free all the queued message, %s\n", tsk->name));
			kmsg_cleanup((kbean) a_tsk);

			klog(("goto delete all the registered message dispatch, %s\n", tsk->name));
			kmsg_slot_del((kbean) a_tsk, KMSG_ANY);

			klog(("leave ktsk_def_proc, %s, tskid:%x, proc:%x\n", tsk->name, tsk->tskid, tsk->mainproc));

			/*
			 * free the resource, MUST called before ksal_tsk_exit
			 */
			ktsk_free_res(a_tsk);

			klog(("task resource freed, the tast vanished from the world! Bye\n"));

			/* the last statement in this task */
			/*
			 * XXX What if the system do not support abort current thread?
			 *
			 * another method is setjmp and longjmp:
			 * after call this function, longjmp to the ktsk_def_proc and then
			 * quit the message by leave the ktsk_def_proc function.
			 */
			ksal_tsk_exit();
		}

	} else {
		/*
		 * send to other task
		 */

		if (kflg_chk(tsk->flg, KTF_QUITING)) {
			klog(("task [%s] quiting, abort\n", tsk->name));
			return -1;
		}

		msg.flg = 0;
		msg.serial = tsk->next_serial++;
		msg.msg = a_msg;
		msg.ur0 = a_ur0;
		msg.ur1 = a_ur1;
		msg.ur2 = a_ur2;
		msg.ur3 = a_ur3;

		if (tsk->next_serial == 0)
			tsk->next_serial++;

		kflg_set(msg.flg, KMF_SNDMSG);

		if (!kmsg_push(a_tsk, &msg))
			/*
			 * return kfalse, maybe the task quiting, abort the current
			 * operation
			 */
			return -1;

		/*
		 * should wait till done
		 */
		while (!kflg_chk(msg.flg, KMF_DONE))
			ktsk_sleep(20);

#if 0				/* XXX ksyn_sem can not serialize the queue task when unlock */
		if (0 != ksyn_sem_get(tsk->msg_send_wait_sema, -1)) {
			klog(("msg %x unsuccessfully returned\n", a_msg));
			return -1;
		}
#endif

		klog(("msg %x successfully returned from %s.\n", a_msg, tsk->name));
	}
	return 0;
}

/**
 * @brief Insert a message to task and return immediately, do not wait the
 * message been processed.\n
 * If the target task is KTSK_ALL, the message will be broadcast to all the
 * task in __g_tsk_hdr.
 *
 * @param a_tsk Target task.
 * @param a_msg Message.
 * @param a_ur0 User data used for message dispatch function.
 * @param a_ur1 User data used for message dispatch function.
 * @param a_ur2 User data used for message dispatch function.
 * @param a_ur3 User data used for message dispatch function.
 *
 * @return Message Unique IDentify  for success, 0 for error.
 */
kint kmsg_post(kbean a_tsk, kuint a_msg, kvoid *a_ur0, kvoid *a_ur1, kvoid *a_ur2, kvoid *a_ur3)
{
	ktsk *tsk = (ktsk *) a_tsk, *tmptsk;
	kmsg *msg;
	K_dlist_entry *ent;

	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_post: bad task\n"));
		return 0;
	}

	if (KMSG_NULL == a_msg) {
		kerror(("KMSG_NULL reserved\n"));
		return 0;
	}

	if (KTSK_ALL == a_tsk) {
		ent = __g_tsk_hdr.next;
		while (ent != &__g_tsk_hdr) {
			tmptsk = FIELD_TO_STRUCTURE(ent, ktsk, ent);
			ent = ent->next;

			kmsg_post(tmptsk, a_msg, a_ur0, a_ur1, a_ur2, a_ur3);
		}
		return 0;
	} else {
		if (kflg_chk(tsk->flg, KTF_QUITING)) {
			klog(("task quiting, abort\n"));
			return 0;
		}
		msg = kmem_alloz(sizeof(kmsg));
		if (!msg) {
			kerror(("kmsg_post: no memory for kmsg!\n"));
			return 0;
		}
		msg->serial = tsk->next_serial++;
		msg->msg = a_msg;
		msg->ur0 = a_ur0;
		msg->ur1 = a_ur1;
		msg->ur2 = a_ur2;
		msg->ur3 = a_ur3;

		if (tsk->next_serial == 0)
			tsk->next_serial++;

		kflg_set(msg->flg, KMF_DYNMEM);

		if (kmsg_push(tsk, msg))
			return msg->serial;
		else {
			kmem_free(msg);
			return 0;
		}
	}
}

/**
 * @brief
 * kmsg_kill only work with the POST message
 *
 * @param a_tsk Target task.
 * @param a_type 0 kill all belong to type, 1 kill single
 * @param a_muid
 *
 * @return 0 for success, -1 for error.
 */
kint kmsg_kill(kbean a_tsk, kbool a_type, kuint a_muid)
{
	K_dlist_entry *ent, *hdr;
	ktsk *tsk = (ktsk *) a_tsk;
	kmsg *msg;

	kassert(ktsk_valid(a_tsk));
	if (!ktsk_valid(a_tsk)) {
		kerror(("kmsg_kill: bad task\n"));
		return -1;
	}

	ksyn_lck_get(tsk->msg_qlck);
	hdr = &tsk->msg_qhdr;
	ent = hdr->next;
	while (ent != hdr) {
		msg = FIELD_TO_STRUCTURE(ent, kmsg, ent);

		if (msg->serial == a_muid && !kflg_chk(msg->flg, KMF_SNDMSG)) {
			remove_dlist_entry(&msg->ent);
			kmsg_disp(a_tsk, msg->msg, msg->ur0, msg->ur1, msg->ur2, msg->ur3, KMDR_CANCEL);
			break;
		}

		ent = ent->next;
	}

	/* can not kill sent message */
	ksyn_lck_rel(tsk->msg_qlck);

	return 0;
}

/**
 * @brief Call the dispatch function for given message and parameters.
 *
 * @param a_tsk Target task.
 * @param a_msg Message.
 * @param a_ur0 User data used for message dispatch function.
 * @param a_ur1 User data used for message dispatch function.
 * @param a_ur2 User data used for message dispatch function.
 * @param a_ur3 User data used for message dispatch function.
 * @param a_rsn Call reason for that message.
 *
 * @return
 */
kint kmsg_disp(kbean a_tsk, kuint a_msg, kvoid *a_ur0, kvoid *a_ur1, kvoid *a_ur2, kvoid *a_ur3, kint a_rsn)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kuint i;
	kmsg_slot *slots;

	kassert(ktsk_valid(a_tsk));

	slots = tsk->slot_arr;
	for (i = 0; i < tsk->slot_cnt; i++)
		if (slots[i].msg == a_msg) {
			slots[i].disp(a_tsk, a_msg, a_ur0, a_ur1, a_ur2, a_ur3, a_rsn);
			return 0;
		}

	return -1;
}

/**
 * @brief Cleanup all the pending message.
 * - Call dispatch function for each message.
 * - Call dispatch function with call reason KMDR_CANCEL
 * - Windup each message.
 *
 * @pre Called when quiting the task.
 *
 * @param a_tsk Target task.
 *
 * @return Always return ktrue.
 */
static kbool kmsg_cleanup(kbean a_tsk)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kmsg *curmsg;
	K_dlist_entry *ent;
	kuint flg;

	klog(("QUITING..., free all the message, %s\n", tsk->name));

	while (!is_dlist_empty(&tsk->msg_qhdr)) {

		ksyn_lck_get(tsk->msg_qlck);
		ent = remove_dlist_head_entry(&tsk->msg_qhdr);
		ksyn_lck_rel(tsk->msg_qlck);

		curmsg = FIELD_TO_STRUCTURE(ent, kmsg, ent);

		kmsg_disp(a_tsk, curmsg->msg, curmsg->ur0, curmsg->ur1, curmsg->ur2, curmsg->ur3, KMDR_CANCEL);

		flg = curmsg->flg;
		if (kflg_chk(flg, KMF_SNDMSG)) {
			klog(("[%s] Return Send Message:%x\n", tsk->name, curmsg->msg));

#if 0				/* XXX ksyn_sem can not serialize the queue task when unlock */
			ksyn_sem_rel(tsk->msg_send_wait_sema);
#endif

			kflg_set(curmsg->flg, KMF_DONE);
		}
		if (kflg_chk(flg, KMF_DYNMEM)) {
			klog(("Free memory for kmsg:%x\n", curmsg->msg));
			kmem_free(curmsg);
		}
	}

	klog(("[%s] QUITING done, all the message freed!\n", tsk->name));

	return ktrue;
}

/**
 * @brief End the process of a message after it been dispatched.
 * Include:\n
 * - releases semaphore for sending message.
 * - free memory for message if neccessary.
 * - mark quit flag if current message is KMSG_QUIT.
 *
 * @pre Must called immediately after ::kmsg_disp().
 *
 * @param a_tsk Target task.
 *
 * @return none.
 *
 * @see ::kmsg_disp
 */
static kvoid kmsg_windup(kbean a_tsk)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kmsg *curmsg;
	kuint msgid, flg;

	kassert(ktsk_valid(a_tsk));

	curmsg = tsk->curmsg;
	tsk->curmsg = knil;

	kassert(curmsg && "kmsg_windup should be called after kmsg_peek in which the tsk->curmsg will be set");
	if (!curmsg) {
		kerror(("kmsg_windup, can not find current message\n"));
		return;
	}

	klog(("windup message: msg:%x, flg:%x ars(%x, %x, %x, %x)\n",
				curmsg->msg, curmsg->flg, curmsg->ur0, curmsg->ur1, curmsg->ur2, curmsg->ur3));

	/* save, for checking QUIT after kmsg is freed for KMF_DYNMEM */
	msgid = curmsg->msg;
	flg = curmsg->flg;

	if (kflg_chk(flg, KMF_SNDMSG)) {
		klog(("[%s] Return Send Message:%x\n", tsk->name, msgid));

#if 0				/* XXX ksyn_sem can not serialize the queue task when unlock */
		ksyn_sem_rel(tsk->msg_send_wait_sema);
#endif

		kflg_set(curmsg->flg, KMF_DONE);
	}
	if (kflg_chk(flg, KMF_DYNMEM)) {
		klog(("Free memory for kmsg:%x\n", msgid));
		kmem_free(curmsg);
	}
	if (KMSG_QUIT == msgid) {
		klog(("QUITING..., free all the message\n"));
		kflg_set(tsk->flg, KTF_QUITING);
	}
}

/*
 * if the curmsg is quit, the routine return kfalse,
 * then the task will quit the loop
 */
/**
 * @brief Peek a message in message queue.
 * It will wait till a message available in ktsk::msg_qhdr.\n
 * If this function return kfalse, the task must quit immediately from message
 * pump loop, and cleanup all the pending message then quit the task.
 *
 * @param a_tsk Target task.
 * @param a_msg return peeked message.
 * @param *a_ur0 return peeked user data for this message.
 * @param *a_ur1 return peeked user data for this message.
 * @param *a_ur2 return peeked user data for this message.
 * @param *a_ur3 return peeked user data for this message.
 *
 * @retval ktrue A message is successfully peeked from message queue
 * @retval kfalse The tasking is quiting.
 *
 * @see ::kmsg_cleanup
 */
kbool kmsg_peek(kbean a_tsk, kuint *a_msg, kvoid **a_ur0, kvoid **a_ur1, kvoid **a_ur2, kvoid **a_ur3)
{
	ktsk *tsk = (ktsk *) a_tsk;
	kint ret;
	K_dlist_entry *ent;

	kassert(ktsk_valid(a_tsk));

	/**
	 * For send message, when the message is processed,
	 * task must tell the caller that the call can be returned.
	 */

again:

	if (kflg_chk(tsk->flg, KTF_QUITING))
		return kfalse;

	klog(("start wait message\n"));
	ret = ksyn_sem_get(tsk->msg_wait_sema, -1);
	klog(("end wait message\n"));

	/**
	 * if the KMSG_QUIT received, call the user's quit callback to
	 * do their cleanup operation
	 */

	kassert(!is_dlist_empty(&tsk->msg_qhdr));
	if (!is_dlist_empty(&tsk->msg_qhdr)) {

		ksyn_lck_get(tsk->msg_qlck);
		ent = remove_dlist_head_entry(&tsk->msg_qhdr);
		init_dlist_head(ent);
		tsk->curmsg = FIELD_TO_STRUCTURE(ent, kmsg, ent);

		klog(("peek message: msg:%x, ars(%x, %x, %x, %x)\n",
					tsk->curmsg->msg, tsk->curmsg->ur0, tsk->curmsg->ur1, tsk->curmsg->ur2, tsk->curmsg->ur3));

		*a_msg = tsk->curmsg->msg;
		*a_ur0 = tsk->curmsg->ur0;
		*a_ur1 = tsk->curmsg->ur1;
		*a_ur2 = tsk->curmsg->ur2;
		*a_ur3 = tsk->curmsg->ur3;

		ksyn_lck_rel(tsk->msg_qlck);

		return ktrue;
	}

	/**
	 * on normal occassion, when msg_wait_sema returns, there must exist a
	 * message in the queue
	 */
	kassert(!"should not goes to here\n");
	goto again;
}

/**
 * @brief Push or queue a message to ktsk::msg_qhdr.
 *
 * @param a_tsk Target task.
 * @param a_msg kmsg structure should be queued.
 *
 * @return ktrue if successfully queued, otherwise return kfalse.
 */
static kbool kmsg_push(kbean a_tsk, kmsg *a_msg)
{
	kbool ret = kfalse;
	ktsk *tsk = (ktsk *) a_tsk;

	if (kflg_chk(tsk->flg, KTF_QUITING))
		return kfalse;

	init_dlist_head(&a_msg->ent);
	ksyn_lck_get(tsk->msg_qlck);
	if (!kflg_chk(tsk->flg, KTF_QUITING)) {
		insert_dlist_tail_entry(&tsk->msg_qhdr, &a_msg->ent);

		ksyn_sem_rel(tsk->msg_wait_sema);
		ret = ktrue;
	}
	ksyn_lck_rel(tsk->msg_qlck);

	return ret;
}
