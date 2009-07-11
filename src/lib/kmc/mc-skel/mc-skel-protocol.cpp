/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include <kmccontainer.h>
#include "MCSkel-protocol.h"

KMC_MCSkelProtocol::KMC_MCSkelProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type)
        : KMediaProtocol(a_parentContainer, a_name, a_type)
{
    kchar hash[33];
    kim_setint(__g_im, "i.kmc.evt.protocol.new", 1, (void**)(hash), (void**)this);
}

KMC_MCSkelProtocol::~KMC_MCSkelProtocol()
{
    kim_setint(__g_im, "i.kmc.evt.protocol.del", 1, (void**)getHash(), (void**)this);
}

int KMC_MCSkelProtocol::scanDevice()
{
    return EC_NOT_SUPPORT;
}

kbool KMC_MCSkelProtocol::start()
{
    return KMediaProtocol::start();
}

kbool KMC_MCSkelProtocol::stop()
{
    return KMediaProtocol::stop();
}

