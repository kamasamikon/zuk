#ifndef __TEXT_CONV_H__
#define __TEXT_CONV_H__

#if defined(__WIN32__)
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

char *utf8_to_acp(char *utf8);
char *acp_to_utf8(char *acp);

#ifdef __cplusplus
}
#endif

#endif /* __TEXT_CONV_H__ */
