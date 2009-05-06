/* vim:set et sw=4 sts=4 ff=unix cino=:0: */

#ifndef __SET_GLOBALS_H__
#define __SET_GLOBALS_H__

/* XXX this should only be included xxx_impl.c */

#include <kim.h>
#include <kmm.h>
#include <kmccontainer.h>

KIM *__g_im = (KIM*)knil;
KMM *__g_mm = (KMM*)knil;
KMediaContainer *__g_mc = (KMediaContainer*)knil;

#define SET_GLOBALS(im) \
    do { \
        __g_im = (KIM*)(im); \
        __g_mm = (KMM*)kim_getptr((im), "p.sys.kmm", NULL); \
        __g_mc = (KMediaContainer*)kim_getptr((im), "p.sys.kmc", NULL); \
    } while (0)

#endif /* __SET_GLOBALS_H__ */


