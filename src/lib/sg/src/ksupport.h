/* vim:set et sw=4 sts=4: */
#ifndef __KSUPPORT_H__
#define __KSUPPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>


    /////////////////////////////////////////////////////////////////////////
    // Used for the function return an array
    //
#define RETLST_DECL_VAR(TT, STEP) \
    kuchar __rl_cache_grow_step = STEP; \
    kushort __rl_cache_all_depth = __rl_cache_grow_step; \
    kushort __rl_cache_cur_depth = 0; \
    TT *__rl_cache[STEP]; \
    TT **__p_rl_cache = __rl_cache

#define RETLST_ADD_REC(TT, REC) \
    if (__rl_cache_cur_depth >= __rl_cache_all_depth) { \
        kvoid *oldCache = __p_rl_cache; \
        __rl_cache_all_depth += __rl_cache_grow_step; \
        __p_rl_cache = (TT **)kmem_alloc(__rl_cache_all_depth * sizeof(TT *)); \
        memcpy(__p_rl_cache, oldCache, (__rl_cache_all_depth - __rl_cache_grow_step) * sizeof(TT *)); \
        if (oldCache != __rl_cache) { \
            kmem_free(oldCache); \
        } \
    } \
    __p_rl_cache[__rl_cache_cur_depth++] = REC

#define RETLST_END_PROC(TT) \
    if (__p_rl_cache == __rl_cache) { \
        kvoid *oldCache = __p_rl_cache; \
        __p_rl_cache = (TT **)kmem_alloc(__rl_cache_cur_depth * sizeof(TT *)); \
        memcpy(__p_rl_cache, oldCache, __rl_cache_cur_depth * sizeof(TT *)); \
    }

#define RETLST_GET_BUF \
    __p_rl_cache

#define RETLST_GET_CNT \
    __rl_cache_cur_depth

    /////////////////////////////////////////////////////////////////////////


    kchar *kspt_strdup(const kchar *strSource);

    kbool streq(const kchar *s1, const kchar *s2);

    kchar *kspt_get_next_file_name(void);

    kuint kspt_dateTime_to_ntp(kchar *dateTime);
    kuint kspt_gzip_orglen(kuchar *buf, kuint buflen);
    kbool kspt_str_to_bool(kchar *str);

#define STR_SET(_D_, _S_) \
    do { \
        kmem_free(_D_); \
        (_D_) = kspt_strdup(_S_); \
    } while (0)

#define STR_SET(_D_, _S_) \
    do { \
        kmem_free(_D_); \
        (_D_) = kspt_strdup(_S_); \
    } while (0)

#ifdef __cplusplus
}
#endif


#endif /* __KSUPPORT_H__ */
