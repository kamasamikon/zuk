/* vim:set et sw=4 sts=4: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMC_LocalProtocol::KMC_LocalProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type)
{
}

KMC_LocalProtocol::~KMC_LocalProtocol(void)
{
}

int KMC_LocalProtocol::scanDevice(void)
{
    return EC_NOT_SUPPORT;
}

