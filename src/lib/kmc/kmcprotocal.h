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
        // 初始化结构，并自动启动

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
        // 只计算一次即可，必须保证该哈希值得唯一性，并且在对同一事物的计算结构是相同的
        virtual char* getHash(void) = 0;
        const char* getName(void) { return name; }
        const char* getDesc(void) { return desc; }

        KMediaContainer *getContainer(void) const { return parentContainer; }

        int getType(void) { return type; };

        kbool start(void) { kbool ret = started; started = true; return ret; }
        kbool stop(void) { kbool ret = started; started = false; return ret; }
        kbool isStarted(void) { return started; }

        // 得到本KMC中所有的加载的设备列表，返回其哈希列表，空列表项代表结束。
        char** getMediaDeviceList(void);

        // 得到本KMC中所有的加载的频道列表，返回其哈希列表，空列表项代表结束。
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

        // 当系统启动的时候，必须扫描各个设备，并和保存的信息进行对比
        virtual int scanDevice(void);

    public:
        // XXX the list must be public.
        //
        // 保存所有加载的协议，协议实例化时将把自己增加到该链表中
        K_dlist_entry deviceHeader;

        // 列表项，排入parentContainer->protocalHeader;
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
