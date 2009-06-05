/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include <kmccontainer.h>
#include "MCSkel-protocol.h"

KMC_MCSkelProtocol::KMC_MCSkelProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type)
    : KMediaProtocal(a_parentContainer, a_name, a_type)
{
}

KMC_MCSkelProtocol::~KMC_MCSkelProtocol()
{
}

int KMC_MCSkelProtocol::scanDevice()
{
    return EC_NOT_SUPPORT;
}

kbool KMC_MCSkelProtocol::start()
{
    return KMediaProtocal::start();
}

kbool KMC_MCSkelProtocol::stop()
{
    return KMediaProtocal::stop();
}

