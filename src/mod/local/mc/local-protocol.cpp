/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <kmem.h>
#include <kstr.h>

#include <kmccontainer.h>
#include "local-protocol.h"

KMC_LocalProtocol::KMC_LocalProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type)
    : KMediaProtocal(a_parentContainer, a_name, a_type)
{
}

KMC_LocalProtocol::~KMC_LocalProtocol()
{
}

int KMC_LocalProtocol::scanDevice()
{
    return EC_NOT_SUPPORT;
}

kbool KMC_LocalProtocol::start()
{
    return KMediaProtocal::start();
}

kbool KMC_LocalProtocol::stop()
{
    return KMediaProtocal::stop();
}

