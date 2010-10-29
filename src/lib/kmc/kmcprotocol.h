/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

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

typedef struct _KMediaProtocolHeader KMediaProtocolHeader;
struct _KMediaProtocolHeader {
    K_dlist_entry m_deviceHeader;
    K_dlist_entry m_protocolEntry;
};

class KMediaProtocol : public KMediaProtocolHeader
{
public:
    KMediaProtocol(KIM *a_im, KMediaContainer* a_parentContainer, const char* a_name, int a_type);
    virtual ~KMediaProtocol(void);

    KIM *im();

    const char* getHash(void);
    void setHash(const char *a_hash);
    const char* getName(void);
    const char* getDesc(void);

    KMediaContainer *getContainer(void) const;

    int getType(void);

    virtual kbool start(void);
    virtual kbool stop(void);
    kbool isStarted(void);

    char** getMediaDeviceHashList(void);
    KMediaDevice** getMediaDeviceClassList(void);

    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

    virtual int scanDevice(void);

protected:
    char* m_desc;

private:
    KIM* m_im;

    KMediaContainer* m_parentContainer;
    char* m_name;
    char m_hash[33];

    kuint m_flg;
    int m_type;
};

#endif /*__KMC_PROTOCOL_H__*/

