/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

/*---------------------------------------------------------------------------------

FileName:

ktypes.h

Abstract:

Override some data type to provide independance between some platform.

The assumption for the data length is:
sizeof(kllint) >= 8 byte
sizeof(klong) >= 4 byte
sizeof(kint) >= 4 byte
sizeof(kshort) >= 2 byte
sizeof(kchar) >= 1 byte

---------------------------------------------------------------------------------*/

#ifndef __K_TYPES_H__
#define __K_TYPES_H__

typedef long long int kllint;
typedef unsigned long long int kullint;

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

#endif /* __K_TYPES_H__ */
