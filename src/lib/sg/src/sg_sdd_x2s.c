/* vim:set et sw=4 sts=4: */
#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_sdd.h"
#include "sg_x2s_fun.h"

kint sg_sdd_x2s(sg_sdd *sdd)
{
    kuint curtime = ksys_ntp_time() + sdd->rt->mgr->env->time_diff;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    if (!sdd) {
        kerror(("No sdd input...\n"));
        return -1;
    }
    if ((!sdd->dat.buf) || (0 == sdd->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_sdd_add_cache(sdd);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, sdd->dat.buf, sdd->dat.len);
    sdd->ecnt = 1;

    // walk the xml and structure
    if (!(node = xmldoc_gotoNode(doc, "ServiceGuideDeliveryDescriptor", 0))) {
        xmldoc_del(doc);
        return -1;
    }

    sdd->arr = kmem_alloz(sizeof(ServiceGuideDeliveryDescriptor_rec));
    if (attr = xmlnode_getattr(node, "id")) {
        if (sdd->attr.id) {
            kmem_free(sdd->attr.id);
        }
        sdd->attr.id = GETVAL_kpchar(attr->value);
    }
    if (attr = xmlnode_getattr(node, "version")) {
        sdd->attr.version = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validFrom")) {
        sdd->attr.validFrom = GETVAL_kuint(attr->value);
    }
    if (attr = xmlnode_getattr(node, "validTo")) {
        sdd->attr.validTo = GETVAL_kuint(attr->value);
    }
    if (sdd->attr.validTo && sdd->attr.validTo <= curtime + 2) {
        xmldoc_del(doc);
        return -2;
    }

    sdd->ecnt = 1;
    sdd->arr->NotificationReception = x2s_fill_NotificationReception(doc);
    sdd->arr->BSMList = x2s_fill_BSMList(doc);
    sdd->arr->DescriptorEntry = x2s_fill_DescriptorEntry(doc);

    kflg_set(sdd->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}

#include "stdio.h"

int dfmain(void)
{
    sg_sdd *sdd = knil;
    kchar *buf;
    kint len;

    FILE *fp = fopen("sgdd.xml", "rb");

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = kmem_alloc(len);

    fread(buf, sizeof(char), len, fp);

    sg_sdd_new(&sdd);
    sdd->dat.buf = buf;
    sdd->dat.len = len;

    sg_sdd_x2s(sdd);
    return 0;
}
