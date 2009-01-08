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
static void ui_create_window(KIM *im)
{
    GtkWidget *main_win, *button[8], *vbox;
    GtkWidget *menubar, *file, *tools, *help, *filemenu, *toolsmenu, *helpmenu;
    GtkAccelGroup *group;


    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "zuk");
    gtk_widget_set_size_request(GTK_WIDGET(main_win), 320, 240);

    group = gtk_accel_group_new();
    menubar = gtk_menu_bar_new();
    file = gtk_menu_item_new_with_mnemonic("_File");
    tools = gtk_menu_item_new_with_mnemonic("_Tools");
    help = gtk_menu_item_new_with_mnemonic("_Help");
    filemenu = gtk_menu_new();
    toolsmenu = gtk_menu_new();
    helpmenu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools), toolsmenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), tools);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);

    /* Create the File menu content. */
    GtkWidget *open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, group);
    GtkWidget *sep = gtk_separator_menu_item_new();
    GtkWidget *exit = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, group);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), exit);

    /* Create the Edit menu content. */
    GtkWidget *option = gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT, group);
    gtk_menu_shell_append(GTK_MENU_SHELL(toolsmenu), option);

    /* Create the Help menu content. */
    GtkWidget *contents = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, group);
    GtkWidget *about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, group);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), contents);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);

    vbox = gtk_vbox_new(TRUE, 1);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    for (int i = 0; i < 8; i++) {
        kchar label[111];

        sprintf(label, "Shit %c", 'A' + i);
        button[i] = gtk_button_new_with_label(label);

        gtk_box_pack_start(GTK_BOX(vbox), button[i], FALSE, TRUE, 0);

        g_signal_connect(G_OBJECT(button[i]), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    }


    gtk_container_add(GTK_CONTAINER(main_win), vbox);

    kim_addptr(im, "p.ui.window.main", (kvoid*)main_win, RF_AUTOSET, knil, knil);

    kim_addptr(im, "p.ui.window.main.menu", (kvoid*)main_win, RF_AUTOSET, knil, knil);
    kim_addptr(im, "p.ui.window.main.menu", (kvoid*)main_win, RF_AUTOSET, knil, knil);

    g_signal_connect(G_OBJECT(main_win), "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ktmr routines

/////////////////////////////////////////////////////////////////////////////
// kimat routines
static kint imat_ui_ui_show(struct _KIM *im, struct _KRtiRec *rec, kuchar reason)
{
    kint show = (kint)REC_SET(rec);
    GtkWidget *window = (GtkWidget*)kim_getptr(im, "p.ui.window.main", knil);

    if (show)
        gtk_widget_show_all(window);
    else
        gtk_widget_hide_all(window);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// watch routines

/////////////////////////////////////////////////////////////////////////////
// kmsg dispatch

/////////////////////////////////////////////////////////////////////////////
// standard KMM routines
/**
 * \brief UI module should startup first, so that it can watch other's module's window.
 */
extern "C" EXPORT_FUN void mm_hey(KIM *im)
{
    klog(("into ui hey, THIS SHOULD BE FIRST MOD TO BE CALLED!\n"));
    SET_GLOBALS(im);

    ui_create_window(im);

    kim_addint(im, "i.ui.show", 0, RF_AUTOSET, imat_ui_ui_show, knil);
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

