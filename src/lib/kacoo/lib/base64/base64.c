#include <base64.h>

#define END '='
#define OKCHAR(c) ( \
		('A' <= (c) && (c) <= 'Z') || \
		('a' <= (c) && (c) <= 'z') || \
		('0' <= (c) && (c) <= '9') || \
		((c) == '+') || \
		((c) == '/') \
		)

static kchar base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

kint base64_encode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen)
{
	kchar *s, *p;
	kint i, c;
	const kuchar *q;

	p = s = (kchar*)kmem_alloc(a_inlen * 4 / 3 + 4);
	if (p == knil) {
		return -1;
	}

	q = (const kuchar*)a_in;

	for (i = 0; i < a_inlen;) {
		c = q[i++];
		c *= 256;
		if (i < a_inlen) {
			c += q[i];
		}
		i++;

		c *= 256;
		if (i < a_inlen) {
			c += q[i];
		}
		i++;

		p[0] = base64[(c & 0x00fc0000) >> 18];
		p[1] = base64[(c & 0x0003f000) >> 12];
		p[2] = base64[(c & 0x00000fc0) >> 6];
		p[3] = base64[(c & 0x0000003f) >> 0];
		if (i > a_inlen) {
			p[3] = END;
		}
		if (i > a_inlen + 1) {
			p[2] = END;
		}
		p += 4;
	}
	*p = 0;
	*a_out = s;
	return strlen(s);
}

kint base64_decode(const kchar *a_in, kint a_inlen, kchar **a_out, kint *a_outlen)
{
}

