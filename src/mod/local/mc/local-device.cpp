/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "local-device.h"

KMC_LocalDevice::KMC_LocalDevice(KIM *a_im, KMC_LocalProtocol* a_parentProtocal, char* a_name)
    : KMediaDevice(a_im, a_parentProtocal, a_name)
{
    kim_setint(im(), "i.kmc.evt.device.new", 1, (void**)this, knil);
}

KMC_LocalDevice::~KMC_LocalDevice()
{
    kim_setint(im(), "i.kmc.evt.device.del", 1, (void**)this, knil);
}

