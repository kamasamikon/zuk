/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaProtocol::KMediaProtocol(KIM *a_im, KMediaContainer* a_parentContainer, const char* a_name, int a_type)
{
    init_dlist_head(&m_deviceHeader);
    init_dlist_head(&m_protocolEntry);

    m_im = a_im;
    m_hash[0] = '\0';
    m_parentContainer = a_parentContainer;
    m_name = kstr_dup(a_name);
    m_desc = NULL;
    m_type = a_type;

    insert_dlist_tail_entry(&m_parentContainer->m_protocolHeader, &m_protocolEntry);
}

KMediaProtocol::~KMediaProtocol(void)
{
    kmem_free_s(m_name);
    kmem_free_s(m_desc);

    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
        devEntry = devEntry->next;

        delete dev;
    }

    /* protocol should stopped before call this */
    remove_dlist_entry(&m_protocolEntry);
}

KIM* KMediaProtocol::im()
{
    return m_im;
}

const char* KMediaProtocol::getHash(void)
{
    return m_hash;
}

void KMediaProtocol::setHash(const char *a_hash)
{
    if (m_hash[0])
        kerror(("Already setHash, can not set!\n"));
    else
        memcpy(m_hash, a_hash, 33);
}

const char* KMediaProtocol::getName(void)
{
    return m_name;
}

const char* KMediaProtocol::getDesc(void)
{
    return m_desc;
}

KMediaContainer *KMediaProtocol::getContainer(void) const
{
    return m_parentContainer;
}

int KMediaProtocol::getType(void)
{
    return m_type;
}

kbool KMediaProtocol::start(void)
{
    kuint of = m_flg;

    kflg_set(m_flg, MC_PRO_FLG_STARTED);
    return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false;
}
kbool KMediaProtocol::stop(void)
{
    kuint of = m_flg;

    kflg_clr(m_flg, MC_PRO_FLG_STARTED);
    return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false;
}
kbool KMediaProtocol::isStarted(void)
{
    return kflg_chk(m_flg, MC_PRO_FLG_STARTED) ? true : false;
}

int KMediaProtocol::scanDevice(void)
{
    return EC_NOT_SUPPORT;
}

char** KMediaProtocol::getMediaDeviceHashList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&m_deviceHeader))
        return NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        cnt++;
        devEntry = devEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
        hashList[index++] = kstr_dup(dev->getHash());
        devEntry = devEntry->next;
    }
    hashList[index++] = NULL;

    return hashList;
}

KMediaDevice** KMediaProtocol::getMediaDeviceClassList(void)
{
    int cnt = 0, index = 0;
    KMediaDevice **deviceList;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&m_deviceHeader))
        return NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        cnt++;
        devEntry = devEntry->next;
    }

    deviceList = (KMediaDevice**)kmem_alloc(sizeof(char*) * (cnt + 1));
    deviceList[cnt] = NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
        deviceList[index++] = dev;
        devEntry = devEntry->next;
    }
    deviceList[index++] = NULL;

    return deviceList;
}

char** KMediaProtocol::getMediaChannelHashList(void)
{
    int devCnt = 0, chCnt = 0, devIndex = 0, chIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&m_deviceHeader))
        return NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        devCnt++;
        devEntry = devEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * devCnt);

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
        hashListList[devIndex++] = dev->getMediaChannelHashList();
        devEntry = devEntry->next;
    }

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        for (int i = 0; hashListList[devIndex]&&hashListList[devIndex][i]; i++)
            chCnt++;

    hashList = (char**)kmem_alloc(sizeof(char*) * (chCnt + 1));
    hashList[chCnt] = NULL;

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        for (int i = 0; hashListList[devIndex]&&hashListList[devIndex][i]; i++) {
            hashList[chIndex++] = hashListList[devIndex][i];
            chCnt++;
        }

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        kmem_free_s(hashListList[devIndex]);

    kmem_free_s(hashListList);

    return hashList;
}

KMediaChannel** KMediaProtocol::getMediaChannelClassList(void)
{
    int devCnt = 0, chCnt = 0, devIndex = 0, chIndex = 0;
    KMediaChannel **deviceList = NULL;
    KMediaChannel ***deviceListList = NULL;
    K_dlist_entry* devEntry;
    KMediaDevice* dev;

    if (is_dlist_empty(&m_deviceHeader))
        return NULL;

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        devCnt++;
        devEntry = devEntry->next;
    }

    deviceListList = (KMediaChannel***)kmem_alloc(sizeof(KMediaChannel**) * devCnt);

    devEntry = m_deviceHeader.next;
    while (devEntry != &m_deviceHeader) {
        dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
        deviceListList[devIndex++] = dev->getMediaChannelClassList();
        devEntry = devEntry->next;
    }

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        for (int i = 0; deviceListList[devIndex]&&deviceListList[devIndex][i]; i++)
            chCnt++;

    deviceList = (KMediaChannel**)kmem_alloc(sizeof(KMediaChannel*) * (chCnt + 1));
    deviceList[chCnt] = NULL;

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        for (int i = 0; deviceListList[devIndex]&&deviceListList[devIndex][i]; i++) {
            deviceList[chIndex++] = deviceListList[devIndex][i];
            chCnt++;
        }

    for (devIndex = 0; devIndex < devCnt; devIndex++)
        kmem_free_s(deviceListList[devIndex]);

    kmem_free_s(deviceListList);

    return deviceList;
}

