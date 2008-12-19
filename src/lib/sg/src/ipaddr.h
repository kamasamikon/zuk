#ifndef __IPADDR_H__
#define __IPADDR_H__

#include <ktypes.h>

const kchar* ipv4_scan(const kchar* start, kuchar addr[]);
const kchar* ipv6_scan(const kchar* s, kuchar addr[]);

#endif /* __IPADDR_H__ */
