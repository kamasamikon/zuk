/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <kdbg.h>

#include "kmccontainer.h"

static struct {
    kchar *id;
    kchar *desc;
} __g_im_entry_list[] = {

    { "i.kmc.evt.channel.switchStart", "ת��һ���µ�Ƶ��, ua = channelHash" },
    { "i.kmc.evt.channel.switchEnd", "��Ƶ����ʼ����, ua = channelHash" },

    { "i.kmc.evt.protocol.new", "�����µ�Э�飬���Ѿ���ӵ�ϵͳ��, ua = protocolHash" },
    { "i.kmc.evt.protocol.del", "��Ҫɾ��Э�飬���¼���ɾ��, ua = protocolHash" },

    { "i.kmc.evt.device.new", "�����µ��豸�����Ѿ���ӵ�ϵͳ��, ua = deviceHash" },
    { "i.kmc.evt.device.froze", "���豸�Ѿ����ᣬ, ua = deviceHash, (ub = ture) == ����, (ub = false) == �ⶳ" },
    { "i.kmc.evt.device.del", "��Ҫɾ���豸, ���¼���ɾ��, ua = deviceHash" },

    { "i.kmc.evt.device.searchStart", "��ĳһЭ��ɨ���豸�Ѿ���ʼ, ua = protocolHash" },
    { "i.kmc.evt.device.searchStep", "����ɨ��, ua = protocolHash, ub = percent" },
    { "i.kmc.evt.device.searchEnd", "��ĳһЭ��ɨ���豸����Ѿ����, ua = protocolHash" },

    { "i.kmc.evt.channel.new", "�����µ�Ƶ�������Ѿ���ӵ�ϵͳ��, ua = channelHash " },
    { "i.kmc.evt.channel.froze", "��Ƶ���Ѿ����ᣬ, ua = channelHash,  (ub = ture) == ����, (ub = false) == �ⶳ" },
    { "i.kmc.evt.channel.chg", "Ƶ���������Ѿ�����, ua = channelHash " },
    { "i.kmc.evt.channel.del", "��Ҫɾ��Ƶ��, ���¼���ɾ��, ua = channelHash " },

    { "i.kmc.evt.channel.searchStart", "��ĳһ�豸ɨ��Ƶ���Ѿ���ʼ, ua = deviceHash" },
    { "i.kmc.evt.channel.searchStep", "����ɨ��, ua = deviceHash, ub = percent" },
    { "i.kmc.evt.channel.searchEnd", "��ĳһ�豸ɨ��Ƶ������Ѿ����, ua = deviceHash" },

    { "i.kmc.evt.channel.RecordStart", "��ʼ����¼��Ƶ��, ua = channelHash" },
    { "i.kmc.evt.channel.RecordTime", "��¼�ƽ�Ŀʱ�䳤�ȸ���, ua = channelHash" },
    { "i.kmc.evt.channel.RecordEnd", "��ʼ¼�ƻ�ʧ��, ua = channelHash" },

    { "i.kmc.evt.channel.playWindowState", "��ʼ¼�ƻ�ʧ��, ua = channelHash" },

    { "i.kmc.evt.playEvent", "What's up when play, val=KMCPE', ua=chHash" },

    { "i.kmc.evt.playStateChange", "play, pause, stop �Ѿ�����" },


    { "i.kmc.evt.esgStateChange", "˫���㲥��ESG/EPG��Ϣ" },
    { "i.kmc.evt.esgChannelChange", "Ƶ�������仯" },
    { "i.kmc.evt.esgInteractivityChange", "���������仯" },
    { "i.kmc.evt.esgProgramChange", "��Ŀ�����仯" },
    { "i.kmc.evt.esgAccseeChange", "Ƶ��������������仯" },

    { "i.kmc.evt.hardwareChange", "ϵͳӲ���Ѿ������˱仯" },
};


KMediaContainer::KMediaContainer(KIM* im, char* a_name)
{
    this->m_im = im;
    init_dlist_head(&protocalHeader);
    m_name = kstr_dup(a_name);

    for (int i = 0; i < sizeof(__g_im_entry_list) / sizeof(__g_im_entry_list[0]); i++)
        kim_addint(m_im, __g_im_entry_list[i].id, 0, 0, NULL, __g_im_entry_list[i].desc);

    // var :devHash
    // when find a new channel, emit i.kmc.evt.channel.new

    // ��ǰ���ڲ��ŵ�Ƶ��
    // ����״̬
    //
}

// ģ����ͷ�Ӧ����֮ǰ���Ѿ���ɣ��������ﱣ֤Э���б�Ϊ�ռ��ɡ�
KMediaContainer::~KMediaContainer(void)
{
    kassert(is_dlist_empty(&protocalHeader));

    kmem_free_s(m_name);

    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    while (!is_dlist_empty(&protocalHeader)) {
        protEntry = remove_dlist_tail_entry(&protocalHeader);
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);
        delete prot;
    }

    for (int i = 0; i < sizeof(__g_im_entry_list) / sizeof(__g_im_entry_list[0]); i++)
        kim_delint(m_im, __g_im_entry_list[i].id);
}

KMediaProtocol* KMediaContainer::getMediaProtocalFromProtocal(char* protHash)
{
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);
        if (0 == kstr_icmp(prot->getHash(), protHash, -1)) {
            return prot;
        }
        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaProtocol* KMediaContainer::getMediaProtocalFromDevice(char* devHash)
{
    K_dlist_entry* devEntry,* protEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);

        devEntry = prot->deviceHeader.next;
        while (devEntry != &prot->deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);
            if (0 == kstr_icmp(dev->getHash(), devHash, -1)) {
                return prot;
            }
            devEntry = devEntry->next;
        }

        protEntry = protocalHeader.next;
    }
    return NULL;
}

KMediaProtocol* KMediaContainer::getMediaProtocalFromChannel(char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);

        devEntry = prot->deviceHeader.next;
        while (devEntry != &prot->deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);

            chEntry = dev->channelHeader.next;
            while (chEntry != &dev->channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1)) {
                    return prot;
                }
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = protocalHeader.next;
    }
    return NULL;
}

KMediaDevice* KMediaContainer::getMediaDeviceFromDevice(char* devHash)
{
    K_dlist_entry* devEntry,* protEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);

        devEntry = prot->deviceHeader.next;
        while (devEntry != &prot->deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);
            if (0 == kstr_icmp(dev->getHash(), devHash, -1)) {
                return dev;
            }
            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaDevice* KMediaContainer::getMediaDeviceFromChannel(char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);

        devEntry = prot->deviceHeader.next;
        while (devEntry != &prot->deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);

            chEntry = dev->channelHeader.next;
            while (chEntry != &dev->channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1)) {
                    return dev;
                }
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaChannel* KMediaContainer::getMediaChannelFromChannel(char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocol* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);

        devEntry = prot->deviceHeader.next;
        while (devEntry != &prot->deviceHeader) {
            dev = FIELD_TO_STRUCTURE(devEntry, KMediaDevice, deviceEntry);

            chEntry = dev->channelHeader.next;
            while (chEntry != &dev->channelHeader) {
                ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, channelEntry);
                if (0 == kstr_icmp(ch->getHash(), chHash, -1)) {
                    return ch;
                }
                chEntry = chEntry->next;
            }

            devEntry = devEntry->next;
        }

        protEntry = protEntry->next;
    }
    return NULL;
}

// ������NULLΪ������־�Ĺ�ϣ���б����б��ڱ������з���
// ���������ͷ�
char** KMediaContainer::getMediaProtocalHashList(void)
{
    // TODO
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* protEntry;
    KMediaProtocol * prot;

    if (is_dlist_empty(&protocalHeader)) {
        return NULL;
    }

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        cnt++;
        protEntry = protEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);
        hashList[index++] = kstr_dup(prot->getHash());
        protEntry = protEntry->next;
    }

    return hashList;
    return NULL;
}

// ������NULLΪ������־�Ĺ�ϣ���б����б��ڱ������з���
// ���������ͷ�
char** KMediaContainer::getMediaDeviceHashList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    if (is_dlist_empty(&protocalHeader)) {
        return NULL;
    }

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        protCnt++;
        protEntry = protEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * protCnt);

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);
        hashListList[protIndex++] = prot->getMediaDeviceHashList();
        protEntry = protEntry->next;
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++) {
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            devCnt++;
        }
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (devCnt + 1));
    hashList[devCnt] = NULL;

    for (protIndex = 0; protIndex < protCnt; protIndex++) {
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            hashList[devIndex++] = hashListList[protIndex][i];
            devCnt++;
        }
    }

    kmem_free_s(hashListList);

    return hashList;
}

// ������NULLΪ������־�Ĺ�ϣ���б����б��ڱ������з���
// ���������ͷ�
char** KMediaContainer::getMediaChannelHashList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocol* prot;

    if (is_dlist_empty(&protocalHeader)) {
        return NULL;
    }

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        protCnt++;
        protEntry = protEntry->next;
    }

    hashListList = (char***)kmem_alloc(sizeof(char**) * protCnt);

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocol, protocalEntry);
        hashListList[protIndex++] = prot->getMediaChannelHashList();
        protEntry = protEntry->next;
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++) {
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            devCnt++;
        }
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (devCnt + 1));
    hashList[devCnt] = NULL;

    for (protIndex = 0; protIndex < protCnt; protIndex++) {
        for (int i = 0; hashListList[protIndex]&&hashListList[protIndex][i]; i++) {
            hashList[devIndex++] = hashListList[protIndex][i];
            devCnt++;
        }
    }

    for (protIndex = 0; protIndex < protCnt; protIndex++) {
        kmem_free_s(hashListList[protIndex]);
    }
    kmem_free_s(hashListList);

    return hashList;
}

