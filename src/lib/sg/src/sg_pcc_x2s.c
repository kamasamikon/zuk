/* vim:set et sw=4 sts=4: */

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_pcc.h"
#include "sg_x2s_fun.h"

kint sg_pcc_x2s(sg_pcc *pcc)
{
    kuint curtime = ksys_ntp_time() + pcc->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!pcc) {
        kerror(("No pcc input...\n"));
        return -1;
    }
    if ((!pcc->dat.buf) || (0 == pcc->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_pcc_add_cache(pcc);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, pcc->dat.buf, pcc->dat.len);
    pcc->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "PurchaseChannel", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    pcc->arr = kmem_alloz(sizeof(PurchaseChannel_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (pcc->attr.id) {
            kmem_free(pcc->attr.id);
        }
        pcc->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        pcc->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        pcc->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        pcc->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (pcc->attr.validTo && pcc->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "rightsIssuerURI")) {
        pcc->arr->attr.rightsIssuerURI = GETVAL_kpchar(attr->value);
    }

    pcc->ecnt = 1;
    pcc->arr->Name = x2s_fill_Name(doc);
    pcc->arr->Description = x2s_fill_Description(doc);
    pcc->arr->ContactInfo = x2s_fill_ContactInfo(doc);
    pcc->arr->PortalURL = x2s_fill_PortalURL(doc);
    pcc->arr->PurchaseURL = x2s_fill_PurchaseURL(doc);
    pcc->arr->Extension = x2s_fill_Extension(doc);
    pcc->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(pcc->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
