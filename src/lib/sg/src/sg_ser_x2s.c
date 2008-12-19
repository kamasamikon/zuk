
#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"


#include "sg_root.h"
#include "sg_ser.h"
#include "sg_x2s_fun.h"

kint sg_ser_x2s(sg_ser *ser)
{
    kuint curtime = ksys_ntp_time() + ser->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;
    kint ncnt;

    if (!ser) {
        kerror(("No ser input...\n"));
        return -1;
    }
    if ((!ser->dat.buf) || (0 == ser->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_ser_add_cache(ser);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, ser->dat.buf, ser->dat.len);
    ser->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "Service", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    ser->arr = kmem_alloz(sizeof(Service_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (ser->attr.id) {
            kmem_free(ser->attr.id);
        }
        ser->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        ser->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        ser->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        ser->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (ser->attr.validTo && ser->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "weight")) {
        ser->arr->attr.weight = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "serviceContentProtection")) {
        ser->arr->attr.serviceContentProtection = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "baseCID")) {
        ser->arr->attr.baseCID = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "emergency")) {
        ser->arr->attr.emergency = GETVAL_kuint(attr->value);
    }

    ser->ecnt = 1;
    ser->arr->ProtectionKeyID = x2s_fill_ProtectionKeyID(doc);
    ser->arr->ServiceType = x2s_fill_ServiceType(doc);
    ser->arr->GlobalServiceID = x2s_fill_GlobalServiceID(doc);
    ser->arr->Name = x2s_fill_Name(doc);
    ser->arr->Description = x2s_fill_Description(doc);
    ser->arr->AudioLanguage = x2s_fill_AudioLanguage(doc);
    ser->arr->TextLanguage = x2s_fill_TextLanguage(doc);
    ser->arr->ParentalRating = x2s_fill_ParentalRating(doc);
    ser->arr->TargetUserProfile = x2s_fill_TargetUserProfile(doc);
    ser->arr->Genre = x2s_fill_Genre(doc);
    ser->arr->Extension = x2s_fill_Extension(doc);
    ser->arr->PreviewDataReference = x2s_fill_PreviewDataReference(doc);
    ser->arr->BroadcastArea = x2s_fill_BroadcastArea(doc);
    ser->arr->TermsOfUse = x2s_fill_TermsOfUse(doc);
    ser->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(ser->flg, EFF_PROCOK);    // indicate the data not been processed
    xmldoc_del(doc);
    return 0;
}

