
#ifndef __X2S_MACRO_H__
#define __X2S_MACRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ksupport.h"

#define GETVAL_kpchar(x)        x; x = knil
#define GETVAL_kuchar(x)        (kuchar)atoi(x)
#define GETVAL_kulong(x)        (kulong)strtoul(x, 0, 10)
#define GETVAL_kint(x)          (kint)strtol(x, 0, 10)
#define GETVAL_kchar(x)         (kchar)atoi(x)
#define GETVAL_kuint(x)         (kuint)strtoul(x, 0, 10)
#define GETVAL_kushort(x)       (kushort)atoi(x)
#define GETVAL_kbool(x)         kspt_str_to_bool(x)


#define X2S_TYP(T)              T##_t
#define X2S_REC(T)              T##_rec

#define X2S_CURLY_BRACE_L       {
#define X2S_CURLY_BRACE_R       }

#define X2S_FUN_S               {
#define X2S_FUN_E               }

#define X2S_FUN(T)         		x2s_fill_##T(KXmlDoc *doc)

#define X2S_FUN_DECL(T)         kvoid *x2s_fill_##T(KXmlDoc *doc)
//#define X2S_CURLY_BRACE_L     {
#define X2S_DECL_VAR(T)             kuint i, ncnt; \
                                    KXmlNode *node; \
                                    KXmlAttr *attr; \
                                    T##_t *curNode = (T##_t*)knil; \
                                    T##_rec *arr = (T##_rec*)knil; \
                                    (kvoid)i; (kvoid)ncnt; (kvoid)node; (kvoid)attr; \

#define X2S_GETNODECNT(T)           if (0 < (ncnt = xmldoc_getNodeCnt(doc, #T)))
//#define X2S_CURLY_BRACE_L         {
#define X2S_ALLOC_T(T)                  curNode = kmem_alloz(sizeof(T##_t));
#define X2S_SET_CNT(T)                  curNode->ecnt = ncnt;
#define X2S_ALLOC_REC(T)                curNode->arr = arr = kmem_alloz(ncnt * sizeof(T##_rec));
#define X2S_FOREACH_NCNT(T)             for (i = 0; i < ncnt; i++)
//#define X2S_CURLY_BRACE_L             {
#define X2S_GOTO_iST_NODE(T)                node = xmldoc_gotoNode(doc, #T, i);
#define X2S_BRK_IF_BAD_NODE(T)              if (!node) break;

#define X2S_FILL_ATT(T, N)                  if (attr = xmlnode_getattr(node, #N)) { \
                                                arr[i].attr./**/N = GETVAL_##T(attr->value); \
                                            }

#define X2S_FILL_ATT_DEF(T, N, D)           if (attr = xmlnode_getattr(node, #N)) { \
                                                arr[i].attr./**/N = GETVAL_##T(attr->value); \
                                            } else { \
                                                arr[i].attr./**/N = (D); \
											}


#define X2S_FILL_VAL(T, N)                  if (node->text) { \
                                                arr[i].val./**/N = GETVAL_##T(node->text); \
                                            }

#define X2S_FILL_SUB(T, N)                  arr[i]./**/N = x2s_fill_##N(doc);


#define X2S_GOTO_PRT_NODE(T)                xmldoc_gotoNode(doc, "..", 0);
//#define X2S_CURLY_BRACE_R             }
//#define X2S_CURLY_BRACE_R         }
#define X2S_RETURN_CURNODE          return curNode;
#define X2S_CURLY_BRACE_R       }
#define X2S_END_FUNCTION(T)

/////////////////////////////////////////////////////////////////////////
#define X2S_FILL_ATT_(T, N, N_)             if (attr = xmlnode_getattr(node, N_)) { \
                                                arr[i].attr./**/N = GETVAL_##T(attr->value); \
                                            }

#endif /* __X2S_MACRO_H__ */

