#include <string.h>
#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>

#include "xmldoc.h"


#include "sg_root.h"
#include "sg_ctt.h"
#include "sg_x2s_fun.h"

kint sg_ctt_x2s(sg_ctt *ctt)
{
    kuint curtime = ksys_ntp_time() + ctt->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!ctt) {
        kerror(("No ctt input...\n"));
        return -1;
    }
    if ((!ctt->dat.buf) || (0 == ctt->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_ctt_add_cache(ctt);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, ctt->dat.buf, ctt->dat.len);
    ctt->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "Content", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    ctt->arr = kmem_alloz(sizeof(Content_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (ctt->attr.id) {
            kmem_free(ctt->attr.id);
        }
        ctt->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        ctt->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        ctt->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        ctt->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (ctt->attr.validTo && ctt->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "globalContentID")) {
        ctt->arr->attr.globalContentID = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "emergency")) {
        ctt->arr->attr.emergency = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "serviceContentProtection")) {
        ctt->arr->attr.serviceContentProtection = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "baseCID")) {
        ctt->arr->attr.baseCID = GETVAL_kpchar(attr->value);
    }

    ctt->ecnt = 1;
    ctt->arr->ServiceReference = x2s_fill_ServiceReference(doc);
    ctt->arr->ProtectionKeyID = x2s_fill_ProtectionKeyID(doc);
    ctt->arr->Name = x2s_fill_Name(doc);
    ctt->arr->Description = x2s_fill_Description(doc);
    ctt->arr->StartTime = x2s_fill_StartTime(doc);
    ctt->arr->EndTime = x2s_fill_EndTime(doc);
    ctt->arr->AudioLanguage = x2s_fill_AudioLanguage(doc);
    ctt->arr->TextLanguage = x2s_fill_TextLanguage(doc);
    ctt->arr->Length = x2s_fill_Length(doc);
    ctt->arr->ParentalRating = x2s_fill_ParentalRating(doc);
    ctt->arr->TargetUserProfile = x2s_fill_TargetUserProfile(doc);
    ctt->arr->Genre = x2s_fill_Genre(doc);
    ctt->arr->Extension = x2s_fill_Extension(doc);
    ctt->arr->PreviewDataReference = x2s_fill_PreviewDataReference(doc);
    ctt->arr->BroadcastArea = x2s_fill_BroadcastArea(doc);
    ctt->arr->TermsOfUse = x2s_fill_TermsOfUse(doc);
    ctt->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(ctt->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
