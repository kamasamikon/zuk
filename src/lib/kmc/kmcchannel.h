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
    KMC_Seek_TIME=0,// 单位为 毫秒
    KMC_Seek_FRAM,  // 单位为 帧数
    KMC_Seek_BYTE   // 单位为 字节数
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

        // 只计算一次即可，必须保证该哈希值得唯一性，并且在对同一事物的计算结构是相同的
        virtual char* getHash(void) = 0;                    // 播放列表里可能就保存这个东西
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

        // 针对于播放列表的操作, 如果该特性不被支持, 则以下的函数返回
        // EC_NOT_SUPPORTED
        //
        // 音量是百分比, 最小是零, 最大是100
        virtual int setVolume(int a_vol);
        virtual int getVolume(int* a_vol);

        // mute and unmute
        virtual int setMute(kbool a_mute);
        virtual int getMute(kbool* a_mute);

        // XXX 可以存在多个协议同时播放
        virtual int setPlayState(KMCPS a_state);    // play, stop, pause,
        virtual int getPlayState(KMCPS* a_state);    // play, stop, pause

        // recording
        virtual int setRecState(KMCRS a_state);        // start, end
        virtual int getRecState(KMCRS* a_state);    // start, end

        // 播放速度, 也是百分比 100 为正常
        virtual int setPlaySpeed(int a_speed);    // 快进等
        virtual int getPlaySpeed(int* a_speed);

        // 音轨
        virtual int setAudio(int a_audio);        // select audio track
        virtual int getAudio(int* a_audio);        // 得到所有可用的音轨

        // 语言
        virtual int setLanguage(int a_lang);    // en_US, zh_CN, zh_CN.UTF-8 等等
        virtual int getLanguage(int* a_lang);    // en_US, zh_CN, zh_CN.UTF-8 等等

        // 字幕
        virtual int setTitle(int a_type);        // en_US, zh_CN, zh_CN.UTF-8 等等
        virtual int getTitle(int* a_type);        // en_US, zh_CN, zh_CN.UTF-8 等等

        // 外挂字幕文件
        virtual int setTitleSource(char* a_uri);// 来自文件，或其他资源 (file:///a.txt)
        virtual int getTitleSource(char* a_uri);// 来自文件，或其他资源 (file:///a.txt)

        // 不需要单独设置音频的播放速度，它的速度就等于 PlaySpeed
        //
        // > 0: 慢, < 0: 快
        virtual int audioDelay(int a_millisecond);        // > 0 声音快 < 0 声音慢
        virtual int titleDelay(int a_millisecond);        // 字幕

        // about seek

        // 设置seek的方式
        // 必须在seek 前先调用
        virtual int setSeekFormat(KMC_SeekFormat  a_seekFormat=KMC_Seek_TIME) { return EC_NOT_SUPPORT; };
        // 得到seek的方式
        virtual int getSeekFormat(KMC_SeekFormat  &a_seekFormat) { return EC_NOT_SUPPORT; };

        // 设置播放的位置：
        // a_pos 的单位由 setSeekFormat 决定，可以是帧数，也可以是 毫秒或其他
        //  a_pos=0， 表示从当前位置开始播放
        //  a_pos<0， 表示从当前位置的前 a_pos 个单位开始播放
        //  a_pos>0， 表示从当前位置的后 a_pos 个单位开始播放
        virtual int setPosition(int a_pos) ;        // seek, 和播放速度有关系， XXX 那单位是时间还是别的？

        // 得到当前的播放位置：
        // a_pos 的单位由 setSeekFormat 决定，可以是帧数，也可以是 毫秒或其他
        virtual int getPosition(int* a_pos);

        // 得到当前时刻的seek 范围
        // * a_pFrom 和  * a_pTo的单位由 setSeekFormat 决定，可以是帧数，也可以是 毫秒或其他
        // 一般  * a_pFrom 为0，该范围是可以拖动进度条的范围，但不一定是播放媒体的有效播放范围，
        // 若    * a_pFrom 和 * a_pTo 都 <0 ，表示不能得到seek 范围，如直播节目
        // 一般seek 的范围比播放媒体的有效范围小或者等于
        virtual int getSeekRange(int * a_pFrom, int * a_pTo);

        //得到播放媒体的播放长度范围
        // * a_pFrom 和  * a_pTo的单位由 setSeekFormat 决定，可以是帧数，也可以是 毫秒或其他
        // 若 * a_pFrom 和 * a_pTo 都 <0 ，表示不能得到播放的长度范围，如直播节目
        virtual int getPlayRange(int * a_pFrom, int  * a_pTo);

#if 1
        //单步定位显示媒体所在位置的图像，而不连续播放,欲开始播放需接着调用setPlayState（play）
        // a_step 的单位由 setSeekFormat 决定，可以是帧数，也可以是 毫秒或其他
        //  a_step=0， 表示定位到当前位置
        //  a_step<0， 表示定位到当前位置的前 a_pos 个单位
        //  a_step>0， 表示定位到当前位置的后 a_pos 个单位
        virtual int stepSeek(int a_step){ return EC_NOT_SUPPORT; };
#else
        // 单步的时候, 播放是停止的
        // (step, millisecond) > 0: 向前
        // (step, millisecond) < 0: 向后
        virtual int stepByFrame(int a_step);
        virtual int stepByMilliSecond(int a_millisecond);
#endif

        // XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX
        virtual int setOutputWindow(void* a_ow);
        virtual int getOutputWindow(void** a_ow);

        // 由于我们把频道定义为可以播放的媒体资源，那就不能确定该媒体资源的类型
        // 同样就不能具体确定该媒体资源用什么来播放，那么，是否应该预定义一些类型
        // 然后使用给该类型注册的程序来播放呢？
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
        // 列表项，排入parentDevice->channelHeader;
        K_dlist_entry channelEntry;

        private:
        KMediaDevice* parentDevice;
        char* name;
        kbool started;
        };

#endif /*KMCCHANNEL_H_*/
