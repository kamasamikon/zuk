#include <kmem.h>
#include <kstr.h>

#include "kmcdevice.h"

// 在调用该函数的时候，必须先采集该设备的必要信息，比如名字。
KMediaDevice::KMediaDevice(KMediaProtocal* a_parentProtocal, char* a_name)
{
    init_dlist_head(&channelHeader);
    init_dlist_head(&deviceEntry);
    parentProtocal = a_parentProtocal;
    name = kstr_dup(a_name);
    isEnable = true;
    isPresent = true;
    desc = NULL;

    // 把自己添加到上级类的列表中去
    // XXX 设备尚未启动
    insert_dlist_tail_entry(&parentProtocal->deviceHeader, &deviceEntry);
}

KMediaDevice::~KMediaDevice(void)
{
    kmem_free_s(name);
    kmem_free_s(desc);

    K_dlist_entry* chEntry;
    KMediaChannel* ch;

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, channelEntry);
        chEntry = chEntry->next;

        delete ch;
    }

    // 将自己从上级类的列表剔除，要求调用该函数的时候，本实例已经停止
    remove_dlist_entry(&deviceEntry);
}

// 信号强度
int KMediaDevice::setSignalAmp(int a_amp)
{
    return EC_NOT_SUPPORT;
}

int KMediaDevice::getSignalAmp(int *a_amp)
{
    return EC_NOT_SUPPORT;
}

// 更新频道列表，其实就是重新搜台，但不要忘记释放已经搜索的
// 频道信息。
int KMediaDevice::updateChannelList(void)
{
    return EC_NOT_SUPPORT;
}
int KMediaDevice::updateChannelList(unsigned int class_mask, unsigned int type_mask)
{
    return EC_NOT_SUPPORT;
}
int KMediaDevice::cancelUpdateChannelList(void)
{
    return EC_NOT_SUPPORT;
}
char** getMediaChannelList(unsigned int class_mask)
{
    return NULL;
}
// 返回以NULL为结束标志的哈希的列表，该列表在本函数中分配
// 被调用者释放
char** KMediaDevice::getMediaChannelList(void)
{
    int cnt = 0, index = 0;
    char **hashList;
    K_dlist_entry* chEntry;
    KMediaChannel* ch;

    if (is_dlist_empty(&channelHeader)) {
        return NULL;
    }

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        cnt++;
        chEntry = chEntry->next;
    }

    hashList = (char**)kmem_alloc(sizeof(char*) * (cnt + 1));
    hashList[cnt] = NULL;

    chEntry = channelHeader.next;
    while (chEntry != &channelHeader) {
        ch = FIELD_TO_STRUCTURE(chEntry, KMediaChannel, channelEntry);
        hashList[index++] = kstr_dup(ch->getHash());
        chEntry = chEntry->next;
    }
    hashList[index++] = NULL;

    return hashList;
}
