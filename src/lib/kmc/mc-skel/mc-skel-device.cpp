/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "MCSkel-device.h"

KMC_MCSkelDevice::KMC_MCSkelDevice(KMC_MCSkelProtocol* a_parentProtocal, char* a_name)
    : KMediaDevice(a_parentProtocal, a_name)
{
}

KMC_MCSkelDevice::~KMC_MCSkelDevice()
{
}

