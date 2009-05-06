/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#ifndef __K_INI_H__
#define __K_INI_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

kbool kini_getstr(const kchar *a_sec, const kchar *a_key, kchar *a_val, kint a_size, const kchar *a_path);
kbool kini_getint(const kchar *a_sec, const kchar *a_key, kint *a_ret, const kchar *a_path);

kbool kini_setstr(const kchar *a_sec, const kchar *a_key, const kchar *a_val, const kchar *a_path);
kbool kini_setint(const kchar *a_sec, const kchar *a_key, kint a_val, const kchar *a_path);

#ifdef __cplusplus
}
#endif


#endif /* __K_INI_H__ */

