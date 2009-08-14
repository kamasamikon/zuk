/* vim:set et sw=4 sts=4 ff=unix: */
#include <stdio.h>

#include <kmem.h>
#include <kstr.h>
#include <md5sum.h>

#include "kmccontainer.h"
#include "local-device.h"
#include "local-channel.h"

KMC_LocalDevice::KMC_LocalDevice(KIM *a_im, KMC_LocalProtocol* a_parentProtocal, char* a_name) :
    KMediaDevice(a_im, a_parentProtocal, a_name)
{
    char *src = "KMC_LocalDevice";
    setHash((char*)md5_calculate((const unsigned char*)src, strlen(src)));

    m_backend = 0;

    kim_setint(im(), "i.kmc.evt.device.new", 1, (void**)this, knil);
}

KMC_LocalDevice::~KMC_LocalDevice()
{
    kim_setint(im(), "i.kmc.evt.device.del", 1, (void**)this, knil);
}

kbool KMC_LocalDevice::start()
{
    if (!m_backend)
        m_backend = backend_init(NULL, NULL);

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
    KMC_LocalChannel *channel;
    kchar *baseDir;

    KVFS_FINDDATA finfo;
    kbean fd = 0;

    kchar fullpath[256];    /* for findfirst *.* */

    kchar ps[2] = { 0 };
    ps[0] = kvfs_path_sep();

    baseDir = "/home/auv";
    sprintf(fullpath, "%s", baseDir);

    fd = kvfs_findfirst(fullpath, &finfo);
    if (fd) {
        do {
            if (0 != strcmp(finfo.name, ".") && 0 != strcmp(finfo.name, "..")) {
                if (strstr(finfo.name, ".flv") || strstr(finfo.name, ".mp3")) {
                    sprintf(fullpath, "%s%s%s", baseDir, ps, finfo.name);
                    klog(("found: file:%s\n", fullpath));
                    KMC_LocalChannel *channel = new KMC_LocalChannel(im(), this, finfo.name, fullpath);
                    channel->setBackend(m_backend);
                }
            }
        } while (-1 != kvfs_findnext(fd, &finfo));
    }

    return EC_OK;
}
int KMC_LocalDevice::cancelUpdateChannelList(void)
{
    return EC_NOT_SUPPORT;
}

