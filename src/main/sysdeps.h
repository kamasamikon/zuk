#ifndef __K_SYSDEPS_H__
#define __K_SYSDEPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <kim.h>

kint zuk_sysdeps_start(KIM *im, kint argc, kchar **argv);
kint zuk_sysdeps_term();

#ifdef __cplusplus
}
#endif

#endif /* __K_SYSDEPS_H__ */

