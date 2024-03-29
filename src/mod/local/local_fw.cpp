/* vim:set et sw=4 sts=4 ff=unix: */
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

#include "mc/local-protocol.h"

static char guid[] = "7D378382-9351-4f4e-BF83-4FF20C456B6D";
static kbean __g_worker_thread = knil;

static kchar *__g_mod_dir = knil;

static KMC_LocalProtocol *__g_local_protocol = knil;

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

void *mediaWindow;
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

    __g_local_protocol = new KMC_LocalProtocol(__g_im, __g_mc, "mc-local-protocol", 1);
    __g_local_protocol->scanDevice();

    /* XXX test */
    int argc = kim_getint(im, "i.env.argc", knil);
    char **argv = (char**)kim_getptr(im, "p.env.argv", knil);

    mediaWindow = (void*)kim_getptr(im, "p.ui.ui.window.optn", knil);
    int __not_main(int argc, char *argv[]);
    //__not_main(argc, argv);
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

