/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
#include <syscfg.h>
#include <setglobals.h>

#include <stdio.h>

#include <xmldoc.h>
#include <kmem.h>
#include <kdbg.h>
#include <ktsk.h>
#include <kstr.h>
#include <ktmr.h>
#include <ksal.h>

#include <textconv.h>

static char guid[] = "7D378382-9351-4f4e-BF83-4FF20C456B6D";
static kbean __g_worker_thread = knil;

static kchar *__g_mod_dir = knil;

/////////////////////////////////////////////////////////////////////////////
// defines
#define KMPL_SCAN                   0x00000001

/////////////////////////////////////////////////////////////////////////////
// support routines

/////////////////////////////////////////////////////////////////////////////
// kimat routines

/////////////////////////////////////////////////////////////////////////////
// watch routines

/////////////////////////////////////////////////////////////////////////////
// ktmr routines

/////////////////////////////////////////////////////////////////////////////
// kmsg dispatch

/////////////////////////////////////////////////////////////////////////////
// standard KMM routines
extern "C" EXPORT_FUN void mm_hey(KIM *im)
{
    klog(("into local hey, you may add some init code here\n"));
    SET_GLOBALS(im);

    ktmr_init();

    char iniPath[1024];
    sprintf(iniPath, "%s\\local\\main.ini", kim_getstr(im, "s.env.path.moduleDir", knil));
    kstr_subs(iniPath, '\\', kvfs_path_sep());

    // worker thread
    __g_worker_thread = ktsk_new("local", knil, 0, 0, knil, knil, knil, knil);
}

extern "C" EXPORT_FUN void mm_bye(KIM *im)
{
    klog(("into local bye\n"));

    /* quit worker thread */
    if (__g_worker_thread)
        ktsk_del(__g_worker_thread);

    // save settings
    char iniPath[1024], *tmp;
    sprintf(iniPath, "%s\\local\\main.ini", kim_getstr(im, "s.env.path.moduleDir", knil));
    kstr_subs(iniPath, '\\', kvfs_path_sep());

    ktmr_final();
}

extern "C" EXPORT_FUN void mm_guid(KIM *im, char **retguid)
{
    klog(("into local guid, %s\n", guid));
    *retguid = guid;
}

