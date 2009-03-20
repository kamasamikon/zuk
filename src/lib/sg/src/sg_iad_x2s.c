/* vim:set et sw=4 sts=4: */
#include <string.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include <ktsk.h>
#include "ksupport.h"
#include <kflg.h>
#include <kstr.h>

#include "xmldoc.h"
#include "sg_mgr2.h"
#include "sg_root.h"
#include <ktmr.h>
#include "sg_com.h"

#include "base64.h"
#include "ipaddr.h"

#include "sg_ses.h"
#include "sg_ser.h"
#include "sg_iad.h"
#include "sg_sch.h"
#include "sg_acc.h"
#include "sg_sdp.h"
#include "sg_imd.h"
#include "sg_x2s_fun.h"

#include "zlib.h"

#include "ivisa_sg_user_api.h"
#include "ivisa_sg_config.h"

extern char __dir_delimeter;
extern kchar __bdsg_base_url[256];

kint sg_iad_x2s(sg_iad *iad)
{
    kuint curtime = ksys_ntp_time() + iad->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!iad) {
        kerror(("No iad input...\n"));
        return -1;
    }
    if ((!iad->dat.buf) || (0 == iad->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_iad_add_cache(iad);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, iad->dat.buf, iad->dat.len);
    iad->ecnt = 1;

    // walk the XML and structure
    if (!(node = xmldoc_gotoNode(doc, "InteractivityData", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    iad->arr = kmem_alloz(sizeof(InteractivityData_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (iad->attr.id) {
            kmem_free(iad->attr.id);
        }
        iad->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        iad->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        iad->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        iad->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (iad->attr.validTo && iad->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "preListenIndicator")) {
        iad->arr->attr.preListenIndicator = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "interactivityMediaDocumentPointer")) {
        iad->arr->attr.interactivityMediaDocumentPointer = GETVAL_kpchar(attr->value);
    }

    iad->ecnt = 1;
    iad->arr->InteractivityType = x2s_fill_InteractivityType(doc);
    iad->arr->ServiceReference = x2s_fill_ServiceReference(doc);
    iad->arr->ContentReference = x2s_fill_ContentReference(doc);
    iad->arr->ScheduleReference = x2s_fill_ScheduleReference(doc);
    iad->arr->InteractivityWindow = x2s_fill_InteractivityWindow(doc);
    iad->arr->InteractiveDelivery = x2s_fill_InteractiveDelivery(doc);
    iad->arr->Extension = x2s_fill_Extension(doc);
    iad->arr->BackOffTiming = x2s_fill_BackOffTiming(doc);
    iad->arr->TermsOfUse = x2s_fill_TermsOfUse(doc);
    iad->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(iad->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}

typedef struct _K_MOS_FILE {
    sg_ses *ses;
    kint iHandle;
    kint iStatus;
    kuint uiObjectID;
    kchar*pObjectName;
    kchar *pObjectUrl;
    kint iObjectVersion;
    kuchar *pDataBuffer;
    kuint uiObjectLength;
    kchar *pSavePathName;
    kchar *pContentEncoding;

    void *vpUserData0;
    void *vpUserData1;
    void *vpUserData2;
    void *vpUserData3;
} K_MOS_FILE;


#define ICF_IMD_PARSE_DONE        0x00000001
typedef struct _sg_imd_common
{
    kuint flg;
    kuint uncomplete_file_cnt;
    IV_INTERACTIVITY_DIRECTORY *idir;
    kchar *serviceID;
    sg_root *rt;
} sg_imd_common;

static kint mos_file_callback(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    K_MOS_FILE mos;
    sg_root *rt = (sg_root*)vpUserData0;

    mos.ses = ses;
    mos.iHandle = iHandle;
    mos.iStatus = iStatus;
    mos.uiObjectID = uiObjectID;
    mos.pObjectName = pObjectName;
    mos.pObjectUrl = pObjectUrl;
    mos.iObjectVersion = iObjectVersion;
    mos.pDataBuffer = pDataBuffer;
    mos.uiObjectLength = uiObjectLength;
    mos.pSavePathName = pSavePathName;
    mos.pContentEncoding = pContentEncoding;

    mos.vpUserData0 = vpUserData0;
    mos.vpUserData1 = vpUserData1;
    mos.vpUserData2 = vpUserData2;
    mos.vpUserData3 = vpUserData3;

    return kmsg_send(rt->worker_thread, KRTM_MOS_FILE, &mos, 0, 0, 0);
}

kint om_mos_file(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MOS_FILE *mos = (K_MOS_FILE*)a_ar0;

    sg_ses *ses = mos->ses;
    kint iHandle = mos->iHandle;
    kint iStatus = mos->iStatus;
    kuint uiObjectID = mos->uiObjectID;
    kchar*pObjectName = mos->pObjectName;
    kchar *pObjectUrl = mos->pObjectUrl;
    kint iObjectVersion = mos->iObjectVersion;
    kuchar *pDataBuffer = mos->pDataBuffer;
    kuint uiObjectLength = mos->uiObjectLength;
    kchar *pSavePathName = mos->pSavePathName;
    kchar *pContentEncoding = mos->pContentEncoding;

    void *vpUserData0 = mos->vpUserData0;
    void *vpUserData1 = mos->vpUserData1;
    void *vpUserData2 = mos->vpUserData2;
    void *vpUserData3 = mos->vpUserData3;

    sg_imd_common *imd_com = (sg_imd_common*)vpUserData1;
    IV_ITV_MEDIAOBJECTS *itv_mo = (IV_ITV_MEDIAOBJECTS*)vpUserData2;
    kchar *contentType = (kchar*)vpUserData3;
    sg_ser *ser;

    kuchar *gunzip_buf = pDataBuffer, *gunzip_buf_end = pDataBuffer + uiObjectLength, *gunzip_next_cptr;
    kchar itv_ser_path[IV_PATH_MAX_SIZE];
    kchar tmp[IV_PATH_MAX_SIZE];

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    klog(("MOS received, Name = %s\n",pObjectName));
    if (iStatus) {
        kfatal(("mos_file_callback:callback failed with status :%d\n", iStatus));
        sg_ses_try_end_session(ses);
        return -1;
    }
    ser = (sg_ser*)sg_root_find_file(imd_com->rt, SER, knil, imd_com->serviceID);
    if(!ser){
        if (--imd_com->uncomplete_file_cnt == 0 && kflg_chk(imd_com->flg, ICF_IMD_PARSE_DONE)) {
            kmem_free(imd_com->serviceID);
            kmem_free(imd_com);
        }
        kmem_free(contentType);
        sg_ses_try_end_session(ses);
        return -1;
    }

    //create the dir for interactivity
    sprintf(itv_ser_path,"%s%c%08x%c%s%c%d",imd_com->rt->mgr->env->data_dir,
            __dir_delimeter,ser->rt->pvdr->id,
            __dir_delimeter,ITV_DIR_NAME,
            __dir_delimeter,ser->tid);
    kvfs_mkdir(itv_ser_path);


    if (streq(contentType, "application/x-gzip")) {

        kint ret, mem_inc_step = 0, index = 0, init_buffer_length = 4096;
        kuchar *uncompr;
        kulong uncomprLen;


try_with_more_mem:
        uncomprLen = init_buffer_length + mem_inc_step;
        kassert(uncomprLen);
        uncompr = kmem_alloc(uncomprLen);
        kassert(uncompr);

process_next_file_with_same_buf:
        ret = zei_uncompress(uncompr, &uncomprLen, gunzip_buf, gunzip_buf_end - gunzip_buf, &gunzip_next_cptr);
        if (0 == ret) {

            klog(("mos_file : %s\n", itv_mo->objects[index].contentLocation));

            // Set the save path
            sprintf(tmp,"%s%c%s",itv_ser_path,__dir_delimeter,itv_mo->objects[index].contentLocation);
            STR_SET(itv_mo->objects[index].contentLocation, tmp);

            ksal_file_save(tmp, (kchar*)uncompr, (kint)uncomprLen);

            index++;
            if (index < itv_mo->moNum) {
                // not all the file is unzipped

                // reset the input buffer length
                uncomprLen = init_buffer_length + mem_inc_step;
                gunzip_buf = gunzip_next_cptr;

                goto process_next_file_with_same_buf;
            }
        } else if (-5 == ret) {
            kmem_free(uncompr);
            mem_inc_step += 2048;
            goto try_with_more_mem;
        } else {
            kerror(("mos_file_callback: unzip %d file (%s) failed, maybe end of the zip\n"));
        }

        // free the memory
        kmem_free(uncompr);
    } else {
        STR_SET(itv_mo->objects->contentLocation, kspt_get_next_file_name());
        ksal_file_save(itv_mo->objects->contentLocation, (kchar*)pDataBuffer, uiObjectLength);
    }

    if (--imd_com->uncomplete_file_cnt == 0 && kflg_chk(imd_com->flg, ICF_IMD_PARSE_DONE)) {

        if (ser->cur_idir) {                        //if the new one is different with the new one, release the old itv
            if (!streq(itv_mo->objects->contentLocation,
                        ((IV_INTERACTIVITY_DIRECTORY*)(ser->cur_idir))->interactivityMedia->contents.mediaObjects->objects->contentLocation)) {
            }
            sg_imd_free_idir(ser->cur_idir);
        }
        ser->cur_idir = imd_com->idir;            //save the current itv
        // save idir in service then call the notify callback
        if (ser->rt->curser && streq(imd_com->serviceID, ser->rt->curser)) {
            klog(("ser:%x, ser->cur_idir:%x, serid:%x\n", ser, ser->cur_idir, imd_com->serviceID));
            ser->rt->mgr->conf.evtCallback(IV_SG_CALLBACK_INTERACTIVITY, imd_com->serviceID, knil, knil, knil);
        }

        klog(("MOS received, Name = %s, anlayse finsed, and sent callback to UI\n",pObjectName));

        kmem_free(imd_com->serviceID);
        kmem_free(imd_com);
    }
    kmem_free(contentType);

    sg_ses_try_end_session(ses);
    return 0;
}

typedef struct _K_MB_IAD_OBJ {
    sg_ses *ses;
    kint iHandle;
    kint iStatus;
    kuint uiObjectID;
    kchar*pObjectName;
    kchar *pObjectUrl;
    kint iObjectVersion;
    kuchar *pDataBuffer;
    kuint uiObjectLength;
    kchar *pSavePathName;
    kchar *pContentEncoding;

    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} K_MB_IAD_OBJ;

kint om_iad_obj_cbk(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MB_IAD_OBJ *obj = (K_MB_IAD_OBJ*)a_ar0;

    sg_ses *ses = obj->ses;
    kint iHandle = obj->iHandle;
    kint iStatus = obj->iStatus;
    kuint uiObjectID = obj->uiObjectID;
    kchar*pObjectName = obj->pObjectName;
    kchar *pObjectUrl = obj->pObjectUrl;
    kint iObjectVersion = obj->iObjectVersion;
    kuchar *pDataBuffer = obj->pDataBuffer;
    kuint uiObjectLength = obj->uiObjectLength;
    kchar *pSavePathName = obj->pSavePathName;
    kchar *pContentEncoding = obj->pContentEncoding;

    kvoid *vpUserData0 = obj->vpUserData0;
    kvoid *vpUserData1 = obj->vpUserData1;
    kvoid *vpUserData2 = obj->vpUserData2;
    kvoid *vpUserData3 = obj->vpUserData3;

    kchar *deflang, *contentType = knil;
    sg_root *rt = (sg_root*) vpUserData0;
    kchar *serid = (kchar*)vpUserData3;

    IV_INTERACTIVITY_DIRECTORY *idir;
    sg_imd *imd = knil;
    IV_INTERACTIVITY_MEDIA *im;
    kuint i, mediaNumber = 0, ret = -1;
    sg_ser *ser;
    sg_imd_common *imd_com = knil;

    kchar *ob = knil;
    kint ol = 0;
    kuchar *gunzip_buf = pDataBuffer, *gunzip_buf_end = pDataBuffer + uiObjectLength;

    K_pvdr_acc *acc;

    kassert(ses == (sg_ses*) vpUserData1);

    klog(("into om_iad_obj_cbk:ses = %x,Objid:%d\n", ses,uiObjectID));

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        kmem_free(serid);
        return -1;
    }

    if (!rt->freq.current || !rt->freq.current->acc) {
        kerror(("om_iad_obj_cbk: no current freq or acc\n"));
        kmem_free(serid);
        sg_ses_try_end_session(ses);
        return -1;
    }

    acc = rt->freq.current->acc;

    ser = (sg_ser*)sg_root_find_file(rt, SER, knil, serid);
    if (!ser) {
        kerror(("om_iad_obj_cbk: service (%s) is not found\n", serid));
        kmem_free(serid);
        sg_ses_try_end_session(ses);
        return -1;
    }
    deflang = rt->mgr->conf.lang;

    if (pContentEncoding && streq(pContentEncoding, "gzip")) {
        kint ret, mem_inc_step = 0;
        kuchar *uncompr;
        kulong uncomprLen;

try_with_more_mem:
        uncomprLen = kspt_gzip_orglen(pDataBuffer, uiObjectLength) + mem_inc_step;
        kassert(uncomprLen);
        uncompr = kmem_alloc(uncomprLen);
        kassert(uncompr);

        ret = zei_uncompress(uncompr, &uncomprLen, gunzip_buf, gunzip_buf_end - gunzip_buf, knil);
        if (0 == ret) {
            ob = (kchar*)uncompr;
            ol = uncomprLen;
        } else if (-5 == ret) {
            kmem_free(uncompr);
            mem_inc_step += 1024;
            goto try_with_more_mem;
        } else {
            kmem_free(serid);
            sg_ses_try_end_session(ses);
            return -1;
        }
    } else {
        ob = kmem_alloc(uiObjectLength);
        memcpy(ob, (kchar*)pDataBuffer, uiObjectLength);
        ol = uiObjectLength;
    }
    klog(("into om_iad_obj_cbk:ses=%x,Objid:%d uncompresss finished\n", ses,uiObjectID));

    sg_imd_new(&imd);
    imd->rt = rt;
    imd->dat.buf = ob;
    imd->dat.len = ol;

    //kmsg_post(...);

    if (-2 == sg_imd_x2s(imd)) {
        kassert("FIXME: how to do if imd failed\n");
        FREE_ORG_DAT(imd);
        sg_imd_del(imd, ktrue);
        kmem_free(serid);
        klog(("into om_iad_obj_cbk:sg_imd_x2s objectID :%d failed\n", uiObjectID));
        sg_ses_try_end_session(ses);
        return -1;
    }
    FREE_ORG_DAT(imd);

    if (!imd->arr->MediaObjectGroup) {
        sg_imd_del(imd, ktrue);
        kmem_free(serid);
        klog(("into om_iad_obj_cbk:objectID :%d no mos ,return\n", uiObjectID));
        sg_ses_try_end_session(ses);
        return -1;
    }
    klog(("into om_iad_obj_cbk:try to find the media in ses=%x,objectID :%d \n", ses,uiObjectID));

    for (i = 0; i < imd->arr->MediaObjectGroup->ecnt; i++) {
        kint mogcnt;
        MediaObjectGroup_rec *mog_rec;

        mog_rec = &imd->arr->MediaObjectGroup->arr[i];

        if (mog_rec->SMSTemplate) {
            // mediaNumber += mog_rec->SMSTemplate->ecnt;
            mediaNumber++;
        }
        if (mog_rec->EmailTemplate) {
            // mediaNumber += mog_rec->EmailTemplate->ecnt;
            mediaNumber++;
        }
        if (mog_rec->VoiceCall) {
            // mediaNumber += mog_rec->VoiceCall->ecnt;
            mediaNumber++;
        }
        if (mog_rec->Weblink) {
            // mediaNumber += mog_rec->Weblink->ecnt;
            mediaNumber++;
        }

        /* FIXME: MediaObjectSet not support in BDIR */
        if (mog_rec->MediaObjectSet && (acc->tmode[acc->tindex] != SG_TMODE_BDIR)) {
            mediaNumber += mog_rec->MediaObjectSet->ecnt;
        }
    }

    klog(("om_iad_obj_cbk:ses= %x, objectID %d,%d media found\n", ses,uiObjectID,mediaNumber));

    if (!mediaNumber) {
        kerror(("No media in this IAD\n"));
        // sg_imd_del(imd, ktrue);
        // kmem_free(serid);
        // return -1;
    }

    // First, Get the mediaNumber for IV_INTERACTIVITY_DIRECTORY
    idir = (IV_INTERACTIVITY_DIRECTORY*)kmem_alloz(sizeof(IV_INTERACTIVITY_DIRECTORY) +
            (mediaNumber - 1) * sizeof(IV_INTERACTIVITY_MEDIA));

    // Allocate memory for idir
    idir->startTime = imd->attr.validFrom;
    idir->endTime = imd->attr.validTo;
    idir->mediaNumber = mediaNumber;
    im = idir->interactivityMedia;

    if (!imd_com) {
        imd_com = kmem_alloz(sizeof(sg_imd_common));
        imd_com->idir = idir;
        imd_com->rt = rt;
        STR_SET(imd_com->serviceID, serid);
    }

    for (i = 0; i < imd->arr->MediaObjectGroup->ecnt; i++) {
        kuint j;
        MediaObjectGroup_rec *mog_rec;
        IV_ITV_SMS *itv_sms;

        mog_rec = &imd->arr->MediaObjectGroup->arr[i];

        if (mog_rec->SMSTemplate) {
            kuint j, choiceNum;
            im->type = IV_INTERACTIVITY_TYPE_SMS;
            im->relativePreference = mog_rec->SMSTemplate->arr->attr.relativePreference;

            itv_sms = knil;

            if (mog_rec->SMSTemplate->arr->Description) {
                for (j = 0; j < mog_rec->SMSTemplate->arr->Description->ecnt; j++) {
                    if (streq(deflang, mog_rec->SMSTemplate->arr->Description->arr[j].attr.xml_lang)) {
                        im->description = kstr_dup(mog_rec->SMSTemplate->arr->Description->arr[j].val.Description);
                        break;
                    }
                }
                if (j == mog_rec->SMSTemplate->arr->Description->ecnt) {
                    im->description = kstr_dup(mog_rec->SMSTemplate->arr->Description->arr[0].val.Description);
                }
            }

            if (mog_rec->SMSTemplate->arr->SelectChoice) {

                choiceNum = mog_rec->SMSTemplate->arr->SelectChoice->ecnt;
                itv_sms = (IV_ITV_SMS*)kmem_alloz(sizeof(IV_ITV_SMS) + (choiceNum - 1) * sizeof(IV_SMS_CHOICE));
                if (!itv_sms) {
                    /* FIXME : What What What */
                    mediaNumber--;
                    continue;
                }

                itv_sms->choiceNum = choiceNum;

                if (mog_rec->SMSTemplate->arr->Description) {
                    for (j = 0; j < mog_rec->SMSTemplate->arr->Description->ecnt; j++) {
                        if (streq(deflang, mog_rec->SMSTemplate->arr->Description->arr[j].attr.xml_lang)) {
                            itv_sms->text = kstr_dup(mog_rec->SMSTemplate->arr->Description->arr[j].attr.text);
                            break;
                        }
                    }
                    if (j == mog_rec->SMSTemplate->arr->Description->ecnt) {
                        itv_sms->text = kstr_dup(mog_rec->SMSTemplate->arr->Description->arr[0].attr.text);
                    }
                }

                for (j = 0; j < choiceNum; j++) {
                    kuint x;
                    SelectChoice_rec *sc_rec = &mog_rec->SMSTemplate->arr->SelectChoice->arr[j];

                    if (sc_rec->ChoiceText) {
                        for (x = 0; x < sc_rec->ChoiceText->ecnt; x++) {
                            if (streq(deflang, sc_rec->ChoiceText->arr[x].attr.xml_lang)) {
                                itv_sms->choices[j].choiceText = kstr_dup(sc_rec->ChoiceText->arr[x].val.ChoiceText);
                                break;
                            }
                        }
                        if (x == sc_rec->ChoiceText->ecnt) {
                            itv_sms->choices[j].choiceText = kstr_dup(sc_rec->ChoiceText->arr[0].val.ChoiceText);
                        }
                    }

                    itv_sms->choices[j].smsURI = kstr_dup(sc_rec->attr.smsURI);
                }
            }
            im->contents.sms = itv_sms;
            im++;
        }
        if (mog_rec->EmailTemplate) {
            im->type = IV_INTERACTIVITY_TYPE_EMAIL;
            im->relativePreference = mog_rec->EmailTemplate->arr->attr.relativePreference;

            if (mog_rec->EmailTemplate->arr->Description) {
                for (j = 0; j < mog_rec->EmailTemplate->arr->Description->ecnt; j++) {
                    if (streq(deflang, mog_rec->EmailTemplate->arr->Description->arr[j].attr.xml_lang)) {
                        im->description = kstr_dup(mog_rec->EmailTemplate->arr->Description->arr[j].val.Description);
                        break;
                    }
                }
                if (j == mog_rec->EmailTemplate->arr->Description->ecnt) {
                    im->description = kstr_dup(mog_rec->EmailTemplate->arr->Description->arr[0].val.Description);
                }
            }

            im->contents.email.to = kstr_dup(mog_rec->EmailTemplate->arr->attr.toHeader);
            im->contents.email.cc = kstr_dup(mog_rec->EmailTemplate->arr->attr.ccHeader);
            im->contents.email.bcc = kstr_dup(mog_rec->EmailTemplate->arr->attr.bccHeader);
            im->contents.email.subject = kstr_dup(mog_rec->EmailTemplate->arr->attr.subjectHeader);
            if (mog_rec->EmailTemplate->arr->MessageBody) {
                kint slen = strlen(mog_rec->EmailTemplate->arr->MessageBody->arr->val.MessageBody);
                kuchar *buffer = kmem_alloc(sizeof(kuchar) * (slen * 3 / 4));
                base64_decode(mog_rec->EmailTemplate->arr->MessageBody->arr->val.MessageBody, buffer);
                im->contents.email.body = (kchar*)buffer;
            }
            im++;
        }
        if (mog_rec->VoiceCall) {
            im->type = IV_INTERACTIVITY_TYPE_VOICECALL;
            im->relativePreference = mog_rec->VoiceCall->arr->attr.relativePreference;

            if (mog_rec->VoiceCall->arr->Description) {
                for (j = 0; j < mog_rec->VoiceCall->arr->Description->ecnt; j++) {
                    if (streq(deflang, mog_rec->VoiceCall->arr->Description->arr[j].attr.xml_lang)) {
                        im->description = kstr_dup(mog_rec->VoiceCall->arr->Description->arr[j].val.Description);
                        break;
                    }
                }
                if (j == mog_rec->VoiceCall->arr->Description->ecnt) {
                    im->description = kstr_dup(mog_rec->VoiceCall->arr->Description->arr[0].val.Description);
                }
            }
            if (mog_rec->VoiceCall->arr->PhoneNumber) {
                strncpy(im->contents.phoneNumber, mog_rec->VoiceCall->arr->PhoneNumber->arr->val.PhoneNumber,
                        sizeof(im->contents.phoneNumber));
            }
            im++;
        }
        if (mog_rec->Weblink) {
            im->type = IV_INTERACTIVITY_TYPE_WEBLINK;
            im->relativePreference = mog_rec->Weblink->arr->attr.relativePreference;

            if (mog_rec->Weblink->arr->Description) {
                for (j = 0; j < mog_rec->Weblink->arr->Description->ecnt; j++) {
                    if (streq(deflang, mog_rec->Weblink->arr->Description->arr[j].attr.xml_lang)) {
                        im->description = kstr_dup(mog_rec->Weblink->arr->Description->arr[j].val.Description);
                        break;
                    }
                }
                if (j == mog_rec->Weblink->arr->Description->ecnt) {
                    im->description = kstr_dup(mog_rec->Weblink->arr->Description->arr[0].val.Description);
                }
            }
            im->contents.webLink = kstr_dup(mog_rec->Weblink->arr->attr.webURL);
            klog(("webLink:%s\n", im->contents.webLink));
            im++;
        }
        if (mog_rec->MediaObjectSet && (acc->tmode[acc->tindex] != SG_TMODE_BDIR)) {
            IV_ITV_MEDIAOBJECTS *itv_mo = knil;
            MediaObjectSet_rec *mos_arr = mog_rec->MediaObjectSet->arr;
            kint mos_cnt = mog_rec->MediaObjectSet->ecnt;
            kint a;

            for (a = 0; a < mos_cnt; a++) {

                //                if (streq(mos_arr[a].attr.Content_Type, "application/gzip")) {
                //                }

                imd_com->uncomplete_file_cnt++;

                im->type = IV_INTERACTIVITY_TYPE_MEDIAOBJ;
                im->relativePreference = mos_arr[a].attr.relativePreference;

                if (mos_arr[a].Description) {
                    for (j = 0; j < mos_arr[a].Description->ecnt; j++) {
                        if (streq(deflang, mos_arr[a].Description->arr[j].attr.xml_lang)) {
                            im->description = kstr_dup(mos_arr[a].Description->arr[j].val.Description);
                            break;
                        }
                    }
                    if (j == mos_arr[a].Description->ecnt) {
                        im->description = kstr_dup(mos_arr[a].Description->arr[0].val.Description);
                    }
                }

                if (mos_arr[a].Object) {
                    Object_rec *obj_arr;
                    kint obj_cnt;
                    kint b;

                    itv_mo = kmem_alloz(sizeof(IV_ITV_MEDIAOBJECTS) + (mos_arr[a].Object->ecnt - 1) * sizeof(IV_MEDIA_OBJECT));
                    itv_mo->moNum = mos_arr[a].Object->ecnt;

                    obj_arr = mos_arr[a].Object->arr;
                    obj_cnt = mos_arr[a].Object->ecnt;

                    for (b = 0; b < obj_cnt; b++) {
                        itv_mo->objects[b].flag = obj_arr[b].attr.start;
                        STR_SET(itv_mo->objects[b].contentLocation, obj_arr[b].attr.Content_Location);
                        STR_SET(itv_mo->objects[b].contentType, obj_arr[b].attr.Content_Type);
                    }
                } else {
                    itv_mo = kmem_alloz(sizeof(IV_ITV_MEDIAOBJECTS));
                    itv_mo->moNum = 1;

                    itv_mo->objects[0].flag = 1;
                    STR_SET(itv_mo->objects[0].contentLocation, mos_arr->attr.Content_Location);
                    STR_SET(itv_mo->objects[0].contentType, mos_arr->attr.Content_Type);
                }
                im->contents.mediaObjects = itv_mo;


                klog(("IMD received, send cmd to receive mos by name = %s\n, ses = %x",mos_arr[a].attr.Content_Location,ses));
                contentType = kstr_dup(mos_arr[a].attr.Content_Type);
                if (sg_ses_get_obj(ses, 5000000, 0, 0xFFFFFFFF, (kvoid*)mos_file_callback, mos_arr[a].attr.Content_Location, (kvoid*)rt,
                            imd_com, im->contents.mediaObjects, contentType)) {
                    kmem_free(contentType);
                }
                klog(("IMD received, send cmd to receive mos: END\n"));
                im->contents.mediaObjects = itv_mo;
                im++;
            }
        }
    }

    if (imd_com) {
        kflg_set(imd_com->flg, ICF_IMD_PARSE_DONE);
    }

    if (imd_com && imd_com->uncomplete_file_cnt == 0) {
        if(ser->cur_idir){
            sg_imd_free_idir(ser->cur_idir);
        }

        ser->cur_idir = imd_com->idir;
        klog(("ser:%x, ser->cur_idir:%x, serid:%x\n", ser, ser->cur_idir, imd_com->serviceID));
        ser->rt->mgr->conf.evtCallback(IV_SG_CALLBACK_INTERACTIVITY, imd_com->serviceID, knil, knil, knil);

        kmem_free(imd_com->serviceID);
        kmem_free(imd_com);
    }

    sg_imd_del(imd, ktrue);
    kmem_free(serid);
    sg_ses_try_end_session(ses);
    return 0;
}

kint sg_iad_retrieve_data_callback(sg_ses *ses, kint iHandle, kint iStatus, kuint uiObjectID, kchar*pObjectName, kchar *pObjectUrl,
        kint iObjectVersion, kuchar *pDataBuffer, kuint uiObjectLength, kchar *pSavePathName, kchar *pContentEncoding,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{
    K_MB_IAD_OBJ obj;
    sg_root *rt = (sg_root*)vpUserData0;
    klog(("sg_iad_retrieve_data_callback:rt:%x, ses:%x\n", rt, ses));

    if (iStatus) {
        kfatal(("sg_iad_retrieve_data_callback:callback failed with status :%d\n", iStatus));
        kmem_free((kchar*)vpUserData3);
        sg_ses_try_end_session(ses);
        return -1;
    }

    obj.ses = ses;
    obj.iHandle = iHandle;
    obj.iStatus = iStatus;
    obj.uiObjectID = uiObjectID;
    obj.pObjectName = pObjectName;
    obj.pObjectUrl = pObjectUrl;
    obj.iObjectVersion = iObjectVersion;
    obj.pDataBuffer = pDataBuffer;
    obj.uiObjectLength = uiObjectLength;
    obj.pSavePathName = pSavePathName;
    obj.pContentEncoding = pContentEncoding;

    obj.vpUserData0 = vpUserData0;
    obj.vpUserData1 = vpUserData1;
    obj.vpUserData2 = vpUserData2;
    obj.vpUserData3 = vpUserData3;

    return kmsg_send(rt->worker_thread, KRTM_IAD_OBJ, &obj, 0, 0, 0);
}

typedef struct _K_MB_IAD_DIR {
    sg_ses *ses;
    kint iHandle;
    kint iStatus;
    IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute;
    IV_OBJECT_INFO *pInfoLisHead;
    kint iObjectNum;

    kvoid *vpUserData0;
    kvoid *vpUserData1;
    kvoid *vpUserData2;
    kvoid *vpUserData3;
} K_MB_IAD_DIR;

kint om_iad_dir_cbk(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    K_MB_IAD_DIR *dir = (K_MB_IAD_DIR*)a_ar0;

    kint iHandle = dir->iHandle;
    kint iStatus = dir->iStatus;
    IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute = dir->pObjectList_attibute;
    IV_OBJECT_INFO *pInfoLisHead = dir->pInfoLisHead;
    kint iObjectNum = dir->iObjectNum;

    kvoid *vpUserData0 = dir->vpUserData0;
    kvoid *vpUserData1 = dir->vpUserData1;
    kvoid *vpUserData2 = dir->vpUserData2;
    kvoid *vpUserData3 = dir->vpUserData3;

    kint i, ret = -1;
    sg_root *root = (sg_root*) vpUserData0;
    sg_ses *ses = (sg_ses*) vpUserData1;
    kchar *imdp = (kchar*)vpUserData2;
    kchar *serid = (kchar*)vpUserData3;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        kmem_free(imdp);
        kmem_free(serid);
        return -1;
    }

    klog(("into om_iad_dir_cbk:ses = %x,imdp = %s,  **objNumber:%d\n", ses,imdp,iObjectNum));
    //klog(("into om_iad_dir_cbk: ses = %x\n", ses));

    for (i = 0; i < iObjectNum; i++) {
        klog(("into om_iad_dir_cbk:ses=%x,pInfoLisHead[%d].name = %s, Toi = %u\n", ses,i,pInfoLisHead[i].pObjectName,pInfoLisHead[i].uiObjectID));
        if (!strncmp(pInfoLisHead[i].pObjectName, imdp, strlen(imdp))) {

            klog(("     om_iad_dir_cbk, ses=%x,Send cmd imdp = %s, Toi = %u\n", ses,imdp,pInfoLisHead[i].uiObjectID));
            ret = sg_ses_get_obj(ses, 6000000, pInfoLisHead[i].uiObjectID, 0xFFFFFFFF,
                    (kvoid*)sg_iad_retrieve_data_callback, knil, root, ses, knil, serid);
            klog(("     om_iad_dir_cbk,ses=%x, End  cmd imdp = %s, Toi = %u \n",ses, imdp,pInfoLisHead[i].uiObjectID));
            break;
        }
    }

    kmem_free(imdp);
    if (ret) {
        kmem_free(serid);
    }

    klog(("end  om_iad_dir_cbk :ses = %x,imdp = %s\n", ses,imdp));
    sg_ses_try_end_session(ses);
    return 0;
}

kint sg_iad_retrieve_data_list_callback(sg_ses *ses, kint iHandle, kint iStatus, IV_OBJECT_LIST_ATTRIBUTE *pObjectList_attibute,
        IV_OBJECT_INFO *pInfoLisHead, kint iObjectNum,
        void *vpUserData0, void *vpUserData1, void *vpUserData2, void *vpUserData3)
{

    K_MB_IAD_DIR dir;
    sg_root *rt = (sg_root*)vpUserData0;

    kassert(ses == vpUserData1);

    klog(("sg_iad_retrieve_data_list_callback: rt:%x, ses:%x\n", rt, ses));
    klog(("sg_iad_retrieve_data_list_callback: serid:%s\n", vpUserData3));

    if (iStatus) {
        kfatal(("sg_iad_retrieve_data_list_callback:callback failed with status :%d\n", iStatus));
        kmem_free((kchar*)vpUserData2);
        kmem_free((kchar*)vpUserData3);
        sg_ses_try_end_session(ses);
        return -1;
    }

    dir.ses = ses;
    dir.iHandle = iHandle;
    dir.iStatus = iStatus;
    dir.pObjectList_attibute = pObjectList_attibute;
    dir.pInfoLisHead = pInfoLisHead;
    dir.iObjectNum = iObjectNum;

    dir.vpUserData0 = vpUserData0;
    dir.vpUserData1 = vpUserData1;
    dir.vpUserData2 = vpUserData2;
    dir.vpUserData3 = vpUserData3;

    return kmsg_send(rt->worker_thread, KRTM_IAD_DIR, &dir, 0, 0, 0);
}

/*
   SES_DONE_CBK sg_iad_retrieve_done_callback(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
   {
   sg_ser *ser = (sg_ser*)ses->done.usr0;
   kflg_clr(ser->flg, SER_IAD); //iad retrivie(caused by sg_iad_retrieve_data) finised
   return 0;
   }
   */


kint sg_iad_retrieve_data(sg_root *rt, kchar *imdp, sg_ser *ser, sg_acc *acc, K_freq *fblk)
{
    kuint i;
    sg_ses *ses;

    kchar value[256];

    IV_FLUTE_ALC_SESSION_ENTRYINFO *alc_sei = knil;
    IV_DAB_SESSION_ENTRYINFO *dab_sei = knil;

    sg_sdp *sdp;

    kchar *sdp_buf = knil, *imdp_for_dir, *serid_for_dir;
    kint sdp_len = 0;

    kchar ipaddr[256] = { 0 }, ipvx[24] = { 0 };
    kuint TSI = (kuint)-1;
    kuint iAlcChannelNum = 1;
    kbool iIPv4_flag = 1;
    kchar ipaddrbuf[16] = { 0 };
    kint iSSM_flag = 0, index;

    K_dlist_entry *entry, *hdr;

    klog(("into sg_iad_retrieve_data(%s)\n",imdp));

    hdr = &fblk->fhdrs[SDP];

    if (acc->arr->AccessType &&
            acc->arr->AccessType->arr->BroadcastServiceDelivery &&
            acc->arr->AccessType->arr->BroadcastServiceDelivery->arr->SessionDescription) {
        if (acc->arr->AccessType->arr->BroadcastServiceDelivery->arr->SessionDescription->arr->SDP) {
            // embed SDP
            sdp_buf = acc->arr->AccessType->arr->BroadcastServiceDelivery->arr->SessionDescription->arr->SDP->arr->val.SDP;
            sdp_len = strlen(sdp_buf);
        } else {
            kchar *sdpRef = acc->arr->AccessType->arr->BroadcastServiceDelivery->arr->SessionDescription->arr->SDPRef->arr->attr.idRef;

            entry = hdr->next;
            while (entry != hdr) {
                sdp = FIELD_TO_STRUCTURE(entry, sg_sdp, cls_le);
                entry = entry->next;

                if (streq(sdpRef, sdp->attr.id)) {
                    sdp_buf = sdp->dat.buf;
                    sdp_len = sdp->dat.len;
                    break;
                }
            }
        }
    }

    for (i = 0; !sdp_getval(sdp_buf, sdp_len, "a", value, i); i++) {
        if (!memcmp(value, "flute-tsi:", 10)) {
            TSI = GETVAL_kuint(value + 10);
        }
        if (!memcmp(value, "flute-ch:", 9)) {
            iAlcChannelNum = GETVAL_kuint(value + 9);
            alc_sei = kmem_alloz(sizeof(IV_FLUTE_ALC_SESSION_ENTRYINFO) +
                    sizeof(IV_ALC_CHANNEL_INFO) * (iAlcChannelNum - 1));
        }
        if (!memcmp(value, "source-filter:", 14)) {
            iSSM_flag = 1;
            sscanf(value + 14, "%s %s %s %s %s", ipaddr, ipaddr, ipvx, ipaddr, ipaddr);
            if (streq(ipvx, "IP6")) {
                ipv6_scan(ipaddr, (kuchar*)ipaddrbuf);
                iIPv4_flag = kfalse;
            } else {
                iIPv4_flag = ktrue;
                ipv4_scan(ipaddr, (kuchar*)ipaddrbuf);
            }
        }
    }

    if (!alc_sei) {
        kerror(("alc_sei: not set\n"));
        return -1;
    }

    alc_sei->iAlcChannelNum = iAlcChannelNum;
    alc_sei->uiTSI = TSI;
    alc_sei->iSSM_flag = iSSM_flag;
    alc_sei->source_address.iIPv4_flag = iIPv4_flag;
    memcpy(alc_sei->source_address.ubIP, ipaddrbuf, 16);

    index = 0;
    for (i = 0; !sdp_getval(sdp_buf, sdp_len, "c", value, i); i++) {
        if (!memcmp(value, "IN", 2)) {
            sscanf(value + 3, "%s %s", ipvx, ipaddr);
            if (streq(ipvx, "IP6")) {
                ipv6_scan(ipaddr, (kuchar*)alc_sei->pAlcChannelHead[index].target_address.ubIP);
                alc_sei->pAlcChannelHead[index].target_address.iIPv4_flag = kfalse;
            } else {
                alc_sei->pAlcChannelHead[index].target_address.iIPv4_flag = ktrue;
                ipv4_scan(ipaddr, (kuchar*)alc_sei->pAlcChannelHead[index].target_address.ubIP);
            }

            if (!sdp_getval(sdp_buf, sdp_len, "m", value, i)) {
                if (!memcmp(value, "application", 11)) {
                    alc_sei->pAlcChannelHead[index].u16TargetPort = GETVAL_kuint(value + 12);
                }
            }
            index++;
        }
    }

    ses = knil;
    if (SG_TMODE_FLUTE == fblk->acc->tmode[fblk->acc->tindex]) {
        sg_ses_new(&ses, IV_FILEREV_PROTOCOL_FLUTEALC, alc_sei);
    } else if (SG_TMODE_MOT == fblk->acc->tmode[fblk->acc->tindex]) {
        sg_ses_new(&ses, IV_FILEREV_PROTOCOL_MOT, dab_sei);
    } else if (SG_TMODE_BDIR == fblk->acc->tmode[fblk->acc->tindex]) {
        if (1) {
            kchar url[1024];
            if (strstr(rt->freq.current->acc->bdir->url, "?")) {
                sprintf(url, "%s&NID=%s&TSID=%s&",
                        rt->freq.current->acc->bdir->url, rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
            } else {
                sprintf(url, "%s?NID=%s&TSID=%s&",
                        rt->freq.current->acc->bdir->url, rt->freq.current->acc->bdir->nid, rt->freq.current->acc->bdir->tsid);
            }
            sg_ses_new(&ses, IV_FILEREV_PROTOCOL_BDIR, url);
        }
    } else {
        kerror(("Unknown file tmode: %d\n", fblk->acc->tmode[fblk->acc->tindex]));
        kassert(!"Unknown file tmode");
        return -1;
    }

    if (sg_ses_start_session(&fblk->ii_ses, ktrue)){
        sg_ses_del(ses);
        klog(("sg_iad_retrieve_data: sg_ses_start_session, ses= %x failed\n",ses));
        return -1;
    }
    //sg_ses_set_done_cbk(ses, (SES_DONE_CBK)sg_iad_retrieve_done_callback, ses, knil, knil, knil);
    kflg_set(ses->flg, SES_AUTO_DEL | SES_AUTO_STOP);
    klog(("into sg_iad_retrieve_data(%s), try to get the IMD dir: ses = %x\n",imdp,ses));

    imdp_for_dir = kstr_dup(imdp);
    serid_for_dir = kstr_dup(ser->attr.id);
    if (sg_ses_get_dir(ses, 7000000, (kvoid*)sg_iad_retrieve_data_list_callback, rt, ses, imdp_for_dir, serid_for_dir)) {
        kmem_free(imdp_for_dir);
        kmem_free(serid_for_dir);
    }

    klog(("exit from sg_iad_retrieve_data(%s)\n",imdp));
    return 0;
}



static kint sg_iad_timer_callback(kvoid *ar0, kvoid *ar1, kvoid *ar2, kvoid *ar3)
{
    sg_ser *ser = (sg_ser*)ar1;
    klog(("iad timer callback encountering...\n"));

    kflg_clr(ser->flg, SER_IAD_TIMER);
    //kflg_clr(&ser->flg, SER_IAD);
    sg_start_iad_search(ar0, ser->attr.id, kfalse);
    return 0;
}

kvoid sg_start_iad_search_for_frequency(sg_root *rt, kuint frequency, kbool force)
{
    kmsg_post(rt->worker_thread, KRTM_IAD_SEARCH_FOR_FREQ, (kvoid*)rt, (kvoid*)frequency, (kvoid*)force, knil);
}
kint om_start_iad_search_for_frequency(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kuint frequency = (kuint)a_ar1;
    kbool force = (kbool)a_ar2;

    K_dlist_entry *entry, *hdr;
    sg_ser *ser;
    K_freq *fblk;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        return -1;
    }

    fblk = sg_root_freq_blk_find(rt, frequency);
    if (!fblk) {
        klog(("Not frequency for rt\n"));
        return 0;
    }

    hdr = &fblk->fhdrs[SER];
    entry = hdr->next;
    while (entry != hdr) {
        ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
        entry = entry->next;

        if (frequency == ser->frequency) {
            sg_start_iad_search(rt, ser->attr.id, force);
        }
    }
    return 0;
}

// XXX called when channel switch
// FIXME: to speedup retrieve interacivity information
// all the interacivity in the same frequency should be
// received at some time, so the prototype should changes to
// sg_start_iad_search(sg_root *rt, kuint freqency);
// should be called in the following case:
//   1. switch channel
//   2. timer due
//   3. when schedule updated
kint sg_start_iad_search(sg_root *rt, kchar *serviceID, kbool force)
{
    return kmsg_post(rt->worker_thread, KRTM_IAD_SEARCH, (kvoid*)rt, (kvoid*)kstr_dup(serviceID), (kvoid*)force, knil);
}
kint om_start_iad_search(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kchar *serviceID = (kchar*)a_ar1;
    kbool force = (kbool)a_ar2;
    K_freq *fblk;

    kuint curtime = ksys_ntp_time() + rt->mgr->env->time_diff, nextTime = 0xFFFFFFFF;
    DistributionWindow_rec *dwin, *nextdw = knil;
    sg_ser *ser;
    sg_sch *sch;
    sg_acc *acc, *tmpacc;
    K_dlist_entry *entry, *hdr;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        kmem_free(serviceID);
        return -1;
    }

    klog(("into iad_search: curTime:%u serviceID:%s\n", curtime,serviceID));

    ser = (sg_ser*)sg_root_find_file(rt, SER, knil, serviceID);

    fblk = sg_root_freq_blk_find(rt, ser->frequency);

    if (ser && force) {
        ktmr_kill(ser->iad_probe_tmr);
        kflg_clr(ser->flg, SER_IAD_TIMER);
    }

    //ser = (sg_ser*)sg_root_find_file(rt, SER, knil, knil);
    if (!ser || !fblk || (/*kflg_chk(ser->flg, SER_IAD) &&*/ kflg_chk(ser->flg, SER_IAD_TIMER))) {
        //kflg_chk(ser->flg, SER_IAD): not necesary now
        //kflg_chk(ser->flg, SER_IAD_TIMER): if timer has been set, no need to search iad again.
        kmem_free(serviceID);
        klog(("iad_search: no ser(%x) or now searching\n", ser));
        return 0;
    }

    hdr = &fblk->fhdrs[SCH];
    entry = hdr->next;
    while (entry != hdr) {
        sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
        entry = entry->next;
        klog(("line:%d, sch->tid:%d, %x\n", __LINE__, sch->tid, sch->tid));

        if (streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID) && sch->arr->InteractivityDataReference) {
            kuint j;
            // XXX currently only use ONE schedule for all Interactivity
            for (j = 0; j < sch->arr->InteractivityDataReference->ecnt; j++) {
                InteractivityDataReference_rec *idr = &sch->arr->InteractivityDataReference->arr[j];
                kuint i;
                if (!idr->DistributionWindow) {
                    continue;
                }

                for (i = 0; i < idr->DistributionWindow->ecnt; i++) {
                    dwin = &idr->DistributionWindow->arr[i];

                    if ((dwin->attr.startTime <= curtime) && ((dwin->attr.endTime - dwin->attr.duration) > curtime)) {
                        // Get the current Data
                        kint k;
                        ScheduleReference_rec *sr;
                        K_dlist_entry *le1, *hdr1;
                        klog(("iad_search: this DistributionWindow's time is OK:\n"));
                        klog(("dwin[%d]:dwin->attr.startTime:%u, dwin->attr.endTime:%u, dwin->attr.duration:%u\n", i, dwin->attr.startTime, dwin->attr.endTime, dwin->attr.duration));
                        klog(("the InteractivityDataReference: %s, varibale idr %u\n",idr->attr.idRef,&idr));


                        tmpacc = knil;
                        hdr1 = &fblk->fhdrs[ACC];
                        le1 = hdr1->next;
                        while (le1 != hdr1) {
                            acc = FIELD_TO_STRUCTURE(le1, sg_acc, cls_le);
                            le1 = le1->next;


                            if (!acc->arr->ScheduleReference) {
                                klog(("nil ScheduleReference, next loop\n"));
                                continue;
                            }

                            for (k = 0; k < acc->arr->ScheduleReference->ecnt; k++) {
                                sr = &acc->arr->ScheduleReference->arr[k];

                                if (streq(sr->attr.idRef, sch->attr.id)) {
                                    if (!sr->DistributionWindowID) {
                                        // OK
                                        //
                                        klog(("iad_search: acc scheduleReference id matched-> no DistributionWindow\n"));
                                        tmpacc = acc;
                                    } else {
                                        kuint w;
                                        DistributionWindowID_rec *dwid;
                                        for (w = 0; w < sr->DistributionWindowID->ecnt; w++) {
                                            dwid = &sr->DistributionWindowID->arr[w];
                                            if (dwin->attr.id == dwid->val.DistributionWindowID) {
                                                // OK
                                                klog(("iad_search: ID matched, goto label1\n"));
                                                tmpacc = acc;
                                                goto label1;
                                            }
                                        }
                                    }
                                }
                            }
                        }

label1:
#if 0
                        if ((fblk->acc->tmode[fblk->acc->tindex] == SG_TMODE_BDIR) ||
                            (tmpacc /*&&!kspt_chkflg(&ser->flg, SER_IAD)*/)) {
#else
                        if (tmpacc /*&&!kspt_chkflg(&ser->flg, SER_IAD)*/) {
#endif
                            sg_iad *iad;
                            K_dlist_entry *entry, *hdr;
                            hdr = &fblk->fhdrs[IAD];
                            entry = hdr->next;
                            while (entry != hdr) {
                                iad = FIELD_TO_STRUCTURE(entry, sg_iad, cls_le);
                                entry = entry->next;
                                if (streq(iad->attr.id, idr->attr.idRef)) {
                                    klog(("[IAD], this iad matched with before distributionwindow's idRef:%s \n",idr->attr.idRef));
                                    klog(("[IAD], this iad's validTo %u, curtime %u\n",iad->attr.validTo,curtime));


                                    if (curtime < iad->attr.validTo) {
                                        if (fblk->acc->tmode[fblk->acc->tindex] != SG_TMODE_BDIR) {
                                            klog(("call sg_iad_retrieve_data. imdp = %s, curtime:%u \n",iad->arr->attr.interactivityMediaDocumentPointer,curtime));
                                            sg_iad_retrieve_data(rt, iad->arr->attr.interactivityMediaDocumentPointer, ser, tmpacc, fblk);
                                            klog(("return from sg_iad_retrieve_data\n"));
                                        } else {
                                            kchar url[IV_PATH_MAX_SIZE];
                                            if (strstr(__bdsg_base_url, "?")) {
                                                sprintf(url, "%s&Type=IMD&DocPointer=%s", __bdsg_base_url,
                                                        iad->arr->attr.interactivityMediaDocumentPointer);
                                            } else {
                                                sprintf(url, "%s?Type=IMD&DocPointer=%s", __bdsg_base_url,
                                                        iad->arr->attr.interactivityMediaDocumentPointer);
                                            }

                                            if (1) {
                                                kchar url[1024];
                                                if (strstr(rt->freq.current->acc->bdir->url, "?")) {
                                                    sprintf(url, "%s&NID=%s&TSID=%s&Type=IMD&DocPointer=%s",
                                                            rt->freq.current->acc->bdir->url,
                                                            rt->freq.current->acc->bdir->nid,
                                                            rt->freq.current->acc->bdir->tsid,
                                                            iad->arr->attr.interactivityMediaDocumentPointer);
                                                } else {
                                                    sprintf(url, "%s?NID=%s&TSID=%s&Type=IMD&DocPointer=%s",
                                                            rt->freq.current->acc->bdir->url,
                                                            rt->freq.current->acc->bdir->nid,
                                                            rt->freq.current->acc->bdir->tsid,
                                                            iad->arr->attr.interactivityMediaDocumentPointer);
                                                }

                                                Http_GetWebObject(knil, url, sg_iad_retrieve_data_callback, knil, 0x87654321,
                                                        rt, knil, knil, kstr_dup(ser->attr.id));
                                            }

                                        }

                                        //kflg_set(ser->flg, SER_IAD);

                                        // this version support only one iad at the same time.
                                        // to support mult iad at the same time, can add a list in ther service structure to save the iad that already to receive
                                        // and can avoid the problem that cause multi time to recive one iad when user switch between channel in short time.


                                    } else {
                                        klog(("iad_search: time not matched, try next InteractivityData\n"));
                                    }
                                }
                            }
                        } else {
                            klog(("iad_search: label1: check frequency and flg, failed!\n"));
                        }
                    }

                    if (dwin->attr.startTime > curtime && dwin->attr.startTime < nextTime) {
                        nextTime = dwin->attr.startTime;
                        nextdw = dwin;
                        klog(("iad_search: next time changed: nextTime:%u, next DistributionWindow:%x\n", nextTime, nextdw));
                    }
                }
            }
        }
    }
    if (nextdw) {
        if (!kflg_chk(ser->flg, SER_IAD_TIMER)) {
            ser->iad_probe_tmr = knil;
        }
        //ktmr_kill(ser->iad_probe_tmr);
        klog(("iad_search: start iad_probe_tmr:  dwin->attr.startTime : %u\n", nextTime));
        klog(("iad_search: start iad_probe_tmr:  currentTime          : %u\n", curtime));
        klog(("iad_search: start iad_probe_tmr: gap : %d\n", nextTime - curtime));

        ser->iad_probe_tmr = (kvoid*)ktmr_set(rt->worker_thread, (nextTime - curtime+50/*temp ren*/) * 1000, kfalse, sg_iad_timer_callback, rt, ser, knil, knil);
        //add 20: to match with headead now, 1019
        kflg_set(ser->flg, SER_IAD_TIMER);
    }

    kmem_free(serviceID);
    klog(("iad_search endtime:%u\n", ksys_ntp_time() + rt->mgr->env->time_diff));
    return 0;
}


