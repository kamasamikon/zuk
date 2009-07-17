/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaDevice::KMediaDevice(KIM *a_im, KMediaProtocol* a_parentProtocal, const char* a_name)
{
    init_dlist_head(&m_channelHeader);
    init_dlist_head(&m_deviceEntry);

    m_im = a_im;
    m_hash[0] = '\0';
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

KIM* KMediaDevice::im()
{
    return m_im;
}

const char* KMediaDevice::getHash(void)
{
    return m_hash;
}
void KMediaDevice::setHash(const char *a_hash)
{
    if (m_hash[0])
        kerror(("Already setHash, can not set!\n"));
    else
        memcpy(m_hash, a_hash, 33);
}
const char* KMediaDevice::getName(void)
{
    return m_name;
}
const char* KMediaDevice::getDesc(void)
{
    return m_desc;
}

int KMediaDevice::getType(void)
{
    return m_type;
}

KMediaProtocol *KMediaDevice::getProtocal(void) const
{
    return m_parentProtocal;
}

kbool KMediaDevice::start(void)
{
    kuint of = m_flg;
    kflg_set(m_flg, MC_DEV_FLG_STARTED);
    return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false;
}
kbool KMediaDevice::stop(void)
{
    kuint of = m_flg;
    kflg_clr(m_flg, MC_DEV_FLG_STARTED);
    return kflg_chk(of, MC_DEV_FLG_STARTED) ? true : false;
}
kbool KMediaDevice::isStarted(void)
{
    return kflg_chk(m_flg, MC_DEV_FLG_STARTED) ? true : false;
}

kbool KMediaDevice::freeze(void)
{
    kuint of = m_flg;
    kflg_set(m_flg, MC_DEV_FLG_DEFREEZED);
    return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false;
}
kbool KMediaDevice::defreeze(void)
{
    kuint of = m_flg;
    kflg_clr(m_flg, MC_DEV_FLG_DEFREEZED);
    return kflg_chk(of, MC_DEV_FLG_DEFREEZED) ? true : false;
}
kbool KMediaDevice::isFreeze(void)
{
    return kflg_chk(m_flg, MC_DEV_FLG_DEFREEZED) ? true : false;
}

int KMediaDevice::remove(void)
{
    return EC_NOT_SUPPORT;
}

int KMediaDevice::setSignalAmp(int a_amp)
{
    return EC_NOT_SUPPORT;
}
int KMediaDevice::getSignalAmp(int *a_pamp)
{
    return EC_NOT_SUPPORT;
}

int KMediaDevice::updateChannelList(void)
{
    return EC_NOT_SUPPORT;
}
int KMediaDevice::cancelUpdateChannelList(void)
{
    return EC_NOT_SUPPORT;
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

