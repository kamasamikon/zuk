#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>
#include "ksupport.h"
#include <kflg.h>

#include "xmldoc.h"

#include "sg_root.h"
#include "sg_fdt.h"
#include "x2s_macro.h"

kint sg_fdt_x2s(sg_fdt *fdt)
{
    kuint curtime = ksys_ntp_time() + fdt->rt->mgr->env->time_diff;
    KXmlDoc *doc;

    if (!fdt) {
        kerror(("No fdt input...\n"));
        return -1;
    }
    if ((!fdt->dat.buf) || (0 == fdt->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_fdt_add_cache(fdt);
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, fdt->dat.buf, fdt->dat.len);

    kflg_set(fdt->flg, EFF_PROCOK);    // indicate the data not been processed

    xmldoc_del(doc);
    return 0;
}
