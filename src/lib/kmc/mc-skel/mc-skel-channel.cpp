/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "MCSkel-channel.h"

KMC_MCSkelChannel::KMC_MCSkelChannel(KMC_MCSkelDevice* a_parentDevice, char* a_name)
    : KMediaChannel(a_parentDevice, a_name)
{
    kim_setint(__g_im, "i.kmc.evt.channel.new", 1, (void**)this, knil);
}

KMC_MCSkelChannel::~KMC_MCSkelChannel(void)
{
    kim_setint(__g_im, "i.kmc.evt.channel.del", 1, (void**)this, knil);
}

