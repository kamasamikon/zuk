#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaChannel::KMediaChannel(KMediaDevice* a_parentDevice, char* a_name)
{
    init_dlist_head(&channelEntry);
    parentDevice = a_parentDevice;
    name = kstr_dup(a_name);
    flg = 0;

    insert_dlist_tail_entry(&parentDevice->channelHeader, &channelEntry);
}

KMediaChannel::~KMediaChannel(void)
{
    kmem_free_s(name);
    remove_dlist_entry(&channelEntry);
}

