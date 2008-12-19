#ifndef KMCPROTOCAL_H_
#define KMCPROTOCAL_H_

#include <string.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmccontainer.h"
#include "kmcdevice.h"
#include "kmcchannel.h"

class KMediaContainer;

class KMediaProtocal
{
    public:
        // ��ʼ���ṹ�����Զ�����

        //
        //a_type ---> 0   reserve;
        //       ---> 1   dvb-c
        //        ----> 2   dvb-s
        //        ----> 3   dvb-t
        //        ----> 4   dvb-h
        //       ---> 5   dmb-t
        //        ----> 6   streaming media
        //        ----> 7      cmmb
        //        ----> 8

        KMediaProtocal(KMediaContainer* a_parentContainer, char* a_name, int a_type);
        virtual ~KMediaProtocal(void);

        virtual void* allocMemory(unsigned int size) { return NULL; }
        virtual void freeMemory(void *ptr) {}
        // ֻ����һ�μ��ɣ����뱣֤�ù�ϣֵ��Ψһ�ԣ������ڶ�ͬһ����ļ���ṹ����ͬ��
        virtual char* getHash(void) = 0;
        const char* getName(void) { return name; }
        const char* getDesc(void) { return desc; }

        KMediaContainer *getContainer(void) const { return parentContainer; }

        int getType(void) { return type; };

        kbool start(void) { kbool ret = started; started = true; return ret; }
        kbool stop(void) { kbool ret = started; started = false; return ret; }
        kbool isStarted(void) { return started; }

        // �õ���KMC�����еļ��ص��豸�б��������ϣ�б����б�����������
        char** getMediaDeviceList(void);

        // �õ���KMC�����еļ��ص�Ƶ���б��������ϣ�б����б�����������
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

        // ��ϵͳ������ʱ�򣬱���ɨ������豸�����ͱ������Ϣ���жԱ�
        virtual int scanDevice(void);

    public:
        // XXX the list must be public.
        //
        // �������м��ص�Э�飬Э��ʵ����ʱ�����Լ����ӵ���������
        K_dlist_entry deviceHeader;

        // �б������parentContainer->protocalHeader;
        K_dlist_entry protocalEntry;
    protected:
        char* desc;

    private:
        KMediaContainer* parentContainer;
        char* name;

        bool started;
        int type;
};

#endif /*KMCPROTOCAL_H_*/
