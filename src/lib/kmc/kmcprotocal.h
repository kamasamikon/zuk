/* vim:set et sw=4 sts=4: */
#ifndef KMCPROTOCAL_H_
#define KMCPROTOCAL_H_

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include <kflg.h>
#include "kerrcode.h"

class KMediaContainer;

#define MC_PRO_FLG_STARTED   0x00000001

class KMediaProtocal
{
public:
    //
    //a_type ---> 0   reserve;
    //       ---> 1   dvb-c
    //        ----> 2   dvb-s
    //        ----> 3   dvb-t
    //        ----> 4   dvb-h
    //       ---> 5   dmb-t
    //        ----> 6   streaming media
    //        ----> 7      cmmb
    //        ----> 8

    KMediaProtocal(KMediaContainer* a_parentContainer, char* a_name, int a_type);
    virtual ~KMediaProtocal(void);

    virtual void* allocMemory(unsigned int size) { return NULL; }
    virtual void freeMemory(void *ptr) {}

    virtual char* getHash(void) = 0;
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

    kuint flg;
    int type;
};

#endif /*KMCPROTOCAL_H_*/

