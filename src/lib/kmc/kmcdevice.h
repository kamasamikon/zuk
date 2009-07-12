/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_DEVICE_H__
#define __KMC_DEVICE_H__

#include <string.h>
#include <stdlib.h>
#include <sdlist.h>
#include <kim.h>
#include <kflg.h>
#include <kdbg.h>
#include "kerrcode.h"

class KMediaProtocol;

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
    KMediaDevice(KMediaProtocol* a_parentProtocal, const char* a_name);
    virtual ~KMediaDevice(void);

    virtual char* getHash(void) = 0;
    void setHash(const char *a_hash) { if (m_hash[0]) kerror(("Already setHash, can not set!\n")); else memcpy(m_hash, a_hash, 33); }
    const char* getName(void) { return m_name; }
    const char* getDesc(void) { return m_desc; }

    int getType(void) { return m_type; };

    KMediaProtocol *getProtocal(void) const { return m_parentProtocal; }

    virtual kbool start(void) { kuint of = m_flg; kflg_set(m_flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = m_flg; kflg_clr(m_flg, MC_DEV_FLG_STARTED); return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false; }
    kbool isStarted(void) { return kflg_chk(m_flg, MC_DEV_FLG_STARTED) ? true : false; }

    kbool freeze(void) { kuint of = m_flg; kflg_set(m_flg, MC_DEV_FLG_DEFREEZED); return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false; }
    kbool defreeze(void) { kuint of = m_flg; kflg_clr(m_flg, MC_DEV_FLG_DEFREEZED); return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false; }
    kbool isFreeze(void) { return kflg_chk(m_flg, MC_DEV_FLG_DEFREEZED) ? true : false; }

    virtual int remove(void) { return EC_NOT_SUPPORT; }

    /** all channel hash belong to this device, with 0 end */
    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp) { return EC_NOT_SUPPORT; }
    virtual int getSignalAmp(int *a_pamp) { return EC_NOT_SUPPORT; }

    virtual int updateChannelList(void) { return EC_NOT_SUPPORT; }
    virtual int updateChannelList(unsigned int class_mask, unsigned int type_mask) { return EC_NOT_SUPPORT; }
    virtual int cancelUpdateChannelList(void) { return EC_NOT_SUPPORT; }

public:
    K_dlist_entry m_channelHeader;
    K_dlist_entry m_deviceEntry;

    char* m_desc;
    int  m_type;

private:
    KMediaProtocol* m_parentProtocal;
    char* m_name;
    char m_hash[33];

    kuint m_flg;
};

#endif /*__KMC_DEVICE_H__*/
