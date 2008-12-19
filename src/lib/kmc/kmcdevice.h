#ifndef KMCDEVICE_H_
#define KMCDEVICE_H_

#include <string.h>
#include <stdlib.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmccontainer.h"
#include "kmcprotocal.h"
#include "kmcchannel.h"

class KMediaProtocal;

/* deviec type define
   type  =0   reserved
   1   broadcast
   2   web
   3   local::record

*/
class KMediaDevice
{
    public:
        KMediaDevice(KMediaProtocal* a_parentProtocal, char* a_name);
        virtual ~KMediaDevice(void);

        virtual void* allocMemory(unsigned int size) { return NULL; }
        virtual void freeMemory(void *ptr) {}
        // ֻ����һ�μ��ɣ����뱣֤�ù�ϣֵ��Ψһ�ԣ������ڶ�ͬһ����ļ���ṹ����ͬ��
        virtual char* getHash(void) = 0;
        const char* getName(void) { return name; }
        const char* getDesc(void) { return desc; }

        int getType(void) { return type; };

        KMediaProtocal *getProtocal(void) const { return parentProtocal; }

        virtual kbool start(void) { kbool ret = started; started = true; return ret; }
        virtual kbool stop(void) { kbool ret = started; started = false; return ret; }
        kbool isStarted(void) { return started; }

        virtual int remove(void) { return EC_NOT_SUPPORT; }

        // �õ���KMC�����еļ��ص�Ƶ���б��������ϣ�б����б�����������
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

        // �ź�ǿ�� 0 <= a_amp < 100
        virtual int setSignalAmp(int a_amp);
        virtual int getSignalAmp(int *a_pamp);

        // �൱���ֹ���̨
        // ��ϵͳ������ʱ��ֻ��ȡ���������Ƶ���б�
        virtual int updateChannelList(void);    // �൱����̨��
        virtual int updateChannelList(unsigned int class_mask, unsigned int type_mask);    // ָ���������򷽷�
        virtual int cancelUpdateChannelList(void);

    public:
        // XXX the list must be public.
        //
        // �������ڱ��豸�����е�Ƶ����Ƶ��ʵ����ʱ���Լ����ӵ���������
        K_dlist_entry channelHeader;

        // �б������parentProtocal->deviceHeader;
        K_dlist_entry deviceEntry;
    protected:
        char* desc;
        int  type;

    private:
        KMediaProtocal* parentProtocal;
        char* name;


        kbool started;
        kbool isEnable;
        kbool isPresent;
};

#endif /*KMCDEVICE_H_*/
