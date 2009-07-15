/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_CONTAINER_H__
#define __KMC_CONTAINER_H__

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmcprotocol.h"
#include "kmcdevice.h"
#include "kmcchannel.h"

typedef struct _KMediaContainerHeader KMediaContainerHeader;
struct _KMediaContainerHeader {
    K_dlist_entry m_protocolHeader;
};

class KMediaContainer : public KMediaContainerHeader
{
public:
    KMediaContainer(KIM *a_im, const char* a_name);
    virtual ~KMediaContainer(void);

    KIM *im();

    const char* getName(void);

    KMediaProtocol* getMediaProtocolFromProtocol(const char* protHash);
    KMediaProtocol* getMediaProtocolFromDevice(const char* devHash);
    KMediaProtocol* getMediaProtocolFromChannel(const char* chHash);

    KMediaDevice* getMediaDeviceFromDevice(const char* devHash);
    KMediaDevice* getMediaDeviceFromChannel(const char* chHash);

    KMediaChannel* getMediaChannelFromChannel(const char* chHash);

    /** protocol hash list, 0 end */
    char** getMediaProtocolHashList(void);
    KMediaProtocol** getMediaProtocolClassList(void);

    /** device hash list, 0 end */
    char** getMediaDeviceHashList(void);
    KMediaDevice** getMediaDeviceClassList(void);

    /** channel hash list, 0 end */
    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

private:
    char* m_name;
    KIM* m_im;
};

#endif /*__KMC_CONTAINER_H__*/

