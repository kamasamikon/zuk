
#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_pvd.h"
#include "sg_x2s_fun.h"

kint sg_pvd_x2s(sg_pvd *pvd)
{
    kuint curtime = ksys_ntp_time() + pvd->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!pvd) {
        kerror(("No pvd input...\n"));
        return -1;
    }
    if ((!pvd->dat.buf) || (0 == pvd->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_pvd_add_cache(pvd);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, pvd->dat.buf, pvd->dat.len);
    pvd->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "PreviewData", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    pvd->arr = kmem_alloz(sizeof(PreviewData_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (pvd->attr.id) {
            kmem_free(pvd->attr.id);
        }
        pvd->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        pvd->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        pvd->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        pvd->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (pvd->attr.validTo && pvd->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }

    pvd->ecnt = 1;
    pvd->arr->SMIL = x2s_fill_SMIL(doc);
    pvd->arr->Video = x2s_fill_Video(doc);
    pvd->arr->Audio = x2s_fill_Audio(doc);
    pvd->arr->Picture = x2s_fill_Picture(doc);
    pvd->arr->Text = x2s_fill_Text(doc);
    pvd->arr->AccessReference = x2s_fill_AccessReference(doc);
    pvd->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(pvd->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
