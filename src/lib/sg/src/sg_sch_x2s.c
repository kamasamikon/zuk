/* vim:set et sw=4 sts=4: */

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_sch.h"
#include "sg_x2s_fun.h"

kint sg_sch_x2s(sg_sch *sch)
{
    kuint curtime = ksys_ntp_time() + sch->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!sch) {
        kerror(("No sch input...\n"));
        return -1;
    }
    if ((!sch->dat.buf) || (0 == sch->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_sch_add_cache(sch);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, sch->dat.buf, sch->dat.len);
    sch->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "Schedule", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    sch->arr = kmem_alloz(sizeof(Schedule_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (sch->attr.id) {
            kmem_free(sch->attr.id);
        }
        sch->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        sch->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        sch->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        sch->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (sch->attr.validTo && sch->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "defaultSchedule")) {
        sch->arr->attr.defaultSchedule = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "onDemand")) {
        sch->arr->attr.onDemand = GETVAL_kuint(attr->value);
    }

    sch->ecnt = 1;
    sch->arr->ServiceReference = x2s_fill_ServiceReference(doc);
    sch->arr->InteractivityDataReference = x2s_fill_InteractivityDataReference(doc);
    sch->arr->ContentReference = x2s_fill_ContentReference(doc);
    sch->arr->PreviewDataReference = x2s_fill_PreviewDataReference(doc);
    sch->arr->TermsOfUse = x2s_fill_TermsOfUse(doc);
    sch->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(sch->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
