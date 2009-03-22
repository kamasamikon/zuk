/* vim:set et sw=4 sts=4: */

#ifndef __S2N_MACRO_H__
#define __S2N_MACRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define S2N_CURLY_BRACE_L       {
#define S2N_CURLY_BRACE_R       }

#define S2N_FUN_S               {
#define S2N_FUN_E               }

#define S2N_FUN(T, P)             s2n_free_##T((T##_t*)P)

#define S2N_FUN_DECL(T)         kint s2n_free_##T(T##_t *T)
    //#define S2N_CURLY_BRACE_L     {
#define S2N_DECL_VAR(T)             kuint i, ncnt; \
    T##_rec *rec; \

#define S2N_SET_NCNT(T)             ncnt = (T)->ecnt;
#define S2N_FOREACH_NCNT(T)         for (i = 0; i < ncnt; i++)
    //#define S2N_CURLY_BRACE_L         {
#define S2N_SET_REC(T)                  rec = &(T)->arr[i];

    // delete attribute XXX
#define S2N_DEL_ATT(T, N)               kmem_free(rec->attr./**/N);

#define S2N_DEL_VAL(T, N)               kmem_free(rec->val./**/N);

    // delete sub elements XYZ
#define S2N_DEL_SUB(T, N)               if (rec->/**/N) s2n_free_##N(rec->/**/N);
    //#define S2N_CURLY_BRACE_R         }

#define S2N_DEL_ARR(T)              if (ncnt) { \
    kmem_free((T)->arr); \
}

// delete this
#define S2N_DEL_SELF(T)             kmem_free(T);
#define S2N_FUN_RET                 return 0;
//#define S2N_CURLY_BRACE_R     }
#define S2N_END_FUNCTION(T)

#endif /* __S2N_MACRO_H__ */

