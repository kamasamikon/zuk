/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_LOCAL_CHANNEL_H__
#define __KMC_LOCAL_CHANNEL_H__

#include <kmccontainer.h>
#include "local-device.h"

class KMC_LocalChannel : public KMediaChannel
{
public:
    KMC_LocalChannel(KIM *im, KMC_LocalDevice* a_parentDevice, char* a_name, char *a_uri);
    virtual ~KMC_LocalChannel(void);

    virtual kbool getCapability(KMC_CAP cap);

    virtual int setChannel(kbool isSet);
    virtual int remove(void);

public:
    void setBackend(void *a_backend);

public:
    virtual int getChannelInfo(KMC_ChannelInfo * serviceInfo);
    virtual int getChannelPreviewInfo(KMC_PREVIEW_TYPE infoType, KMC_PreviewInfo *previewInfo);

    virtual int getProgramList(time_t startTime, time_t endTime, KMC_ProgInfo** a_pi_lst, int *piCnt);
    virtual int getProgramDescrition(char *progID, char **description);

    virtual int getInteractivityInfo(time_t a_time,
            KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
            KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt);

    /**
     * return EC_NOT_SUPPORT if operation not supported
     */

    /** percent 0 - 100 */
    virtual int setVolume(int a_vol);
    virtual int getVolume(int* a_vol);

    /** mute and unmute */
    virtual int setMute(kbool a_mute);
    virtual int getMute(kbool* a_mute);

    /** play, stop, pause, */
    virtual int setPlayState(KMCPS a_state);
    virtual int getPlayState(KMCPS* a_state);

    /** recording: start, end */
    virtual int setRecState(KMCRS a_state);
    virtual int getRecState(KMCRS* a_state);

    /** percent 100% is normal */
    virtual int setPlaySpeed(int a_speed);
    virtual int getPlaySpeed(int* a_speed);

    /**
     * sound track
     *
     * FIXME: How to get all sound track?
     */
    virtual int setAudio(int a_audio);
    virtual int getAudio(int* a_audio);

    /** Language and locale: en_US, zh_CN, zh_CN.UTF-8 etc */
    virtual int setLanguage(int a_lang);
    virtual int getLanguage(int* a_lang);

    /** Title: en_US, zh_CN, zh_CN.UTF-8 etc */
    virtual int setTitle(int a_type);
    virtual int getTitle(int* a_type);

    /** Url for  title files, file: / / /a.txt, http: / / xx.x.x.x */
    virtual int setTitleSource(char* a_uri);
    virtual int getTitleSource(char* a_uri);

    /**
     * Speed should set to equal to video speed when play start
     *
     * > 0: delay, < 0: ahead
     */
    virtual int audioDelay(int a_millisecond);
    virtual int titleDelay(int a_millisecond);

    /** method use for seek operation */
    virtual int setSeekFormat(KMC_SeekFormat a_format);
    virtual int getSeekFormat(KMC_SeekFormat *a_format);

    /**
     * \warning play with KMC_SeekFormat
     * XXX: when play speed changed, how can KMC_Seek_TIME do?
     */
    virtual int setPosition(int a_pos);
    virtual int getPosition(int* a_pos);

    /**
     * \warning play with KMC_SeekFormat
     *
     * Seek range is not same as play range, it should less then play range.
     */
    virtual int getSeekRange(int *a_from, int *a_to);

    /**
     * \warning play with KMC_SeekFormat
     */
    virtual int getPlayRange(int *a_from, int  *a_to);

    /**
     * \warning play with KMC_SeekFormat
     */
    virtual int stepSeek(int a_step);

    virtual int setOutputWindow(void* a_window);
    virtual int getOutputWindow(void** a_window);

    /**
     * Channel is one playable media resource, which can be played by
     * some type of codec or player.
     *
     * should be file, video, audio, etc, MIME type?
     */
    virtual int regOutputType(int a_type);
    virtual int getOutputType(int* a_type);

    virtual int setOutputWindowRect(KMC_RECT *a_rc);
    virtual int getOutputWindowRect(KMC_RECT *a_rc);
    virtual int getOrgOutputWindowRect(KMC_RECT *a_rc);

    virtual int setOutputWindowRatio(int a_h, int a_w);
    virtual int getOutputWindowRatio(int a_h, int a_w);
    virtual int getOrgOutputWindowRatio(int a_h, int a_w);

    /** clockwise, default is zero */
    virtual int setVideoAngle(int a_angle);
    virtual int getVideoAngle(int *a_angle);

private:
    int m_volume;
    kbool m_mute;
    KMCPS m_playState;
    KMCRS m_recState;

private:
    void *m_backend;
    char *m_uri;
};

#endif /*__KMC_LOCAL_CHANNEL_H__*/

