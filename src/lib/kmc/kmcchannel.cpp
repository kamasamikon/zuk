#include <kmem.h>
#include <kstr.h>

#include "kmcchannel.h"

KMediaChannel::KMediaChannel(KMediaDevice* a_parentDevice, char* a_name)
{
    init_dlist_head(&channelEntry);
    parentDevice = a_parentDevice;
    name = kstr_dup(a_name);

    insert_dlist_tail_entry(&parentDevice->channelHeader, &channelEntry);
}

KMediaChannel::~KMediaChannel(void)
{
    kmem_free_s(name);
    remove_dlist_entry(&channelEntry);
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

#if 0
int KMediaChannel::getProgramInfo(time_t a_time, KProgramInfo** a_pi_cur, KProgramInfo** a_pi_nxt)
{
    return EC_NOT_SUPPORT;
}
#endif

int KMediaChannel::getInteractivityInfo(time_t a_time,
        KMC_ItvInfo** a_ii_cur, int *iiCurCnt,
        KMC_ItvInfo** a_ii_nxt, int *iiNxtCnt)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setChannel(kbool isSet)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setVolume(int a_vol)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getVolume(int* a_vol)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setMute(kbool a_mute)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getMute(kbool* a_mute)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setPlayState(KMCPS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPlayState(KMCPS* a_state)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setRecState(KMCRS a_state)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getRecState(KMCRS* a_state)
{
    return EC_NOT_SUPPORT;
}

int KMediaChannel::setPlaySpeed(int a_speed)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPlaySpeed(int* a_speed)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setAudio(int a_audio)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getAudio(int* a_audio)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setLanguage(int a_lang)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getLanguage(int* a_lang)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setTitle(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getTitle(int* a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setTitleSource(char* a_file)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getTitleSource(char* a_file)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::audioDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::titleDelay(int a_millisecond)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setPosition(int a_pos)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPosition(int* a_pos)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getSeekRange(int * a_pFrom, int * a_pTo)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getPlayRange(int * a_pFrom, int * a_pTo)
{
    return EC_NOT_SUPPORT;
}


int KMediaChannel::setOutputWindow(void* a_ow)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputWindow(void** a_ow)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::regOutputType(int a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::getOutputType(int* a_type)
{
    return EC_NOT_SUPPORT;
}
int KMediaChannel::setOutputWindowRect(void /*RECT*/* a_rc)
                                                      {
                                                      return EC_NOT_SUPPORT;
                                                      }
                                                      int KMediaChannel::getOutputWindowRect(void /*RECT*/* a_rc)
                                                                                                            {
                                                                                                            return EC_NOT_SUPPORT;
                                                                                                            }
                                                                                                            int KMediaChannel::getOrgOutputWindowRect(void /*RECT*/* a_rc)
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
