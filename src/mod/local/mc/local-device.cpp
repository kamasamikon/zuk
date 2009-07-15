/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <md5sum.h>

#include "kmccontainer.h"
#include "local-device.h"

KMC_LocalDevice::KMC_LocalDevice(KIM *a_im, KMC_LocalProtocol* a_parentProtocal, char* a_name) :
    KMediaDevice(a_im, a_parentProtocal, a_name)
{
    char *src = "KMC_LocalDevice";
    setHash((char*)md5_calculate((const unsigned char*)src, strlen(src)));

    kim_setint(im(), "i.kmc.evt.device.new", 1, (void**)this, knil);
}

KMC_LocalDevice::~KMC_LocalDevice()
{
    kim_setint(im(), "i.kmc.evt.device.del", 1, (void**)this, knil);
}

kbool KMC_LocalDevice::start()
{
    return KMediaDevice::start();
}

kbool KMC_LocalDevice::stop()
{
    return KMediaDevice::stop();
}

int KMC_LocalDevice::remove(void)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalDevice::setSignalAmp(int a_amp)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalDevice::getSignalAmp(int *a_pamp)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalDevice::updateChannelList(void)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalDevice::cancelUpdateChannelList(void)
{
    return EC_NOT_SUPPORT;
}

