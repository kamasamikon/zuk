/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <md5sum.h>

#include <kmccontainer.h>
#include "local-protocol.h"

KMC_LocalProtocol::KMC_LocalProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type)
    : KMediaProtocol(a_parentContainer, a_name, a_type)
{
}

KMC_LocalProtocol::~KMC_LocalProtocol()
{
}

char* KMC_LocalProtocol::getHash(void)
{
    char *src = "KMC_LocalProtocol";
    return (char*)md5_calculate((const unsigned char*)src, strlen(src));
}

int KMC_LocalProtocol::scanDevice()
{
    return EC_NOT_SUPPORT;
}

kbool KMC_LocalProtocol::start()
{
    return KMediaProtocol::start();
}

kbool KMC_LocalProtocol::stop()
{
    return KMediaProtocol::stop();
}

