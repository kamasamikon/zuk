/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_LOCAL_DEVICE_H__
#define __KMC_LOCAL_DEVICE_H__

#include <kmccontainer.h>
#include "local-protocol.h"

class KMC_LocalDevice : public KMediaDevice
{
public:
    KMC_LocalDevice(KIM *a_im, KMC_LocalProtocol* a_parentProtocal, char* a_name);
    virtual ~KMC_LocalDevice(void);

    virtual char* getHash(void) = 0;

    virtual int remove(void) { return EC_NOT_SUPPORT; }

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp) { return EC_NOT_SUPPORT; }
    virtual int getSignalAmp(int *a_pamp) { return EC_NOT_SUPPORT; }

    virtual int updateChannelList(void) { return EC_NOT_SUPPORT; }
    virtual int cancelUpdateChannelList(void) { return EC_NOT_SUPPORT; }

private:
};

#endif /* __KMC_LOCAL_DEVICE_H__ */
