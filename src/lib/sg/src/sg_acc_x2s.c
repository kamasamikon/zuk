/* vim:set et sw=4 sts=4: */

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include "ksupport.h"
#include <kflg.h>
#include <kmem.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_acc.h"
#include "x2s_macro.h"
#include "sg_x2s_fun.h"

kint sg_acc_x2s(sg_acc *acc)
{
    kuint curtime = ksys_ntp_time() + acc->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;
    kint i = 0;

    if (!acc) {
        kerror(("No acc input...\n"));
        return -1;
    }
    if ((!acc->dat.buf) || (0 == acc->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_acc_add_cache(acc);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, acc->dat.buf, acc->dat.len);
    acc->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "Access", 0))) {
        xmldoc_del(doc);
        return -2;
    }

    acc->arr = kmem_alloz(sizeof(Access_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (acc->attr.id) {
            kmem_free(acc->attr.id);
        }
        acc->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        acc->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        acc->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        acc->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (acc->attr.validTo && acc->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }

    acc->ecnt = 1;
    acc->arr->AccessType = x2s_fill_AccessType(doc);
    acc->arr->KeyManagementSystem = x2s_fill_KeyManagementSystem(doc);
    acc->arr->EncryptionType = x2s_fill_EncryptionType(doc);
    acc->arr->ServiceReference = x2s_fill_ServiceReference(doc);
    acc->arr->ScheduleReference = x2s_fill_ScheduleReference(doc);
    acc->arr->TerminalCapabilityRequirement = x2s_fill_TerminalCapabilityRequirement(doc);
    acc->arr->BandwidthRequirement = x2s_fill_BandwidthRequirement(doc);
    acc->arr->ServiceClass = x2s_fill_ServiceClass(doc);
    acc->arr->PreviewDataReference = x2s_fill_PreviewDataReference(doc);
    acc->arr->NotificationReception = x2s_fill_NotificationReception(doc);

    acc->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(acc->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
