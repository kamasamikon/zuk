/* vim:set et sw=4 sts=4 ff=unix: */
#include "kmccontainer.h"
#include "kmcprotocal.h"
#include "kmcdevice.h"
#include "kmcchannel.h"


// ���ڱ����ļ�MP3���ԣ����ȷ��"��ǰ"����Ϣ�أ�
// ���ڸ�����û�в��ŵ��ļ������ȷ����ǰ��Ϣ��?
//
// ���ڷǹ㲥��ý�壬�Ͳ�����next��epg��Ϣ��
// Ҳ������ʲô������Ϣ
//
// ���ߣ�Ŀǰ������Ϣ�ͻ�����Ϣ�Ķ��嶼�����
// �㲥ý��ȥ���õģ����ԣ����������� KMediaContainer ����
// ���ǵ�����ý����ʽ��
//
// KEsgProgramBean �ȵĽṹ�����ݻ��д�����
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
// �ϲ���ݹ�ϣ��ֱ�ӷ���Ŀ��
// ʹ��kim���ﵽ�¼�֪ͨ��Ч��
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
        ch->setPlaySpeed(120);        // ��120�����ٶȲ���
        ch->setPlayState(KMCPS_PLAY);

        ...
    }
}
#endif
