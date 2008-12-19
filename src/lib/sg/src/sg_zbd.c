#include <string.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>

#include "esgfile.h"
#include "sg_root.h"
#include "sg_zbd.h"

kint sg_zbd_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec)
{
    return 0;
}
kint sg_zbd_new(sg_zbd **zbd)
{
    if (!(*zbd)) {
        (*zbd) = kmem_alloc(sizeof(sg_zbd));
    }
    if ((*zbd)) {
        memset((*zbd), 0, sizeof(sg_zbd));
        INIT_COMMON((*zbd));
        return 0;
    }
    return -1;
}

kint sg_zbd_del(sg_zbd *zbd, kbool rmcache)
{
    if (!zbd) {
        return -1;
    }
    remove_dlist_entry(&zbd->all_le);
    remove_dlist_entry(&zbd->cls_le);

    if (rmcache) {
        sg_root_rm_cache_file(zbd);
    }

    if (zbd->dat.buf) {
        kmem_free_z(zbd->dat.buf);
    }

    DEL_COMMON(zbd);
    return 0;

}

