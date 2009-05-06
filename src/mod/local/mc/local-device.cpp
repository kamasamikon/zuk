/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "local-device.h"

KMC_LocalDevice::KMC_LocalDevice(KMC_LocalProtocol* a_parentProtocal, char* a_name)
    : KMediaDevice(a_parentProtocal, a_name)
{
}

KMC_LocalDevice::~KMC_LocalDevice()
{
}

