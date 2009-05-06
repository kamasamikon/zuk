/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#ifndef __ZUK_MAIN_H__
#define __ZUK_MAIN_H__

#include "sysdeps.h"

kint zuk_init(KIM *im, kint argc, kchar **argv);
kint zuk_show(KIM *im, kint argc, kchar **argv);
kint zuk_final(KIM *im, kint argc, kchar **argv);

#endif /* __ZUK_MAIN_H__ **/

