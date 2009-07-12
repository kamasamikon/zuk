/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaChannel::KMediaChannel(KMediaDevice* a_parentDevice, const char* a_name)
{
    init_dlist_head(&m_channelEntry);
    m_parentDevice = a_parentDevice;
    m_name = kstr_dup(a_name);
    m_flg = 0;

    insert_dlist_tail_entry(&m_parentDevice->m_channelHeader, &m_channelEntry);
}

KMediaChannel::~KMediaChannel(void)
{
    kmem_free_s(m_name);
    remove_dlist_entry(&m_channelEntry);
}

