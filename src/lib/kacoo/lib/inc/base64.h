#ifndef __BASE64_H__
#define __BASE64_H__

#include <ktypes.h>

kint base64_encode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen);
kint base64_decode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen);

#endif /* __BASE64_H__ */
