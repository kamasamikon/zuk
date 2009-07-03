/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef KMCPROTOCAL_H_
#define KMCPROTOCAL_H_

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include <kflg.h>
#include <kdbg.h>
#include "kerrcode.h"

class KMediaContainer;

#define MC_PRO_FLG_STARTED   0x00000001

class KMediaProtocal
{
public:
    KMediaProtocal(KMediaContainer* a_parentContainer, char* a_name, int a_type);
    virtual ~KMediaProtocal(void);

    virtual char* getHash(void) = 0;
    void setHash(const char *a_hash) { if (hash[0]) kerror(("Already setHash, can not set!\n")); else memcpy(hash, a_hash, 33); }
    const char* getName(void) { return name; }
    const char* getDesc(void) { return desc; }

    KMediaContainer *getContainer(void) const { return parentContainer; }

    int getType(void) { return type; };

    virtual kbool start(void) { kuint of = flg; kflg_set(flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = flg; kflg_clr(flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }
    kbool isStarted(void) { return kflg_chk(flg, MC_PRO_FLG_STARTED) ? true : false; }

    char** getMediaDeviceList(void);

    char** getMediaChannelList(void);
    char** getMediaChannelList(unsigned int class_mask);

    virtual int scanDevice(void);

public:
    K_dlist_entry deviceHeader;
    K_dlist_entry protocalEntry;

protected:
    char* desc;

private:
    KMediaContainer* parentContainer;
    char* name;
    char hash[33];

    kuint flg;
    int type;
};

#endif /*KMCPROTOCAL_H_*/

