/* vim:set et sw=4 sts=4: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMC_LocalDevice::KMC_LocalDevice(KMC_LocalProtocol* a_parentProtocal, char* a_name)
{
    init_dlist_head(&channelHeader);
    init_dlist_head(&deviceEntry);
    parentProtocal = a_parentProtocal;
    name = kstr_dup(a_name);
    flg = 0;
    desc = NULL;

    insert_dlist_tail_entry(&parentProtocal->deviceHeader, &deviceEntry);
}

KMC_LocalDevice::~KMC_LocalDevice(void)
{
    kmem_free_s(name);
    kmem_free_s(desc);

    K_dlist_entry* chEntry;
    KMC_LocalChannel* ch;

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMC_LocalChannel, channelEntry);
        chEntry = chEntry->next;

        delete ch;
    }

    /* device should stopped before call this */
    remove_dlist_entry(&deviceEntry);
}

char** KMC_LocalDevice::getMediaChannelList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* chEntry;
    KMC_LocalChannel* ch;

    if (is_dlist_empty(&channelHeader)) {
        return NULL;
    }

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        cnt++;
        chEntry = chEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMC_LocalChannel, channelEntry);
        hashList[index++] = kstr_dup(ch->getHash());
        chEntry = chEntry->next;
    }
    hashList[index++] = NULL;

    return hashList;
}

