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

typedef struct _KMediaDeviceHeader KMediaDeviceHeader;
struct _KMediaDeviceHeader {
    K_dlist_entry m_channelHeader;
    K_dlist_entry m_deviceEntry;
};


class KMediaDevice : public KMediaDeviceHeader
{
public:
    KMediaDevice(KIM *a_im, KMediaProtocol* a_parentProtocal, const char* a_name);
    virtual ~KMediaDevice(void);

    KIM *im();

    const char* getHash(void);
    void setHash(const char *a_hash);
    const char* getName(void);
    const char* getDesc(void);

    int getType(void);;

    KMediaProtocol *getProtocal(void) const;

    virtual kbool start(void);
    virtual kbool stop(void);
    kbool isStarted(void);

    kbool freeze(void);
    kbool defreeze(void);
    kbool isFreeze(void);

    virtual int remove(void);

    /** all channel hash belong to this device, with 0 end */
    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

    /**  0 <= a_amp < 100 */
    virtual int setSignalAmp(int a_amp);
    virtual int getSignalAmp(int *a_pamp);

    virtual int updateChannelList(void);
    virtual int cancelUpdateChannelList(void);

public:
    char* m_desc;
    int  m_type;

private:
    KIM *m_im;

    KMediaProtocol* m_parentProtocal;
    char* m_name;
    char m_hash[33];

    kuint m_flg;
};

#endif /*__KMC_DEVICE_H__*/
