#ifndef __K_DBG_H__
#define __K_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ksal.h>

/*---------------------------------------------------------------------------------
 * debug message mask
 */
#define DBG_LOG         0x00000001          /**< show log */
#define DBG_ERR         0x00000002          /**< show error */
#define DBG_FAT         0x00000004          /**< show fatal error */
#define DBG_ASS         0x00000008          /**< show assert message */
#define DBG_BRK         0x00000010          /**< raise debug trap when assert failed */

static kuint __gc_kdbg_level = (kuint)-1;

kuint kdbg_init(kuint a_level);
kuint kdbg_getlevel(const kchar *a_file);

#define GET_DBG_LEVEL() do { \
    if (((kuint)-1) == __gc_kdbg_level) { \
        __gc_kdbg_level = kdbg_getlevel(__FILE__); \
    } \
} while (0)

#if defined(DBG_HIDE_LOG) || defined(DBG_HIDE_CUR_LOG)
#define klog(x) do {} while (0)
#else
#define klog(x) do { \
    GET_DBG_LEVEL(); \
    if (__gc_kdbg_level & DBG_LOG) { \
        kprintf("[klog]-"); \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_ERR) || defined(DBG_HIDE_CUR_ERR)
#define kerror(x) do {} while (0)
#else
#define kerror(x) do { \
    GET_DBG_LEVEL(); \
    if (__gc_kdbg_level & DBG_ERR) { \
        kprintf("[kerr]-"); \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_FAT) || defined(DBG_HIDE_CUR_FAT)
#define kfatal(x) do {} while (0)
#else
#define kfatal(x) do { \
    GET_DBG_LEVEL(); \
    if (__gc_kdbg_level & DBG_FAT) { \
        kprintf("[kfat]-"); \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_ASSERT) || defined(DBG_HIDE_ASSERT)
#define kassert(x) do {} while (0)
#else
#define kassert(_x_) \
    do { \
        GET_DBG_LEVEL(); \
        if ((__gc_kdbg_level & DBG_ASS) && (!(_x_))) { \
            kprintf("\n\n\tkassert failed!!!\n\t[%s], \n\tFILE:%s, LINES:%d\n\n", \
                #_x_, __FILE__, __LINE__); \
            \
            if (__gc_kdbg_level & DBG_BRK) { \
                kdbgbrk(); \
            } \
        } \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __K_DBG_H__ */

