#include <string.h>
#include <stdio.h>
#include "ksupport.h"
#include <kflg.h>
#include <kdbg.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kmem.h>

#include "sg_root.h"
#include "sg_sdp.h"

kint sg_sdp_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    sg_sdp *sdp = knil;
    sg_zbd *zbd;
    if (!sg_root_chk_cache_rec(root, rec)) {
        return -1;
    }
    if (zbd = sg_root_find_file(root, SDP, &rec->tid, knil)) {
        if (zbd->attr.version == rec->attr.version) {
            klog(("same frag exist: %d\n", rec->tid));
            return 0;
        }
        sg_delete_file(zbd, kfalse);
    }

    sg_sdp_new(&sdp);
    sdp->rt = root;
    sg_root_rec_to_zbd(root, rec, (sg_zbd*)sdp);
    kflg_set(sdp->flg, EFF_LOADING);

    if (-2 == sg_sdp_x2s(sdp)) {
        FREE_ORG_DAT(sdp);
        sg_sdp_del(sdp, ktrue);
        return -1;
    }
    FREE_ORG_DAT(sdp);

    insert_dlist_tail_entry(&root->allhdr, &sdp->all_le);
    insert_dlist_tail_entry(&fblk->fhdrs[SDP], &sdp->cls_le);
    kflg_clr(sdp->flg, EFF_LOADING);
    return 0;
}
kint sg_sdp_new(sg_sdp **sdp)
{
    if (!(*sdp)) {
        (*sdp) = (sg_sdp*)kmem_alloc(sizeof(sg_sdp));
    }
    if ((*sdp)) {
        memset((*sdp), 0, sizeof(sg_sdp));
        INIT_COMMON((*sdp));
        (*sdp)->cls = CLS_SDPF;
        return 0;
    }
    return -1;
}

kint sg_sdp_del(sg_sdp *sdp, kbool rmcache)
{
    if (!sdp) {
        return -1;
    }
    remove_dlist_entry(&sdp->all_le);
    remove_dlist_entry(&sdp->cls_le);

    kassert(CLS_SDPF == sdp->cls);

    if (rmcache) {
        sg_root_rm_cache_file(sdp);
    }

    DEL_COMMON(sdp);
    return 0;
}

kint sg_sdp_add_cache(sg_sdp *sdp)
{
    // save the content of the file to disk etc
    if (kflg_chk(sdp->flg, EFF_LOADING)) {
        return 0;
    }
    sg_root_save_cache_file(sdp);
    return 0;
}
kint sg_sdp_del_cache(sg_sdp *sdp)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_sdp_ld_cache(sg_sdp *sdp)
{
    // save the content of the file to disk etc
    kflg_clr(sdp->flg, EFF_LOADING);
    return 0;
}

kint sg_sdp_getval(sg_sdp *sdp, kchar *type, kchar *value, kint index)
{
    kchar *buffer = sdp->dat.buf;
    kint buflen = sdp->dat.len;

    if (buffer && buflen && type && value) {

        kint pos = 0, curindex = 0, idx, ofs;
        kchar *ret;
        kchar tag[128], ch;

        strcpy(tag, type);
        strcat(tag, "=");

        ofs = strlen(tag);

        while ((ret = strstr(buffer + pos, tag))) {

            if (ret >= buffer + buflen) {
                // reach the end of the buffer
                break;
            }

            // tag must be the first of a line
            if (ret[-1] != '\r'|| ret[-1] != '\n') {
                if (pos) {
                    pos = ret - buffer + 1;
                    continue;
                }
            }

            if (curindex == index) {
                for (idx = 0; 1; idx++) {
                    ch = (ret + ofs)[idx];
                    if (ch == '\r'|| ch == '\n'|| ch == '\0') {
                        value[idx] = '\0';
                        return 0;
                    }
                    value[idx] = ch;
                }
            }
            pos = ret - buffer + 1;
            curindex++;
        }
        return -1;
    }
    kassert("buffer not loaded");
    return -1;
}

kint sdp_getval(kchar *buf, kuint len, kchar *type, kchar *value, kint index)
{
    kchar *buffer = buf;
    kint buflen = len;

    if (buffer && buflen && type && value) {

        kint pos = 0, curindex = 0, idx, ofs;
        kchar *ret;
        kchar tag[128], ch;

        strcpy(tag, type);
        strcat(tag, "=");

        ofs = strlen(tag);

        while ((ret = strstr(buffer + pos, tag))) {

            if (ret >= buffer + buflen) {
                // reach the end of the buffer
                break;
            }

            // tag must be the first of a line
            if ((ret[-1] != '\r') && (ret[-1] != '\n')) {
                if (pos) {
                    pos = ret - buffer + 1;
                    continue;
                }
            }

            if (curindex == index) {
                for (idx = 0; 1; idx++) {
                    ch = (ret + ofs)[idx];
                    if (ch == '\r'|| ch == '\n'|| ch == '\0') {
                        value[idx] = '\0';
                        return 0;
                    }
                    value[idx] = ch;
                }
            }
            pos = ret - buffer + 1;
            curindex++;
        }
        return -1;
    }
    kassert("buffer not loaded");
    return -1;
}


#if 0
int xmain(void)
{
    char *buffer = "\
                    v=0\n\
                    o=IPDC 15548 3378133808 IN IP4 127.0.0.1\n\
                    s=Video: H.264 350kbps QVGA, Audio: AAC 64 stereo\n\
                    c=IN IP6 FF15:0:0:0:0:0:1:0\n\
                    t=3378049200 3378132000\n\
                    a=Timeout:integer;30\n\
                    a=NetworkDelay:integer;2000\n\
                    a=BurstInterval:integer;500\n\
                    m=audio 10002 RTP/AVP 97\n\
                    c=IN IP6 FF15:0:0:0:0:0:1:0\n\
                    b=AS:64\n\
                    a=control:streamid=1\n\
                    a=StreamId:integer;1\n\
                    a=rtpmap:97 MP4A-LATM/32000/2\n\
                    a=fmtp:97 profile-level-id=15; object=2; cpresent=0; config=400025203FC0\n\
                    m=video 10000 RTP/AVP 96\n\
                    c=IN IP6 FF15:0:0:0:0:0:1:0\n\
                    b=AS:350\n\
                    a=control:streamid=0\n\
                    a=StreamId:integer;0\n\
                    a=rtpmap:96 H264/90000\n\
                    a=fmtp:96 profile-level-id=42E00C; sprop-parameter-sets=Z0LgDJZUCg/I,aM48gA==; packetization-mode=1";
    int ret;
    char val[2560], type[64];

    strcpy(type, "a");
    ret = sdpGetValue(buffer, 100000, type, (char*)val, 0);
    klog(("ret:%x\n%s=%s\n\n", ret, type, val));

    strcpy(type, "a");
    ret = sdpGetValue(buffer, 100000, type, (char*)val, 2);
    klog(("ret:%x\n%s=%s\n\n", ret, type, val));

    strcpy(type, "m");
    ret = sdpGetValue(buffer, 100000, type, (char*)val, 8);
    klog(("ret:%x\n%s=%s\n\n", ret, type, val));

    return 0;
}

#endif

#if 0

v= (protocol version)
    // v=0

    o= (owner/creator and session identifier).
    // o=<username> <session id> <version> <network type> <address type> <address>
s= (session name)
    // s=<session name>

i=* (session information)
    //  i=<session description>

u=* (URI of description)
    // u=<URI>

e=* (email address)
    // e=<email address>
p=* (phone number)
    // p=<phone number>

c=* (connection information - not required if included in all media)
    // c=<network type> <address type> <connection address>
b=* (bandwidth information)
    // b=<modifier>:<bandwidth-value>
    One or more time descriptions (see below)
z=* (time zone adjustments)
    // z=<adjustment time> <offset> <adjustment time> <offset> ....

k=* (encryption key)
    // k=<method>
    // k=<method>:<encryption key>

a=* (zero or more session attribute lines)
    // a=<attribute>
    // a=<attribute>:<value>

    a=cat:<category>
    a=keywds:<keywords>
    a=tool:<name and version of tool>
    a=ptime:<packet time>
    a=recvonly
    a=sendrecv
    a=sendonly
    a=orient:<whiteboard orientation>
    a=type:<conference type>
    a=charset:<character set>
    a=sdplang:<language tag>
    a=lang:<language tag>
    a=framerate:<frame rate>
    a=quality:<quality>
    a=fmtp:<format> <format specific parameters>

t= (time the session is active)
    // t=<start time>  <stop time>
r=* (zero or more repeat times)
    // r=<repeat interval> <active duration> <list of offsets from start- time>

m= (media name and transport address)
    // m=<media> <port> <transport> <fmt list>

#endif

