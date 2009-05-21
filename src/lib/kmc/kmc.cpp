/* vim:set et sw=4 sts=4 ff=unix: */
#include "kmccontainer.h"
#include "kmcprotocal.h"
#include "kmcdevice.h"
#include "kmcchannel.h"


// 对于本地文件MP3而言，如何确定"当前"的信息呢？
// 对于根本就没有播放的文件，如何确定当前信息呢?
//
// 对于非广播的媒体，就不存在next的epg信息。
// 也不存在什么互动信息
//
// 或者，目前播放信息和互动信息的定义都是针对
// 广播媒体去设置的，所以，并不适用于 KMediaContainer 所能
// 涵盖的所有媒体形式。
//
// KEsgProgramBean 等的结构的内容还有待斟酌
//// file:kmcprogram.h
#if 0
typedef struct _KMediaProgram {
    struct {
        KEsgProgramBean *cur;
        KEsgProgramBean *next;
    } epgInfo;

    struct {
        int curCnt;
        KHDBean *cur;

        int nextCnt;
        KHDBean *next;
    } itvInfo;
} KMediaProgram;
#endif
// 上层根据哈希表直接访问目的
// 使用kim来达到事件通知的效果
#if 0
///////////////////////////////////////////////////////////////////////////////////////////
/// example
//

class KDVBTChannel : public KMediaChannel {
    public:
        KDVBTChannel(KMediaDevice* a_parentDevice, char* a_name) : KMediaChannel(a_parentDevice, a_name) {}

        char* getHash() { return "oi5yelrm"; }
};

class KDVBTDevice : public KMediaDevice {
    public:
        KDVBTDevice(KMediaProtocal* a_parentProtocal, char* a_name) : KMediaDevice(a_parentProtocal, a_name) {}

        char* getHash() { return "sdasdfas"; }

        int updateChannelList(void) {
            int cnt = 22;
            while (cnt--) {
                KDVBTChannel *ch = new KDVBTChannel(this, "DVBTChannel");
                ch->start();
            }
            return EC_OK;
        }
};

class KDVBTProtocal : public KMediaProtocal {
    char* getHash() { return "45oineroi"; }

    int scanDevice(void) {
        int cnt = 22;
        while (cnt--) {
            // get each of device and collect neccessary infomation

            KDVBTDevice *dev = new KDVBTDevice(this, "DVBTDevice");
            dev->start();
        }
        return EC_OK;
    }
};
#endif
#if 0
void entry(void)
{
    KMediaContainer *kmc = new KMediaContainer();
    kim_setptr(im, "KMediaContainer", kmc, NULL, NULL);

    KDVBTModule::mm_hey(im) {
        KMediaContainer *kmc = kim_getptr(im, "KMediaContainer");

        KDVBTProtocal *theProtocal = new KDVBTProtocal(kmc, "DVBTProtocal");
        kmc->addProtocal(theProtocal);
        kmc->startProtocal(theProtocal);

        // create a window and put the window to KWM
        CPlayDlg *playDlg;
        playDlg->Create(CPlayDlg::IDD, mainDlg);
        playDlg->kal_set_size(...);
        playDlg->kal_set_pos(...);
        kim_addptr(im, "playDlg", playDlg, 0, NULL);
    }

    if (10) {
        KPlayListItem *pli = getCurrentPlayListItem();
        KMediaDevice *dev = kmc->getMediaDeviceFromChannel(pli->hash);
        if (!dev->isPresent) {
            klog(("device not used skip this segment"));
            return;
        }

        KMediaChannel *ch = kmc->getMediaChannelFromChannel(pli->hash);
        ch->setOutputWindow(playDlg->m_hWnd);
        ch->setPlaySpeed(120);        // 以120％的速度播放
        ch->setPlayState(KMCPS_PLAY);

        ...
    }
}
#endif
