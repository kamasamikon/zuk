/* vim:set et sw=4 sts=4: */
#ifndef __KMC_LOCAL_DEVICE_H__
#define __KMC_LOCAL_DEVICE_H__

#include <kmccontainer.h>

class KMC_LocalDevice
{
public:
    KMC_LocalDevice(KMC_LocalProtocol* a_parentProtocal, char* a_name);
    virtual ~KMC_LocalDevice(void);

    virtual char* getHash(void) = 0;

    virtual kbool start(void) { kuint of = flg; kflg_set(flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = flg; kflg_clr(flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }

    virtual int remove(void) { return EC_NOT_SUPPORT; }

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp) { return EC_NOT_SUPPORT; }
    virtual int getSignalAmp(int *a_pamp) { return EC_NOT_SUPPORT; }

    virtual int updateChannelList(void) { return EC_NOT_SUPPORT; }
    virtual int updateChannelList(unsigned int class_mask, unsigned int type_mask) { return EC_NOT_SUPPORT; }
    virtual int cancelUpdateChannelList(void) { return EC_NOT_SUPPORT; }

private:
};

#endif /* __KMC_LOCAL_DEVICE_H__ */