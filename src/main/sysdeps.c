/* vim:set et sw=4 sts=4: */

#include <ksal.h>
#include <kdbg.h>

#include "sysdeps.h"

#if defined(__WIN32__) || defined(__WINCE__)
#include <Winsock2.h>
#else
#endif

kint zuk_sysdeps_start(KIM *im, kint argc, kchar **argv)
{
#if defined(__WIN32__) || defined(__WINCE__)
    /* use wsa SOCKET */
    WSADATA wsd;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsd)) {
        klog(("WSAStartup() failed\n"));
        return -1;
    }
    klog(("WSAStartup() success\n"));
#else
#endif
    return 0;
}

kint zuk_sysdeps_term()
{
#if defined(__WIN32__) || defined(__WINCE__)
    /* undo WSAStartup */
    WSACleanup();
#else
#endif
    return 0;
}

