#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ktypes.h>
#include <kdbg.h>
#include <kmem.h>
#include <ktmr.h>
#include <ksyn.h>
#include <kstr.h>

#if (defined(__WIN32__) || defined(__WINCE__))
#include <winsock2.h>
/* #define EINTR (errno + 1) */
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <signal.h>
#include <errno.h>

#include <hget.h>

#ifdef HGET_TEST
char ctbuffer[200];
char *curtime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    sprintf(ctbuffer, "%d.%d", ts.tv_sec, ts.tv_nsec / 1000);
    return ctbuffer;
}
#define klog(x) do { printf("[klog-%s]", curtime()); printf x ; } while (0)
#define DUMP_BUF() do { klog(("%s\n\n\n\n\n\n\n\n", a_retbuf)); } while (0)
#else
#define DUMP_BUF() do { } while (0)
#endif

#define SOCKET kint

#define PATH_LEN    1024
#define DATA_LEN    2048                            /**< XXX data buffer for one recv call, should be large enough to hold on header */

static kint socket_last_error()
{
#if (defined(__WIN32__) || defined(__WINCE__))
    return WSAGetLastError();
#else
    perror("last_error");
    return errno;
#endif
}

/**
 * @brief Close socket
 */
kvoid hget_free_socket(SOCKET a_socket)
{
    if (-1 != a_socket) {
#if (defined(__WIN32__) || defined(__WINCE__))
        shutdown(a_socket, SD_BOTH);
        closesocket(a_socket);
#else
        shutdown(a_socket, SHUT_RDWR);
        close(a_socket);
#endif
    }
}

/**
 * @brief parse the url like, https://auv:woshiauv@192.155.35.65:9088/index.php?l=en
 */
kint hget_parseurl(const kchar *a_url, kuint *a_prot, kchar a_user[], kchar a_pass[],
        kchar a_host[], kchar a_path[], kushort *a_port)
{
    const kchar *url_cur = a_url;
    kchar *temp;

    /* get a_prot */
    *a_prot = PROT_HTTP;
    if (strstr(url_cur, "http://")) {
        url_cur += 7;
        *a_prot = PROT_HTTP;
    } else if (strstr(url_cur, "https://")) {
        url_cur += 8;
        *a_prot = PROT_HTTPS;
    }

    /* get user and pass */
    *a_user = '\0';
    *a_pass = '\0';
    if (temp = strchr(url_cur, '@')) {
        strncpy(a_user, url_cur, temp - url_cur);
        a_user[temp - url_cur] = '\0';

        /* adjust url_cur to next word of @ */
        url_cur = temp + 1;

        /* get pass */
        if (temp = strchr(a_user, ':')) {
            /* user: auv:woshiauv to auv */
            *temp = '\0';

            strcpy(a_pass, temp + 1);
        }
    }

    /* get port */
    *a_port = 80;
    if (temp = strchr(url_cur, ':')) {
        *a_port = (kushort)strtoul(temp + 1, 0, 10);
    }

    /* get host */
    strcpy(a_host, url_cur);
    for (temp = a_host; *temp != '/' && *temp != ':' && *temp != '\0'; ++temp);
    *temp = '\0';
    url_cur += temp - a_host;

    /* get path */
    if (temp = strchr(url_cur, '/')) {
        strcpy(a_path, temp);
    } else {
        strcpy(a_path, "/");
    }

    return 0;
}

/* create socket and connect */
/* a_socket for quick free while connecting */
kint hget_connect(kint a_prot, const kchar *a_user, const kchar *a_pass,
        const kchar *a_host, const kchar *a_path, kushort a_port, SOCKET *a_socket)
{
    kint ret = -1;
    SOCKET sockfd = -1;
    struct sockaddr_in addr;
    struct hostent *host_ent;
    kuint ip_addr;
    struct timeval timeout;

#if (defined(__WIN32__) || defined(__WINCE__))
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    if (a_socket) {
        *a_socket = -1;
    }

    /* Set socket parameters */
    klog(("hget_connect: inet_addr\n"));
    ip_addr = inet_addr(a_host);
    if (INADDR_NONE == ip_addr) {
        klog(("hget_connect: gethostbyname\n"));
        host_ent = gethostbyname(a_host);
        if (!host_ent) {
            kerror(("hget_connect: gethostbyname failed: %d\n", socket_last_error()));
            return PGEC_HOST;
        }
        ip_addr = *((kulong*)host_ent->h_addr);
    }

    klog(("hget_connect: socket\n"));
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockfd) {
        kerror(("hget_connect: create socket failed: %d\n", socket_last_error()));
        return PGEC_SOCKET;
    }

    if (a_socket) {
        *a_socket = sockfd;
    }

    /* set timeout for recv and send */
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        perror("hget_connect: setsockopt:SO_RCVTIMEO");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        perror("hget_connect: setsockopt:SO_SNDTIMEO");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip_addr;
    addr.sin_port = htons(a_port);

    /* connect */
    klog(("hget_connect: connect\n"));
    while ((-1 == (ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)))) && (EINTR == errno));
    if (-1 == ret) {
        if (a_socket) {
            *a_socket = -1;
        }

        kerror(("hget_connect: connect failed: %d\n", socket_last_error()));
        hget_free_socket(sockfd);
        return PGEC_CONNECT;
    }

    klog(("hget_connect: sockfd:%d\n", sockfd));
    return PGEC_SUCCESS;
}

kint hget_recv(SOCKET a_socket, const kchar *a_host, const kchar *a_path,
        const kchar *a_proxy, kbool a_get, const kchar *a_cmd,
        kchar **a_datbuf, kint *a_datlen, kchar **a_hdrbuf, kint *a_hdrlen)
{
    kchar *pTemp, *pNext;
    kchar header[PATH_LEN] = "";

    kchar data[DATA_LEN];
    kint rc = PGEC_ERROR, ret = -1, resp_code, dat_len = 0, hdr_len, cur_len = 0, lft_len;

    /*
     * Fill the HTTP header information
     */
    /*
     * An Example for Request
     *
     * "POST /SGHCServ/SGHCI.ashx HTTP/1.0\r\n"
     * "Accept: text/xml\r\n"
     * "Content-Length: 36\r\n"
     * "Host: 192.168.1.19\r\n"
     * "Content-Type: text/xml\r\n"
     * "\r\n"
     * "<Msg><Cmd>GetServiceList</Cmd></Msg>"
     */
    if (a_get) {
        sprintf(header,
                "GET %s HTTP/1.1\r\n"
                "Connection: Keep-Alive\r\n"
                "Host: %s\r\n"
                "Connection: Close\r\n\r\n",
                a_path, a_host
               );
    } else {
        sprintf(header,
                "POST %s HTTP/1.1\r\n"
                "Accept: text/xml\r\n"
                "Connection: Keep-Alive\r\n"
                "Content-Length: %d\r\n"
                "Host: %s\r\n"
                "Content-Type: text/xml\r\n\r\n"
                "%s",
                a_path, a_cmd ? strlen(a_cmd) : 0, a_host, a_cmd ? a_cmd : ""
               );
    }

    klog(("start send\n"));

    while ((-1 == (ret = send(a_socket, header, strlen(header), 0))) && (EINTR == errno));
    if (-1 == ret) {
        kerror(("send failed: %d\n", socket_last_error()));
        rc = PGEC_SEND;
        goto done;
    }

    /*
     * first, get the information
     */
    /* try the first buffer as full as posible */
    ret = cur_len = 0;
    data[0] = '\0';
    do {
        cur_len += ret;
        if (strstr(data, "\r\n\r\n")) {
            break;
        }
        while ((-1 == (ret = recv(a_socket, data + ret, DATA_LEN - ret, 0))) && (EINTR == errno));
        if (-1 == ret) {
            kerror(("recv failed: %d\n", socket_last_error()));
        }
        if (0 == ret) {
            kerror(("recv failed: socket closed.\n"));
        }
        if (ret >= DATA_LEN) {
            cur_len += ret;
        }
    } while ((ret > 0) && (ret < DATA_LEN));

    ret = cur_len;

    if (ret > 0) {

        klog(("hget_recv:%d: cont-data:%s\n", __LINE__, (ret > 0) ? data : "(nil)"));

        /*
         * try to get header, only when (*a_hdrbuf != zero)
         */
        if (*a_hdrlen < 0) {
            *a_hdrlen = 0x7FFFFFFF;
        }
        if (*a_hdrlen > 0) {
            cur_len = *a_hdrlen;
            pTemp = strstr(data, "\r\n\r\n");
            hdr_len = pTemp - data;
            if (!*a_hdrbuf) {
                if (hdr_len > cur_len) {
                    *a_hdrbuf = (kchar*)kmem_alloz(cur_len + 1);
                } else {
                    *a_hdrbuf = (kchar*)kmem_alloz(hdr_len + 1);
                }
            }
            *a_hdrlen = 0;
            if (*a_hdrbuf) {
                if (hdr_len > cur_len) {
                    memcpy(*a_hdrbuf, data, cur_len);
                    *a_hdrlen = cur_len;
                } else {
                    memcpy(*a_hdrbuf, data, hdr_len);
                    *a_hdrlen = hdr_len;
                }
            }
        }

        if (*a_datlen < 0) {
            *a_datlen = 0x7FFFFFFF;
        }
        if (*a_datlen == 0) {
            /* not need content, return */
            rc = PGEC_SUCCESS;
            goto done;
        }

        /* Get Response Code */
        pTemp = strstr(data, "HTTP/1.1 ");
        if (!pTemp) {
            pTemp = strstr(data, "HTTP/1.0 ");
        }
        if (!pTemp) {
            kerror(("Can not get Response code\n"));
            rc = PGEC_HEADER;
            goto done;
        }
        resp_code = atoi(pTemp + 9);
        if (resp_code < 200 || resp_code >= 300) {
            kerror(("resp_code tell an error\n"));
            rc = PGEC_RESP_CODE;
            goto done;
        }

        /* Get Buffer Length */
        pTemp = strstr(data, "Content-Length: ");
        if (!pTemp) {
            kerror(("Can not get content length\n"));
            rc = PGEC_CONTENT_LEN;
            goto done;
        }

        dat_len = atoi(pTemp + 16);
        cur_len = *a_datlen;
        if (!*a_datbuf) {
            if (dat_len > cur_len) {
                *a_datbuf = (kchar*)kmem_alloz(cur_len + 1);
                dat_len = cur_len;
            } else {
                *a_datbuf = (kchar*)kmem_alloz(dat_len + 1);
                dat_len = dat_len;
            }
        }
        pNext = *a_datbuf;
        if (!pNext) {
            kerror(("Can not alloc memory\n"));
            rc = PGEC_MEMORY;
            goto done;
        }

        cur_len = 0;

        /* Copy the first part of data */
        pTemp = strstr(data, "\r\n\r\n");

        lft_len = ret - (pTemp + 4 - data);

        if (lft_len < dat_len) {
            memcpy(pNext, pTemp + 4, lft_len);
            pNext += lft_len;
            cur_len += lft_len;
        } else {
            memcpy(pNext, pTemp + 4, dat_len);
            pNext += dat_len;
            cur_len += dat_len;
        }
        kassert(cur_len <= dat_len || !"bad cur_len or all_en");

        /*
         * Then got the rest data
         */
        klog(("hget_recv: cur_len:%d, dat_len:%d\n", cur_len, dat_len));
        if (cur_len < dat_len) {
            resp_code = 1;
            while ((-1 == (ret = recv(a_socket, data, DATA_LEN, 0))) && (EINTR == errno));
            if (-1 == ret) {
                kerror(("recv failed: %d\n", socket_last_error()));
            }
            if (0 == ret) {
                kerror(("recv failed: socket closed.\n"));
            }

            klog(("hget_recv: dat_len:%d, cur_len:%d, recv_cnt:%d, ret:%d, err:%d\n",
                        dat_len, cur_len, resp_code++, ret, socket_last_error()));

            while (ret > 0) {

                lft_len = ret;

                if (lft_len < (dat_len - cur_len)) {
                    memcpy(pNext, data, lft_len);
                    pNext += lft_len;
                    cur_len += lft_len;
                } else {
                    memcpy(pNext, data, dat_len - cur_len);
                    pNext += dat_len - cur_len;
                    cur_len += dat_len - cur_len;
                }
                kassert(cur_len <= dat_len || !"bad cur_len or all_en");

                if (cur_len < dat_len) {
                    while ((-1 == (ret = recv(a_socket, data, DATA_LEN, 0))) && (EINTR == errno));
                } else {
                    klog(("hget_recv: all received\n"));
                    break;
                }

                klog(("hget_recv: dat_len:%d, cur_len:%d, recv_cnt:%d, ret:%d, err:%d\n",
                            dat_len, cur_len, resp_code++, ret, socket_last_error()));
            }
        }

        *a_datlen = cur_len;
        rc = PGEC_SUCCESS;
    }

done:
    klog(("hget_recv is : %d\n", rc));
    return rc;
}

kint hget(const kchar *a_url, const kchar *a_proxy, kbool a_get, const kchar *a_cmd,
        kchar **a_datbuf, kint *a_datlen, kchar **a_hdrbuf, kint *a_hdrlen, kint *a_socket)
{
    kchar user[32] = { 0 }, pass[32] = { 0 }, host[1024] = { 0 }, path[1024] = { 0 };
    kuint prot;
    kushort port;
    kint ret = PGEC_ERROR;
    SOCKET ls, *s = a_socket ? a_socket : &ls;

    hget_parseurl(a_url, &prot, user, pass, host, path, &port);

    if (PGEC_SUCCESS == (ret = hget_connect(prot, user, pass, host, path, port, s))) {
        ret = hget_recv(*s, host, path, a_proxy, a_get, a_cmd, a_datbuf, a_datlen, a_hdrbuf, a_hdrlen);
        hget_free_socket(*s);
    }
    return ret;
}

/**
 * @brief Get SG data.
 *
 * @param[in] a_url URL from which get data.
 * @param[in] a_proxy Proxy URL
 * @param[in] a_get GET or POST method
 * @param[in] a_cmd Specific what kind of SG data should be returned.
 * @param[in] a_datbuf IN:*a_datbuf: buffer provider by caller, if (*a_datbuf == zero) (*a_datbuf = kmem_alloc).
 * @param[in] a_datlen IN:*a_datlen: max data len should be return, if zero, do not return hdr data
 *                     OUT:*a_datlen: actually read data length.
 * @param[in] a_hdrbuf IN:*a_hdrbuf: buffer provider by caller, if (*a_hdrbuf == zero) (*a_hdrbuf = kmem_alloc).
 * @param[in] a_hdrlen IN:*a_hdrlen: max hdr len should be return, if zero, do not return hdr data
 *                     OUT:*a_hdrlen: actually read header length.
 * @param[in] a_socket socket which can be close when hget last too long.
 *
 * @return 0 for success, -1 for error.
 */
kint hget_old(const kchar *a_url, const kchar *a_proxy, kbool a_get, const kchar *a_cmd,
        kchar **a_datbuf, kint *a_datlen, kchar **a_hdrbuf, kint *a_hdrlen, kint *a_socket)
{
    SOCKET sockfd;

    kchar *pHost = 0, *pTemp, *pNext;
    kchar host[PATH_LEN], dir[PATH_LEN];
    kchar header[PATH_LEN] = "";

    kchar data[DATA_LEN];
    kint rc = PGEC_ERROR, ret = -1, resp_code, dat_len = 0, hdr_len, cur_len = 0, lft_len;

    struct sockaddr_in addr;
    struct hostent *host_ent;

    kushort port = 80;
    kuint ip_addr;

    struct timeval timeout;

    klog(("hget[in]:%s\n", a_cmd));

#if (defined(__WIN32__) || defined(__WINCE__))
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    /*
     * renew the client
     */
    pTemp = strstr(a_url, "http://");
    if (pTemp) {
        a_url += 7;
    }

    /* Seperate Host and path in url */
    strcpy(host, a_url);
    for (pHost = host; *pHost != '/' && *pHost != '\0'; ++pHost);
    *pHost = '\0';

    /* get port */
    pTemp = strchr(host, ':');
    if (pTemp) {
        /* set port */
        port = (kushort)strtoul(pTemp + 1, 0, 10);

        /* remove port part from host */
        *pTemp = '\0';
    }

    /* process proxy */

    /* Set socket parameters */
    klog(("hget: inet_addr\n"));
    ip_addr = inet_addr(host);
    if (INADDR_NONE == ip_addr) {
        klog(("hget: gethostbyname\n"));
        host_ent = gethostbyname(host);
        if (!host_ent) {
            kerror(("gethostbyname failed: %d\n", socket_last_error()));
            return PGEC_HOST;
        }
        ip_addr = *((kulong*)host_ent->h_addr);
    }

    klog(("hget: socket\n"));
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockfd) {
        kerror(("create socket failed: %d\n", socket_last_error()));
        return PGEC_SOCKET;
    }
    if (a_socket) {
        *a_socket = sockfd;
    }

    /* set timeout for recv and send */
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        perror("be:setsockopt:SO_RCVTIMEO");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        perror("be:setsockopt:SO_SNDTIMEO");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip_addr;
    addr.sin_port = htons(port);

    /*
     * connect and send request
     */
    klog(("hget: connect\n"));
    while ((-1 == (ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)))) && (EINTR == errno));
    if (-1 == ret) {
        if (a_socket) {
            *a_socket = -1;
        }

        kerror(("connect failed: %d\n", socket_last_error()));
        return PGEC_CONNECT;
    }

    klog(("hget: sockfd:%d\n", sockfd));

    /* Seperate Host and path in url */
    strcpy(host, a_url);
    for (pHost = host; *pHost != '/' && *pHost != '\0'; ++pHost);
    if ((kuint)(pHost - host) == strlen(host)) {
        strcpy(dir, "/");
    } else {
        strcpy(dir, pHost);
    }
    *pHost = '\0';

    /*
     * Fill the HTTP header information
     */
    /*
     * An Example for Request
     *
     * "POST /SGHCServ/SGHCI.ashx HTTP/1.0\r\n"
     * "Accept: text/xml\r\n"
     * "Content-Length: 36\r\n"
     * "Host: 192.168.1.19\r\n"
     * "Content-Type: text/xml\r\n"
     * "\r\n"
     * "<Msg><Cmd>GetServiceList</Cmd></Msg>"
     */
    if (a_get) {
        sprintf(header,
                "GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: Close\r\n\r\n",
                dir, host
               );
    } else {
        sprintf(header,
                "POST %s HTTP/1.1\r\n"
                "Accept: text/xml\r\n"
                "Content-Length: %d\r\n"
                "Host: %s\r\n"
                "Content-Type: text/xml\r\n\r\n"
                "%s",
                dir, a_cmd ? strlen(a_cmd) : 0, host, a_cmd ? a_cmd : ""
               );
    }

    klog(("hget: start send\n"));

    while ((-1 == (ret = send(sockfd, header, strlen(header), 0))) && (EINTR == errno));
    if (-1 == ret) {
        kerror(("send failed: %d\n", socket_last_error()));
        rc = PGEC_SEND;
        goto done;
    }

    /*
     * first, get the information
     */
    /* try the first buffer as full as posible */
    ret = cur_len = 0;
    data[0] = '\0';
    do {
        cur_len += ret;
        if (strstr(data, "\r\n\r\n")) {
            break;
        }
        while ((-1 == (ret = recv(sockfd, data + ret, DATA_LEN - ret, 0))) && (EINTR == errno));
        if (-1 == ret) {
            kerror(("recv failed: %d\n", socket_last_error()));
        }
        if (0 == ret) {
            kerror(("recv failed: socket closed.\n"));
        }
        if (ret >= DATA_LEN) {
            cur_len += ret;
        }
    } while ((ret > 0) && (ret < DATA_LEN));

    ret = cur_len;

    if (ret > 0) {

        klog(("hget: cont-data:%s\n", (ret > 0) ? data : "(nil)"));

        /*
         * try to get header, only when (*a_hdrbuf != zero)
         */
        if (*a_hdrlen < 0) {
            *a_hdrlen = 0x7FFFFFFF;
        }
        if (*a_hdrlen > 0) {
            cur_len = *a_hdrlen;
            pTemp = strstr(data, "\r\n\r\n");
            hdr_len = pTemp - data;
            if (!*a_hdrbuf) {
                if (hdr_len > cur_len) {
                    *a_hdrbuf = (kchar*)kmem_alloz(cur_len + 1);
                } else {
                    *a_hdrbuf = (kchar*)kmem_alloz(hdr_len + 1);
                }
            }
            *a_hdrlen = 0;
            if (*a_hdrbuf) {
                if (hdr_len > cur_len) {
                    memcpy(*a_hdrbuf, data, cur_len);
                    *a_hdrlen = cur_len;
                } else {
                    memcpy(*a_hdrbuf, data, hdr_len);
                    *a_hdrlen = hdr_len;
                }
            }
        }

        if (*a_datlen < 0) {
            *a_datlen = 0x7FFFFFFF;
        }
        if (*a_datlen == 0) {
            /* not need content, return */
            rc = PGEC_SUCCESS;
            goto done;
        }

        /* Get Response Code */
        pTemp = strstr(data, "HTTP/1.1 ");
        if (!pTemp) {
            pTemp = strstr(data, "HTTP/1.0 ");
        }
        if (!pTemp) {
            kerror(("Can not get Response code\n"));
            rc = PGEC_HEADER;
            goto done;
        }
        resp_code = atoi(pTemp + 9);
        if (resp_code < 200 || resp_code >= 300) {
            kerror(("resp_code tell an error\n"));
            rc = PGEC_RESP_CODE;
            goto done;
        }

        /* Get Buffer Length */
        pTemp = strstr(data, "Content-Length: ");
        if (!pTemp) {
            kerror(("Can not get content length\n"));
            rc = PGEC_CONTENT_LEN;
            goto done;
        }

        dat_len = atoi(pTemp + 16);
        cur_len = *a_datlen;
        if (!*a_datbuf) {
            if (dat_len > cur_len) {
                *a_datbuf = (kchar*)kmem_alloz(cur_len + 1);
                dat_len = cur_len;
            } else {
                *a_datbuf = (kchar*)kmem_alloz(dat_len + 1);
                dat_len = dat_len;
            }
        }
        pNext = *a_datbuf;
        if (!pNext) {
            kerror(("Can not alloc memory\n"));
            rc = PGEC_MEMORY;
            goto done;
        }

        cur_len = 0;

        /* Copy the first part of data */
        pTemp = strstr(data, "\r\n\r\n");

        lft_len = ret - (pTemp + 4 - data);

        if (lft_len < dat_len) {
            memcpy(pNext, pTemp + 4, lft_len);
            pNext += lft_len;
            cur_len += lft_len;
        } else {
            memcpy(pNext, pTemp + 4, dat_len);
            pNext += dat_len;
            cur_len += dat_len;
        }
        kassert(cur_len <= dat_len || !"bad cur_len or all_en");

        /*
         * Then got the rest data
         */
        klog(("hget: cur_len:%d, dat_len:%d\n", cur_len, dat_len));
        if (cur_len < dat_len) {
            resp_code = 1;
            while ((-1 == (ret = recv(sockfd, data, DATA_LEN, 0))) && (EINTR == errno));
            if (-1 == ret) {
                kerror(("hget: recv failed: %d\n", socket_last_error()));
            }
            if (0 == ret) {
                kerror(("hget: recv failed: socket closed.\n"));
            }
            klog(("hget: recv_count: %d, ret:%d\n", resp_code++, ret));
            klog(("hget: cont-data:%s\n", (ret > 0) ? data : "(nil)"));
            while (ret > 0) {

                lft_len = ret;

                if (lft_len < (dat_len - cur_len)) {
                    memcpy(pNext, data, lft_len);
                    pNext += lft_len;
                    cur_len += lft_len;
                } else {
                    memcpy(pNext, data, dat_len - cur_len);
                    pNext += dat_len - cur_len;
                    cur_len += dat_len - cur_len;
                }
                kassert(cur_len <= dat_len || !"bad cur_len or all_en");

                if (cur_len < dat_len) {
                    while ((-1 == (ret = recv(sockfd, data, DATA_LEN, 0))) && (EINTR == errno));
                    klog(("hget: recv_count: %d, ret:%d\n", resp_code++, ret));
                    klog(("hget: cont-data:%s\n", (ret > 0) ? data : "(nil)"));
                } else {
                    break;
                }
            }
        }

        *a_datlen = cur_len;
        rc = PGEC_SUCCESS;
    }

done:
    hget_free_socket(sockfd);

    if (a_socket) {
        *a_socket = -1;
    }

    klog(("hget: retcode is : %d\n", rc));
    return rc;
}

#if defined(HGET_TEST)
int main()
{
    kchar _datbuf[1024] = { 0 };
    kint _datlen = sizeof(_datbuf);

    kchar _hdrbuf[1024] = { 0 };
    kint _hdrlen = sizeof(_hdrbuf);

    //kchar *url = "http://iGuide.ivisionchina.com/SGServer/SGserver.ashx";
    kchar *url = "http://crossad.ivisionchina.com/cradframe/assembler/registerSubscriber.do?deviceModel=GSmart-T600";
    kchar *cmd = "<Msg><Cmd>GetProviderList</Cmd></Msg>";

    // strcpy(__g_subscriber_url, "http://crossad.ivisionchina.com/cradframe/assembler/registerSubscriber.do?deviceModel=GSmart-T600");

    kchar *datbuf = _datbuf;
    kint datlen = 9000;

    kchar *hdrbuf = knil;
    kint hdrlen = 0;

    hget(url, knil, kfalse, cmd, &datbuf, &datlen, &hdrbuf, &hdrlen);
    printf("dat[%d]:%s\n", datlen, datbuf);
    printf("hdr[%d]:%s\n", hdrlen, hdrbuf);

    return 0;
}
#endif

