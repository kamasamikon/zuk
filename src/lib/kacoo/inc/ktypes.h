/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
/*---------------------------------------------------------------------------------

FileName:

    ivtype.h

Abstract:

    Override some data type to provide independance between some platform.

    The assumption for the data length is:
        sizeof(klong) >= 4 byte
        sizeof(kint) >= 4 byte
        sizeof(kshort) >= 2 byte
        sizeof(kchar) >= 1 byte

---------------------------------------------------------------------------------*/

#ifndef __IVTYPE_H__
#define __IVTYPE_H__

typedef long klong;
typedef unsigned long kulong;

typedef int kint;
typedef unsigned int kuint;

typedef short kshort;
typedef unsigned short kushort;

typedef char kchar;
typedef unsigned char kuchar;

typedef short kwchar;
typedef unsigned short kuwchar;

typedef kint kbool;

typedef void *kbean;

#define kvoid void

#define kfalse ((kbool)0)
#define ktrue (!kfalse)

#ifndef knull
#ifdef  __cplusplus
#define knull 0
#else
#define knull ((void *)0)
#endif
#endif

#define knil knull

#endif /* __IVTYPE_H__ */

