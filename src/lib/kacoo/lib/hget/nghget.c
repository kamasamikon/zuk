/* vim:set et sw=4 sts=4: */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ktypes.h>
#include <kdbg.h>
#include <kmem.h>
#include <ktmr.h>
#include <ksyn.h>
#include <kstr.h>
#include <ktsk.h>

#if (defined(__WIN32__) || defined(__WINCE__))
#include <winsock2.h>
#include <signal.h>
#include <errno.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#endif

#include <hget.h>

/** \def
 * \brief A request issued, all worker task wakekup to process it.
 */
#define KMSG_HG_WAKEUP          0x00000001

/**
 * \internal
 * \brief Maintain information which keep reuse the keep alive socket.
 */
typedef struct _hg_skt {
    SOCKET socket;              /**< the soket which keep alive for some time */
    kbean timer;                /**< free socket after 5 seconds */

    kint ref;                   /**< hack for cancel */

    /** received data */
    kchar *datbuf;
    kint datlen;
    kchar *hdrbuf;
    kint hdrlen;

    kchar host[1024];           /**< path is not needed, which only for certain request */
    kushort port;
} hg_skt;

/**
 * \internal
 * \brief For each Http_GetWebObject call, new an instance of this structure.
 */
typedef struct _hg_req {
    K_dlist_entry ent;          /**< queue to __g_req_hdr */

    kbean tsk;                  /**< set till it dead */
    kbool can;                  /**< cancelled? */

    kint ref;                   /**< hack for cancel */

    hg_skt *skt;                /**< which socket connect? */

    kchar host[1024];
    kchar path[1024];
    kushort port;

    /** session related */
    kvoid *ses;
    kchar *pObjectUrl;
    kvoid *WebO_cbk;
    kchar *sSavePathName;
    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} hg_req;

static kint __g_hg_tsk_cnt;                 /**< how many task run same time */
static kbean *__g_hg_tsk;                   /**< task array */
static hg_skt *__g_hg_skt;                  /**< socket according to each task */

static K_dlist_entry __g_req_hdr;           /**< queue for hg_req */
static kbean __g_lck;                       /**< proteck all, include queue */

static kint om_skt_tmr(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3);

static kint connect_and_receive(hg_skt *s, hg_req *r);
static kvoid reset_skt_timer(hg_skt *s);

static kint Http_WebObjectCallback(kbean a_hdl, kint a_error, kchar *a_datbuf, kint a_datlen);

kvoid free_hg_req(hg_req *r)
{
    kmem_free_s(r->pObjectUrl);
    kmem_free_s(r->sSavePathName);

    kmem_free(r);
}

/**
 * \internal
 * \brief Try to get a hg_req from __g_req_hdr and do right for it.
 *
 * The KMSG_HG_WAKEUP message will be post to every worker task, even only one request issue.
 * That makes all the task try to get that request can process it, so, one task can do it, all
 * the other's failed.
 *
 * It will loop all the request header and try to load the request has some host and port, it
 * will makes the request directly reuse an opened socket, that make speed up the socket
 * connection.
 */
static kint om_hg_wakeup(kbean a_tsk, kuint a_msg,
        kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    kvoid *tsk_ar0;
    hg_skt *s;

    kbool found = kfalse;
    K_dlist_entry *ent;
    hg_req *r;

    kint ret = -1, error = -1;

    ktsk_get_ars(a_tsk, &tsk_ar0, knil, knil, knil);
    s = (hg_skt*)tsk_ar0;

    ksyn_lck_get(__g_lck);

    ent = __g_req_hdr.next;
    while (ent != &__g_req_hdr) {
        r = FIELD_TO_STRUCTURE(ent, hg_req, ent);
        ent = ent->next;

        /* (not connect to a task.) and (not cancelled.) and (same host and port.) */
        if (!r->tsk && !r->can && !kstr_icmp(r->host, s->host, 0) && r->port == s->port) {
            found = ktrue;
            break;
        }
    }

    /* cannot reuse current socket, select the first idle */
    if (!found) {
        ent = __g_req_hdr.next;
        while (ent != &__g_req_hdr) {
            r = FIELD_TO_STRUCTURE(ent, hg_req, ent);
            ent = ent->next;

            if (!r->tsk && !r->can) {
                found = ktrue;
                break;
            }
        }
    }

    if (found) {
        r->tsk = a_tsk;         /* connect to worker task */
        r->skt = s;             /* connect to current socket */
        r->ref = ++s->ref;

        /* reset socket suisite timer */
        ktmr_kill(s->timer);
        s->timer = knil;        /* om_timer:: if s->timer = knil; return */
    }

    ksyn_lck_rel(__g_lck);

    if (!found) {
        klog(("no idle request found.\n"));
        return -1;
    }

    klog(("om_hg_wakeup::peek request and process ::host:<%s>, path:<%s>, port:<%d>\n", r->host, r->path, r->port));

    if (!r->can && !a_rsn) {
        ret = connect_and_receive(s, r);
    }

    klog(("om_hg_wakeup::connect_and_receive ret:%d\n", ret));
    klog(("om_hg_wakeup::host:<%s>, path:<%s>, port:<%d>\n", r->host, r->path, r->port));
    if (PGEC_SUCCESS == ret) {
        error = 0;
    }

    Http_WebObjectCallback(r, error, s->datbuf, s->datlen);
    klog(("om_hg_wakeup::Http_WebObjectCallback done\n"));

    /* Only dequeue when data done */
    ksyn_lck_get(__g_lck);
    remove_dlist_entry(&r->ent);
    ksyn_lck_rel(__g_lck);

    reset_skt_timer(s);

    free_hg_req(r);

    return 0;
}

/**
 * \internal
 * \brief Connect to the socket by \a s and \a r, then receive data from it
 * \return PGEC_XXX status.
 */
static kint connect_and_receive(hg_skt *s, hg_req *r)
{
    kchar *datbuf = knil, *hdrbuf = knil;
    kint datlen = -1, hdrlen = 0, ret = -1;

    if (-1 == s->socket || r->port != s->port || strcmp(r->host, s->host)) {
        /* renew the socket */
        hget_free_socket(s->socket);
        s->socket = -1;

        if (PGEC_SUCCESS != (ret = hget_connect(PROT_HTTP, "", "",
                        r->host, r->path, r->port, &s->socket))) {
            return PGEC_CONNECT;
        }
    }

    if (r->can) {
        return PGEC_ERROR;
    }

    ret = hget_recv(s->socket, r->host, r->path, "", kfalse, knil,
            &datbuf, &datlen, &hdrbuf, &hdrlen);
    if ((0 == r->can) && (ret == PGEC_SEND || ret == PGEC_RECV)) {
        /* try again */
        if (PGEC_SUCCESS != (ret = hget_connect(PROT_HTTP, "", "",
                        r->host, r->path, r->port, &s->socket))) {
            return PGEC_CONNECT;
        }
        ret = hget_recv(s->socket, r->host, r->path, "", kfalse, knil,
                &datbuf, &datlen, &hdrbuf, &hdrlen);
    }

    s->datbuf = datbuf;
    s->datlen = datlen;
    s->hdrbuf = hdrbuf;
    s->hdrlen = hdrlen;

    return ret;
}

static kvoid reset_skt_timer(hg_skt *s)
{
    s->timer = ktmr_set(knil, 5000, kfalse, om_skt_tmr,
            (kvoid*)s, (kvoid*)s->ref, knil, knil);
}

/**
 * \brief Create task and socket with count \a a_tskcnt.
 *
 * To speed up data receving, \a a_tskcnt tasks created same time, and
 * process the http request same time.
 *
 * \param a_tskcnt Task and socket count created all at once, it should >= 1.
 */
kint hg_init(kint a_tskcnt)
{
    kint i;
    kchar name[24];
    hg_skt *s;

    if (a_tskcnt < 1) {
        a_tskcnt = 1;
    }

    init_dlist_head(&__g_req_hdr);
    __g_lck = ksyn_lck_new();

    __g_hg_tsk_cnt = a_tskcnt;

    __g_hg_tsk = (kbean*)kmem_alloz(sizeof(kbean) * __g_hg_tsk_cnt);
    __g_hg_skt = (hg_skt*)kmem_alloz(sizeof(hg_skt) * __g_hg_tsk_cnt);

    for (i = 0; i < __g_hg_tsk_cnt; i++) {
        s = &__g_hg_skt[i];

        /* set invalid value */
        s->socket = -1;

        sprintf(name, "hg_tsk_%d", i);
        __g_hg_tsk[i] = ktsk_new(name, knil, 0, 0, s, knil, knil, knil);
        kmsg_slot_set(__g_hg_tsk[i], KMSG_HG_WAKEUP, om_hg_wakeup);
    }
    return 0;
}
kint hg_final()
{
    kint i;
    for (i = 0; i < __g_hg_tsk_cnt; i++) {
        ktmr_kill(__g_hg_skt[i].timer);
        hget_free_socket(__g_hg_skt[i].socket);

        ktsk_del(__g_hg_tsk[i]);
    }

    /* wait till all done */
    while (!is_dlist_empty(&__g_req_hdr)) {
        ktsk_sleep(50);
    }

    kmem_free(__g_hg_tsk);
    kmem_free(__g_hg_skt);

    ksyn_lck_del(__g_lck);

    return 0;
}

/**
 * \brief cancel the socket, only which match ref cancelled
 */
static kint hg_can_skt(kbean a_hdl, kint a_ref)
{
    hg_skt *s;

    ksyn_lck_get(__g_lck);
    s = (hg_skt*)a_hdl;

    if (s->ref == a_ref) {
        ktmr_kill(s->timer);
        hget_free_socket(s->socket);
    } else {
        kerror(("hg_can: cur_ref:%d, can_ref:%d\n", s->ref, a_ref));
    }

    ksyn_lck_rel(__g_lck);

    return 0;
}
/**
 * \brief Cancel a request, specific by \a a_hdl, only hg_req has ref equal
 * to \a a_ref be cancelled.
 *
 * If \a a_wait is set, this function should block till the request's
 * callback called and be destroyed.
 *
 * \param a_hdl A pointer to hg_req.
 * \param a_ref The hg_req's ref.
 * \param a_wait Wait till done.
 */
static kint hg_can_req(kbean a_hdl, kint a_ref, kbool a_wait)
{
    kbool found = kfalse;
    K_dlist_entry *ent;
    hg_req *r;

    ksyn_lck_get(__g_lck);
    ent = __g_req_hdr.next;
    while (ent != &__g_req_hdr) {
        r = FIELD_TO_STRUCTURE(ent, hg_req, ent);
        if ((kbean)r == a_hdl) {

            r->can = ktrue;
            if (!r->tsk) {
                /* om_hg_wakeup will not be called, so, call the r->cbk here */
                Http_WebObjectCallback(r, -1, knil, 0);
            } else if (a_ref == r->skt->ref) {
                /* already connect to a task, in this case, callback ONLY be called in it */
                hg_can_skt(r->skt, a_ref);
            }

            remove_dlist_entry(&r->ent);
            found = ktrue;
            break;
        }
        ent = ent->next;
    }
    ksyn_lck_rel(__g_lck);

    if (found && a_wait) {
        /* DODO */
        while (!is_dlist_empty(&r->ent)) {
            ktsk_sleep(20);
        }
    }

    return 0;
}

/**
 * \internal
 * \brief Socket idle too long, kill it.
 * \param a_ar0 a pointer
 */
static kint om_skt_tmr(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    return hg_can_skt((kbean)a_ar0, (kint)a_ar1);
}

/**
 * \brief Cancel the request by \a a_hdl returned by Http_GetWebObject.
 *
 * This return till the callback called.
 *
 * \sa Http_GetWebObject
 */
kint Http_CancelWebObject(kint a_hdl)
{
    hg_req *r = (hg_req*)a_hdl;
    hg_can_req(a_hdl, r->ref, ktrue);
    return 0;
}
/* new request, queue it, wakeup tasks */
//kint Http_GetWebObject(sg_ses *ses, kchar *pObjectUrl, kvoid *WebO_cbk, kchar *sSavePathName, kuint uiTimeout,
/**
 * \brief Get data from web, \a uiTimeout is not used, timeout is process outside.
 */
kint Http_GetWebObject(kvoid *ses, kchar *pObjectUrl, kvoid *WebO_cbk, kchar *sSavePathName, kuint uiTimeout,
        kvoid *vpUserData0, kvoid *vpUserData1, kvoid *vpUserData2, kvoid *vpUserData3)
{
    hg_req *r = (hg_req*)kmem_alloz(sizeof(hg_req));
    kchar user[32] = { 0 }, pass[32] = { 0 }, host[512] = { 0 }, path[1024] = { 0 };
    kuint prot, i;
    kushort port;

    klog(("Http_GetWebObject: %s\n", pObjectUrl));

    hget_parseurl(pObjectUrl, &prot, user, pass, host, path, &port);

    /* common part */
    init_dlist_head(&r->ent);
    strncpy(r->host, host, sizeof(r->host));
    strncpy(r->path, path, sizeof(r->path));
    r->port = port;

    /* request part */
    r->ses = ses;
    r->pObjectUrl = kstr_dup(pObjectUrl);
    r->WebO_cbk = WebO_cbk;
    r->sSavePathName = kstr_dup(sSavePathName);
    r->vpUserData0 = vpUserData0;
    r->vpUserData1 = vpUserData1;
    r->vpUserData2 = vpUserData2;
    r->vpUserData3 = vpUserData3;

    ksyn_lck_get(__g_lck);
    insert_dlist_tail_entry(&__g_req_hdr, &r->ent);

    for (i = 0; i < __g_hg_tsk_cnt; i++) {
        kmsg_post(__g_hg_tsk[i], KMSG_HG_WAKEUP, knil, knil, knil, knil);
    }
    ksyn_lck_rel(__g_lck);

    return (kint)r;
}

/**
 * \brief Call the callback routine specified by Http_GetWebObject, with error code and received data.
 *
 * \note All the request sent by Http_GetWebObject should be called this function, because maybe
 * some resource should be freed in the callback routine.
 *
 * \sa Http_GetWebObject
 */
static kint Http_WebObjectCallback(kbean a_hdl, kint a_error, kchar *a_datbuf, kint a_datlen)
{
    hg_req *r = (hg_req*)a_hdl;

    kchar *datbuf = a_datbuf, *tmp;
    kint datlen = a_datlen, error = a_error, rc = 0;
    kuint objid = 0;

    SES_OBJECTLIST_CALLBACK objlst_callback = (SES_OBJECTLIST_CALLBACK)r->WebO_cbk;
    SES_TRANSFEROBJECT_CALLBACK tranobj_callback = (SES_TRANSFEROBJECT_CALLBACK)r->WebO_cbk;
    sg_ses *ses = (sg_ses*)r->ses;

    /* XXX Hack! xml parse will go even when data is knil, so change datlen when error */
    if (error) {
        datbuf = knil;
        datlen = 0;
    }

    tmp = strstr(r->pObjectUrl, "&TOI=");
    if (tmp) {
        objid = strtoul(tmp + 5, 0, 10);
    }

    /* get the request type */
    if (!strstr(r->pObjectUrl, "?")) {
        /* BdsgGetBootStrap */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                r->pObjectUrl,          /* pObjectUrl */
                0,                      /* iObjectVersion */
                datbuf,                 /* pDataBuffer */
                datlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                knil,                   /* pContentEncoding */
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        goto quit;
    }

    if (strstr(r->pObjectUrl, "Type=SGDD")) {
        /* BdsgGetSgddList */

        /* Parse the xml and chagne it into SGDD list */
        kint sgdd_cnt = 0, i;
        IV_OBJECT_INFO *oinfo = knil;
        KXmlDoc *doc = xmldoc_new(knil);
        KXmlNode *node;
        KXmlAttr *attr;

        xmldoc_parse(doc, datbuf, datlen);
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
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        xmldoc_del(doc);
        goto quit;
    }

    if (strstr(r->pObjectUrl, "Type=TOI")) {
        /* BdsgGetObject */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                r->pObjectUrl,          /* pObjectUrl */
                0,                      /* iObjectVersion */
                datbuf,                 /* pDataBuffer */
                datlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        goto quit;
    }

    if (strstr(r->pObjectUrl, "Type=IMD")) {
        /* BdsgGetImd */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                r->pObjectUrl,          /* pObjectUrl */
                0,                      /* iObjectVersion */
                datbuf,                 /* pDataBuffer */
                datlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        goto quit;
    }

    if (strstr(r->pObjectUrl, "Type=Msg")) {
        /* BdsgGetNotification */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                r->pObjectUrl,          /* pObjectUrl */
                0,                      /* iObjectVersion */
                datbuf,                 /* pDataBuffer */
                datlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        goto quit;
    }

    if (strstr(r->pObjectUrl, "Type=IMT")) {
        /* BdsgGetImt */

        tranobj_callback(
                ses,
                0,                      /* iHandle: unknown */
                error,                  /* iStatus */
                objid,                  /* uiObjectID */
                knil,                   /* pObjectName */
                r->pObjectUrl,          /* pObjectUrl */
                0,                      /* iObjectVersion */
                datbuf,                 /* pDataBuffer */
                datlen,                 /* uiObjectLength */
                knil,                   /* pSavePathName */
                "gzip",                 /* pContentEncoding */
                r->vpUserData0,         /* vpUserData0 */
                r->vpUserData1,         /* vpUserData1 */
                r->vpUserData2,         /* vpUserData2 */
                r->vpUserData3          /* vpUserData3 */
                );
        goto quit;
    }

quit:

    return 0;
}

