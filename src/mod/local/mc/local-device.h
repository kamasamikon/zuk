/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __KMC_LOCAL_DEVICE_H__
#define __KMC_LOCAL_DEVICE_H__

#include <kmccontainer.h>
#include "local-protocol.h"

#include "../be/gst-backend.h"

class KMC_LocalDevice : public KMediaDevice
{
public:
    KMC_LocalDevice(KIM *a_im, KMC_LocalProtocol *a_parentProtocal, char *a_name);
    virtual ~ KMC_LocalDevice(void);

    virtual kbool start();
    virtual kbool stop();

    virtual int remove(void);

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp);
    virtual int getSignalAmp(int *a_pamp);

    virtual int updateChannelList(void);
    virtual int cancelUpdateChannelList(void);

private:
    void *m_backend;
};

#endif /* __KMC_LOCAL_DEVICE_H__ */
