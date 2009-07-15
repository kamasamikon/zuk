/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"

KMediaChannel::KMediaChannel(KIM *a_im, KMediaDevice* a_parentDevice, const char* a_name)
{
    init_dlist_head(&m_channelEntry);

    m_im = a_im;
    m_parentDevice = a_parentDevice;
    m_name = kstr_dup(a_name);
    m_flg = 0;

    insert_dlist_tail_entry(&m_parentDevice->m_channelHeader, &m_channelEntry);
}

KMediaChannel::~KMediaChannel(void)
{
    kmem_free_s(m_name);
    remove_dlist_entry(&m_channelEntry);
}

KIM* KMediaChannel::im()
{
    return m_im;
}

const char* KMediaChannel::getHash(void)
{
    return m_hash;
}

void KMediaChannel::setHash(const char *a_hash)
{
    if (m_hash[0])
        kerror(("Already setHash, can not set!\n"));
    else
        memcpy(m_hash, a_hash, 33);
}
const char* KMediaChannel::getName(void)
{
    return m_name;
}

KMediaDevice *KMediaChannel::getDevice(void) const
{
    return m_parentDevice;
}

kbool KMediaChannel::start(void)
{
    kuint of = m_flg;
    kflg_set(m_flg, MC_CH_FLG_STARTED);
    return kflg_chk(of, MC_CH_FLG_STARTED) ? true : false;
}
kbool KMediaChannel::stop(void)
{
    kuint of = m_flg;
    kflg_clr(m_flg, MC_CH_FLG_STARTED);
    return kflg_chk(of, MC_CH_FLG_STARTED) ? true : false;
}
kbool KMediaChannel::isStarted(void)
{
    return kflg_chk(m_flg, MC_CH_FLG_STARTED) ? true : false;
}

kbool KMediaChannel::isFreeze(void)
{
    return kflg_chk(m_flg, MC_CH_FLG_DEFREEZED) ? true : false;
}
kbool KMediaChannel::freeze(void)
{
    kuint of = m_flg;
    kflg_set(m_flg, MC_CH_FLG_DEFREEZED);
    return kflg_chk(of, MC_CH_FLG_DEFREEZED) ? true : false;
}
kbool KMediaChannel::defreeze(void)
{
    kuint of = m_flg;
    kflg_clr(m_flg, MC_CH_FLG_DEFREEZED);
    return kflg_chk(of, MC_CH_FLG_DEFREEZED) ? true : false;
}

kbool KMediaChannel::getCapability(KMC_CAP cap)
{
    return kfalse;
}

int KMediaChannel::setChannel(kbool isSet)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::remove(void)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::getChannelInfo(KMC_ChannelInfo * serviceInfo)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getChannelPreviewInfo(KMC_PREVIEW_TYPE infoType, KMC_PreviewInfo *previewInfo)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::getProgramList(time_t startTime, time_t endTime, KMC_ProgInfo** a_pi_lst, int *piCnt)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getProgramDescrition(char *progID, char **description)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::getInteractivityInfo(time_t a_time,
        KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
        KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt)
{
    return EC_NOT_SUPPORT;
}

/** percent 0 - 100 */
int KMediaChannel::setVolume(int a_vol)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getVolume(int* a_vol)
{
    return EC_NOT_SUPPORT;
}

/** mute and unmute */
int KMediaChannel::setMute(kbool a_mute)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getMute(kbool* a_mute)
{
    return EC_NOT_SUPPORT;
}

/** play, stop, pause, */
int KMediaChannel::setPlayState(KMCPS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPlayState(KMCPS* a_state)
{
    return EC_NOT_SUPPORT;
}

/** recording: start, end */
int KMediaChannel::setRecState(KMCRS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getRecState(KMCRS* a_state)
{
    return EC_NOT_SUPPORT;
}

/** percent 100% is normal */
int KMediaChannel::setPlaySpeed(int a_speed)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPlaySpeed(int* a_speed)
{
    return EC_NOT_SUPPORT;
}

/**
 * sound track
 *
 * FIXME: How to get all sound track?
 */
int KMediaChannel::setAudio(int a_audio)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getAudio(int* a_audio)
{
    return EC_NOT_SUPPORT;
}

/** Language and locale: en_US, zh_CN, zh_CN.UTF-8 etc */
int KMediaChannel::setLanguage(int a_lang)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getLanguage(int* a_lang)
{
    return EC_NOT_SUPPORT;
}

/** Title: en_US, zh_CN, zh_CN.UTF-8 etc */
int KMediaChannel::setTitle(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getTitle(int* a_type)
{
    return EC_NOT_SUPPORT;
}

/** Url for  title files, file: / / /a.txt, http: / / xx.x.x.x */
int KMediaChannel::setTitleSource(char* a_uri)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getTitleSource(char* a_uri)
{
    return EC_NOT_SUPPORT;
}

/**
 * Speed should set to equal to video speed when play start
 *
 * > 0: delay, < 0: ahead
 */
int KMediaChannel::audioDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::titleDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}

/** method use for seek operation */
int KMediaChannel::setSeekFormat(KMC_SeekFormat a_format)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getSeekFormat(KMC_SeekFormat *a_format)
{
    return EC_NOT_SUPPORT;
}

/**
 * \warning play with KMC_SeekFormat
 * XXX: when play speed changed, how can KMC_Seek_TIME do?
 */
int KMediaChannel::setPosition(int a_pos)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPosition(int* a_pos)
{
    return EC_NOT_SUPPORT;
}

/**
 * \warning play with KMC_SeekFormat
 *
 * Seek range is not same as play range, it should less then play range.
 */
int KMediaChannel::getSeekRange(int *a_from, int *a_to)
{
    return EC_NOT_SUPPORT;
}

/**
 * \warning play with KMC_SeekFormat
 */
int KMediaChannel::getPlayRange(int *a_from, int  *a_to)
{
    return EC_NOT_SUPPORT;
}

/**
 * \warning play with KMC_SeekFormat
 */
int KMediaChannel::stepSeek(int a_step)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setOutputWindow(void* a_window)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputWindow(void** a_window)
{
    return EC_NOT_SUPPORT;
}

/**
 * Channel is one playable media resource, which can be played by
 * some type of codec or player.
 *
 * should be file, video, audio, etc, MIME type?
 */
int KMediaChannel::regOutputType(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputType(int* a_type)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOrgOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOrgOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}

/** clockwise, default is zero */
int KMediaChannel::setVideoAngle(int a_angle)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getVideoAngle(int *a_angle)
{
    return EC_NOT_SUPPORT;
}


