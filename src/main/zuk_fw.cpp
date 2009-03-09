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

    /* Initialize GThread before calling any Glib or GTK+ functions. */
    g_thread_init(NULL);

#ifdef ENABLE_NLS
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif

#ifdef HAVE_SETLOCALE
    /* Locale initialization is not complete here.  See gtk_init_check() */
    setlocale(LC_ALL, "");
#endif

    printf("gtk_init(&argc, &argv);\n");
    gtk_init(&argc, &argv);

    printf("KIM *im = kim_new(knil);\n");
    KIM *im = kim_new(knil);
    kim_start(im);

    printf("zuk_init(im, argc, argv);\n");
    zuk_init(im, argc, argv);

    printf("zuk_show(im, argc, argv);\n");
    zuk_show(im, argc, argv);

    printf("gtk_main();\n");
    gtk_main();

    printf("zuk_final(argc, argv);\n");
    zuk_final(im, argc, argv);

    kim_del(im);

    printf("main:: return:%d\n", retval);
    return retval;
}

