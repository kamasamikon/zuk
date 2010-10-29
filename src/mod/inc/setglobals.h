/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __SET_GLOBALS_H__
#define __SET_GLOBALS_H__

/* XXX this should only be included xxx_impl.c */

#include <kim.h>
#include <kmm.h>
#include <kmccontainer.h>

static KIM *__g_im = (KIM *) knil;
static KMM *__g_mm = (KMM *) knil;
static KMediaContainer *__g_mc = (KMediaContainer *) knil;

#define SET_GLOBALS(im) \
    do { \
        __g_im = (KIM*)(im); \
        __g_mm = (KMM*)kim_getptr((im), "p.sys.kmm", NULL); \
        __g_mc = (KMediaContainer*)kim_getptr((im), "p.sys.kmc", NULL); \
    } while (0)

#endif /* __SET_GLOBALS_H__ */
