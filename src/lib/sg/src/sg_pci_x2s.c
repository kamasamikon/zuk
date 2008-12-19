
#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_pci.h"
#include "sg_x2s_fun.h"

kint sg_pci_x2s(sg_pci *pci)
{
    kuint curtime = ksys_ntp_time() + pci->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;
    kint ncnt;

    if (!pci) {
        kerror(("No pci input...\n"));
        return -1;
    }
    if ((!pci->dat.buf) || (0 == pci->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_pci_add_cache(pci);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, pci->dat.buf, pci->dat.len);
    pci->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "PurchaseItem", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    pci->arr = kmem_alloz(sizeof(PurchaseItem_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (pci->attr.id) {
            kmem_free(pci->attr.id);
        }
        pci->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        pci->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        pci->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        pci->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (pci->attr.validTo && pci->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }


    if (attr = xmlnode_getattr(node, "globalPurchaseItemID")) {
        pci->arr->attr.globalPurchaseItemID = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "binaryPurchaseItemID")) {
        pci->arr->attr.binaryPurchaseItemID = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "weight")) {
        pci->arr->attr.weight = GETVAL_kushort(attr->value);
    }
    if (attr = xmlnode_getattr(node, "closed")) {
        pci->arr->attr.closed = GETVAL_kbool(attr->value);
    }

    pci->ecnt = 1;
    pci->arr->ServiceReference = x2s_fill_ServiceReference(doc);
    pci->arr->ScheduleReference = x2s_fill_ScheduleReference(doc);
    pci->arr->ContentReference = x2s_fill_ContentReference(doc);
    pci->arr->PurchaseItemReference = x2s_fill_PurchaseItemReference(doc);
    pci->arr->ProtectionKeyID = x2s_fill_ProtectionKeyID(doc);
    pci->arr->Name = x2s_fill_Name(doc);
    pci->arr->Description = x2s_fill_Description(doc);
    pci->arr->StartTime = x2s_fill_StartTime(doc);
    pci->arr->EndTime = x2s_fill_EndTime(doc);
    pci->arr->Extension = x2s_fill_Extension(doc);
    pci->arr->ParentalRating = x2s_fill_ParentalRating(doc);
    pci->arr->Dependencies = x2s_fill_Dependencies(doc);
    pci->arr->Exclusions = x2s_fill_Exclusions(doc);
    pci->arr->PrivateExt = x2s_fill_PrivateExt(doc);

    kflg_set(pci->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
