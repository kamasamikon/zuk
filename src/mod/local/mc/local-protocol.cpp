/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <md5sum.h>

#include <kmccontainer.h>
#include "local-protocol.h"
#include "local-device.h"

KMC_LocalProtocol::KMC_LocalProtocol(KIM *a_im, KMediaContainer* a_parentContainer, char* a_name, int a_type) :
    KMediaProtocol(a_im, a_parentContainer, a_name, a_type)
{
    char *src = "KMC_LocalProtocol";
    setHash((char*)md5_calculate((const unsigned char*)src, strlen(src)));

    kim_setint(im(), "i.kmc.evt.protocol.new", 1, (void**)this, knil);
}

KMC_LocalProtocol::~KMC_LocalProtocol()
{
    kim_setint(im(), "i.kmc.evt.protocol.del", 1, (void**)this, knil);
}

int KMC_LocalProtocol::scanDevice()
{
    KMC_LocalDevice *localDevice = new KMC_LocalDevice(im(), this, "localDevice");
    localDevice->start();
    localDevice->updateChannelList();
    return EC_OK;
}

kbool KMC_LocalProtocol::start()
{
    return KMediaProtocol::start();
}

kbool KMC_LocalProtocol::stop()
{
    return KMediaProtocol::stop();
}

