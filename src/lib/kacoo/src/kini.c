/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <kmem.h>
#include <kini.h>
#include <kdbg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEFT_BRACE      '['
#define RIGHT_BRACE     ']'

static kbool load_ini_file(const kchar *a_path, kchar **a_buf, kint *a_buflen)
{
    kbean in = knil;
    kint flen, retlen;
    kchar *buf;

    if (in = kvfs_open(a_path, "rb", 0)) {
        flen = kvfs_length(in);
        retlen = flen + 100;
        buf = kmem_alloz(retlen);
        kvfs_read(in, &buf, &retlen);
        kvfs_close(in);

        *a_buf = buf;
        *a_buflen = retlen;
        return ktrue;
    }
    return kfalse;
}

#define isend(_l_, _i_) (_l_ < _i_)
#define isnewline(_c_) ('\n' == _c_ || '\r' == _c_)
#define isleftbarce(_c_) (LEFT_BRACE == _c_)
#define isrightbrace(_c_) (RIGHT_BRACE == _c_)

static kbool parse_file(const kchar *a_sec, const kchar *a_key, const kchar *a_buf, kint a_buflen, kint *a_sec_s, kint *a_sec_e,
        kint *a_key_s, kint *a_key_e, kint *a_val_s, kint *a_val_e)
{
    const kchar *p = a_buf;
    kint i = 0, seclen = strlen(a_sec);

    kassert(a_buf != knil);
    kassert(a_sec != knil && strlen(a_sec));
    kassert(a_key != knil && strlen(a_key));

    *a_sec_e = *a_sec_s = *a_key_e = *a_key_s = *a_val_s = *a_val_e = -1;

    while (!isend(a_buflen, i)) {
        /* find the section */
        if ((0 == i || isnewline(p[i - 1])) && isleftbarce(p[i])) {
            kint section_start = i + 1;

            /* find the ']' */
            do {
                i++;
            } while (!isrightbrace(p[i]) && !isend(a_buflen, i));

            if (!isend(a_buflen, i) && (seclen == (i - section_start)) && (0 == strncmp(p + section_start, a_sec, i - section_start))) {
                kint newline_start = 0;

                i++;

                /* Skip over space char after ']' */
                while (isspace(p[i]) && !isend(a_buflen, i)) {
                    i++;
                }

                /* find the section */
                *a_sec_s = section_start;
                *a_sec_e = i;

                while (!(isnewline(p[i - 1]) && isleftbarce(p[i])) && !isend(a_buflen, i)) {
                    kint j = 0;
                    /* get a new line */
                    newline_start = i;

                    while (!isnewline(p[i]) && !isend(a_buflen, i)) {
                        i++;
                    }
                    /* now i is equal to end of the line */

                    j = newline_start;

                    if (';' != p[j]) {
                        /* skip over comment */
                        while (j < i && p[j] != '=') {
                            j++;
                            if ('=' == p[j]) {
                                if (strncmp(a_key, p + newline_start, j - newline_start) == 0) {
                                    /* find the key ok */
                                    *a_key_s = newline_start;
                                    *a_key_e = j - 1;

                                    *a_val_s = j + 1;
                                    *a_val_e = i;

                                    return ktrue;
                                }
                            }
                        }
                    }

                    i++;
                }
            }
        } else {
            i++;
        }
    }
    return kfalse;
}

kbool kini_getstr(const kchar *a_sec, const kchar *a_key, kchar *a_val, kint a_size, const kchar *a_path)
{
    kchar *buf = knil;
    kint ret = kfalse, buflen = 0x7FFFFFFF;

    kint sec_s, sec_e, key_s, key_e, value_s, value_e;

    /* check parameters */
    kassert(a_sec != knil && strlen(a_sec));
    kassert(a_key != knil && strlen(a_key));
    kassert(a_val != knil);
    kassert(a_size > 0);
    kassert(a_path != knil && strlen(a_key));

    if (!load_ini_file(a_path, &buf, &buflen)) {
        kmem_free(buf);
        return kfalse;
    }

    if (parse_file(a_sec, a_key, buf, buflen, &sec_s, &sec_e, &key_s, &key_e, &value_s, &value_e)) {
        kint cpcount = value_e - value_s;

        if (a_size - 1 < cpcount) {
            cpcount = a_size - 1;
        }

        memset(a_val, 0, a_size);
        memcpy(a_val, buf + value_s, cpcount);
        a_val[cpcount] = '\0';

        ret = ktrue;
    }

    kmem_free(buf);
    return ret;
}

kbool kini_getint(const kchar *a_sec, const kchar *a_key, kint *a_ret, const kchar *a_path)
{
    kchar val[32] = { 0 };
    if (!kini_getstr(a_sec, a_key, val, sizeof(val), a_path)) {
        return kfalse;
    } else {
        *a_ret = atoi(val);
        return ktrue;
    }
}

kbool kini_setstr(const kchar *a_sec, const kchar *a_key, const kchar *a_val, const kchar *a_path)
{
    kchar *buf = knil;
    kchar *w_buf;
    kint ret = kfalse, sec_s, sec_e, key_s, key_e, value_s, value_e;
    kint value_len = (kint)strlen(a_val), wbuflen;
    kint buflen = 0x7FFFFFFF;
    kbean out;

    /* check parameters */
    kassert(a_sec != knil && strlen(a_sec));
    kassert(a_key != knil && strlen(a_key));
    kassert(a_val != knil);
    kassert(a_path != knil && strlen(a_key));

    if (!load_ini_file(a_path, &buf, &buflen)) {
        buflen = 0;
        sec_s = -1;
    } else {
        parse_file(a_sec, a_key, buf, buflen, &sec_s, &sec_e, &key_s, &key_e, &value_s, &value_e);
    }

    /* [sec]\r\na_key=a_val\r\n, should left room to maintain all */
    wbuflen = buflen + strlen(a_sec) + strlen(a_key) + strlen(a_val) + 20;
    w_buf = (kchar*)kmem_alloc(wbuflen);
    memset(w_buf, 0, wbuflen);
    if (-1 == sec_s) {

        if (0 == buflen) {
            sprintf(w_buf + buflen, "[%s]\n%s=%s\n", a_sec, a_key, a_val);
        } else {
            /* not find the section, then add the new section at end of the file */
            memcpy(w_buf, buf, buflen);
            sprintf(w_buf + buflen, "\n[%s]\n%s=%s\n", a_sec, a_key, a_val);
        }
    } else if (-1 == key_s) {
        /* not find the key, then add the new key & value at end of the section */
        memcpy(w_buf, buf, sec_e);
        sprintf(w_buf + sec_e, "%s=%s\n", a_key, a_val);
        memcpy(w_buf + sec_e + strlen(a_key) + strlen(a_val) + 2, buf + sec_e, buflen - sec_e);
    } else {
        /* update value with new value */
        memcpy(w_buf, buf, value_s);
        memcpy(w_buf + value_s, a_val, value_len);
        memcpy(w_buf + value_s + value_len, buf + value_e, buflen - value_e);
    }

    if (out = kvfs_open(a_path, "wb", 0)) {
        kvfs_write(out, w_buf, strlen(w_buf));
        kvfs_close(out);
        ret = ktrue;
    }

    kmem_free(buf);
    kmem_free(w_buf);
    return ret;
}

kbool kini_setint(const kchar *a_sec, const kchar *a_key, kint a_val, const kchar *a_path)
{
    kchar str_value[128];
    sprintf(str_value, "%d", a_val);
    return kini_setstr(a_sec, a_key, str_value, a_path);
}

#ifdef __cplusplus
}
#endif


