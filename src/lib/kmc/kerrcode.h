#ifndef KERRCODE_H_
#define KERRCODE_H_

#define EC_OK				0x00000000
#define EC_NG				0x80000000
#define EC_NOT_SUPPORT 		0xFFFFFFFF	// 如果某个特性不被支持，针对该特性的接口调用必须返回　EC_NOT_SUPPORT

#endif /*KERRCODE_H_*/
