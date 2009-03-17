#ifndef KMCDEVICE_H_
#define KMCDEVICE_H_

#include <string.h>
#include <stdlib.h>
#include <sdlist.h>
#include <kim.h>
#include <kflg.h>
#include "kerrcode.h"

class KMediaProtocal;

#define MC_DEV_FLG_STARTED   0x00000001
#define MC_DEV_FLG_DEFREEZED 0x00000002
#define MC_DEV_FLG_ENABLE    0x00000004

/* deviec type define
   type  =0   reserved
   1   broadcast
   2   web
   3   local::record
*/

class KMediaDevice
{
public:
    KMediaDevice(KMediaProtocal* a_parentProtocal, char* a_name);
    virtual ~KMediaDevice(void);

    virtual void* allocMemory(unsigned int size) { return NULL; }
    virtual void freeMemory(void *ptr) {}

    virtual char* getHash(void) = 0;
    const char* getName(void) { return name; }
    const char* getDesc(void) { return desc; }

    int getType(void) { return type; };

    KMediaProtocal *getProtocal(void) const { return parentProtocal; }

    virtual kbool start(void) { kuint of = flg; kflg_set(flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = flg; kflg_clr(flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }
    kbool isStarted(void) { return kflg_chk(flg, MC_DEV_FLG_STARTED) ? true : false; }

    kbool freeze(void) { kuint of = flg; kflg_set(flg, MC_DEV_FLG_DEFREEZED); return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false; }
    kbool defreeze(void) { kuint of = flg; kflg_clr(flg, MC_DEV_FLG_DEFREEZED); return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false; }
    kbool isFreeze(void) { return kflg_chk(flg, MC_DEV_FLG_DEFREEZED) ? true : false; }

    virtual int remove(void) { return EC_NOT_SUPPORT; }

    /** all channel hash belong to this device, with 0 end */
    char** getMediaChannelList(void);
    char** getMediaChannelList(unsigned int class_mask);

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp) { return EC_NOT_SUPPORT; }
    virtual int getSignalAmp(int *a_pamp) { return EC_NOT_SUPPORT; }

    virtual int updateChannelList(void) { return EC_NOT_SUPPORT; }
    virtual int updateChannelList(unsigned int class_mask, unsigned int type_mask) { return EC_NOT_SUPPORT; }
    virtual int cancelUpdateChannelList(void) { return EC_NOT_SUPPORT; }

public:
    K_dlist_entry channelHeader;
    K_dlist_entry deviceEntry;

    char* desc;
    int  type;

private:
    KMediaProtocal* parentProtocal;
    char* name;

    kuint flg;
};

#endif /*KMCDEVICE_H_*/
