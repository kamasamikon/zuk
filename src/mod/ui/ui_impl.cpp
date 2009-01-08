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

#include <gtk/gtk.h>

static char guid[] = "52727E3B-08FD-4664-97B1-4CBABD17C985";

static kchar *__g_mod_dir = knil;

/////////////////////////////////////////////////////////////////////////////
// defines

/////////////////////////////////////////////////////////////////////////////
// support routines

/////////////////////////////////////////////////////////////////////////////
// ktmr routines

/////////////////////////////////////////////////////////////////////////////
// kimat routines

/////////////////////////////////////////////////////////////////////////////
// watch routines

/////////////////////////////////////////////////////////////////////////////
// kmsg dispatch

/////////////////////////////////////////////////////////////////////////////
// standard KMM routines
/**
 * \brief
 *
 * o ui first, watch all the other modules window.
 * o ui last, scan all the modules to find window.
 */
extern "C" EXPORT_FUN void mm_hey(KIM *im)
{
    /* XXX */
    klog(("into ui hey, THIS SHOULD BE FIRST MOD TO BE CALLED!\n"));
    SET_GLOBALS(im);

    GtkWidget *main_win;

    /* called by main */
    /* gtk_init(&argc, &argv); */

    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "zuk");

    kim_addptr(im, "p.ui.window.main", (kvoid*)main_win, RF_AUTOSET, knil, knil);

    g_signal_connect(G_OBJECT(main_win), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(main_win);

    /* called by main */
    /* gtk_main(); */
}

extern "C" EXPORT_FUN void mm_bye(KIM *im)
{
}

extern "C" EXPORT_FUN void mm_guid(KIM *im, char **retguid)
{
    klog(("into ui guid, %s\n", guid));
    *retguid = guid;
}

extern "C" EXPORT_FUN void jc_ui_get_script(KIM *im, kchar *ar0, kchar *ar1, kchar *ar2, kchar *ar3, kchar **pVarResult)
{
    kchar path[1024], *buf = knil, **ret = (kchar**)pVarResult;
    kint retlen = 0;

    if (!ret)
        return;

    *ret = "";
    sprintf(path, "%s/ui/pl.js", __g_mod_dir);
    *ret = buf;
}

