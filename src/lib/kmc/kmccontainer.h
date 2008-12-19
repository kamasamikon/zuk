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
        // 只是初始化该结构，并不去扫描协议等。协议的加入是由协议模块
        // 启动的时候自己加入的。
        KMediaContainer(KIM* im, char* a_name);

        // 释放该结构的，协议应该保证在调用本析构函数的时候已经都销毁
        virtual ~KMediaContainer(void);

        virtual void* allocMemory(unsigned int size) { return NULL; }
        virtual void freeMemory(void *ptr) {}

        const char* getName(void) { return name; }


        // 根据协议哈希返回协议类指针
        KMediaProtocal* getMediaProtocalFromProtocal(char* protHash);

        // 根据设备哈希返回协议类指针
        KMediaProtocal* getMediaProtocalFromDevice(char* devHash);

        // 根据频道哈希返回协议类指针
        KMediaProtocal* getMediaProtocalFromChannel(char* chHash);


        // 根据设备哈希返回设备类指针
        KMediaDevice* getMediaDeviceFromDevice(char* devHash);

        // 根据频道哈希返回设备类指针
        KMediaDevice* getMediaDeviceFromChannel(char* chHash);


        // 根据频道哈希返回频道类指针
        KMediaChannel* getMediaChannelFromChannel(char* chHash);


        // 得到本KMC中所有的加载的协议列表，返回其哈希列表，空列表项代表结束。
        char** getMediaProtocalList(void);

        // 得到本KMC中所有的加载的设备列表，返回其哈希列表，空列表项代表结束。
        char** getMediaDeviceList(void);

        // 得到本KMC中所有的加载的频道列表，返回其哈希列表，空列表项代表结束。
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

    public:
        // XXX the list must be public.
        //
        // 保存所有支持的协议，协议的实例化时将把自己增加到该链表中
        K_dlist_entry protocalHeader;

    private:
        char* name;
        KIM* im;
};

#endif /*KMCCONTAINER_H_*/

