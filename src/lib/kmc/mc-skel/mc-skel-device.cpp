/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "MCSkel-device.h"

KMC_MCSkelDevice::KMC_MCSkelDevice(KMC_MCSkelProtocol* a_parentProtocal, char* a_name)
    : KMediaDevice(a_parentProtocal, a_name)
{
    kim_setint(__g_im, "i.kmc.evt.device.new", 1, (void**)this, knil);
}

KMC_MCSkelDevice::~KMC_MCSkelDevice()
{
    kim_setint(__g_im, "i.kmc.evt.device.del", 1, (void**)this, knil);
}

