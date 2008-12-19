#include "string.h"
#include "ipaddr.h"

/////////// ipv4
static const kchar* parse_part(const kchar* str, kuchar* num)
{
    kuchar i = 0;
    if (*str < '0' || *str > '9')
        return 0;
    do {
        kuint newi;
        newi = i * 10 + *str - '0';
        if (newi < i || newi > 255) return 0;
        ++str;
        i = newi;
    } while (*str >= '0' && *str <= '9');
    *num = i;
    return str;
}

/** Scan a C string for an IPv4 address.

  \return \c NULL if parsing failed, otherwise a pointer to the
  first character after the end of the address.
  */
const kchar* ipv4_scan(const kchar* start, kuchar addr[])
{
    const kchar* s;
    if ((s = parse_part(start, &addr[0])) == 0) return 0;
    if (*s != '.' && addr[0] == 0) {
        /* Special case -- The number "0" represents 0.0.0.0 */
        memset(addr, 0, 4 * sizeof(kuchar));
        return s;
    }
    if ((s = parse_part(s+1, &addr[1])) == 0 || *s != '.') return 0;
    if ((s = parse_part(s+1, &addr[2])) == 0 || *s != '.') return 0;
    if ((s = parse_part(s+1, &addr[3])) == 0) return 0;
    return s;
}

/////////// ipv6
static kint hex2bin(kchar c)
{
    switch (c) {
        case '0':           return 0x0;
        case '1':           return 0x1;
        case '2':           return 0x2;
        case '3':           return 0x3;
        case '4':           return 0x4;
        case '5':           return 0x5;
        case '6':           return 0x6;
        case '7':           return 0x7;
        case '8':           return 0x8;
        case '9':           return 0x9;
        case 'A': case 'a': return 0xa;
        case 'B': case 'b': return 0xb;
        case 'C': case 'c': return 0xc;
        case 'D': case 'd': return 0xd;
        case 'E': case 'e': return 0xe;
        case 'F': case 'f': return 0xf;
        default: return -1;
    }
}

static const kchar* parse_hexpart(const kchar* s, kushort* num)
{
    kint i = 0;
    kint t;
    while ((t = hex2bin(*s)) != -1) {
        ++s;
        i = (i << 4) | t;
        if (i > 0xffff)
            return 0;
    }
    *num = i;
    return s;
}

static void set(kuchar addr[], kint offset, kushort bit)
{
    offset *= 2;
    addr[offset++] = bit >> 8;
    addr[offset] = (kuchar)bit;
}

/** Scan a C string for an IPv6 address.

  \return \c NULL if parsing failed, otherwise a pointer to the
  first character after the end of the address.
  */
const kchar* ipv6_scan(const kchar* s, kuchar addr[])
{
    kushort bits[8];
    kuint len1;
    kuint len2;
    kuint i;
    len1 = len2 = 0;
    if (s[0] == ':' && s[1] == ':')
        ++s;
    else {
        while (len1 < 8) {
            const kchar* news;
            if ((news = parse_hexpart(s, &bits[len1])) == 0 ||
                    (news == s && *news != ':'))
                return 0;
            s = news + (*news == ':');
            ++len1;
            if (*s == ':')
                break;
        }
        for (i = 0; i < len1; ++i)
            set(addr, i, bits[i]);
    }
    if (len1 < 8) {
        ++s;
        while (len2 < 8-len1) {
            const kchar* news;
            if ((news = parse_hexpart(s, &bits[len2])) == 0)
                return 0;
            if (news == s) break;
            s = news;
            ++len2;
            if (*s != ':') break;
            ++s;
        }
        for (i = len1; i < 8-len2; ++i)
            set(addr, i, 0);
        for (i = 0; i < len2; ++i)
            set(addr, i+8-len2, bits[i]);
    }
    return s;
}
#ifdef TEST_IPADDRESS
static void test(const kchar* start)
{
    kuchar ip[16];
    kint i;
    const kchar* end;
    end = ipv6_scan(start, &ip);
    printf("input:[%s], end[%08x]:[%s]\n", start, end, end);
    for (i = 0; i < 16; i++) {
        printf(" %02X ", ip.addr[i]);
    }
    printf("\n\n");
}

kint xxmain(void)
{
#if 0
    kint i;
    kuchar ip[16];
    const kchar* end;
    end = ipv4_scan("255.1.80.12", &ip);
    for (i = 0; i < 16; i++) {
        printf(" %02X ", ip.addr[i]);
    }
#endif

    test("255.1.80.12");
    test("::");
    test("::1");
    test("::0.0.0.2");
    test("::FFFF:1.20.100.200");
    test("::FFFF:1.20.100.200x");
    test("::101:0:0:0:0:0");
    test("1080::8:800:200C:417A");
    test("FF01::101");
    test("FF01::");
    test("FF01::x");
    test("112:2334:4556:6778:899A:ABBC:CDDE:EFF0");
    test("112:2334:4556:6778:899A:ABBC:CDDE:EFF0x");

    return 0;
}

#ifdef SELFTEST_EXP
::=00000000000000000000000000000000+
::1=00000000000000000000000000000001+
::0.0.0.2=00000000000000000000000000000002+
::FFFF:1.20.100.200=00000000000000000000ffff011464c8+
::FFFF:1.20.100.200x=00000000000000000000ffff011464c8+x
::101:0:0:0:0:0=00000000010100000000000000000000+
1080::8:800:200C:417A=108000000000000000080800200c417a+
FF01::101=ff010000000000000000000000000101+
FF01::=ff010000000000000000000000000000+
FF01::x=ff010000000000000000000000000000+x
112:2334:4556:6778:899A:ABBC:CDDE:EFF0=0112233445566778899aabbccddeeff0+
112:2334:4556:6778:899A:ABBC:CDDE:EFF0x=0112233445566778899aabbccddeeff0+x
#endif

#endif
