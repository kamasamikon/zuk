/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>
#include <kdbg.h>

#include "kmccontainer.h"

static struct {
    kchar *id;
    kchar *desc;
} __g_im_entry_list[] = {

    { "i.kmc.evt.channel.switchStart", "转到一个新的频道, ua = channelHash" },
    { "i.kmc.evt.channel.switchEnd", "新频道开始播放, ua = channelHash" },

    { "i.kmc.evt.protocol.new", "创建新的协议，并已经添加到系统中, ua = protocolHash" },
    { "i.kmc.evt.protocol.del", "将要删除协议，该事件后删除, ua = protocolHash" },

    { "i.kmc.evt.device.new", "创建新的设备，并已经添加到系统中, ua = deviceHash" },
    { "i.kmc.evt.device.froze", "该设备已经冻结，, ua = deviceHash, (ub = ture) == 冻结, (ub = false) == 解冻" },
    { "i.kmc.evt.device.del", "将要删除设备, 该事件后删除, ua = deviceHash" },

    { "i.kmc.evt.device.searchStart", "对某一协议扫描设备已经开始, ua = protocolHash" },
    { "i.kmc.evt.device.searchStep", "正在扫描, ua = protocolHash, ub = percent" },
    { "i.kmc.evt.device.searchEnd", "对某一协议扫描设备完成已经完成, ua = protocolHash" },

    { "i.kmc.evt.channel.new", "创建新的频道，并已经添加到系统中, ua = channelHash " },
    { "i.kmc.evt.channel.froze", "该频道已经冻结，, ua = channelHash,  (ub = ture) == 冻结, (ub = false) == 解冻" },
    { "i.kmc.evt.channel.chg", "频道的数据已经更新, ua = channelHash " },
    { "i.kmc.evt.channel.del", "将要删除频道, 该事件后删除, ua = channelHash " },

    { "i.kmc.evt.channel.searchStart", "对某一设备扫描频道已经开始, ua = deviceHash" },
    { "i.kmc.evt.channel.searchStep", "正在扫描, ua = deviceHash, ub = percent" },
    { "i.kmc.evt.channel.searchEnd", "对某一设备扫描频道完成已经完成, ua = deviceHash" },

    { "i.kmc.evt.channel.RecordStart", "开始设置录制频道, ua = channelHash" },
    { "i.kmc.evt.channel.RecordTime", "可录制节目时间长度更新, ua = channelHash" },
    { "i.kmc.evt.channel.RecordEnd", "开始录制或失败, ua = channelHash" },

    { "i.kmc.evt.channel.playWindowState", "开始录制或失败, ua = channelHash" },

    { "i.kmc.evt.playEvent", "What's up when play, val=KMCPE', ua=chHash" },

    { "i.kmc.evt.playStateChange", "play, pause, stop 已经发生" },


    { "i.kmc.evt.esgStateChange", "双向或广播的ESG/EPG信息" },
    { "i.kmc.evt.esgChannelChange", "频道发生变化" },
    { "i.kmc.evt.esgInteractivityChange", "互动发生变化" },
    { "i.kmc.evt.esgProgramChange", "节目发生变化" },
    { "i.kmc.evt.esgAccseeChange", "频道物理参数发生变化" },

    { "i.kmc.evt.hardwareChange", "系统硬件已经发生了变化" },
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

    // 当前正在播放的频道
    // 播放状态
    //
}

// 模块的释放应该在之前都已经完成，所以这里保证协议列表为空即可。
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

// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
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

// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
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

// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
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

