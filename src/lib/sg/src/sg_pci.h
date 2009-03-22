/* vim:set et sw=4 sts=4: */
#ifndef __SG_PCI_H__
#define __SG_PCI_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

typedef PurchaseItem_t sg_pci;

//kint sg_pci_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_pci_new(sg_pci **pci);
kint sg_pci_del(sg_pci *pci, kbool rmcache);
kint sg_pci_add_cache(sg_pci *pci);
kint sg_pci_del_cache(sg_pci *pci);
kint sg_pci_ld_cache(sg_pci *pci);


#ifdef __cplusplus
}
#endif

#endif // __SG_PCI_H__

