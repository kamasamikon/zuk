#include <kmem.h>
#include <kstr.h>

#include "kmcdevice.h"

// �ڵ��øú�����ʱ�򣬱����Ȳɼ����豸�ı�Ҫ��Ϣ���������֡�
KMediaDevice::KMediaDevice(KMediaProtocal* a_parentProtocal, char* a_name)
{
    init_dlist_head(&channelHeader);
    init_dlist_head(&deviceEntry);
    parentProtocal = a_parentProtocal;
    name = kstr_dup(a_name);
    isEnable = true;
    isPresent = true;
    desc = NULL;

    // ���Լ���ӵ��ϼ�����б���ȥ
    // XXX �豸��δ����
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

    // ���Լ����ϼ�����б��޳���Ҫ����øú�����ʱ�򣬱�ʵ���Ѿ�ֹͣ
    remove_dlist_entry(&deviceEntry);
}

// �ź�ǿ��
int KMediaDevice::setSignalAmp(int a_amp)
{
    return EC_NOT_SUPPORT;
}

int KMediaDevice::getSignalAmp(int *a_amp)
{
    return EC_NOT_SUPPORT;
}

// ����Ƶ���б���ʵ����������̨������Ҫ�����ͷ��Ѿ�������
// Ƶ����Ϣ��
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
// ������NULLΪ������־�Ĺ�ϣ���б����б��ڱ������з���
// ���������ͷ�
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
