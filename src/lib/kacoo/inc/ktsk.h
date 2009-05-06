/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#ifndef __K_TSK_H__
#define __K_TSK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <sdlist.h>

/**--------------------------------------------------------------------------------
 * Reserved task bean
 */
/* wild char for ALL task, used when broadcast message */
#define KTSK_ALL                (kbean)0xFFFFFFFF

/*
 * message larger than 0xF0000000 is reserved for internal use
 * 0xFFFFFFFF - 0xF0000000  : internal message
 * 0x0FFFFFFF - 0x00000000  : user message
 * other                    : reserved
 */
#define KMSG_MSC_INTERNAL       0xF0000000
#define KMSG_MSC_EXTERNAL

/**--------------------------------------------------------------------------------
 * K message definition
 */
/* wild char, means ANY message */
#define KMSG_ANY                0xFFFFFFFF

/* mean NO message */
#define KMSG_NULL               0xF0000000

/* Last message a task process, task quit after it processed */
#define KMSG_QUIT               0xF0000001

/* Used by ktmr module */
#define KMSG_TIMER              0xF0000002


/**--------------------------------------------------------------------------------
 * Message Flag
 */
#define KMF_SNDMSG              0x00000001
#define KMF_DYNMEM              0x00000002
#define KMF_DONE                0x00000004

/**--------------------------------------------------------------------------------
 * Dispatch callback reason
 */
#define KMDR_NORMAL             0x00000000
#define KMDR_ERROR              0x00000001
#define KMDR_QUIT               0x00000002
#define KMDR_CANCEL             0x00000003
#define KMDR_ABORT              0x00000004

typedef int (*KTSK_PROC)(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);
typedef int (*KMSG_DISP)(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

/* K Task On Message */
#define KTOM(f) (f)(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)

/**--------------------------------------------------------------------------------
 * register the message dispatch routine
 */
kint kmsg_slot_set(kbean a_tsk, kuint a_msg, KMSG_DISP a_disp);
kint kmsg_slot_get(kbean a_tsk, kuint a_msg, KMSG_DISP *a_disp);
kint kmsg_slot_del(kbean a_tsk, kuint a_msg);

kbean ktsk_cur(kvoid);
kbool ktsk_running(kbean a_tsk);

kbean ktsk_new(const kchar *a_name, KTSK_PROC a_mainproc, kint a_prio, kint a_stack_size,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);
kint ktsk_del(kbean a_tsk);

/* return user data set in ktsk_new */
kint ktsk_get_ars(kbean a_tsk, kvoid **a_ar0, kvoid **a_ar1, kvoid **a_ar2, kvoid **a_ar3);

/* kmsg_send: return till the message processed */
kint kmsg_send(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);

/* kmsg_post: queue the message, and return immediately */
kint kmsg_post(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);

kint kmsg_disp(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

kbool kmsg_peek(kbean a_tsk, kuint *a_msg, kvoid **a_ar0, kvoid **a_ar1, kvoid **a_ar2, kvoid **a_ar3);

kint kmsg_kill(kbean a_tsk, kbool a_type, kuint a_muid);

/*
 * pause execution for CURRENT task for ms milliseconds
 */
kvoid ktsk_sleep(kuint a_ms);

kint ktsk_msg_reg_awch(kuint a_msg, ...);
kint ktsk_msg_reg_bwch(kuint a_msg, ...);

kint ktsk_msg_reg_ahook(kuint a_msg, ...);
kint ktsk_msg_reg_bhook(kuint a_msg, ...);

#ifdef __cplusplus
}
#endif

#endif /* __K_TSK_H__ */

