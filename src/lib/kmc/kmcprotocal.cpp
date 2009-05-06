/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaProtocal::KMediaProtocal(KMediaContainer* a_parentContainer, char* a_name, int a_type)
{
    init_dlist_head(&deviceHeader);
    init_dlist_head(&protocalEntry);
    parentContainer = a_parentContainer;
    name = kstr_dup(a_name);
    desc = NULL;
    type = a_type;

    insert_dlist_tail_entry(&parentContainer->protocalHeader, &protocalEntry);
}

KMediaProtocal::~KMediaProtocal(void)
{
    kmem_free_s(name);
    kmem_free_s(desc);

    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    devEntry = deviceHeader.next;
    while (devEntry != &deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);
        devEntry = devEntry->next;

        delete dev;
    }

    /* protocol should stopped before call this */
    remove_dlist_entry(&protocalEntry);
}

char** KMediaProtocal::getMediaDeviceList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&deviceHeader)) {
        return NULL;
    }

    devEntry = deviceHeader.next;
    while (devEntry != &deviceHeader) {
        cnt++;
        devEntry = devEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    devEntry = deviceHeader.next;
    while (devEntry != &deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);
        hashList[index++] = kstr_dup(dev->getHash());
        devEntry = devEntry->next;
    }
    hashList[index++] = NULL;

    return hashList;
}

char** KMediaProtocal::getMediaChannelList(unsigned int class_mask)
{
    return NULL;
}

char** KMediaProtocal::getMediaChannelList(void)
{
    int devCnt = 0, chCnt = 0, devIndex = 0, chIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&deviceHeader)) {
        return NULL;
    }

    devEntry = deviceHeader.next;
    while (devEntry != &deviceHeader) {
        devCnt++;
        devEntry = devEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * devCnt);

    devEntry = deviceHeader.next;
    while (devEntry != &deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);
        hashListList[devIndex++] = dev->getMediaChannelList();
        devEntry = devEntry->next;
    }

    for (devIndex = 0; devIndex < devCnt; devIndex++) {
        for (int i = 0; hashListList[devIndex]&&hashListList[devIndex][i]; i++) {
            chCnt++;
        }
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (chCnt + 1));
    hashList[chCnt] = NULL;

    for (devIndex = 0; devIndex < devCnt; devIndex++) {
        for (int i = 0; hashListList[devIndex]&&hashListList[devIndex][i]; i++) {
            hashList[chIndex++] = hashListList[devIndex][i];
            chCnt++;
        }
    }

    for (devIndex = 0; devIndex < devCnt; devIndex++) {
        kmem_free_s(hashListList[devIndex]);
    }
    kmem_free_s(hashListList);

    return hashList;
}

int KMediaProtocal::scanDevice(void)
{
    return EC_NOT_SUPPORT;
}

