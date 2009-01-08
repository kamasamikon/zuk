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

    GtkWidget *main_win, *button[8], *vbox;

    /* called by main */
    /* gtk_init(&argc, &argv); */

    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "zuk");
    gtk_widget_set_size_request(GTK_WIDGET(main_win), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    for (int i = 0; i < 8; i++) {
        kchar label[111];

        sprintf(label, "Shit %c", 'A' + i);
        button[i] = gtk_button_new_with_label(label);

        gtk_box_pack_start(GTK_BOX(vbox), button[i], FALSE, FALSE, 0);

        g_signal_connect(G_OBJECT(button[i]), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    }

    gtk_container_add(GTK_CONTAINER(main_win), vbox);

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
    kchar **ret = (kchar**)pVarResult;
    GtkWidget *main_win;

    if (!ret)
        return;

    /* called by main */
    /* gtk_init(&argc, &argv); */

    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "zuk");

    // kim_addptr(im, "p.ui.window.main", (kvoid*)main_win, RF_AUTOSET, knil, knil);

    g_signal_connect(G_OBJECT(main_win), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(main_win);

    *ret = (kchar*)main_win;
}

