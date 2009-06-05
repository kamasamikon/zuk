/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "MCSkel-channel.h"

KMC_MCSkelChannel::KMC_MCSkelChannel(KMC_MCSkelDevice* a_parentDevice, char* a_name)
    : KMediaChannel(a_parentDevice, a_name)
{
}

KMC_MCSkelChannel::~KMC_MCSkelChannel(void)
{
}

