/* example.c -- usage example of the zlib compression library
 * Copyright (C) 1995-2004 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include <stdio.h>
#include "zlib.h"

#ifdef STDC
#  include <string.h>
#  include <stdlib.h>
#endif

#if defined(VMS) || defined(RISCOS)
#  define TESTFILE "foo-gz"
#else
#  define TESTFILE "foo.gz"
#endif

#define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
        fprintf(stderr, "%s error: %d\n", msg, err); \
        exit(1); \
    } \
}

const char hello[] = "hello, hello!";
/* "hello world" would be more standard, but the repeated "hello"
 * stresses the compression code better, sorry...
 */

const char dictionary[] = "hello";
uLong dictId; /* Adler32 value of the dictionary */

void test_compress      OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
void test_gzio          OF((const char *fname,
                            Byte *uncompr, uLong uncomprLen));
void test_deflate       OF((Byte *compr, uLong comprLen));
void test_inflate       OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
void test_large_deflate OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
void test_large_inflate OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
void test_flush         OF((Byte *compr, uLong *comprLen));
void test_sync          OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
void test_dict_deflate  OF((Byte *compr, uLong comprLen));
void test_dict_inflate  OF((Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen));
int  main               OF((int argc, char *argv[]));

/* ===========================================================================
 * Test zei_compress() and zei_uncompress()
 */
void test_compress(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    int err;
    uLong len = (uLong)strlen(hello)+1;

    err = zei_compress(compr, &comprLen, (const Bytef*)hello, len);
    CHECK_ERR(err, "zei_compress");

    strcpy((char*)uncompr, "garbage");

    err = zei_uncompress(uncompr, &uncomprLen, compr, comprLen, NULL);
    CHECK_ERR(err, "zei_uncompress");

    if (strcmp((char*)uncompr, hello)) {
        fprintf(stderr, "bad zei_uncompress\n");
        exit(1);
    } else {
        printf("zei_uncompress(): %s\n", (char *)uncompr);
    }
}

/* ===========================================================================
 * Test read/write of .gz files
 */
void test_gzio(fname, uncompr, uncomprLen)
    const char *fname; /* compressed file name */
    Byte *uncompr;
    uLong uncomprLen;
{
#ifdef NO_GZCOMPRESS
    fprintf(stderr, "NO_GZCOMPRESS -- gz* functions cannot zei_compress\n");
#else
    int err;
    int len = (int)strlen(hello)+1;
    gzFile file;
    z_off_t pos;

    file = zei_gzopen(fname, "wb");
    if (file == NULL) {
        fprintf(stderr, "zei_gzopen error\n");
        exit(1);
    }
    zei_gzputc(file, 'h');
    if (zei_gzputs(file, "ello") != 4) {
        fprintf(stderr, "zei_gzputs err: %s\n", zei_gzerror(file, &err));
        exit(1);
    }
    if (zei_gzprintf(file, ", %s!", "hello") != 8) {
        fprintf(stderr, "zei_gzprintf err: %s\n", zei_gzerror(file, &err));
        exit(1);
    }
    zei_gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
    zei_gzclose(file);

    file = zei_gzopen(fname, "rb");
    if (file == NULL) {
        fprintf(stderr, "zei_gzopen error\n");
        exit(1);
    }
    strcpy((char*)uncompr, "garbage");

    if (zei_gzread(file, uncompr, (unsigned)uncomprLen) != len) {
        fprintf(stderr, "zei_gzread err: %s\n", zei_gzerror(file, &err));
        exit(1);
    }
    if (strcmp((char*)uncompr, hello)) {
        fprintf(stderr, "bad zei_gzread: %s\n", (char*)uncompr);
        exit(1);
    } else {
        printf("zei_gzread(): %s\n", (char*)uncompr);
    }

    pos = zei_gzseek(file, -8L, SEEK_CUR);
    if (pos != 6 || zei_gztell(file) != pos) {
        fprintf(stderr, "zei_gzseek error, pos=%ld, zei_gztell=%ld\n",
                (long)pos, (long)zei_gztell(file));
        exit(1);
    }

    if (zei_gzgetc(file) != ' ') {
        fprintf(stderr, "zei_gzgetc error\n");
        exit(1);
    }

    if (zei_gzungetc(' ', file) != ' ') {
        fprintf(stderr, "zei_gzungetc error\n");
        exit(1);
    }

    zei_gzgets(file, (char*)uncompr, (int)uncomprLen);
    if (strlen((char*)uncompr) != 7) { /* " hello!" */
        fprintf(stderr, "zei_gzgets err after zei_gzseek: %s\n", zei_gzerror(file, &err));
        exit(1);
    }
    if (strcmp((char*)uncompr, hello + 6)) {
        fprintf(stderr, "bad zei_gzgets after zei_gzseek\n");
        exit(1);
    } else {
        printf("zei_gzgets() after zei_gzseek: %s\n", (char*)uncompr);
    }

    zei_gzclose(file);
#endif
}

/* ===========================================================================
 * Test zei_deflate() with small buffers
 */
void test_deflate(compr, comprLen)
    Byte *compr;
    uLong comprLen;
{
    z_stream c_stream; /* compression stream */
    int err;
    uLong len = (uLong)strlen(hello)+1;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = zei_deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    CHECK_ERR(err, "zei_deflateInit");

    c_stream.next_in  = (Bytef*)hello;
    c_stream.next_out = compr;

    while (c_stream.total_in != len && c_stream.total_out < comprLen) {
        c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
        err = zei_deflate(&c_stream, Z_NO_FLUSH);
        CHECK_ERR(err, "zei_deflate");
    }
    /* Finish the stream, still forcing small buffers: */
    for (;;) {
        c_stream.avail_out = 1;
        err = zei_deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END) break;
        CHECK_ERR(err, "zei_deflate");
    }

    err = zei_deflateEnd(&c_stream);
    CHECK_ERR(err, "zei_deflateEnd");
}

/* ===========================================================================
 * Test zei_inflate() with small buffers
 */
void test_inflate(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;

    err = zei_inflateInit(&d_stream);
    CHECK_ERR(err, "zei_inflateInit");

    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        err = zei_inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        CHECK_ERR(err, "zei_inflate");
    }

    err = zei_inflateEnd(&d_stream);
    CHECK_ERR(err, "zei_inflateEnd");

    if (strcmp((char*)uncompr, hello)) {
        fprintf(stderr, "bad zei_inflate\n");
        exit(1);
    } else {
        printf("zei_inflate(): %s\n", (char *)uncompr);
    }
}

/* ===========================================================================
 * Test zei_deflate() with large buffers and dynamic change of compression level
 */
void test_large_deflate(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = zei_deflateInit(&c_stream, Z_BEST_SPEED);
    CHECK_ERR(err, "zei_deflateInit");

    c_stream.next_out = compr;
    c_stream.avail_out = (uInt)comprLen;

    /* At this point, uncompr is still mostly zeroes, so it should zei_compress
     * very well:
     */
    c_stream.next_in = uncompr;
    c_stream.avail_in = (uInt)uncomprLen;
    err = zei_deflate(&c_stream, Z_NO_FLUSH);
    CHECK_ERR(err, "zei_deflate");
    if (c_stream.avail_in != 0) {
        fprintf(stderr, "zei_deflate not greedy\n");
        exit(1);
    }

    /* Feed in already compressed data and switch to no compression: */
    zei_deflateParams(&c_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
    c_stream.next_in = compr;
    c_stream.avail_in = (uInt)comprLen/2;
    err = zei_deflate(&c_stream, Z_NO_FLUSH);
    CHECK_ERR(err, "zei_deflate");

    /* Switch back to compressing mode: */
    zei_deflateParams(&c_stream, Z_BEST_COMPRESSION, Z_FILTERED);
    c_stream.next_in = uncompr;
    c_stream.avail_in = (uInt)uncomprLen;
    err = zei_deflate(&c_stream, Z_NO_FLUSH);
    CHECK_ERR(err, "zei_deflate");

    err = zei_deflate(&c_stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        fprintf(stderr, "zei_deflate should report Z_STREAM_END\n");
        exit(1);
    }
    err = zei_deflateEnd(&c_stream);
    CHECK_ERR(err, "zei_deflateEnd");
}

/* ===========================================================================
 * Test zei_inflate() with large buffers
 */
void test_large_inflate(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = (uInt)comprLen;

    err = zei_inflateInit(&d_stream);
    CHECK_ERR(err, "zei_inflateInit");

    for (;;) {
        d_stream.next_out = uncompr;            /* discard the output */
        d_stream.avail_out = (uInt)uncomprLen;
        err = zei_inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        CHECK_ERR(err, "large zei_inflate");
    }

    err = zei_inflateEnd(&d_stream);
    CHECK_ERR(err, "zei_inflateEnd");

    if (d_stream.total_out != 2*uncomprLen + comprLen/2) {
        fprintf(stderr, "bad large zei_inflate: %ld\n", d_stream.total_out);
        exit(1);
    } else {
        printf("large_inflate(): OK\n");
    }
}

/* ===========================================================================
 * Test zei_deflate() with full flush
 */
void test_flush(compr, comprLen)
    Byte *compr;
    uLong *comprLen;
{
    z_stream c_stream; /* compression stream */
    int err;
    uInt len = (uInt)strlen(hello)+1;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = zei_deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    CHECK_ERR(err, "zei_deflateInit");

    c_stream.next_in  = (Bytef*)hello;
    c_stream.next_out = compr;
    c_stream.avail_in = 3;
    c_stream.avail_out = (uInt)*comprLen;
    err = zei_deflate(&c_stream, Z_FULL_FLUSH);
    CHECK_ERR(err, "zei_deflate");

    compr[3]++; /* force an error in first compressed block */
    c_stream.avail_in = len - 3;

    err = zei_deflate(&c_stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        CHECK_ERR(err, "zei_deflate");
    }
    err = zei_deflateEnd(&c_stream);
    CHECK_ERR(err, "zei_deflateEnd");

    *comprLen = c_stream.total_out;
}

/* ===========================================================================
 * Test zei_inflateSync()
 */
void test_sync(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 2; /* just read the zlib header */

    err = zei_inflateInit(&d_stream);
    CHECK_ERR(err, "zei_inflateInit");

    d_stream.next_out = uncompr;
    d_stream.avail_out = (uInt)uncomprLen;

    zei_inflate(&d_stream, Z_NO_FLUSH);
    CHECK_ERR(err, "zei_inflate");

    d_stream.avail_in = (uInt)comprLen-2;   /* read all compressed data */
    err = zei_inflateSync(&d_stream);           /* but skip the damaged part */
    CHECK_ERR(err, "zei_inflateSync");

    err = zei_inflate(&d_stream, Z_FINISH);
    if (err != Z_DATA_ERROR) {
        fprintf(stderr, "zei_inflate should report DATA_ERROR\n");
        /* Because of incorrect zei_adler32 */
        exit(1);
    }
    err = zei_inflateEnd(&d_stream);
    CHECK_ERR(err, "zei_inflateEnd");

    printf("after zei_inflateSync(): hel%s\n", (char *)uncompr);
}

/* ===========================================================================
 * Test zei_deflate() with preset dictionary
 */
void test_dict_deflate(compr, comprLen)
    Byte *compr;
    uLong comprLen;
{
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = zei_deflateInit(&c_stream, Z_BEST_COMPRESSION);
    CHECK_ERR(err, "zei_deflateInit");

    err = zei_deflateSetDictionary(&c_stream,
                               (const Bytef*)dictionary, sizeof(dictionary));
    CHECK_ERR(err, "zei_deflateSetDictionary");

    dictId = c_stream.adler;
    c_stream.next_out = compr;
    c_stream.avail_out = (uInt)comprLen;

    c_stream.next_in = (Bytef*)hello;
    c_stream.avail_in = (uInt)strlen(hello)+1;

    err = zei_deflate(&c_stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        fprintf(stderr, "zei_deflate should report Z_STREAM_END\n");
        exit(1);
    }
    err = zei_deflateEnd(&c_stream);
    CHECK_ERR(err, "zei_deflateEnd");
}

/* ===========================================================================
 * Test zei_inflate() with a preset dictionary
 */
void test_dict_inflate(compr, comprLen, uncompr, uncomprLen)
    Byte *compr, *uncompr;
    uLong comprLen, uncomprLen;
{
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = (uInt)comprLen;

    err = zei_inflateInit(&d_stream);
    CHECK_ERR(err, "zei_inflateInit");

    d_stream.next_out = uncompr;
    d_stream.avail_out = (uInt)uncomprLen;

    for (;;) {
        err = zei_inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        if (err == Z_NEED_DICT) {
            if (d_stream.adler != dictId) {
                fprintf(stderr, "unexpected dictionary");
                exit(1);
            }
            err = zei_inflateSetDictionary(&d_stream, (const Bytef*)dictionary,
                                       sizeof(dictionary));
        }
        CHECK_ERR(err, "zei_inflate with dict");
    }

    err = zei_inflateEnd(&d_stream);
    CHECK_ERR(err, "zei_inflateEnd");

    if (strcmp((char*)uncompr, hello)) {
        fprintf(stderr, "bad zei_inflate with dict\n");
        exit(1);
    } else {
        printf("zei_inflate with dictionary: %s\n", (char *)uncompr);
    }
}

/* ===========================================================================
 * Usage:  example [output.gz  [input.gz]]
 */

int main(argc, argv)
    int argc;
    char *argv[];
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000*sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    static const char* myVersion = ZLIB_VERSION;

    if (zei_zlibVersion()[0] != myVersion[0]) {
        fprintf(stderr, "incompatible zlib version\n");
        exit(1);

    } else if (strcmp(zei_zlibVersion(), ZLIB_VERSION) != 0) {
        fprintf(stderr, "warning: different zlib version\n");
    }

    printf("zlib version %s = 0x%04x, compile flags = 0x%lx\n",
            ZLIB_VERSION, ZLIB_VERNUM, zei_zlibCompileFlags());

    compr    = (Byte*)calloc((uInt)comprLen, 1);
    uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
    /* compr and uncompr are cleared to avoid reading uninitialized
     * data and to ensure that uncompr compresses well.
     */
    if (compr == Z_NULL || uncompr == Z_NULL) {
        printf("out of memory\n");
        exit(1);
    }
    test_compress(compr, comprLen, uncompr, uncomprLen);

    test_gzio((argc > 1 ? argv[1] : TESTFILE),
              uncompr, uncomprLen);

    test_deflate(compr, comprLen);
    test_inflate(compr, comprLen, uncompr, uncomprLen);

    test_large_deflate(compr, comprLen, uncompr, uncomprLen);
    test_large_inflate(compr, comprLen, uncompr, uncomprLen);

    test_flush(compr, &comprLen);
    test_sync(compr, comprLen, uncompr, uncomprLen);
    comprLen = uncomprLen;

    test_dict_deflate(compr, comprLen);
    test_dict_inflate(compr, comprLen, uncompr, uncomprLen);

    free(compr);
    free(uncompr);

    return 0;
}
