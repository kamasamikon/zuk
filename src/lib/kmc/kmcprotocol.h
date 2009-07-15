/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_PROTOCOL_H__
#define __KMC_PROTOCOL_H__

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include <kflg.h>
#include <kdbg.h>
#include "kerrcode.h"

class KMediaContainer;
class KMediaDevice;
class KMediaChannel;

#define MC_PRO_FLG_STARTED   0x00000001

class KMediaProtocol
{
public:
    KMediaProtocol(KMediaContainer* a_parentContainer, const char* a_name, int a_type);
    virtual ~KMediaProtocol(void);

    virtual char* getHash(void) = 0;
    void setHash(const char *a_hash) { if (m_hash[0]) kerror(("Already setHash, can not set!\n")); else memcpy(m_hash, a_hash, 33); }
    const char* getName(void) { return m_name; }
    const char* getDesc(void) { return m_desc; }

    KMediaContainer *getContainer(void) const { return m_parentContainer; }

    int getType(void) { return m_type; };

    virtual kbool start(void) { kuint of = m_flg; kflg_set(m_flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = m_flg; kflg_clr(m_flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }
    kbool isStarted(void) { return kflg_chk(m_flg, MC_PRO_FLG_STARTED) ? true : false; }

    char** getMediaDeviceHashList(void);
    KMediaDevice** getMediaDeviceClassList(void);

    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

    virtual int scanDevice(void) { return EC_NOT_SUPPORT; }

public:
    K_dlist_entry m_deviceHeader;
    K_dlist_entry m_protocolEntry;

protected:
    char* m_desc;

private:
    KMediaContainer* m_parentContainer;
    char* m_name;
    char m_hash[33];

    kuint m_flg;
    int m_type;
};

#endif /*__KMC_PROTOCOL_H__*/

