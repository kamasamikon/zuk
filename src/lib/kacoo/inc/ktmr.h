/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <sdlist.h>

#define TMF_LOOP    0x00000001

kint ktmr_init(kvoid);
kint ktmr_final(kvoid);
kbean ktmr_set(kbean a_tsk, kuint a_elapse, kbool a_loop,
        kint (*a_cbk)(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3),
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);
kvoid ktmr_killall(kbean a_tsk);
kbool ktmr_kill(kbean a_tmr);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */

