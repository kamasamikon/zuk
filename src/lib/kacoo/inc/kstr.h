/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __K_STR_H__
#define __K_STR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

typedef struct _K_str {
	kuint size;			    /**< size of buffer */
	kuint len;			    /**< length of string */
	kchar *ptr;			    /**< address of string */
} K_str;

kint kstr_cmp(const kchar *a_str0, const kchar *a_str1, kint a_len);
kint kstr_icmp(const kchar *a_str0, const kchar *a_str1, kint a_len);
kchar *kstr_dup(const kchar *a_str);
kchar *kstr_subs(kchar *a_str, kchar a_from, kchar a_to);
kint kstr_escxml(const kchar *a_is, kchar **a_os, kint *a_ol);

#if 0
kint kstr_new
	/**< new a kstr */
	kint kstr_del
	/**< del a kstr */
	kint kstr_set
	/**< set value */
	kint kstr_ptr
	/**< get real string */
	kint kstr_cmp
	/**< strncmp */
	kchar *kstr_str
	/**< convert to kchar */
#endif
#ifdef __cplusplus
}
#endif
#endif /* __K_STR_H__ */
