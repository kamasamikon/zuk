/* vim:set et sw=4 sts=4: */
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

    const char* getName(void) { return name; }

    KMediaProtocal* getMediaProtocalFromProtocal(char* protHash);
    KMediaProtocal* getMediaProtocalFromDevice(char* devHash);
    KMediaProtocal* getMediaProtocalFromChannel(char* chHash);

    KMediaDevice* getMediaDeviceFromDevice(char* devHash);
    KMediaDevice* getMediaDeviceFromChannel(char* chHash);

    KMediaChannel* getMediaChannelFromChannel(char* chHash);

    /** protocol hash list, 0 end */
    char** getMediaProtocalList(void);

    /** device hash list, 0 end */
    char** getMediaDeviceList(void);

    /** channel hash list, 0 end */
    char** getMediaChannelList(void);
    char** getMediaChannelList(unsigned int class_mask);

public:
    K_dlist_entry protocalHeader;

private:
    char* name;
    KIM* im;
};

#endif /*KMCCONTAINER_H_*/

