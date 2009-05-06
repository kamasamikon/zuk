/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#ifndef __K_SYN_H__
#define __K_SYN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

kbean ksyn_lck_new(kvoid);
kint ksyn_lck_del(kbean a_lck);
kvoid ksyn_lck_get(kbean a_lck);
kvoid ksyn_lck_rel(kbean a_lck);

kbean ksyn_sem_new(kint a_initval);
kint ksyn_sem_del(kbean a_sema);
kint ksyn_sem_get(kbean a_sema, kint a_timeout);
kint ksyn_sem_rel(kbean a_sema);

#ifdef __cplusplus
}
#endif

#endif /* __K_SYN_H__ */

