/* vim:set et sw=4 sts=4: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMC_LocalChannel::KMC_LocalChannel(KMC_LocalDevice* a_parentDevice, char* a_name)
{
    init_dlist_head(&channelEntry);
    parentDevice = a_parentDevice;
    name = kstr_dup(a_name);
    flg = 0;

    insert_dlist_tail_entry(&parentDevice->channelHeader, &channelEntry);
}

KMC_LocalChannel::~KMC_LocalChannel(void)
{
    kmem_free_s(name);
    remove_dlist_entry(&channelEntry);
}

