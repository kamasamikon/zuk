/* vim:set et sw=4 sts=4: */
#include <string.h>

#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include <kstr.h>

#include "xmldoc.h"

#include "sg_imd.h"
#include "sg_ser.h"

#include "s2n_macro.h"
#include "x2s_macro.h"

#include "sg_x2s_fun.h"

#include "ivisa_sg_config.h"

extern char __dir_delimeter;

kint sg_imd_new(sg_imd **imd)
{
    if (!(*imd)) {
        (*imd) = (sg_imd*)kmem_alloc(sizeof(sg_imd));
    }
    if ((*imd)) {
        memset((*imd), 0, sizeof(sg_imd));
        INIT_COMMON((*imd));
        (*imd)->cls = CLS_IMDA;
        return 0;
    }
    return -1;
}

kint sg_imd_del(sg_imd *imd, kbool rmcache)
{
    if (!imd) {
        return -1;
    }
    remove_dlist_entry(&imd->all_le);
    remove_dlist_entry(&imd->cls_le);

    kassert(CLS_IMDA == imd->cls);

    if (rmcache) {
        sg_root_rm_cache_file(imd);
    }

    kmem_free(imd->attr.id);
    S2N_FUN(InteractivityMediaDocument, imd);
    //kmem_free(imd);
    return 0;
}

kint sg_imd_add_cache(sg_imd *imd)
{
    // save the content of the file to disk etc
    if (kflg_chk(imd->flg, EFF_LOADING)) {
        return 0;
    }
    return 0;
}
kint sg_imd_del_cache(sg_imd *imd)
{
    // save the content of the file to disk etc
    return 0;
}
kint sg_imd_ld_cache(sg_imd *imd)
{
    // save the content of the file to disk etc
    kflg_clr(imd->flg, EFF_LOADING);
    return 0;
}
kint sg_imd_x2s(sg_imd *imd)
{
    kuint curtime = ksys_ntp_time() + imd->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!imd) {
        kerror(("No imd input...\n"));
        return -1;
    }
    if ((!imd->dat.buf) || (0 == imd->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_imd_add_cache(imd);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, imd->dat.buf, imd->dat.len);

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "InteractivityMediaDocument", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    imd->ecnt = 1;
    imd->arr = kmem_alloz(sizeof(InteractivityMediaDocument_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (imd->attr.id) {
            kmem_free(imd->attr.id);
        }
        imd->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        imd->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        imd->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        imd->attr.validTo = GETVAL_kuint(attr->value);
    }
    if ((imd->attr.validTo) && (imd->attr.validTo <= curtime)) {
        xmldoc_del(doc);
        return -2;
    }

    if (attr = xmlnode_getattr(node, "groupID")) {
        imd->arr->attr.groupID = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "groupPosition")) {
        imd->arr->attr.groupPosition = GETVAL_kushort(attr->value);
    }

    imd->ecnt = 1;
    imd->arr->MediaObjectGroup = x2s_fill_MediaObjectGroup(doc);

    kflg_set(imd->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}

#if 0
typedef struct _INTERACTIVITY_MEDIA_
{
    kuchar          type;                       //互动的类型, IV_INTERACTIVITY_TYPE
    kchar           *description;               //互动描述
    kuchar          relativePreference;         //当同时有多个互动时,各互动的显示顺序,按递增顺序显示,即值小的先显示

    union {
        IV_ITV_SMS                *sms;
        IV_ITV_EMAIL           email;
        kchar               phoneNumber[20];
        kchar               *webLink;           //网页连接互动时,网页所在的URL,应该通过该连接取得互动网页
        IV_ITV_MEDIAOBJECTS    *mediaObjects;
    } contents;

} IV_INTERACTIVITY_MEDIA;
#endif

kint sg_imd_free_idir(IV_INTERACTIVITY_DIRECTORY *idir)
{
    kint i;
    kint j;
    IV_ITV_SMS                *sms;
    kchar                    *webLink;           //网页连接互动时,网页所在的URL,应该通过该连接取得互动网页
    IV_ITV_MEDIAOBJECTS        *mediaObjects;
    IV_ITV_EMAIL            *email;

    if (!idir) {
        return -1;
    }

    for (i = 0; i < idir->mediaNumber; i++) {
        kmem_free((idir->interactivityMedia[i]).description);

        switch ((idir->interactivityMedia[i]).type) {

            case IV_INTERACTIVITY_TYPE_SMS:
                sms = (IV_ITV_SMS*)(idir->interactivityMedia[i]).contents.sms;
                if(!sms)
                    break;

                kmem_free(sms->text);

                for(j = 0;j < sms->choiceNum; j++){
                    kmem_free((sms->choices[j]).choiceText);
                    kmem_free((sms->choices[j]).smsURI);
                }
                kmem_free(sms);
                break;

            case IV_INTERACTIVITY_TYPE_EMAIL:
                email = (IV_ITV_EMAIL*)(&((idir->interactivityMedia[i]).contents.email));
                if(!email)
                    break;
                kmem_free(email->bcc);
                kmem_free(email->body);
                kmem_free(email->cc);
                kmem_free(email->subject);
                kmem_free(email->to);
                break;


            case IV_INTERACTIVITY_TYPE_VOICECALL:
                break;

            case IV_INTERACTIVITY_TYPE_WEBLINK:
                webLink = (kchar*)(idir->interactivityMedia[i]).contents.webLink;
                kmem_free(webLink);
                break;

            case IV_INTERACTIVITY_TYPE_MEDIAOBJ:
                mediaObjects = (IV_ITV_MEDIAOBJECTS*)(idir->interactivityMedia[i]).contents.mediaObjects;
                if(!mediaObjects)
                    break;

                for(j = 0;j < mediaObjects->moNum; j++){
                    kvfs_remove((mediaObjects->objects[j]).contentLocation);
                    kmem_free((mediaObjects->objects[j]).contentLocation);
                    kmem_free((mediaObjects->objects[j]).contentType);
                }
                kmem_free(mediaObjects);
                break;

            default:
                kerror(("sg_img_free_idir:unknown mediatype:%d\n", idir[i].interactivityMedia->type));
        }
    }
    kmem_free(idir);
    return 0;
}

kchar* sg_imd_f2buf(kchar* aFile, klong* fileLen)
{
    kbean fp;
    kchar *buf = knil;
    kint read_bytes = 0x7FFFFFFF;

    if((fp = kvfs_open(aFile, "rb", 0)) == 0) {
        klog(("Error: unable to open tx_file %s\n", aFile));
        return knil;
    }
    kvfs_read(fp, &buf, &read_bytes);
    *fileLen = read_bytes;
    kvfs_close(fp);
    return buf;
}

kint sg_imd_copy_idir(IV_INTERACTIVITY_DIRECTORY **dst_idir, IV_INTERACTIVITY_DIRECTORY *src_idir,kchar *dst_directory, kchar *src_directory)
{
    K_dlist_entry *entry, *hdr;
    sg_ser *ser;
    sg_imd *imd;
    kint i, j;
    IV_INTERACTIVITY_DIRECTORY *cur_idir = src_idir, *ret_idir;
    IV_INTERACTIVITY_MEDIA *cur_im, *ret_im;
    IV_ITV_SMS *cur_sms, *ret_sms;
    IV_SMS_CHOICE *cur_choice;
    IV_ITV_MEDIAOBJECTS *cur_mo, *ret_mo;

    kchar tempStr[IV_PATH_MAX_SIZE];
    kchar* p1 = knil;
    kchar* p2 = knil;
    kint len = 0;
    kint root_str_len = strlen(src_directory);
    kint swap_str_len = strlen(dst_directory);

    kbean fp_des = 0;
    kint fileLen = 0;
    kchar* buf = knil;
    kint content_location_len = 0;

    if (cur_idir) {
        kint struct_len = sizeof(IV_INTERACTIVITY_DIRECTORY) + (cur_idir->mediaNumber - 1) * sizeof(IV_INTERACTIVITY_MEDIA);
        ret_idir = kmem_alloc(struct_len);
        if (ret_idir) {
            memcpy(ret_idir, cur_idir, struct_len);
            for (i = 0; i < cur_idir->mediaNumber; i++) {
                cur_im = &cur_idir->interactivityMedia[i];
                ret_im = &ret_idir->interactivityMedia[i];

                ret_im->description = kstr_dup(cur_im->description);
                switch (cur_im->type) {
                    case IV_INTERACTIVITY_TYPE_SMS:
                        cur_sms = cur_im->contents.sms;

                        struct_len = sizeof(IV_ITV_SMS) + (cur_sms->choiceNum - 1) * sizeof(IV_SMS_CHOICE);
                        ret_sms = kmem_alloc(struct_len);
                        memcpy(ret_sms, cur_sms, struct_len);

                        ret_sms->text = kstr_dup(cur_sms->text);

                        for (j = 0; j < cur_sms->choiceNum; j++) {
                            ret_sms->choices[j].choiceText = kstr_dup(cur_sms->choices[j].choiceText);
                            ret_sms->choices[j].smsURI = kstr_dup(cur_sms->choices[j].smsURI);
                        }
                        ret_im->contents.sms = ret_sms;
                        break;

                    case IV_INTERACTIVITY_TYPE_EMAIL:
                        ret_im->contents.email.bcc = kstr_dup(cur_im->contents.email.bcc);
                        ret_im->contents.email.body = kstr_dup(cur_im->contents.email.body);
                        ret_im->contents.email.cc = kstr_dup(cur_im->contents.email.cc);
                        ret_im->contents.email.subject = kstr_dup(cur_im->contents.email.subject);
                        ret_im->contents.email.to = kstr_dup(cur_im->contents.email.to);
                        break;

                    case IV_INTERACTIVITY_TYPE_VOICECALL:
                        strcpy(ret_im->contents.phoneNumber,cur_im->contents.phoneNumber);
                        break;

                    case IV_INTERACTIVITY_TYPE_WEBLINK:
                        ret_im->contents.webLink = kstr_dup(cur_im->contents.webLink);
                        break;

                    case IV_INTERACTIVITY_TYPE_MEDIAOBJ:
                        cur_mo = cur_im->contents.mediaObjects;

                        struct_len = sizeof(IV_ITV_MEDIAOBJECTS) + (cur_mo->moNum - 1) * sizeof(IV_MEDIA_OBJECT);
                        ret_mo = kmem_alloc(struct_len);
                        memcpy(ret_mo, cur_mo, struct_len);

                        for (j = 0; j < cur_mo->moNum; j++) {

                            if(0 == strncmp(src_directory,cur_mo->objects[j].contentLocation, root_str_len)){
                                content_location_len = strlen(cur_mo->objects[j].contentLocation) - root_str_len + swap_str_len + 1;

                                ret_mo->objects[j].contentLocation = kmem_alloc(content_location_len);
                                memcpy(ret_mo->objects[j].contentLocation, dst_directory, swap_str_len);
                                ret_mo->objects[j].contentLocation[swap_str_len] = '\0';

                                p1 = cur_mo->objects[j].contentLocation + root_str_len;
                                strcat(ret_mo->objects[j].contentLocation, p1);
                            }
                            else{
                                /*content_location_len = strlen(cur_mo->objects[j].contentLocation) + swap_str_len + 1;
                                  ret_mo->objects[j].contentLocation = ksal_mem_alloc(content_location_len);
                                  memcpy(ret_mo->objects[j].contentLocation, dst_directory, swap_str_len);

                                  ret_mo->objects[j].contentLocation[swap_str_len] = '\0';
                                  strcat(ret_mo->objects[j].contentLocation, cur_mo->objects[j].contentLocation);*/
                                return IVEC_FAILURE;

                            }

                            // create directory and file
                            p1 = strrchr(ret_mo->objects[j].contentLocation, __dir_delimeter);
                            len = p1 - ret_mo->objects[j].contentLocation;
                            strncpy(tempStr, ret_mo->objects[j].contentLocation, len);
                            tempStr[len] = '\0';

                            kvfs_mkdir(tempStr);
                            fileLen = 0;

                            buf = sg_imd_f2buf(cur_mo->objects[j].contentLocation, (klong*)&fileLen);

                            if(buf){

                                fp_des = kvfs_open(ret_mo->objects[j].contentLocation, "wb+", 0);
                                if(fp_des){
                                    if((kint)kvfs_write(fp_des, buf, (kuint)fileLen) != fileLen) {
                                        klog(("Error: fwrite failed\n"));
                                    }
                                    kvfs_close(fp_des);
                                    kmem_free(buf);

                                    ret_mo->objects[j].contentType = kstr_dup(cur_mo->objects[j].contentType);
                                }
                                else{ //open target file to write failure
                                    kmem_free(buf);
                                    kmem_free(ret_mo->objects[j].contentLocation);
                                    ret_mo->objects[j].contentLocation = knil;
                                    ret_mo->objects[j].contentType = knil;
                                    ret_mo->moNum -= 1;
                                    kmem_free(ret_im->description );
                                    ret_im->description  = knil;
                                }
                            }
                            else{//if file doesn't exist or read error
                                kmem_free(ret_mo->objects[j].contentLocation);
                                ret_mo->objects[j].contentLocation = knil;
                                ret_mo->objects[j].contentType = knil;
                                ret_mo->moNum -= 1;
                                kmem_free(ret_im->description );
                                ret_im->description  = knil;
                            }
                        }
                        if(!ret_mo->moNum){
                            klog(("ret_mo has not MO, decrease ret_idir->mediaNumber, cur:%d\n", ret_idir->mediaNumber));
                            kmem_free(ret_mo);
                            ret_mo = knil;
                            ret_idir->mediaNumber -= 1;
                        }
                        ret_im->contents.mediaObjects = ret_mo;
                        break;

                    default:
                        break;
                }
            }
            *dst_idir = ret_idir;
            return IVEC_SUCCESS;
        }
    }
    return IVEC_FAILURE;
}
