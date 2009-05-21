/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __K_DBG_H__
#define __K_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ksal.h>

/*---------------------------------------------------------------------------------
 * debug message mask
 */
#define DBG_LOG         0x00000001
#define DBG_ERR         0x00000002
#define DBG_FAT         0x00000004
#define DBG_TIME        0x00000080

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
        if (__gc_kdbg_level & DBG_TIME) { \
            kprintf("[klog:%u]-", ksal_get_tick()); \
        } else { \
            kprintf("[klog]-"); \
        } \
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
        if (__gc_kdbg_level & DBG_TIME) { \
            kprintf("[kerr:%u]-", ksal_get_tick()); \
        } else { \
            kprintf("[kerr]-"); \
        } \
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
        if (__gc_kdbg_level & DBG_TIME) { \
            kprintf("[kfat:%u]-", ksal_get_tick()); \
        } else { \
            kprintf("[kfat]-"); \
        } \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_ASSERT) || defined(DBG_HIDE_ASSERT)
#define kassert(x) do {} while (0)
#elif defined(DBG_ASSERT_AS_ERR)
#define kassert(_x_) \
    do { \
        if (!(_x_)) { \
            kprintf("\n\n\tkassert failed!!!\n\t[%s], \n\tFILE:%s, LINES:%d\n\n", \
                #_x_, __FILE__, __LINE__); \
        } \
    } while (0)
#else
#define kassert(_x_) \
    do { \
        if (!(_x_)) { \
            kprintf("\n\n\tkassert failed!!!\n\t[%s], \n\tFILE:%s, LINES:%d\n\n", \
                #_x_, __FILE__, __LINE__); \
            kdbgbrk(); \
        } \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __K_DBG_H__ */

