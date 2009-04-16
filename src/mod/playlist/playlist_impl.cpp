/* vim:set et sw=4 sts=4: */
#include <syscfg.h>
#include <setglobals.h>

#include <stdio.h>

#include <xmldoc.h>
#include <kmem.h>
#include <kdbg.h>
#include <ktsk.h>
#include <kstr.h>
#include <ktmr.h>

#include <textconv.h>

#include "playlistdefine.h"

static char guid[] = "7D378382-9351-4f4e-BF83-4FF20C456B6D";
static kbean __g_worker_thread = knil;

static kint __g_try_scan_when_mod_loaded = 0;
static kint __g_try_scan_when_dev_defreeze = 0;

static kbool __g_unloading = kfalse;

CPlayListRunTimeInfo __g_plrti;
static int __g_chSearchingRef = 0;

static kbean __g_sig_tmr = knil;

/////////////////////////////////////////////////////////////////////////////
// defines
#define KMPL_SCAN                   0x00000001

/////////////////////////////////////////////////////////////////////////////
// support routines
static kvoid update_urls(kbool add)
{
    kchar fnbuf[1024], *utf8;
    kchar *modDir;

    modDir = kim_getstr(__g_im, "s.env.path.moduleDir", knil);
    const kchar *lang = kim_getstr(__g_im, "s.env.language", knil);
    if (!lang || strncmp(lang, "zh_", 3))
        lang = "en";

    /* the loading page */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\loading.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\loading.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.loading", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.loading", fnbuf, knil, knil);


    /* not no_device page */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\no_device.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\no_device.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.no_device", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.no_device", fnbuf, knil, knil);

    /* not scanning page */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\scanning.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\scanning.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.scanning", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.scanning", fnbuf, knil, knil);

    /* not scanning_failed page */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\scanning_failed.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\scanning_failed.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.scanning_failed", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.scanning_failed", fnbuf, knil, knil);

    /* no live program */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\no_prg_live.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\no_prg_live.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.no_prg_live", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.no_prg_live", fnbuf, knil, knil);

    /* no live program */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\no_prg_vod.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\no_prg_vod.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.no_prg_vod", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.no_prg_vod", fnbuf, knil, knil);


    /* no live program */
    sprintf(fnbuf, "%s\\playlist\\page\\live\\no_prg_rec.png", modDir);
    kstr_subs(fnbuf, '\\', kvfs_path_sep());
    if (!kvfs_exist(fnbuf)) {
        sprintf(fnbuf, "%s\\playlist\\page\\default\\%s\\no_prg_rec.png", modDir, lang);
        kstr_subs(fnbuf, '\\', kvfs_path_sep());
    }

    kstr_subs(fnbuf, kvfs_path_sep(), '/');
    utf8 = acp_to_utf8(fnbuf);
    sprintf(fnbuf, "file://%s", utf8);
    kmem_free_s(utf8);

    if (add)
        kim_addstr(__g_im, "s.playlist.url.no_prg_rec", fnbuf, RF_AUTOSET, knil, knil);
    else
        kim_setstr(__g_im, "s.playlist.url.no_prg_rec", fnbuf, knil, knil);
}


/////////////////////////////////////////////////////////////////////////////
// ktmr routines
/**
 * @brief for playing channel check the signal
 */
kint pltmr_sig_checker(kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3)
{
    if (__g_unloading)
        return -1;

    kchar *hash = kim_getstr(__g_im, "s.playlist.channel.current", knil);
    KMediaDevice *dev;
    KMediaChannel *ch;
    kint amp;

    if (hash && (ch = __g_mc->getMediaChannelFromChannel(hash)) && (dev = ch->getDevice()) && (EC_OK == dev->getSignalAmp(&amp)))
        kim_setint(__g_im, "i.playlist.media.signal", amp, knil, knil);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// kimat routines

/////////////////////////////////////////////////////////////////////////////
// watch routines
int IMWCH(plwch_channelSwitchStart)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    ktmr_kill(__g_sig_tmr);
    kim_setint(im, "i.playlist.media.signal", -1, knil, knil);
    return 0;
}
int IMWCH(plwch_channelSwitchEnd)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    int error = (int)REC_UB(rec), amp;
    klog(("plwch_channelSwitchEnd:%s:%d\n", hash, error));

    if (!error) {
        KMediaDevice *dev;
        KMediaChannel *ch;
        if ((ch = __g_mc->getMediaChannelFromChannel(hash)) && (dev = ch->getDevice()) && (EC_OK == dev->getSignalAmp(&amp)))
            kim_setint(im, "i.playlist.media.signal", amp, knil, knil);

        __g_sig_tmr = ktmr_set(knil, 5000, ktrue, pltmr_sig_checker, knil, knil, knil, knil);
    }
    return 0;
}
int IMWCH(plwch_deviceNew)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    return 0;
}
int IMWCH(plwch_deviceDel)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    return 0;
}
int IMWCH(plwch_deviceFroze)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    int param = (int)REC_UB(rec);
    klog(("plwch_deviceFroze:hash:%s, ub:%x\n", hash, param));

    if (param & 0x80000000)
        ;
    else if (__g_try_scan_when_dev_defreeze) {
        kchar **chlist;
        KMediaDevice *dev;

        if (dev = __g_mc->getMediaDeviceFromDevice(hash)) {
            chlist = dev->getMediaChannelList();
            if (!chlist || !chlist[0]) {
                kmsg_post(__g_worker_thread, KMPL_SCAN, kstr_dup(hash), (kvoid*)ktrue, knil, knil);
                kmem_free(chlist);
            }
        }
    }
    return 0;
}
int IMWCH(plwch_channelNew)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    return 0;
}
int IMWCH(plwch_channelDel)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    return 0;
}
int IMWCH(plwch_channelFroze)
{
    if (__g_unloading)
        return -1;

    char* hash = (char*)REC_UA(rec);
    if (REC_UB(rec)) {
    } else {
    }
    return 0;
}
static int IMWCH(plwch_mod_unload)
{
    __g_unloading = ktrue;
    ktmr_kill(__g_sig_tmr);
    return 0;
}

/**
 * @brief if some device has channels do not scan it, because scan will cause the old channel deleted.
 */
static int IMWCH(plwch_mod_loaded)
{
    kchar **devlist, **chlist;
    KMediaDevice *dev;

    if (!__g_try_scan_when_mod_loaded)
        return 0;

    devlist = __g_mc->getMediaDeviceList();
    if (!devlist)
        return 0;

    for (int i = 0; devlist[i]; i++)
        if (dev = __g_mc->getMediaDeviceFromDevice(devlist[i])) {
            chlist = dev->getMediaChannelList();
            if (!chlist || !chlist[0]) {
                kmsg_post(__g_worker_thread, KMPL_SCAN, kstr_dup(devlist[i]), (kvoid*)ktrue, knil, knil);
                kmem_free(chlist);
            }
        }

    if (devlist)
        kmem_free(devlist);

    return 0;
}

int IMWCH(plwch_liveadComplete)
{
    if (__g_unloading)
        return -1;


    kchar * fromDir = (char*)REC_CUR_STR(rec);
    kchar * moduleDir = kim_getstr(im, "s.env.path.moduleDir", knil);

    kchar fromDirBuf[260], moduleDirBuf[260];

    strcpy(fromDirBuf, fromDir);
    strcat(fromDirBuf, "\\playlist");

    strcpy(moduleDirBuf, moduleDir);
    strcat(moduleDirBuf, "\\playlist\\page\\live");

    /* move the default page to current dir */
    kstr_subs(fromDirBuf, '\\', kvfs_path_sep());
    kstr_subs(moduleDirBuf, '\\', kvfs_path_sep());

    /* delete old to avoid junks */
    kvfs_rmdir(moduleDirBuf);

    /* copy the new */
    kvfs_cpdir(fromDirBuf, moduleDirBuf);

    /*
     * Update the url
     */
    update_urls(kfalse);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// kmsg dispatch
static kint om_scan(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    kchar *devHash = (kchar*)a_ar0, **devlist = knil;
    kbool start = (kbool)a_ar1;
    kbool *success = (kbool*)a_ar3;
    kint ret = EC_NG;
    KMediaDevice *dev;
    kint i;

    if ((!__g_unloading) && (!a_rsn)) {
        if (start) {
            if (devHash && (dev = __g_mc->getMediaDeviceFromDevice(devHash))) {
                if (!dev->isStarted())
                    dev->start();

                ret = dev->updateChannelList();
            } else if (!devHash) {
                devlist = __g_mc->getMediaDeviceList();
                for (i = 0; devlist[i]; i++) {
                    if (dev = __g_mc->getMediaDeviceFromDevice(devHash)) {
                        if (!dev->isStarted())
                            dev->start();
                        ret = dev->updateChannelList();
                    }
                }
            }
        } else {
            if (devHash && (dev = __g_mc->getMediaDeviceFromDevice(devHash))) {
                ret = dev->cancelUpdateChannelList();
                if (dev->isStarted())
                    dev->stop();
            } else if (!devHash) {
                devlist = __g_mc->getMediaDeviceList();
                for (i = 0; devlist[i]; i++) {
                    if (dev = __g_mc->getMediaDeviceFromDevice(devHash)) {
                        ret = dev->cancelUpdateChannelList();
                        if (dev->isStarted())
                            dev->stop();
                    }
                }
            }
        }
    }
    if (success)
        *success = (EC_OK == ret) ? ktrue : kfalse;

    kmem_free_s(a_ar0);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// watches
static struct _wchmap {
    kbean bean;
    kchar ab;
    kchar *name;
    WCHPROC proc;
} __g_wchmap[] = {
    { knil, 'b', "i.kmc.evt.channel.switchStart", plwch_channelSwitchStart },
    { knil, 'b', "i.kmc.evt.channel.switchEnd", plwch_channelSwitchEnd },
    { knil, 'b', "i.kmc.evt.device.new", plwch_deviceNew },
    { knil, 'b', "i.kmc.evt.device.del", plwch_deviceDel },
    { knil, 'b', "i.kmc.evt.device.froze", plwch_deviceFroze },
    { knil, 'b', "i.kmc.evt.channel.new", plwch_channelNew },
    { knil, 'b', "i.kmc.evt.channel.chg", plwch_channelNew },
    { knil, 'b', "i.kmc.evt.channel.froze", plwch_channelFroze },
    { knil, 'b', "i.kmc.evt.channel.del", plwch_channelDel },
    { knil, 'b', "i.kmm.evt.modulesLoaded", plwch_mod_loaded },
    { knil, 'b', "i.kmm.evt.modulesUnload", plwch_mod_unload },
    { knil, 'a', "s.livead.evt.dataComplete", plwch_liveadComplete },
};

static void add_im_watches()
{
    for (int i = 0; i < sizeof(__g_wchmap) / sizeof(struct _wchmap); i++)
        if (__g_wchmap[i].ab == 'a')
            __g_wchmap[i].bean = kim_addawch(__g_im, __g_wchmap[i].name, __g_wchmap[i].proc, knil, knil, knil);
        else
            __g_wchmap[i].bean = kim_addbwch(__g_im, __g_wchmap[i].name, __g_wchmap[i].proc, knil, knil, knil);
}

static void del_im_watches()
{
    for (int i = 0; i < sizeof(__g_wchmap) / sizeof(struct _wchmap); i++)
        if (__g_wchmap[i].ab == 'a')
            kim_delawch(__g_im, __g_wchmap[i].bean);
        else
            kim_delbwch(__g_im, __g_wchmap[i].bean);
}

/////////////////////////////////////////////////////////////////////////////
// standard KMM routines
extern "C" EXPORT_FUN void mm_hey(KIM *im)
{
    klog(("into playlist hey, you may add some init code here\n"));

    SET_GLOBALS(im);

    /////////////////////////////////////////////////////////////////////////////
    // load saved RTI from disk
    __g_plrti.im = im;

    ktmr_init();

    char iniPath[1024];
    sprintf(iniPath, "%s\\playlist\\playlist.ini", kim_getstr(im, "s.env.path.moduleDir", knil));
    kstr_subs(iniPath, '\\', kvfs_path_sep());

    if (!kini_getint("general", "autoPlay", &__g_plrti.autoPlay, iniPath))
        __g_plrti.autoPlay = 1;

    if (!kini_getint("general", "loopPlay", &__g_plrti.loopPlay, iniPath))
        __g_plrti.loopPlay = 1;

    if (!kini_getint("general", "randomPlay", &__g_plrti.randomPlay, iniPath))
        __g_plrti.randomPlay = 0;

    if (!kini_getint("channel", "volume", &__g_plrti.volume, iniPath))
        __g_plrti.volume = 50;

    kim_addint(im, "i.playlist.media.volume", __g_plrti.volume, RF_AUTOSET, knil, knil);

    if (!kini_getint("channel", "mute", &__g_plrti.mute, iniPath))
        __g_plrti.mute = 0;

    kim_addint(im, "i.playlist.media.mute", __g_plrti.mute, RF_AUTOSET, knil, knil);

    char retValue[64];
    int ret = kini_getstr("channel", "curChannel", retValue, sizeof(retValue), iniPath);
    if (ret) {
        __g_plrti.setCurChannel(retValue);
        kim_addstr(im, "s.playlist.channel.current", retValue, 0, knil, "chhash");
        kim_addstr(im, "s.playlist.channel.autoplay", retValue, RF_AUTOSET, knil, knil);
    } else {
        kim_addstr(im, "s.playlist.channel.current", knil, 0, knil, "chhash");
        kim_addstr(im, "s.playlist.channel.autoplay", "", RF_AUTOSET, knil, knil);
    }

    kim_addstr(im, "s.playlist.device.current", knil, 0, knil, "devhash");

    if (!kini_getint("scan", "try_when_loaded", &__g_try_scan_when_mod_loaded, iniPath))
        __g_try_scan_when_mod_loaded = 0;
    if (!kini_getint("scan", "try_when_defreeze", &__g_try_scan_when_dev_defreeze, iniPath))
        __g_try_scan_when_dev_defreeze = 0;

    void loadui(KIM *im);
    // XXX loadui(im);

    update_urls(ktrue);

    // kim
    /* show or hide the media window */
    kim_addint(im, "i.playlist.media.show", 0, RF_AUTOSET, knil, knil);
    /* current channel */
    kim_addint(im, "i.playlist.media.progress", knil, 0, knil, knil);
    /* current channel */
    kim_addint(im, "i.playlist.media.speed", 100, RF_AUTOSET, knil, knil);
    /* current channel, 0 - 99, < 0 means not support */
    kim_addint(im, "i.playlist.media.signal", -1, RF_AUTOSET, knil, knil);

    // kmc watches
    /* if container change the size, video should resize too */
    add_im_watches();

    // worker thread
    __g_worker_thread = ktsk_new("playlist", knil, 0, 0, knil, knil, knil, knil);

    /* ar0(devHash, 0 for all) */
    kmsg_slot_set(__g_worker_thread, KMPL_SCAN, om_scan);

    // ONLY, start ONCE on BOOT.
    // Main
}

extern "C" EXPORT_FUN void mm_bye(KIM *im)
{
    klog(("into playlist bye\n"));

    /* quit worker thread */
    if (__g_worker_thread)
        ktsk_del(__g_worker_thread);

    // save settings
    char iniPath[1024], *tmp;
    sprintf(iniPath, "%s\\playlist\\playlist.ini", kim_getstr(im, "s.env.path.moduleDir", knil));
    kstr_subs(iniPath, '\\', kvfs_path_sep());

    kini_setint("general", "autoPlay", __g_plrti.autoPlay, iniPath);
    kini_setint("general", "loopPlay", __g_plrti.loopPlay, iniPath);
    kini_setint("general", "randomPlay", __g_plrti.randomPlay, iniPath);

    kini_setint("channel", "volume", kim_getint(__g_im, "i.playlist.media.volume", knil), iniPath);
    kini_setint("channel", "mute", kim_getint(__g_im, "i.playlist.media.mute", knil), iniPath);

    tmp = kim_getstr(__g_im, "s.playlist.channel.current", knil);
    kini_setstr("channel", "curChannel", tmp ? tmp : "", iniPath);

    del_im_watches();

    kim_delint(__g_im, "i.playlist.media.signal");
    kim_delint(__g_im, "i.playlist.media.speed");
    kim_delint(__g_im, "i.playlist.media.progress");
    kim_delint(__g_im, "i.playlist.media.show");
    kim_delstr(__g_im, "s.playlist.device.current");
    kim_delstr(__g_im, "s.playlist.channel.autoplay");
    kim_delstr(__g_im, "s.playlist.channel.current");
    kim_delint(__g_im, "i.playlist.media.mute");
    kim_delint(__g_im, "i.playlist.media.volume");
    kim_delstr(__g_im, "s.playlist.url.scanning_failed");
    kim_delstr(__g_im, "s.playlist.url.scanning");
    kim_delstr(__g_im, "s.playlist.url.no_device");
    kim_delstr(__g_im, "s.playlist.url.loading");
    kim_delstr(__g_im, "s.playlist.url.no_prg_live");
    kim_delstr(__g_im, "s.playlist.url.no_prg_vod");
    kim_delstr(__g_im, "s.playlist.url.no_prg_rec");

    ktmr_final();
}

extern "C" EXPORT_FUN void mm_guid(KIM *im, char **retguid)
{
    klog(("into playlist guid, %s\n", guid));
    *retguid = guid;
}

extern "C" EXPORT_FUN void jc_playlist_device_scan(KIM *im, kchar *ar0, kchar *ar1, kchar *ar2, kchar *ar3, kchar **pVarResult)
{
    kmsg_send(__g_worker_thread, KMPL_SCAN, kstr_dup(ar0), ar1, ar2, ar3);
}

