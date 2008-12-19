#include "ksupport.h"
#include <kflg.h>

#include <ktypes.h>
#include <sdlist.h>
#include <kdbg.h>
#include <kmem.h>

#include "sg_root.h"
#include "sg_sdp.h"

kint sg_sdp_x2s(sg_sdp *sdp)
{
    kuint curtime = ksys_ntp_time() + sdp->rt->mgr->env->time_diff;
    if (!sdp) {
        kerror(("No sdp input...\n"));
        return -1;
    }
    if ((!sdp->dat.buf) || (0 == sdp->dat.len)) {
        kerror(("not buffer\n"));
        return -1;
    }

    sg_sdp_add_cache(sdp);

    kflg_set(sdp->flg, EFF_PROCOK);    // indicate the data not been processed
    return 0;
}


