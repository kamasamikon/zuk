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
        kint (*a_cbk)(kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3),
        kvoid *a_ur0, kvoid *a_ur1, kvoid *a_ur2, kvoid *a_ur3);
kvoid ktmr_killall(kbean a_tsk);
kbool ktmr_kill(kbean a_tmr);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */

