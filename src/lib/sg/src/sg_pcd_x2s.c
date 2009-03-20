/* vim:set et sw=4 sts=4: */

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>


#include "xmldoc.h"

#include "sg_root.h"
#include "sg_pcd.h"
#include "sg_x2s_fun.h"

kint sg_pcd_x2s(sg_pcd *pcd)
{
    kuint curtime = ksys_ntp_time() + pcd->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!pcd) {
        kerror(("No pcd input...\n"));
        return -1;
    }
    if ((!pcd->dat.buf) || (0 == pcd->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_pcd_add_cache(pcd);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, pcd->dat.buf, pcd->dat.len);
    pcd->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "PurchaseData", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    pcd->arr = kmem_alloz(sizeof(PurchaseData_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (pcd->attr.id) {
            kmem_free(pcd->attr.id);
        }
        pcd->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        pcd->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        pcd->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        pcd->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (pcd->attr.validTo && pcd->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }

    pcd->ecnt = 1;
    pcd->arr->Description = x2s_fill_Description(doc);
    pcd->arr->PriceInfo = x2s_fill_PriceInfo(doc);
    pcd->arr->PromotionInfo = x2s_fill_PromotionInfo(doc);
    pcd->arr->Extension = x2s_fill_Extension(doc);
    pcd->arr->PurchaseItemReference = x2s_fill_PurchaseItemReference(doc);
    pcd->arr->PurchaseChannelReference = x2s_fill_PurchaseChannelReference(doc);
    pcd->arr->PreviwDataReference = x2s_fill_PreviwDataReference(doc);
    pcd->arr->TermsOfUse = x2s_fill_TermsOfUse(doc);
    pcd->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(pcd->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
