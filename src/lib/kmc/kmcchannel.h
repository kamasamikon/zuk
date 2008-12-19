#ifndef KMCCHANNEL_H_
#define KMCCHANNEL_H_

#include <string.h>
#include <time.h>
#include <sdlist.h>
#include <kim.h>
#include "kerrcode.h"

#include "kmccontainer.h"
#include "kmcprotocal.h"
#include "kmcdevice.h"

#include "kmcdefines.h"

class KMediaDevice;

// Play State by setPlayState/getPlayState
typedef enum { KMCPS_PLAY, KMCPS_STOP, KMCPS_PAUSE } KMCPS;

// event issued when playing
typedef enum { KMCPE_END, KMCPE_ERR, KMCPE_ABT } KMCPE;

// Recording state by setRecState/getRecState
typedef enum { KMCRS_START, KMCRS_END} KMCRS;


// channel play window state
typedef enum { KMCPW_HIDE, KMCPW_SHOW} KMCPW;



// record  channel  start event
typedef enum
{
    KMCRC_STARTEVT_SUCCESS=0,  // When  UI call setRecState(KMCRS_START),the bottom will often sent the result
    KMCRC_STARTEVT_NOSPACE,    // When  no space  for record
    KMCRC_STARTEVT_DIRLOST,// the dir of record do not exist
    KMCRC_STARTEVT_DEVICELOST //  broadcast device is removed

} KMCRC_STARTEVT;


// record  channel  end event
typedef enum
{
    KMCRC_ENDEVT_SUCCESS=0,  // When  UI call setRecState(KMCRS_END),the bottom will often sent the result
    KMCRC_ENDEVT_NOSPACE,    // When  no space  for record,the bottom will sent the result and auto stop to record
    KMCRC_ENDEVT_SIGNALBREAK,// When signal is  breaked,the bottom will sent the result and auto stop to record
    KMCRC_ENDEVT_DEVICELOST // When broadcast device is removed ,the bottom will sent the result and auto stop to record

} KMCRC_ENDEVT;



typedef enum {
    KMC_Seek_TIME=0,// ��λΪ ����
    KMC_Seek_FRAM,  // ��λΪ ֡��
    KMC_Seek_BYTE   // ��λΪ �ֽ���
}KMC_SeekFormat;

typedef struct _KMC_RECT
{
    long left;
    long top;
    long right;
    long bottom;
} KMC_RECT;
class KMediaChannel
{
    public:
        KMediaChannel(KMediaDevice* a_parentDevice, char* a_name);
        virtual ~KMediaChannel(void);

        virtual void* allocMemory(kint size) { return NULL; }
        virtual void freeMemory(void *ptr) {}

        // ֻ����һ�μ��ɣ����뱣֤�ù�ϣֵ��Ψһ�ԣ������ڶ�ͬһ����ļ���ṹ����ͬ��
        virtual char* getHash(void) = 0;                    // �����б�����ܾͱ����������
        const char* getName(void) { return name; }

        KMediaDevice *getDevice(void) const { return parentDevice; }

        kbool start(void) { kbool ret = started; started = true; return ret; }
        kbool stop(void) { kbool ret = started; started = false; return ret; }
        kbool isStarted(void) { return started; }

        virtual int setChannel(kbool isSet);
        virtual int remove(void) { return EC_NOT_SUPPORT; }

    public:

        virtual int getChannelInfo(KMC_ChannelInfo * serviceInfo);
        virtual int getChannelPreviewInfo(KMC_PREVIEW_TYPE infoType, KMC_PreviewInfo *previewInfo);

        virtual int getProgramList(time_t startTime, time_t endTime, KMC_ProgInfo** a_pi_lst, int *piCnt);
        virtual int getProgramDescrition(char *progID, char **description);

        virtual int getInteractivityInfo(time_t a_time,
                KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
                KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt);

        // ����ڲ����б�Ĳ���, ��������Բ���֧��, �����µĺ�������
        // EC_NOT_SUPPORTED
        //
        // �����ǰٷֱ�, ��С����, �����100
        virtual int setVolume(int a_vol);
        virtual int getVolume(int* a_vol);

        // mute and unmute
        virtual int setMute(kbool a_mute);
        virtual int getMute(kbool* a_mute);

        // XXX ���Դ��ڶ��Э��ͬʱ����
        virtual int setPlayState(KMCPS a_state);    // play, stop, pause,
        virtual int getPlayState(KMCPS* a_state);    // play, stop, pause

        // recording
        virtual int setRecState(KMCRS a_state);        // start, end
        virtual int getRecState(KMCRS* a_state);    // start, end

        // �����ٶ�, Ҳ�ǰٷֱ� 100 Ϊ����
        virtual int setPlaySpeed(int a_speed);    // �����
        virtual int getPlaySpeed(int* a_speed);

        // ����
        virtual int setAudio(int a_audio);        // select audio track
        virtual int getAudio(int* a_audio);        // �õ����п��õ�����

        // ����
        virtual int setLanguage(int a_lang);    // en_US, zh_CN, zh_CN.UTF-8 �ȵ�
        virtual int getLanguage(int* a_lang);    // en_US, zh_CN, zh_CN.UTF-8 �ȵ�

        // ��Ļ
        virtual int setTitle(int a_type);        // en_US, zh_CN, zh_CN.UTF-8 �ȵ�
        virtual int getTitle(int* a_type);        // en_US, zh_CN, zh_CN.UTF-8 �ȵ�

        // �����Ļ�ļ�
        virtual int setTitleSource(char* a_uri);// �����ļ�����������Դ (file:///a.txt)
        virtual int getTitleSource(char* a_uri);// �����ļ�����������Դ (file:///a.txt)

        // ����Ҫ����������Ƶ�Ĳ����ٶȣ������ٶȾ͵��� PlaySpeed
        //
        // > 0: ��, < 0: ��
        virtual int audioDelay(int a_millisecond);        // > 0 ������ < 0 ������
        virtual int titleDelay(int a_millisecond);        // ��Ļ

        // about seek

        // ����seek�ķ�ʽ
        // ������seek ǰ�ȵ���
        virtual int setSeekFormat(KMC_SeekFormat  a_seekFormat=KMC_Seek_TIME) { return EC_NOT_SUPPORT; };
        // �õ�seek�ķ�ʽ
        virtual int getSeekFormat(KMC_SeekFormat  &a_seekFormat) { return EC_NOT_SUPPORT; };

        // ���ò��ŵ�λ�ã�
        // a_pos �ĵ�λ�� setSeekFormat ������������֡����Ҳ������ ���������
        //  a_pos=0�� ��ʾ�ӵ�ǰλ�ÿ�ʼ����
        //  a_pos<0�� ��ʾ�ӵ�ǰλ�õ�ǰ a_pos ����λ��ʼ����
        //  a_pos>0�� ��ʾ�ӵ�ǰλ�õĺ� a_pos ����λ��ʼ����
        virtual int setPosition(int a_pos) ;        // seek, �Ͳ����ٶ��й�ϵ�� XXX �ǵ�λ��ʱ�仹�Ǳ�ģ�

        // �õ���ǰ�Ĳ���λ�ã�
        // a_pos �ĵ�λ�� setSeekFormat ������������֡����Ҳ������ ���������
        virtual int getPosition(int* a_pos);

        // �õ���ǰʱ�̵�seek ��Χ
        // * a_pFrom ��  * a_pTo�ĵ�λ�� setSeekFormat ������������֡����Ҳ������ ���������
        // һ��  * a_pFrom Ϊ0���÷�Χ�ǿ����϶��������ķ�Χ������һ���ǲ���ý�����Ч���ŷ�Χ��
        // ��    * a_pFrom �� * a_pTo �� <0 ����ʾ���ܵõ�seek ��Χ����ֱ����Ŀ
        // һ��seek �ķ�Χ�Ȳ���ý�����Ч��ΧС���ߵ���
        virtual int getSeekRange(int * a_pFrom, int * a_pTo);

        //�õ�����ý��Ĳ��ų��ȷ�Χ
        // * a_pFrom ��  * a_pTo�ĵ�λ�� setSeekFormat ������������֡����Ҳ������ ���������
        // �� * a_pFrom �� * a_pTo �� <0 ����ʾ���ܵõ����ŵĳ��ȷ�Χ����ֱ����Ŀ
        virtual int getPlayRange(int * a_pFrom, int  * a_pTo);

#if 1
        //������λ��ʾý������λ�õ�ͼ�񣬶�����������,����ʼ��������ŵ���setPlayState��play��
        // a_step �ĵ�λ�� setSeekFormat ������������֡����Ҳ������ ���������
        //  a_step=0�� ��ʾ��λ����ǰλ��
        //  a_step<0�� ��ʾ��λ����ǰλ�õ�ǰ a_pos ����λ
        //  a_step>0�� ��ʾ��λ����ǰλ�õĺ� a_pos ����λ
        virtual int stepSeek(int a_step){ return EC_NOT_SUPPORT; };
#else
        // ������ʱ��, ������ֹͣ��
        // (step, millisecond) > 0: ��ǰ
        // (step, millisecond) < 0: ���
        virtual int stepByFrame(int a_step);
        virtual int stepByMilliSecond(int a_millisecond);
#endif

        // XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX
        virtual int setOutputWindow(void* a_ow);
        virtual int getOutputWindow(void** a_ow);

        // �������ǰ�Ƶ������Ϊ���Բ��ŵ�ý����Դ���ǾͲ���ȷ����ý����Դ������
        // ͬ���Ͳ��ܾ���ȷ����ý����Դ��ʲô�����ţ���ô���Ƿ�Ӧ��Ԥ����һЩ����
        // Ȼ��ʹ�ø�������ע��ĳ����������أ�
        virtual int regOutputType(int a_type);
        virtual int getOutputType(int* a_type);
        // file, video, audio, etc, MIME type

        virtual int setOutputWindowRect(void /*RECT*/* a_rc);
                                                       virtual int getOutputWindowRect(void /*RECT*/* a_rc);
                                                                                                      virtual int getOrgOutputWindowRect(void /*RECT*/* a_rc);

                                                                                                                                                         virtual int setOutputWindowRatio(int a_h, int a_w);
                                                                                                                                                         virtual int getOutputWindowRatio(int a_h, int a_w);
                                                                                                                                                         virtual int getOrgOutputWindowRatio(int a_h, int a_w);

                                                                                                                                                         public:
        // XXX the list must be public.
        //
        // �б������parentDevice->channelHeader;
        K_dlist_entry channelEntry;

        private:
        KMediaDevice* parentDevice;
        char* name;
        kbool started;
        };

#endif /*KMCCHANNEL_H_*/
