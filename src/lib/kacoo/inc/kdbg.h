#ifndef __K_DBG_H__
#define __K_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#include <ksal.h>

/*---------------------------------------------------------------------------------
 * debug message mask
 */
#define DBG_LOG         0x00000001          /**< show log */
#define DBG_ERR         0x00000002          /**< show error */
#define DBG_FAT         0x00000004          /**< show fatal error */
#define DBG_ASS         0x00000008          /**< show assert message */
#define DBG_BRK         0x00000010          /**< raise debug trap when assert failed */
#define DBG_TIM         0x00000020          /**< time stamp for log etc */

static kuint __gc_kdbg_level = (kuint)-1;
static kchar __gc_strtime[100] = "";

kuint kdbg_init();
kuint kdbg_getlevel(const kchar *a_file);

#define GET_DBG_LEVEL() do { \
    if (((kuint)-1) == __gc_kdbg_level) { \
        __gc_kdbg_level = kdbg_getlevel(__FILE__); \
    } \
} while (0)

/* GET_DBG_TIME should be used after GET_DBG_LEVEL */
#define GET_DBG_TIME() do { \
    if (__gc_kdbg_level & DBG_TIM) { \
        sprintf(__gc_strtime, ":%d", ksal_cur_time()); \
    } else { \
        __gc_strtime[0] = '\0'; \
    } \
} while (0)


#if defined(DBG_HIDE_LOG) || defined(DBG_HIDE_CUR_LOG)
#define klog(x) do {} while (0)
#else
#define klog(x) do { \
    GET_DBG_LEVEL(); \
    GET_DBG_TIME(); \
    if (__gc_kdbg_level & DBG_LOG) { \
        kprintf("[klog%s]-", __gc_strtime); \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_ERR) || defined(DBG_HIDE_CUR_ERR)
#define kerror(x) do {} while (0)
#else
#define kerror(x) do { \
    GET_DBG_LEVEL(); \
    GET_DBG_TIME(); \
    if (__gc_kdbg_level & DBG_ERR) { \
        kprintf("[kerr%s]-", __gc_strtime); \
        kprintf x ; \
    } \
} while (0)
#endif

#if defined(DBG_HIDE_FAT) || defined(DBG_HIDE_CUR_FAT)
#define kfatal(x) do {} while (0)
#else
#define kfatal(x) do { \
    GET_DBG_LEVEL(); \
    GET_DBG_TIME(); \
    if (__gc_kdbg_level & DBG_FAT) { \
        kprintf("[kfat%s]-", __gc_strtime); \
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
        GET_DBG_TIME(); \
        if ((__gc_kdbg_level & DBG_ASS) && (!(_x_))) { \
            kprintf("\n\n\tkassert failed!%s!!\n\t[%s], \n\tFILE:%s, LINES:%d\n\n", \
                __gc_strtime, #_x_, __FILE__, __LINE__); \
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

