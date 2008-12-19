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
        // 只计算一次即可，必须保证该哈希值得唯一性，并且在对同一事物的计算结构是相同的
        virtual char* getHash(void) = 0;
        const char* getName(void) { return name; }
        const char* getDesc(void) { return desc; }

        int getType(void) { return type; };

        KMediaProtocal *getProtocal(void) const { return parentProtocal; }

        virtual kbool start(void) { kbool ret = started; started = true; return ret; }
        virtual kbool stop(void) { kbool ret = started; started = false; return ret; }
        kbool isStarted(void) { return started; }

        virtual int remove(void) { return EC_NOT_SUPPORT; }

        // 得到本KMC中所有的加载的频道列表，返回其哈希列表，空列表项代表结束。
        char** getMediaChannelList(void);
        char** getMediaChannelList(unsigned int class_mask);

        // 信号强度 0 <= a_amp < 100
        virtual int setSignalAmp(int a_amp);
        virtual int getSignalAmp(int *a_pamp);

        // 相当于手工搜台
        // 在系统启动的时候，只是取缓存而生成频道列表
        virtual int updateChannelList(void);    // 相当于搜台了
        virtual int updateChannelList(unsigned int class_mask, unsigned int type_mask);    // 指定搜索类别或方法
        virtual int cancelUpdateChannelList(void);

    public:
        // XXX the list must be public.
        //
        // 保存属于本设备的所有的频道，频道实例化时把自己增加到该链表中
        K_dlist_entry channelHeader;

        // 列表项，排入parentProtocal->deviceHeader;
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
