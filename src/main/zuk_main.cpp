/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <xmldoc.h>
#include <kmem.h>
#include <ktmr.h>
#include <kdbg.h>
#include <kim.h>
#include <ksal.h>
#include <kini.h>
#include <kstr.h>
#include <ktsk.h>

#include <textconv.h>

#include <kmm.h>
#include <kmccontainer.h>

#if defined(__WIN32__)
#else
#include <unistd.h>
#include <langinfo.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <ktypes.h>
#include <kdbg.h>
#include <kstr.h>
#include <ktmr.h>

#include <kim.h>

#include "sysdeps.h"
#include "options.h"

static KMM* __g_mm = knil;
static KMediaContainer* __g_mc = knil;

#define ERSN_QUIT               0
#define ERSN_REBOOT             11

static kbool get_sysinfo(KIM *im)
{
    FILE *s;
    kchar osid[128], osver[40];

#if defined(__WIN32__) || defined(__WINCE__)
    OSVERSIONINFO ovf;
    ZeroMemory(&ovf, sizeof(ovf));
    ovf.dwOSVersionInfoSize = sizeof(ovf);
    if (!GetVersionEx(&ovf))
        return kfalse;

    switch (ovf.dwPlatformId) {
        case VER_PLATFORM_WIN32s:
            strncpy(osid, "Win32s", sizeof(osid) - 1);
            break;
        case VER_PLATFORM_WIN32_WINDOWS:
            strncpy(osid, "Win9x", sizeof(osid) - 1);
            break;
#if defined(__WINCE__)
        case VER_PLATFORM_WIN32_CE:
            strncpy(osid, "WinCE", sizeof(osid) - 1);
            break;
#endif
        case VER_PLATFORM_WIN32_NT:
        default:
            strncpy(osid, "WinNT", sizeof(osid) - 1);
            break;
    }

    kim_addstr(im, "s.env.os.id", osid, RF_AUTOSET, knil, "PlatformId");

    sprintf(osver, "%d.%d.%d", ovf.dwPlatformId, ovf.dwMajorVersion, ovf.dwMinorVersion);
    kim_addstr(im, "s.env.os.ver", osver, RF_AUTOSET, knil, "Install directory of program");

    return ktrue;
#else
    memset(osid, '\0', sizeof(osid));
    s = popen("uname -s", "r");
    if (!s)
        return kfalse;

    fread(osid, sizeof(char), sizeof(osid), s);
    kstr_subs(osid, '\r', '\n');
    kstr_subs(osid, '\n', '\0');
    kim_addstr(im, "s.env.os.id", osid, RF_AUTOSET, knil, "PlatformId");
    pclose(s);

    memset(osver, '\0', sizeof(osver));
    s = popen("uname -r", "r");
    if (!s)
        return kfalse;

    fread(osver, sizeof(char), sizeof(osver), s);
    kstr_subs(osver, '-', '\0');
    kim_addstr(im, "s.env.os.ver", osver, RF_AUTOSET, knil, "version");
    pclose(s);

    return ktrue;
#endif
}

static kbool check_application(KIM *im)
{
    kuint major, minor, rev, os_curver, os_maxver, os_minver;
    kchar osid[128], osver[40];
    kbool rc = kfalse;
    kbean cfgfile;
    kchar cfgpath[1024], *instdir, *buf, *val;
    kint buflen, k, cnt;

    KXmlDoc *doc = knil;
    KXmlNode *node;

    instdir = kim_getstr(im, "s.env.path.instDir", knil);
    sprintf(cfgpath, "%s%ccfg.xml", instdir, kvfs_path_sep());

    buf = knil;
    buflen = 0x7FFFFFFF;
    if (!(cfgfile = kvfs_open(cfgpath, "rt", 0))) {
        klog(("Access error [%s], ignored\n", cfgpath));
        return kfalse;
    }

    doc = xmldoc_new((KXmlDoc*)knil);
    kvfs_read(cfgfile, (void**)&buf, &buflen);
    kvfs_close(cfgfile);

    xmldoc_parse(doc, buf, buflen);
    kmem_free(buf);

    if (!(node = xmldoc_gotoNode(doc, "module", 0))) {
        kerror(("chkapp: no module element\n"));
        goto quit;
    }

    if (!(val = xmlnode_get_attr_value(node, "id"))) {
        kerror(("chkapp: no id element\n"));
        goto quit;
    }
    if (kstr_cmp(val, "zuk", 0)) {
        kerror(("chkapp: id is not zuk, but %s\n", val));
        goto quit;
    }
    kim_addstr(im, "s.env.app.name", val, RF_AUTOSET, knil, "appName");

    if (!(val = xmlnode_get_attr_value(node, "ver"))) {
        kerror(("chkapp: no ver element\n"));
        goto quit;
    }
    kim_addstr(im, "s.env.app.ver", val, RF_AUTOSET, knil, "version");

    /*
     * Fill Platform
     */

    /* current os version */
    major = minor = rev = 0;
    sprintf(osver, "%s", kim_getstr(im, "s.env.os.ver", knil));
    sscanf(osver, "%u.%u.%u", &major, &minor, &rev);
    os_curver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

    cnt = xmldoc_getNodeCnt(doc, "platform");
    for (k = 0; k < cnt; k++) {
        node = xmldoc_getNode(doc, "platform", k);

        if (!(val = xmlnode_get_attr_value(node, "id"))) {
            kerror(("chkapp: no id attr\n"));
            continue;
        }
        sprintf(osid, "%s", kim_getstr(im, "s.env.os.id", knil));
        if (kstr_icmp(val, osid, 0)) {
            kerror(("chkapp: mismatch OS, cur is %s, wanted is %s\n", osid, val));
            continue;
        }

        /* wanted max version */
        if (!(val = xmlnode_get_attr_value(node, "maxver"))) {
            kerror(("chkapp: no os_maxver attr\n"));
            continue;
        }
        major = minor = rev = 0;
        sscanf(val, "%u.%u.%u", &major, &minor, &rev);
        os_maxver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

        /* wanted min version */
        if (!(val = xmlnode_get_attr_value(node, "minver"))) {
            kerror(("chkapp: no os_minver attr\n"));
            continue;
        }
        major = minor = rev = 0;
        sscanf(val, "%u.%u.%u", &major, &minor, &rev);
        os_minver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

        klog(("chkapp: os_curver:%08x, os_maxver:%08x, os_minver: %08x\n", os_curver, os_maxver, os_minver));
        if (os_curver >= os_minver && os_curver <= os_maxver) {
            rc = ktrue;
            goto quit;
        }
    }

quit:
    xmldoc_del(doc);

    return rc;
}

static kint fill_path_im_env(KIM *im, kint argc, kchar **argv)
{
    kchar buffer[1024], instdir[1024], ps[2] = { 0 };
    ps[0] = kvfs_path_sep();

    /* install dir, where the application in */
    kvfs_cwd(buffer, sizeof(buffer));
    ksal_exedir(buffer, argv, instdir);

    /* install dir, where the application in */
    kim_addstr(im, "s.env.path.instDir", instdir, RF_AUTOSET, knil, "Install directory of program");
    klog(("instdir: %s\n", buffer));

    /* modules's dir */
    sprintf(buffer, "%s%smodules", instdir, ps);
    kim_addstr(im, "s.env.path.moduleDir", buffer, RF_AUTOSET, knil, "modules' directory");

    /* cache dir */
    sprintf(buffer, "%s%scache", instdir, ps);
    kvfs_mkdir(buffer);
    kim_addstr(im, "s.env.path.cacheDir", buffer, RF_AUTOSET, knil, "caches' directory");

    /* help dir */
    sprintf(buffer, "%s%sdoc\\help\\index.html", instdir, ps);
    kstr_subs(buffer, '\\', ps[0]);
    kstr_subs(buffer, kvfs_path_sep(), '/');
    kim_addstr(im, "s.env.path.help", buffer, RF_AUTOSET, knil, knil);

    return 0;
}

static kint fill_lang_im_env(KIM *im, kint argc, kchar **argv)
{
    kchar buffer[1024], instdir[1024], ps[2] = { 0 };
    /* App Data dir */
#if defined(__WIN32__)
    sprintf(buffer, "%s\\dyi", getenv("APPDATA"));
#elif defined(__UNIX__)
    sprintf(buffer, "%s/.zuk/appdata", getenv("HOME"));
#endif
    kvfs_mkdir(buffer);
    kim_addstr(im, "s.env.path.appDataDir", buffer, RF_AUTOSET, knil, "user application data directory");

    /* make nl_langinfo works correctly */
    setlocale(LC_ALL, "");

    /* language */
#if defined(__WIN32__)
    /* win32 return LANG=zh_CN LOCALE_SENGCOUNTRY*/
    LANGID lid = GetUserDefaultLangID();
    kchar *lang = "en_US";
    if (0x04 == (lid & 0xFF)) {
        lang = "zh_TW";
        if (0x0800 == (lid & 0xFF00)) {
            lang = "zh_CN";
        }
    }
    kim_addstr(im, "s.env.language", lang, RF_AUTOSET, knil, "language");

    GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTCODEPAGE, buffer, sizeof(buffer));
    kim_addstr(im, "s.env.encoding", buffer, RF_AUTOSET, knil, "codepage");
#elif defined(__UNIX__)
    /* win32 return LANG=zh_CN.UTF-8 */
    strcpy(buffer, getenv("LANG"));
    kstr_subs(buffer, '.', '\0');
    kim_addstr(im, "s.env.language", buffer, RF_AUTOSET, knil, "language");

    const kchar *codeset;
    if (!(codeset = nl_langinfo(CODESET)))
        codeset = "";
    kim_addstr(im, "s.env.encoding", (kchar*)codeset, RF_AUTOSET, knil, "codepage");
#endif

#if defined(__WIN32__)
#else
    strcpy(buffer, getenv("LANG"));
    setlocale(LC_MESSAGES, buffer);
#endif
}


/**
 * @brief main
 *
 * @return 0: normal quit, 1: normal reboot
 */
kint zuk_init(KIM *im, kint argc, kchar **argv)
{
    kbean loading_application = knil;

    zuk_opt *opt;
    kint i;

    kchar *o, buffer[1024], *instdir, inipath[1024], ps[2] = { 0 };
    ps[0] = kvfs_path_sep();

    fill_path_im_env(im, argc, argv);

    instdir = kim_getstr(im, "s.env.path.instdir", knil);

    /* install dir, where the application in */
    kvfs_cwd(buffer, sizeof(buffer));
    sprintf(inipath, "%s%smain.ini", instdir, ps);

    opt = zuk_opt_init();
    read_ini_opt(opt, inipath);
    read_arg_opt(opt, argc, argv);

    if (read_opt(opt, "help")) {
        show_help(opt);
        zuk_opt_final(opt);
        return 0;
    }

    if (o = read_opt(opt, "log-level"))
        kdbg_init(atoi(o));

    /* TODO check single instance */
    if ((o = read_opt(opt, "skip-single")) && ('1' == o[0])) {
        /* TODO mark single instance */
    }

    /* TODO install update */
    if ((o = read_opt(opt, "skip-update")) && ('1' == o[0])) {
    }

    if ((o = read_opt(opt, "show-loading")) && ('1' == o[0])) {
        sprintf(buffer, "%s%sz-loading", instdir, ps);
        ksal_exec(kfalse, kfalse, &loading_application, 0, buffer, 0);
        kim_addptr(im, "p.misc.app.bean.loading", loading_application, RF_AUTOSET, knil, knil);
    }

    /* initialize the timer, needed by SubscriberId */
    ktmr_init();

    /* set "x.env.os.xxx" */
    if (kfalse == get_sysinfo(im))
        return -1;

    /* application info */
    if (kfalse == check_application(im))
        return -1;

    /* system depends init */
    zuk_sysdeps_start(im, argc, argv);

    /* KMC */
    klog(("kmc_init\n"));
    KMediaContainer *mc = new KMediaContainer(im, "K Media Container");
    __g_mc = mc;
    kim_addptr(im, "p.sys.kmc", mc, RF_AUTOSET, NULL, "K Media Container");

    /* KMM */
    klog(("kmm_init\n"));
    kbean mm = kmm_init(im);
    __g_mm = (KMM*)mm;
    kim_addptr(im, "p.sys.kmm", mm, RF_AUTOSET, knil, "K Module Manager");

    fill_lang_im_env(im, argc, argv);

#if defined(KCFG_GETTEXT)
    bindtextdomain("zuk", buffer);
    textdomain("zuk");
#endif

    /* load all the modules */
    klog(("kmm_load_modules start\n"));
    kmm_load_modules(mm);
    klog(("kmm_load_modules end\n"));
}

kint zuk_show(KIM *im, kint argc, kchar **argv)
{
    return kim_setint(im, "i.ui.act.show", 1, knil, knil);
}

kint zuk_final(KIM *im, kint argc, kchar **argv)
{
    if (__g_mm)
        kmm_final(__g_mm);

    if (__g_mc)
        delete __g_mc;

    ktmr_final();

    klog(("leave zuk\n"));
    return ERSN_QUIT;
}

