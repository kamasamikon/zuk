#include <kmem.h>
#include <kstr.h>
#include <kdbg.h>

#include "kmccontainer.h"

// XXX 参数是否有问题？
//
// 只负责初始化本类实例，协议加载由协议模块完成
KMediaContainer::KMediaContainer(KIM* im, char* a_name)
{
    this->im = im;
    init_dlist_head(&protocalHeader);
    name = kstr_dup(a_name);

    /*
     * 以下事件随时都可能发生
     */

    /* 切换动作已经开始 */
    kim_addint(im, "i.kmc.evt.channel.switchStart", 0, 0, NULL, "转到一个新的频道, ua = channelHash");
    /* 切换动作已经结束 */
    kim_addint(im, "i.kmc.evt.channel.switchEnd", 0, 0, NULL, "新频道开始播放, ua = channelHash");
    // var :channelHash

    kim_addint(im, "i.kmc.evt.protocol.new", 0, 0, NULL, "创建新的协议，并已经添加到系统中, ua = protocolHash");
    kim_addint(im, "i.kmc.evt.protocol.del", 0, 0, NULL, "将要删除协议，该事件后删除, ua = protocolHash");
    // var :protHash

    kim_addint(im, "i.kmc.evt.device.new", 0, 0, NULL, "创建新的设备，并已经添加到系统中, ua = deviceHash");
    kim_addint(im, "i.kmc.evt.device.froze", 0, 0, NULL, "该设备已经冻结，, ua = deviceHash, (ub = ture) == 冻结, (ub = false) == 解冻");
    kim_addint(im, "i.kmc.evt.device.del", 0, 0, NULL, "将要删除设备, 该事件后删除, ua = deviceHash");
    // var :devHash

    kim_addint(im, "i.kmc.evt.channel.new", 0, 0, NULL, "创建新的频道，并已经添加到系统中, ua = channelHash ");
    kim_addint(im, "i.kmc.evt.channel.froze", 0, 0, NULL, "该频道已经冻结，, ua = channelHash,  (ub = ture) == 冻结, (ub = false) == 解冻");
    kim_addint(im, "i.kmc.evt.channel.chg", 0, 0, NULL, "频道的数据已经更新, ua = channelHash ");
    kim_addint(im, "i.kmc.evt.channel.del", 0, 0, NULL, "将要删除频道, 该事件后删除, ua = channelHash ");
    // var :channelHash

    kim_addint(im, "i.kmc.evt.channel.searchStart", 0, 0, NULL, "对某一设备扫描频道已经开始, ua = deviceHash");
    kim_addint(im, "i.kmc.evt.channel.searchStep", 0, 0, NULL, "正在扫描, ua = deviceHash, ub = percent");
    kim_addint(im, "i.kmc.evt.channel.searchEnd", 0, 0, NULL, "对某一设备扫描频道完成已经完成, ua = deviceHash");

	// about record 
	// var :channelHash
    kim_addint(im, "i.kmc.evt.channel.RecordStart", 0, 0, NULL, "开始设置录制频道, ua = channelHash");
	kim_addint(im, "i.kmc.evt.channel.RecordTime", 0, 0, NULL, "可录制节目时间长度更新, ua = channelHash");
    kim_addint(im, "i.kmc.evt.channel.RecordEnd", 0, 0, NULL, "开始录制或失败, ua = channelHash");

	// about play window state
	//  issue enum { KMCPW_HIDE, KMCPW_SHOW} KMCPW;
	kim_addint(im, "i.kmc.evt.channel.playWindowState", 0, 0, NULL, "开始录制或失败, ua = channelHash");

    // issue enum { KMCPE_END, KMCPE_ERR, KMCPE_ABT } KMCPE;
    kim_addint(im, "i.kmc.evt.playEvent", 0, 0, NULL, "What's up when play, val=KMCPE', ua=chHash");

    kim_addint(im, "i.kmc.evt.playStateChange", 0, 0, NULL, "play, pause, stop 已经发生");


	

    kim_addint(im, "i.kmc.evt.esgStateChange", 0, 0, NULL, "双向或广播的ESG/EPG信息");
    kim_addint(im, "i.kmc.evt.esgChannelChange", 0, 0, NULL, "频道发生变化");
    kim_addint(im, "i.kmc.evt.esgInteractivityChange", 0, 0, NULL, "互动发生变化");
    kim_addint(im, "i.kmc.evt.esgProgramChange", 0, 0, NULL, "节目发生变化");
    kim_addint(im, "i.kmc.evt.esgAccseeChange", 0, 0, NULL, "频道物理参数发生变化");

    // 目前还不能具体得到是增加还是删除设备。
    kim_addint(im, "i.kmc.evt.hardwareChange", 0, 0, NULL, "系统硬件已经发生了变化");

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

    kmem_free_s(name);

    K_dlist_entry* protEntry;
    KMediaProtocal* prot;

    while (!is_dlist_empty(&protocalHeader)) {
        protEntry = remove_dlist_tail_entry(&protocalHeader);
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        delete prot;
    }
    
    kim_delint(im, "i.kmc.evt.channel.switchStart");
    kim_delint(im, "i.kmc.evt.channel.switchEnd");
    kim_delint(im, "i.kmc.evt.protocol.new");
    kim_delint(im, "i.kmc.evt.protocol.del");
    kim_delint(im, "i.kmc.evt.device.new");
    kim_delint(im, "i.kmc.evt.device.froze");
    kim_delint(im, "i.kmc.evt.device.del");
    kim_delint(im, "i.kmc.evt.channel.new");
    kim_delint(im, "i.kmc.evt.channel.froze");
    kim_delint(im, "i.kmc.evt.channel.chg");
    kim_delint(im, "i.kmc.evt.channel.del");
    kim_delint(im, "i.kmc.evt.channel.searchStart");
    kim_delint(im, "i.kmc.evt.channel.searchStep");
    kim_delint(im, "i.kmc.evt.channel.searchEnd");
    kim_delint(im, "i.kmc.evt.channel.RecordStart");
    kim_delint(im, "i.kmc.evt.channel.RecordTime");
    kim_delint(im, "i.kmc.evt.channel.RecordEnd");
    kim_delint(im, "i.kmc.evt.channel.playWindowState");
    kim_delint(im, "i.kmc.evt.playEvent");
    kim_delint(im, "i.kmc.evt.playStateChange");
    kim_delint(im, "i.kmc.evt.esgStateChange");
    kim_delint(im, "i.kmc.evt.esgChannelChange");
    kim_delint(im, "i.kmc.evt.esgInteractivityChange");
    kim_delint(im, "i.kmc.evt.esgProgramChange");
    kim_delint(im, "i.kmc.evt.esgAccseeChange");
    kim_delint(im, "i.kmc.evt.hardwareChange");
}

KMediaProtocal* KMediaContainer::getMediaProtocalFromProtocal(char* protHash)
{
    K_dlist_entry* protEntry;
    KMediaProtocal* prot;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        if (0 == kstr_icmp(prot->getHash(), protHash, -1)) {
            return prot;
        }
        protEntry = protEntry->next;
    }
    return NULL;
}

KMediaProtocal* KMediaContainer::getMediaProtocalFromDevice(char* devHash)
{
    K_dlist_entry* devEntry,* protEntry;
    KMediaProtocal* prot;
    KMediaDevice* dev;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);

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

KMediaProtocal* KMediaContainer::getMediaProtocalFromChannel(char* chHash)
{
    K_dlist_entry* devEntry,* protEntry,* chEntry;
    KMediaProtocal* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);

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
    KMediaProtocal* prot;
    KMediaDevice* dev;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);

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
    KMediaProtocal* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);

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
    KMediaProtocal* prot;
    KMediaDevice* dev;
    KMediaChannel* ch;

    protEntry = protocalHeader.next;
    while (protEntry != &protocalHeader) {
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);

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
char** KMediaContainer::getMediaProtocalList(void)
{
    // TODO
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* protEntry;
    KMediaProtocal * prot;

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
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        hashList[index++] = kstr_dup(prot->getHash());
        protEntry = protEntry->next;
    }

    return hashList;
    return NULL;
}

// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
char** KMediaContainer::getMediaDeviceList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocal* prot;

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
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        hashListList[protIndex++] = prot->getMediaDeviceList();
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

char** KMediaContainer::getMediaChannelList(unsigned int class_mask)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocal* prot;

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
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        hashListList[protIndex++] = prot->getMediaChannelList(class_mask);
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

// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
char** KMediaContainer::getMediaChannelList(void)
{
    int protCnt = 0, devCnt = 0, protIndex = 0, devIndex = 0;
    char **hashList = NULL;
    char ***hashListList = NULL;
    K_dlist_entry* protEntry;
    KMediaProtocal* prot;

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
        prot = FIELD_TO_STRUCTURE(protEntry, KMediaProtocal, protocalEntry);
        hashListList[protIndex++] = prot->getMediaChannelList();
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

