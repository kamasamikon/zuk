#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__)
#include <windows.h>
#endif

#include <gtk/gtk.h>

#include <ktypes.h>
#include <ksal.h>
#include <kdbg.h>

#include <kim.h>
#include <kini.h>

#include "zuk_main.h"

/**
 * every mod register the window (with type), ui mod provider the frame
 */
int main(int argc, char *argv[])
{
    int retval;
    GtkWidget *window;

    kdbg_init();

    printf("gtk_init(&argc, &argv);\n");
    gtk_init(&argc, &argv);

    printf("KIM *im = kim_new(knil);\n");
    KIM *im = kim_new(knil);
    kim_start(im);

    // window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // gtk_window_set_title(GTK_WINDOW(window), "zuk");

    // g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    printf("zuk_init(im, argc, argv);\n");
    zuk_init(im, argc, argv);

    // kim_addptr(im, "p.ui.window.main", (kvoid*)main_win, RF_AUTOSET, knil, knil);
    window = (GtkWidget*)kim_getptr(im, "p.ui.window.main", knil);
    gtk_widget_show_all(window);

    printf("gtk_main();\n");
    gtk_main();

    printf("zuk_final(argc, argv);\n");
    zuk_final(im, argc, argv);

    kim_del(im);

    printf("main:: return:%d\n", retval);
    return retval;
}

