#include <kcfg.h>                           /**< KCFG_ENCCONV_STATIC_BUFFER */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__WIN32__) || defined(__WINCE__)
#include <win32_iconv.h>
#else
#include <iconv.h>
#endif
#include <errno.h>

#include <kmem.h>
#include <kdbg.h>
#include <kstr.h>

#define ICONV_BUFF_MAX 4096

kchar* enc_conv(const kchar *in, const kchar *enc_from, const kchar *enc_to)
{
    kchar *buff, *sin, *sout;
    kint lenin, lenout;
    iconv_t ct;

#ifdef KCFG_ENCCONV_STATIC_BUFFER
    static kchar __s_iconv_buffer[ICONV_BUFF_MAX];
#endif

    if ((ct = iconv_open(enc_to, enc_from)) == (iconv_t)-1) {
        klog(("%s|%d|iconv_open error!%d\n", __FILE__, __LINE__, errno));
        return knil;
    }

    /* iconvctl(cv, ICONV_SET_DISCARD_ILSEQ, &one); */

    iconv(ct, knil, knil, knil, knil);

    sin = (kchar*)in;
    lenin = strlen(in) + 1;

#ifdef KCFG_ENCCONV_STATIC_BUFFER
    buff = __s_iconv_buffer;
#else
    if ((buff = (kchar*)kmem_alloc(ICONV_BUFF_MAX)) == knil) {
        klog(("%s|%d|malloc error!%d\n", __FILE__, __LINE__, errno));
        iconv_close(ct);
        return knil;
    }
#endif

    sout = buff;
    lenout = ICONV_BUFF_MAX;

    if (iconv(ct, &sin, (size_t*)&lenin, &sout, (size_t*)&lenout) == (size_t)-1) {
        klog(("%s|%d|iconv()error = %d\n", __FILE__, __LINE__, errno));
        kmem_free(buff);
        iconv_close(ct);
        return knil;
    }

    iconv_close(ct);

    sout = kstr_dup(buff);
#ifdef KCFG_ENCCONV_STATIC_BUFFER
#else
    kmem_free(buff);
#endif

    return sout;
}

