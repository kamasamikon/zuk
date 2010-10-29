/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

kint base64_encode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen);
kint base64_decode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen);

#ifdef __cplusplus
}
#endif
#endif /* __BASE64_H__ */
