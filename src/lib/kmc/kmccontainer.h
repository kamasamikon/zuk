#ifndef KMCCONTAINER_H_
#define KMCCONTAINER_H_

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmcprotocal.h"
#include "kmcdevice.h"
#include "kmcchannel.h"

class KMediaChannel;

class KMediaContainer
{
    public:
        // ֻ�ǳ�ʼ���ýṹ������ȥɨ��Э��ȡ�Э��ļ�������Э��ģ��
        // ������ʱ���Լ�����ġ�
        KMediaContainer(KIM* im, char* a_name);

        // �ͷŸýṹ�ģ�Э��Ӧ�ñ�֤�ڵ��ñ�����������ʱ���Ѿ�������
        virtual ~KMediaContainer(void);

        virtual void* allocMemory(unsigned int size) { return NULL; }
        virtual void freeMemory(void *ptr) {}

        const char* getName(void) { return name; }


        // ����Э���ϣ����Э����ָ��
        KMediaProtocal* getMediaProtocalFromProtocal(char* protHash);

        // �����豸��ϣ����Э����ָ��
        KMediaProtocal* getMediaProtocalFromDevice(char* devHash);

        // ����Ƶ����ϣ����Э����ָ��
        KMediaProtocal* getMediaProtocalFromChannel(char* chHash);


        // �����豸��ϣ�����豸��ָ��
        KMediaDevice* getMediaDeviceFromDevice(char* devHash);

        // ����Ƶ����ϣ�����豸��ָ��
        KMediaDevice* getMediaDeviceFromChannel(char* chHash);


        // ����Ƶ����ϣ����Ƶ����ָ��
        KMediaChannel* getMediaChannelFromChannel(char* chHash);


        // �õ���KMC�����еļ��ص�Э���б��������ϣ�б����б�����������
        char** getMediaProtocalList(void);

        // �õ���KMC�����еļ��ص��豸�б��������ϣ�б����б�����������
        char** getMediaDeviceList(void);

        // �õ���KMC�����еļ��ص�Ƶ���б��������ϣ�б����б�����������
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

    public:
        // XXX the list must be public.
        //
        // ��������֧�ֵ�Э�飬Э���ʵ����ʱ�����Լ����ӵ���������
        K_dlist_entry protocalHeader;

    private:
        char* name;
        KIM* im;
};

#endif /*KMCCONTAINER_H_*/

