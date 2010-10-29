/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#include <stdio.h>

#include <ktypes.h>
#include <kdbg.h>
#include <kmem.h>

#include "textconv.h"

#if defined(__WIN32__)
#include <windows.h>
#else
#include <langinfo.h>
#endif

static const char * locale_charset()
{
    const char *codeset;

#if defined(__UNIX__)
    codeset = nl_langinfo(CODESET);
#else /* WIN32 */
    static char buf[2 + 10 + 1];

    /* Win32 has a function returning the locale's codepage as a number.  */
    sprintf(buf, "CP%u", GetACP());
    codeset = buf;
#endif

    if (codeset == NULL) {
        codeset = "";
    }
    return codeset;
}

char *utf8_to_acp(char *utf8)
{
#if defined(__WIN32__)
    int buflen;
    WCHAR *wChars;
    char *mChars;

    if (!utf8) {
        return knil;
    }

    buflen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wChars = kmem_alloc(buflen*sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wChars, buflen);

    buflen = WideCharToMultiByte(CP_ACP, 0, wChars, -1, NULL, 0, NULL, NULL);
    mChars = kmem_alloc(buflen*sizeof(char));
    WideCharToMultiByte(CP_ACP, 0, wChars, -1, mChars, buflen, NULL, NULL);

    kmem_free(wChars);
    return mChars;
#elif defined(__UNIX__)
    kchar* enc_conv(const kchar *in, const kchar *enc_from, const kchar *enc_to);

    static const kchar *__s_charset = knil;

    if (!utf8) {
        return knil;
    }

    if (!__s_charset) {
        __s_charset = locale_charset();
    }
    return enc_conv(utf8, "UTF-8", __s_charset);
#endif
}
char *acp_to_utf8(char *acp)
{
#if defined(__WIN32__)
    int buflen;
    WCHAR *wChars;
    char *mChars;

    if (!acp) {
        return knil;
    }

    buflen = MultiByteToWideChar(CP_ACP, 0, acp, -1, NULL, 0);
    wChars = kmem_alloc(buflen*sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, acp, -1, wChars, buflen);

    buflen = WideCharToMultiByte(CP_UTF8, 0, wChars, -1, NULL, 0, NULL, NULL);
    mChars = kmem_alloc(buflen*sizeof(char));
    WideCharToMultiByte(CP_UTF8, 0, wChars, -1, mChars, buflen, NULL, NULL);

    kmem_free(wChars);
    return mChars;
#elif defined(__UNIX__)
    kchar* enc_conv(const kchar *in, const kchar *enc_from, const kchar *enc_to);

    static const kchar *__s_charset = knil;

    if (!acp) {
        return knil;
    }

    if (!__s_charset) {
        __s_charset = locale_charset();
    }
    return enc_conv(acp, __s_charset, "UTF-8");
#endif
}

