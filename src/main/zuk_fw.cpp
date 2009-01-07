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

int main(int argc, char *argv[])
{
    int retval;
    GtkWidget *window;

    printf("QApplication app(argc, argv);\n");
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "zuk");

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    printf("zuk_init(argc, argv);\n");
    zuk_init(argc, argv);

    gtk_widget_show_all(window);

    printf("retval = app.exec();\n");
    gtk_main();

    printf("zuk_final(argc, argv);\n");
    zuk_final(argc, argv);

    printf("main:: return:%d\n", retval);
    return retval;
}

