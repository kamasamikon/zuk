/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <kdbg.h>

#include "kmccontainer.h"

static struct {
    kchar *id;
    kchar *desc;
} __g_im_entry_list[] = {

    { "i.kmc.evt.channel.switchStart", "Before switch: ua = channelClass" },
    { "i.kmc.evt.channel.switchEnd", "After switch: ua = channelClass" },

    { "i.kmc.evt.protocol.new", "After append: ua = protocolClass" },
    { "i.kmc.evt.protocol.del", "Before delink and delete: ua = protocolClass" },

    { "i.kmc.evt.device.new", "After append: ua = deviceClass" },
    { "i.kmc.evt.device.froze", "After: ua = deviceClass, (ub = ture) == freeze, (ub = false) == defreeze" },
    { "i.kmc.evt.device.del", "Before delink and delete: ua = deviceClass" },

    { "i.kmc.evt.device.searchStart", "After: ua = protocolClass" },
    { "i.kmc.evt.device.searchStep", "Scan progress: ua = protocolClass, ub = percent" },
    { "i.kmc.evt.device.searchEnd", "After: ua = protocolClass" },

    { "i.kmc.evt.channel.new", "After append: ua = channelClass " },
    { "i.kmc.evt.channel.froze", "After: ua = channelClass,  (ub = ture) == freeze, (ub = false) == defreeze" },
    { "i.kmc.evt.channel.chg", "After: ua = channelClass " },
    { "i.kmc.evt.channel.del", "Before delink and delete: ua = channelClass " },

    { "i.kmc.evt.channel.searchStart", "After: ua = deviceClass" },
    { "i.kmc.evt.channel.searchStep", "ua = deviceClass, ub = percent" },
    { "i.kmc.evt.channel.searchEnd", "After: ua = deviceClass" },

    { "i.kmc.evt.channel.RecordStart", "After: ua = channelClass" },
    { "i.kmc.evt.channel.RecordTime", "ua = channelClass" },
    { "i.kmc.evt.channel.RecordEnd", "After: ua = channelClass" },

    // { "i.kmc.evt.channel.playWindowState", "ua = channelClass" },

    { "i.kmc.evt.playEvent", "What's up when play, val=KMCPE', ua=chClass" },

    { "i.kmc.evt.playStateChange", "After: play, pause, stop" },

    // { "i.kmc.evt.esgStateChange", "双向或广播的ESG/EPG信息" },
    // { "i.kmc.evt.esgChannelChange", "频道发生变化" },
    // { "i.kmc.evt.esgInteractivityChange", "互动发生变化" },
    // { "i.kmc.evt.esgProgramChange", "节目发生变化" },
    // { "i.kmc.evt.esgAccseeChange", "频道物理参数发生变化" },

    { "i.kmc.evt.hardwareChange", "After:" },
};

KMediaContainer::KMediaContainer(KIM* im, const char* a_name)
{
    this->m_im = im;
    init_dlist_head(&m_protocolHeader);
    m_name = kstr_dup(a_name);

    for (int i = 0; i < sizeof(__g_im_entry_list) / sizeof(__g_im_entry_list[0]); i++)
        kim_addint(m_im, __g_im_entry_list[i].id, 0, 0, NULL, __g_im_entry_list[i].desc);
}

KMediaContainer::~KMediaContainer(void)
{
    kassert(is_dlist_empty(&m_protocolHeader));

    kmem_free_s(m_name);

    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    while (!is_dlist_empty(&m_protocolHeader)) {
        protEntry = remove_dlist_tail_entry(&m_protocolHeader);
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        delete prot;
    }

    for (int i = 0; i < sizeof(__g_im_entry_list) / sizeof(__g_im_entry_list[0]); i++)
        kim_delint(m_im, __g_im_entry_list[i].id);
}

KMediaProtocol* KMediaContainer::getMediaProtocolFromProtocol(const char* protHash)
{
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        if (0 == kstr_icmp(prot->getHash(), protHash, -1))
            return prot;
        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaProtocol* KMediaContainer::getMediaProtocolFromDevice(const char* devHash)
{
    K_dlist_entry* devEntry,* protEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);

        devEntry = prot->m_deviceHeader.next;
        while (devEntry != &prot->m_deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
            if (0 == kstr_icmp(dev->getHash(), devHash, -1))
                return prot;
            devEntry = devEntry->next;
        }

        protEntry = m_protocolHeader.next;
    }
    return NULL;
}

KMediaProtocol* KMediaContainer::getMediaProtocolFromChannel(const char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);

        devEntry = prot->m_deviceHeader.next;
        while (devEntry != &prot->m_deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);

            chEntry = dev->m_channelHeader.next;
            while (chEntry != &dev->m_channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1))
                    return prot;
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = m_protocolHeader.next;
    }
    return NULL;
}

KMediaDevice* KMediaContainer::getMediaDeviceFromDevice(const char* devHash)
{
    K_dlist_entry* devEntry,* protEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);

        devEntry = prot->m_deviceHeader.next;
        while (devEntry != &prot->m_deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);
            if (0 == kstr_icmp(dev->getHash(), devHash, -1))
                return dev;
            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaDevice* KMediaContainer::getMediaDeviceFromChannel(const char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);

        devEntry = prot->m_deviceHeader.next;
        while (devEntry != &prot->m_deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);

            chEntry = dev->m_channelHeader.next;
            while (chEntry != &dev->m_channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1))
                    return dev;
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaChannel* KMediaContainer::getMediaChannelFromChannel(const char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);

        devEntry = prot->m_deviceHeader.next;
        while (devEntry != &prot->m_deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, m_deviceEntry);

            chEntry = dev->m_channelHeader.next;
            while (chEntry != &dev->m_channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, m_channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1))
                    return ch;
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

char** KMediaContainer::getMediaProtocolHashList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* protEntry;
    KMediaProtocol * prot;

    if (is_dlist_empty(&m_protocolHeader))
        return NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        cnt++;
        protEntry = protEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        hashList[index++] = kstr_dup(prot->getHash());
        protEntry = protEntry->next;
    }

    return hashList;
}

KMediaProtocol** KMediaContainer::getMediaProtocolClassList(void)
{
    int cnt = 0, index = 0;
    KMediaProtocol **classList;
    K_dlist_entry* protEntry;
    KMediaProtocol *prot;

    if (is_dlist_empty(&m_protocolHeader))
        return NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        cnt++;
        protEntry = protEntry->next;
    }

    classList = (KMediaProtocol**)kmem_alloc(sizeof(char*) * (cnt + 1));
    classList[cnt] = NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        classList[index++] = prot;
        protEntry = protEntry->next;
    }

    return classList;
}


char** KMediaContainer::getMediaDeviceHashList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    if (is_dlist_empty(&m_protocolHeader))
        return NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        protCnt++;
        protEntry = protEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * protCnt);

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        hashListList[protIndex++] = prot->getMediaDeviceHashList();
        protEntry = protEntry->next;
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++)
            devCnt++;

    hashList = (char**)kmem_alloc(sizeof(char*) * (devCnt + 1));
    hashList[devCnt] = NULL;

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            hashList[devIndex++] = hashListList[protIndex][i];
            devCnt++;
        }

    kmem_free_s(hashListList);

    return hashList;
}

KMediaDevice** KMediaContainer::getMediaDeviceClassList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    KMediaDevice **deviceList = NULL;
    KMediaDevice ***deviceListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    if (is_dlist_empty(&m_protocolHeader))
        return NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        protCnt++;
        protEntry = protEntry->next;
    }

    deviceListList = (KMediaDevice***)kmem_alloc(sizeof(KMediaDevice**) * protCnt);

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        deviceListList[protIndex++] = prot->getMediaDeviceClassList();
        protEntry = protEntry->next;
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; deviceListList[protIndex]&&deviceListList[protIndex][i]; i++)
            devCnt++;

    deviceList = (KMediaDevice**)kmem_alloc(sizeof(KMediaDevice*) * (devCnt + 1));
    deviceList[devCnt] = NULL;

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; deviceListList[protIndex]&&deviceListList[protIndex][i]; i++) {
            deviceList[devIndex++] = deviceListList[protIndex][i];
            devCnt++;
        }

    kmem_free_s(deviceListList);

    return deviceList;
}

char** KMediaContainer::getMediaChannelHashList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    if (is_dlist_empty(&m_protocolHeader))
        return NULL;

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        protCnt++;
        protEntry = protEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * protCnt);

    protEntry = m_protocolHeader.next;
    while (protEntry != &m_protocolHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, m_protocolEntry);
        hashListList[protIndex++] = prot->getMediaChannelHashList();
        protEntry = protEntry->next;
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++)
            devCnt++;

    hashList = (char**)kmem_alloc(sizeof(char*) * (devCnt + 1));
    hashList[devCnt] = NULL;

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            hashList[devIndex++] = hashListList[protIndex][i];
            devCnt++;
        }

    for (protIndex = 0; protIndex < protCnt; protIndex++)
        kmem_free_s(hashListList[protIndex]);

    kmem_free_s(hashListList);

    return hashList;
}

