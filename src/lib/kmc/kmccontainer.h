/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef KMCCONTAINER_H_
#define KMCCONTAINER_H_

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmcprotocal.h"
#include "kmcdevice.h"
#include "kmcchannel.h"

class KMediaContainer
{
public:
    KMediaContainer(KIM* im, char* a_name);
    virtual ~KMediaContainer(void);

    const char* getName(void) { return m_name; }

    KMediaProtocol* getMediaProtocalFromProtocal(char* protHash);
    KMediaProtocol* getMediaProtocalFromDevice(char* devHash);
    KMediaProtocol* getMediaProtocalFromChannel(char* chHash);

    KMediaDevice* getMediaDeviceFromDevice(char* devHash);
    KMediaDevice* getMediaDeviceFromChannel(char* chHash);

    KMediaChannel* getMediaChannelFromChannel(char* chHash);

    /** protocol hash list, 0 end */
    char** getMediaProtocalHashList(void);
    KMediaProtocol** getMediaProtocalClassList(void);

    /** device hash list, 0 end */
    char** getMediaDeviceHashList(void);
    KMediaDevice** getMediaDeviceClassList(void);

    /** channel hash list, 0 end */
    char** getMediaChannelHashList(void);
    KMediaChannel** getMediaChannelClassList(void);

public:
    K_dlist_entry protocalHeader;

private:
    char* m_name;
    KIM* m_im;
};

#endif /*KMCCONTAINER_H_*/

