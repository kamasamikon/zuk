/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __MD5SUM_H__
#define __MD5SUM_H__

#ifdef __cplusplus
extern "C" {
#endif

int md5_file(const char *a_path, char a_retmd5[33]);
unsigned char* md5_calculate(const unsigned char* pachSource, int nLen);

#ifdef __cplusplus
}
#endif

#endif /* __MD5SUM_H__ */
