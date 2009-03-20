/* vim:set et sw=4 sts=4: */
#ifndef KMCCHANNEL_H_
#define KMCCHANNEL_H_

#include <string.h>
#include <time.h>

#include <sdlist.h>
#include <kstr.h>
#include <kmem.h>
#include <kflg.h>
#include "kerrcode.h"

#include "kmcdefines.h"

class KMediaDevice;

#define MC_CH_FLG_STARTED   0x00000001
#define MC_CH_FLG_DEFREEZED 0x00000002

/**
 * \brief Play State by setPlayState/getPlayState
 */
typedef enum { KMCPS_PLAY, KMCPS_STOP, KMCPS_PAUSE } KMCPS;

/**
 * event issued when playing
 */
typedef enum { KMCPE_END, KMCPE_ERR, KMCPE_ABT } KMCPE;

/**
 * Recording state by setRecState/getRecState
 */
typedef enum { KMCRS_START, KMCRS_END} KMCRS;

/**
 * channel play window state
 */
typedef enum { KMCPW_HIDE, KMCPW_SHOW} KMCPW;

/**
 * record channel start event
 */
typedef enum
{
    KMCRC_STARTEVT_SUCCESS = 0, /**< When  UI call setRecState(KMCRS_START),the bottom will often sent the result */
    KMCRC_STARTEVT_NOSPACE,     /**< When  no space  for record */
    KMCRC_STARTEVT_DIRLOST,     /**< The dir of record do not exist */
    KMCRC_STARTEVT_DEVICELOST   /**< Broadcast device is removed */
} KMCRC_STARTEVT;

/**
 * record channel end event
 */
typedef enum
{
    KMCRC_ENDEVT_SUCCESS = 0,   /**< When  UI call setRecState(KMCRS_END),the bottom will often sent the result */
    KMCRC_ENDEVT_NOSPACE,       /**< When  no space  for record,the bottom will sent the result and auto stop to record */
    KMCRC_ENDEVT_SIGNALBREAK,   /**< When signal is  breaked,the bottom will sent the result and auto stop to record */
    KMCRC_ENDEVT_DEVICELOST     /**< When broadcast device is removed ,the bottom will sent the result and auto stop to record */
} KMCRC_ENDEVT;

typedef enum {
    KMC_Seek_TIME = 0,          /**< millisecond */
    KMC_Seek_FRAM,              /**< frame */
    KMC_Seek_BYTE               /**< by byte */
} KMC_SeekFormat;

typedef struct _KMC_RECT
{
    long left;
    long top;
    long right;
    long bottom;
} KMC_RECT;

typedef enum _KMC_CAP
{
    KMC_CAP_RECORDER,
    KMC_CAP_SEEK,
    KMC_CAP_CHG_SPEED,
    KMC_ETC
} KMC_CAP;

class KMediaChannel
{
public:
    KMediaChannel(KMediaDevice* a_parentDevice, char* a_name);
    virtual ~KMediaChannel(void);

    virtual void* allocMemory(kint size) { return knil; }
    virtual void freeMemory(void *ptr) {}

    virtual char* getHash(void) = 0;
    const char* getName(void) { return name; }

    KMediaDevice *getDevice(void) const { return parentDevice; }

    virtual kbool start(void) { kuint of = flg; kflg_set(flg, MC_CH_FLG_STARTED); return kflg_chk(of, MC_CH_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = flg; kflg_clr(flg, MC_CH_FLG_STARTED); return kflg_chk(of, MC_CH_FLG_STARTED) ? true : false; }
    kbool isStarted(void) { return kflg_chk(flg, MC_CH_FLG_STARTED) ? true : false; }

    kbool isFreeze(void) { return kflg_chk(flg, MC_CH_FLG_DEFREEZED) ? true : false; }
    kbool freeze(void) { kuint of = flg; kflg_set(flg, MC_CH_FLG_DEFREEZED); return kflg_chk(of, MC_CH_FLG_DEFREEZED) ? true : false; }
    kbool defreeze(void) { kuint of = flg; kflg_clr(flg, MC_CH_FLG_DEFREEZED); return kflg_chk(of, MC_CH_FLG_DEFREEZED) ? true : false; }

    virtual kbool getCapability(KMC_CAP cap) { return kfalse; }

    virtual int setChannel(kbool isSet) { return EC_NOT_SUPPORT; }
    virtual int remove(void) { return EC_NOT_SUPPORT; }

public:

    virtual int getChannelInfo(KMC_ChannelInfo * serviceInfo) { return EC_NOT_SUPPORT; }
    virtual int getChannelPreviewInfo(KMC_PREVIEW_TYPE infoType, KMC_PreviewInfo *previewInfo) { return EC_NOT_SUPPORT; }

    virtual int getProgramList(time_t startTime, time_t endTime, KMC_ProgInfo** a_pi_lst, int *piCnt) { return EC_NOT_SUPPORT; }
    virtual int getProgramDescrition(char *progID, char **description) { return EC_NOT_SUPPORT; }

    virtual int getInteractivityInfo(time_t a_time,
            KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
            KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt) { return EC_NOT_SUPPORT; }

    /**
     * return EC_NOT_SUPPORT if operation not supported
     */

    /** percent 0 - 99 */
    virtual int setVolume(int a_vol) { return EC_NOT_SUPPORT; }
    virtual int getVolume(int* a_vol) { return EC_NOT_SUPPORT; }

    /** mute and unmute */
    virtual int setMute(kbool a_mute) { return EC_NOT_SUPPORT; }
    virtual int getMute(kbool* a_mute) { return EC_NOT_SUPPORT; }

    /** play, stop, pause, */
    virtual int setPlayState(KMCPS a_state) { return EC_NOT_SUPPORT; }
    virtual int getPlayState(KMCPS* a_state) { return EC_NOT_SUPPORT; }

    /** can recorder? */
    virtual int canRecorder(kbool *a_can) { return EC_NOT_SUPPORT; }

    /** recording: start, end */
    virtual int setRecState(KMCRS a_state) { return EC_NOT_SUPPORT; }
    virtual int getRecState(KMCRS* a_state) { return EC_NOT_SUPPORT; }

    /** percent 100% is normal */
    virtual int setPlaySpeed(int a_speed) { return EC_NOT_SUPPORT; }
    virtual int getPlaySpeed(int* a_speed) { return EC_NOT_SUPPORT; }

    /**
     * sound track
     *
     * FIXME: How to get all sound track?
     */
    virtual int setAudio(int a_audio) { return EC_NOT_SUPPORT; }
    virtual int getAudio(int* a_audio) { return EC_NOT_SUPPORT; }

    /** Language and locale: en_US, zh_CN, zh_CN.UTF-8 etc */
    virtual int setLanguage(int a_lang) { return EC_NOT_SUPPORT; }
    virtual int getLanguage(int* a_lang) { return EC_NOT_SUPPORT; }

    /** Title: en_US, zh_CN, zh_CN.UTF-8 etc */
    virtual int setTitle(int a_type) { return EC_NOT_SUPPORT; }
    virtual int getTitle(int* a_type) { return EC_NOT_SUPPORT; }

    /** Url for  title files, file: / / /a.txt, http: / / xx.x.x.x */
    virtual int setTitleSource(char* a_uri) { return EC_NOT_SUPPORT; }
    virtual int getTitleSource(char* a_uri) { return EC_NOT_SUPPORT; }

    /**
     * Speed should set to equal to video speed when play start
     *
     * > 0: delay, < 0: ahead
     */
    virtual int audioDelay(int a_millisecond) { return EC_NOT_SUPPORT; }
    virtual int titleDelay(int a_millisecond) { return EC_NOT_SUPPORT; }

    /** method use for seek operation */
    virtual int setSeekFormat(KMC_SeekFormat a_format) { return EC_NOT_SUPPORT; }
    virtual int getSeekFormat(KMC_SeekFormat *a_format) { return EC_NOT_SUPPORT; }

    /**
     * \warning play with KMC_SeekFormat
     * XXX: when play speed changed, how can KMC_Seek_TIME do?
     */
    virtual int setPosition(int a_pos) { return EC_NOT_SUPPORT; }
    virtual int getPosition(int* a_pos) { return EC_NOT_SUPPORT; }

    /**
     * \warning play with KMC_SeekFormat
     *
     * Seek range is not same as play range, it should less then play range.
     */
    virtual int getSeekRange(int *a_from, int *a_to) { return EC_NOT_SUPPORT; }

    /**
     * \warning play with KMC_SeekFormat
     */
    virtual int getPlayRange(int *a_from, int  *a_to) { return EC_NOT_SUPPORT; }

    /**
     * \warning play with KMC_SeekFormat
     */
    virtual int stepSeek(int a_step) { return EC_NOT_SUPPORT; }

    virtual int setOutputWindow(void* a_ow) { return EC_NOT_SUPPORT; }
    virtual int getOutputWindow(void** a_ow) { return EC_NOT_SUPPORT; }

    /**
     * Channel is one playable media resource, which can be played by
     * some type of codec or player.
     *
     * should be file, video, audio, etc, MIME type?
     */
    virtual int regOutputType(int a_type) { return EC_NOT_SUPPORT; }
    virtual int getOutputType(int* a_type) { return EC_NOT_SUPPORT; }

    virtual int setOutputWindowRect(KMC_RECT *a_rc) { return EC_NOT_SUPPORT; }
    virtual int getOutputWindowRect(KMC_RECT *a_rc) { return EC_NOT_SUPPORT; }
    virtual int getOrgOutputWindowRect(KMC_RECT *a_rc) { return EC_NOT_SUPPORT; }

    virtual int setOutputWindowRatio(int a_h, int a_w) { return EC_NOT_SUPPORT; }
    virtual int getOutputWindowRatio(int a_h, int a_w) { return EC_NOT_SUPPORT; }
    virtual int getOrgOutputWindowRatio(int a_h, int a_w) { return EC_NOT_SUPPORT; }

    /** clockwise, default is zero */
    virtual int setVideoAngle(int a_angle) { return EC_NOT_SUPPORT; }
    virtual int getVideoAngle(int *a_angle) { return EC_NOT_SUPPORT; }

public:
    K_dlist_entry channelEntry;

private:
    KMediaDevice* parentDevice;
    char* name;
    kuint flg;
};

#endif /*KMCCHANNEL_H_*/

