/* vim:set et sw=4 sts=4 ff=unix: */
#include <kmem.h>
#include <kstr.h>

#include "kmccontainer.h"
#include "local-channel.h"

KMC_LocalChannel::KMC_LocalChannel(KIM *a_im, KMC_LocalDevice* a_parentDevice, char* a_name) :
    KMediaChannel(a_im, a_parentDevice, a_name)
{
    kim_setint(im(), "i.kmc.evt.channel.new", 1, (void**)this, knil);
}

KMC_LocalChannel::~KMC_LocalChannel(void)
{
    kim_setint(im(), "i.kmc.evt.channel.del", 1, (void**)this, knil);
}

kbool KMC_LocalChannel::getCapability(KMC_CAP cap)
{
    return kfalse;
}

int KMC_LocalChannel::setChannel(kbool isSet)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::remove(void)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::getChannelInfo(KMC_ChannelInfo * serviceInfo)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getChannelPreviewInfo(KMC_PREVIEW_TYPE infoType, KMC_PreviewInfo *previewInfo)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::getProgramList(time_t startTime, time_t endTime, KMC_ProgInfo** a_pi_lst, int *piCnt)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getProgramDescrition(char *progID, char **description)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::getInteractivityInfo(time_t a_time,
        KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
        KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setVolume(int a_vol)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getVolume(int* a_vol)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setMute(kbool a_mute)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getMute(kbool* a_mute)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setPlayState(KMCPS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getPlayState(KMCPS* a_state)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setRecState(KMCRS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getRecState(KMCRS* a_state)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setPlaySpeed(int a_speed)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getPlaySpeed(int* a_speed)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setAudio(int a_audio)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getAudio(int* a_audio)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setLanguage(int a_lang)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getLanguage(int* a_lang)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setTitle(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getTitle(int* a_type)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setTitleSource(char* a_uri)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getTitleSource(char* a_uri)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::audioDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::titleDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setSeekFormat(KMC_SeekFormat a_format)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getSeekFormat(KMC_SeekFormat *a_format)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setPosition(int a_pos)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getPosition(int* a_pos)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::getSeekRange(int *a_from, int *a_to)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::getPlayRange(int *a_from, int  *a_to)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::stepSeek(int a_step)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setOutputWindow(void* a_window)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOutputWindow(void** a_window)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::regOutputType(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOutputType(int* a_type)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOrgOutputWindowRect(KMC_RECT *a_rc)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getOrgOutputWindowRatio(int a_h, int a_w)
{
    return EC_NOT_SUPPORT;
}

int KMC_LocalChannel::setVideoAngle(int a_angle)
{
    return EC_NOT_SUPPORT;
}
int KMC_LocalChannel::getVideoAngle(int *a_angle)
{
    return EC_NOT_SUPPORT;
}
