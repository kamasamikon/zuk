/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "local-channel.h"

KMC_LocalChannel::KMC_LocalChannel(KMC_LocalDevice* a_parentDevice, char* a_name)
    : KMediaChannel(a_parentDevice, a_name)
{
}

KMC_LocalChannel::~KMC_LocalChannel(void)
{
}

