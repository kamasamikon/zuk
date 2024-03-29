/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __ZUK_MAIN_H__
#define __ZUK_MAIN_H__

#include "sysdeps.h"

#ifdef __cplusplus
extern "C" {
#endif

kint zuk_init(KIM *im, kint argc, kchar **argv);
kint zuk_show(KIM *im, kint argc, kchar **argv);
kint zuk_final(KIM *im, kint argc, kchar **argv);

#ifdef __cplusplus
extern "C" {
#endif

#endif /* __ZUK_MAIN_H__ */
