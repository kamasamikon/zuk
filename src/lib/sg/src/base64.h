/* vim:set et sw=4 sts=4: */
#ifndef __BASE64_H__
#define __BASE64_H__

#include <ktypes.h>

int base64_encode(const void *data, int size, char **str);
int base64_decode(const char *str, void *data);

#endif /* __BASE64_H__ */
