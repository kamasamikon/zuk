/* vim:set et sw=4 sts=4: */
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
