/* vim:set et sw=4 sts=4: */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ktypes.h>
#include <kdbg.h>
#include <ktsk.h>
#include <ktmr.h>
#include <kmem.h>
#include <kstr.h>

#include "ivisa_sg_filerev.h"

#include <xmldoc.h>
#include "ksupport.h"

#include "sg_mgr2.h"

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
typedef kint (*SES_OBJECTLIST_CALLBACK)(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE * pObjectList_attibute, IV_OBJECT_INFO * pInfoLisHead,
        kint iObjectNum, void * vpUserData0, void * vpUserData1, void * vpUserData2, void * vpUserData3);
typedef kint (*SES_TRANSFEROBJECT_CALLBACK)(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar* pObjectName, kchar * pObjectUrl,
        kint iObjectVersion, kuchar * pDataBuffer, kuint uiObjectLength, kchar * pSavePathName, kchar * pContentEncoding,
        void * vpUserData0, void * vpUserData1, void * vpUserData2, void * vpUserData3);

#include "../../kacoo/lib/hget/nghget.c"

#define SOCKET kint

#define PATH_LEN                    1024
#define DATA_LEN                    4096

#define PGEC_SUCCESS                0
#define PGEC_HOST                   -1
#define PGEC_SOCKET                 -2
#define PGEC_CONNECT                -3
#define PGEC_SEND                   -4
#define PGEC_HEADER                 -5
#define PGEC_RESP_CODE              -6
#define PGEC_CONTENT_LEN            -7
#define PGEC_RECV                   -8
#define PGEC_MEMORY                 -9

static SOCKET __g_cur_socket = -1;

static kint socket_last_error()
{
#if (defined(__WIN32__) || defined(__WINCE__))
    return WSAGetLastError();
#else
    return -1;
#endif
}

static kvoid free_socket(SOCKET s)
{
#if (defined(__WIN32__) || defined(__WINCE__))
    shutdown(s, SD_BOTH);
    closesocket(s);
#else
    shutdown(s, SHUT_RDWR);
    close(s);
#endif
    __g_cur_socket = -1;
}


kint http_get_data(kchar *a_url, kchar **a_retbuf, kint *a_retlen)
{
    SOCKET sockfd;
    struct sockaddr_in addr;
    struct hostent *host_ent;

    kchar *pHost = 0, *pPort, *pTemp, *pNext;
    kchar host[PATH_LEN], post_or_get[PATH_LEN];
    kchar header[PATH_LEN] = "";
    static kchar data[DATA_LEN];
    kint ret, resp_code;
    kushort port = 80;
    kuint ip_addr;

    pTemp = strstr(a_url, "http://");
    if (pTemp) {
        a_url += 7;
    }

    /* Seperate Host and path in url */
    strcpy(host, a_url);
    for (pHost = host; *pHost != '/' && *pHost != '\0'; ++pHost);
    if ((kuint)(pHost - host) == strlen(host)) {
        strcpy(post_or_get, "/");
    } else {
        strcpy(post_or_get, pHost);
    }
    *pHost = '\0';

    pPort = strchr(host, ':');
    if (pPort) {
        /* set port */
        port = (kushort)strtoul(pPort + 1, 0, 10);

        /* remove port part from host */
        *pPort = '\0';
    }

    /* Set socket parameters */
    ip_addr = inet_addr(host);
    if (INADDR_NONE == ip_addr) {
        host_ent = gethostbyname(host);
        if (!host_ent) {
            kerror(("gethostbyname failed: %d\n", socket_last_error()));
            kerror(("a_url:%s\n", a_url));
            return PGEC_HOST;
        }
        ip_addr = *((kulong*)host_ent->h_addr);
    }
    __g_cur_socket = sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockfd) {
        kerror(("create socket failed: %d\n", socket_last_error()));
        kerror(("a_url:%s\n", a_url));
        return PGEC_SOCKET;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip_addr;
    addr.sin_port = htons(port);

    /* Fill the HTTP header information */
    strcat(header, "GET ");
    strcat(header, post_or_get);
    strcat(header, " HTTP/1.1\r\n");
    strcat(header, "HOST: ");
    strcat(header, host);
    strcat(header, "\r\nConnection: Close\r\n\r\n");

    /* connect and send request */
    ret = connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    if (-1 == ret) {
        kerror(("connect failed: %d\n", socket_last_error()));
        kerror(("a_url:%s\n", a_url));
        free_socket(sockfd);
        return PGEC_CONNECT;
    }
    ret = send(sockfd, header, strlen(header), 0);
    if (-1 == ret) {
        kerror(("send failed: %d\n", socket_last_error()));
        kerror(("a_url:%s\n", a_url));
        free_socket(sockfd);
        return PGEC_SEND;
    }

    /* first, get the information */
    ret = recv(sockfd, data, DATA_LEN, 0);
    if (ret > 0) {
        /* Get Response Code */
        pTemp = strstr(data, "HTTP/1.1 ");
        if (!pTemp) {
            kerror(("Can not get Response code\n"));
            kerror(("a_url:%s\n", a_url));
            free_socket(sockfd);
            return PGEC_HEADER;
        }
        resp_code = atoi(pTemp + 9);
        if (resp_code < 200 || resp_code >= 300) {
            kerror(("resp_code tell an error:%d\n", resp_code));
            kerror(("a_url:%s\n", a_url));
            free_socket(sockfd);
            return PGEC_RESP_CODE;
        }

        /* Get Buffer Length */
        pTemp = strstr(data, "Content-Length: ");
        if (!pTemp) {
            kerror(("Can not get content length\n"));
            kerror(("a_url:%s\n", a_url));
            free_socket(sockfd);
            return PGEC_CONTENT_LEN;
        }
        *a_retlen = atoi(pTemp + 16);
        pNext = *a_retbuf = kmem_alloz(*a_retlen);
        if (!*a_retbuf) {
            kerror(("Can not alloc memory: %d\n", *a_retlen));
            kerror(("a_url:%s\n", a_url));
            free_socket(sockfd);
            return PGEC_MEMORY;
        }

        /* Copy the first part of data */
        pTemp = strstr(data, "\r\n\r\n");
        memcpy(pNext, pTemp + 4, ret - (pTemp + 4 - data));
        pNext += ret - (pTemp + 4 - data);
    }

    /* Then got the rest */
    ret = recv(sockfd, data, DATA_LEN, 0);
    while (ret > 0) {
        memcpy(pNext, data, ret);
        pNext += ret;
        ret = recv(sockfd, data, DATA_LEN, 0);
    }

    free_socket(sockfd);
    return PGEC_SUCCESS;
}
#ifdef HTTP_GET_TEST
kint main()
{
    kchar *retbuf;
    kint retlen;

    kchar url[256];
    klog(("http://"));
    scanf("%s", url);
    http_get_data(url, &retbuf, &retlen);
    klog(("%s\n", retbuf));
    return 0;
}
#endif

/*
 * 1, timer to deal with timeout.
 * 2, a request is a message
 */

typedef struct _K_bdsg_rqst {
    sg_ses *ses;
    kchar *pObjectUrl;
    Iv_TransferObject_CALLBACK WebO_cbk;
    kchar *sSavePathName;
    kuint uiTimeout;
    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} K_bdsg_rqst;

static kint om_bdsg_data_timeout(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);
static kint om_bdsg_data_received(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn);

static kbool __g_bdsg_engine_inited = kfalse;
static kbool __g_bdsg_engine_quit = kfalse;
static kbean __g_bdsg_tsk = knil;

#define KMBDSG_DATA                         0x00000001

#define TYPE_BdsgGetBootStrap               0x00000001
#define TYPE_BdsgGetSgddList                0x00000002
#define TYPE_BdsgGetObject                  0x00000003
#define TYPE_BdsgGetImd                     0x00000004
#define TYPE_BdsgGetNotification            0x00000005
#define TYPE_BdsgGetImt                     0x00000005

kint sg_bdsg_new(kvoid)
{
    hg_init(4);
    __g_bdsg_tsk = ktsk_new("bdsg", knil, knil, knil, knil, knil);
    kmsg_slot_set(__g_bdsg_tsk, KMBDSG_DATA, om_bdsg_data_received);
    kmsg_slot_set(__g_bdsg_tsk, KMSG_TIMER, om_bdsg_data_timeout);
    return 0;
}

kint sg_bdsg_del(kvoid)
{
    __g_bdsg_engine_quit = ktrue;
    if (-1 != __g_cur_socket) {
        free_socket(__g_cur_socket);
    }
    ktsk_del(__g_bdsg_tsk);
    return 0;
}

static kint om_bdsg_data_timeout(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    kuint muid = (kuint)a_ar0;

    if (a_rsn || __g_bdsg_engine_quit) {
        return -1;
    }

    kmsg_kill(a_tsk, 1, muid);
    return 0;
}

static kint om_bdsg_data_received(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_bdsg_rqst *rqst = (K_bdsg_rqst*)a_ar0;
    kchar *retbuf = knil, *tmp;
    kint retlen = 0, error = 0, rc = 0;
    kuint objid = 0;

    SES_OBJECTLIST_CALLBACK objlst_callback = (SES_OBJECTLIST_CALLBACK)rqst->WebO_cbk;
    SES_TRANSFEROBJECT_CALLBACK tranobj_callback = (SES_TRANSFEROBJECT_CALLBACK)rqst->WebO_cbk;
    sg_ses *ses = (sg_ses*)rqst->ses;

    tmp = strstr(rqst->pObjectUrl, "&TOI=");
    if (tmp) {
        objid = strtoul(tmp + 5, 0, 10);
    }

    if (a_rsn || __g_bdsg_engine_quit || (rc = http_get_data(rqst->pObjectUrl, &retbuf, &retlen))) {
        error = -1;
    }
    if ((rc == PGEC_HOST) || (rc == PGEC_SOCKET) || (rc == PGEC_CONNECT) || (rc == PGEC_SEND) || (rc == PGEC_RECV)) {
        /* -2 stand for socket error */
        error = -2;
    }

    /* get the request type */
    if (!strstr(rqst->pObjectUrl, "?")) {
        /* BdsgGetBootStrap */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                rqst->pObjectUrl,       /* pObjectUrl */
                0,                      /* iObjectVersion */
                retbuf,                 /* pDataBuffer */
                retlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                knil,                   /* pContentEncoding */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        goto free_and_quit;
    }

    if (strstr(rqst->pObjectUrl, "Type=SGDD")) {
        /* BdsgGetSgddList */

        /* Parse the xml and chagne it into SGDD list */
        kint sgdd_cnt = 0, i;
        IV_OBJECT_INFO *oinfo = knil;
        KXmlDoc *doc = xmldoc_new(knil);
        KXmlNode *node;
        KXmlAttr *attr;

        xmldoc_parse(doc, retbuf, retlen);
        if (!(node = xmldoc_gotoNode(doc, "Files", 0))) {
            error = 1;
            goto callback;
        }

        /* get count of File element belong to Files element */
        sgdd_cnt = xmldoc_getNodeCnt(doc, "File");
        if (!sgdd_cnt) {
            error = 1;
            goto callback;
        }

        oinfo = kmem_alloz(sgdd_cnt * sizeof(IV_OBJECT_INFO));
        if (!oinfo) {
            error = 1;
            goto callback;
        }

        for (i = 0; i < sgdd_cnt; i++) {
            node = xmldoc_gotoNode(doc, "File", i);
            if (!node) {
                error = 1;
                goto callback;
            }

            /* Fill pObjectName */
            if (attr = xmlnode_getattr(node, "FName")) {
                oinfo[i].pObjectName = kstr_dup(attr->value);
            }

            /* Fill uiObjectID, iObjectVersion */
            if (attr = xmlnode_getattr(node, "TOI")) {
                kuint TOI = strtoul(attr->value, 0, 10);
                oinfo[i].uiObjectID = TOI; /* (TOI >> 16) & 0xFFFF; */
                oinfo[i].iObjectVersion = TOI & 0xFFFF;
            }

            xmldoc_gotoNode(doc, "..", 0);
        }
callback:
        objlst_callback(
                ses,
                0,                      /* iHandle */
                error,                  /* iStatus */
                knil,                   /* pObjectList_attibute */
                oinfo,                  /* pInfoLisHead */
                sgdd_cnt,               /* iObjectNum */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        xmldoc_del(doc);
        goto free_and_quit;
    }

    if (strstr(rqst->pObjectUrl, "Type=TOI")) {
        /* BdsgGetObject */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                rqst->pObjectUrl,       /* pObjectUrl */
                0,                      /* iObjectVersion */
                retbuf,                 /* pDataBuffer */
                retlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        goto free_and_quit;
    }

    if (strstr(rqst->pObjectUrl, "Type=IMD")) {
        /* BdsgGetImd */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                rqst->pObjectUrl,       /* pObjectUrl */
                0,                      /* iObjectVersion */
                retbuf,                 /* pDataBuffer */
                retlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        goto free_and_quit;
    }

    if (strstr(rqst->pObjectUrl, "Type=Msg")) {
        /* BdsgGetNotification */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                rqst->pObjectUrl,       /* pObjectUrl */
                0,                      /* iObjectVersion */
                retbuf,                 /* pDataBuffer */
                retlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        goto free_and_quit;
    }

    if (strstr(rqst->pObjectUrl, "Type=IMT")) {
        /* BdsgGetImt */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                rqst->pObjectUrl,       /* pObjectUrl */
                0,                      /* iObjectVersion */
                retbuf,                 /* pDataBuffer */
                retlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                rqst->vpUserData0,      /* vpUserData0 */
                rqst->vpUserData1,      /* vpUserData1 */
                rqst->vpUserData2,      /* vpUserData2 */
                rqst->vpUserData3       /* vpUserData3 */
                );
        goto free_and_quit;
    }

free_and_quit:
    kmem_free(rqst->pObjectUrl);
    kmem_free(rqst->sSavePathName);
    return 0;
}

/**
 * @brief Get data via http protocal.
 *
 * @param pObjectUrl URL, seem like "http://192.168.1.11:8888/ESG/Protal.ashx?TSID=29&Type=TOI&TOI=23434"
 * @param WebO_cbk <b>When Get SGDD List data, the callback must be Iv_ObjectList_CALLBACK, or
 * otherwise set Iv_TransferObject_CALLBACK.</b>
 * @param sSavePathName Must be knil
 * @param uiTimeout time out, in milliseconds
 * @param vpUserData0 User data
 * @param vpUserData1 User data
 * @param vpUserData2 User data
 * @param vpUserData3 User data
 *
 * @return IVEC_SUCCESS for success, IVEC_FAILURE for failure
 */
kint Http_GetWebObject_X(sg_ses *ses, kchar *pObjectUrl, kvoid *WebO_cbk, kchar *sSavePathName, kuint uiTimeout,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3)
{
    kuint muid;
    K_bdsg_rqst *rqst = kmem_alloz(sizeof(K_bdsg_rqst));
    if (!rqst) {
        return IVEC_FAILURE;
    }
    rqst->ses = ses;
    STR_SET(rqst->pObjectUrl, pObjectUrl);
    rqst->WebO_cbk = WebO_cbk;
    STR_SET(rqst->sSavePathName, sSavePathName);
    rqst->uiTimeout = uiTimeout;
    rqst->vpUserData0 = vpUserData0;
    rqst->vpUserData1 = vpUserData1;
    rqst->vpUserData2 = vpUserData2;
    rqst->vpUserData3 = vpUserData3;
    muid = kmsg_post(__g_bdsg_tsk, KMBDSG_DATA, rqst, 0, 0, 0);
    ktmr_set(__g_bdsg_tsk, uiTimeout, kfalse, knil, (kvoid*)muid, 0, 0, 0);
    return IVEC_SUCCESS;
}

