/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaDevice::KMediaDevice(KMediaProtocol* a_parentProtocal, const char* a_name)
{
    init_dlist_head(&m_channelHeader);
    init_dlist_head(&m_deviceEntry);
    m_parentProtocal = a_parentProtocal;
    m_name = kstr_dup(a_name);
    m_flg = 0;
    m_desc = NULL;

    insert_dlist_tail_entry(&m_parentProtocal->m_deviceHeader, &m_deviceEntry);
}

KMediaDevice::~KMediaDevice(void)
{
    kmem_free_s(m_name);
    kmem_free_s(m_desc);

    K_dlist_entry* chEntry;
    KMediaChannel* ch;

    chEntry = m_channelHeader.next;
    while (chEntry != &m_channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
        chEntry = chEntry->next;

        delete ch;
    }

    /* device should stopped before call this */
    remove_dlist_entry(&m_deviceEntry);
}

char** KMediaDevice::getMediaChannelHashList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* chEntry;
    KMediaChannel* ch;

    if (is_dlist_empty(&m_channelHeader))
        return NULL;

    chEntry = m_channelHeader.next;
    while (chEntry != &m_channelHeader) {
        cnt++;
        chEntry = chEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    chEntry = m_channelHeader.next;
    while (chEntry != &m_channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
        hashList[index++] = kstr_dup(ch->getHash());
        chEntry = chEntry->next;
    }
    hashList[index++] = NULL;

    return hashList;
}

KMediaChannel** KMediaDevice::getMediaChannelClassList(void)
{
    int cnt = 0, index = 0;
    KMediaChannel **channelList;
    K_dlist_entry* chEntry;
    KMediaChannel* ch;

    if (is_dlist_empty(&m_channelHeader))
        return NULL;

    chEntry = m_channelHeader.next;
    while (chEntry != &m_channelHeader) {
        cnt++;
        chEntry = chEntry->next;
    }

    channelList = (KMediaChannel**)kmem_alloc(sizeof(KMediaChannel*) * (cnt + 1));
    channelList[cnt] = NULL;

    chEntry = m_channelHeader.next;
    while (chEntry != &m_channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
        channelList[index++] = ch;
        chEntry = chEntry->next;
    }
    channelList[index++] = NULL;

    return channelList;
}

