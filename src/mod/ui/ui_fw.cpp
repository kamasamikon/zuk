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

#include <gtk/gtk.h>

static char guid[] = "52727E3B-08FD-4664-97B1-4CBABD17C985";

static kchar *__g_mod_dir = knil;

/////////////////////////////////////////////////////////////////////////////
// defines
static void on_about_activate(GtkAction *action, GtkWidget *window)
{
    const gchar *authors[] = {
        "auv <kamasamikon@gmail.com>",
        NULL
    };

    const gchar *documentors[] = {
        "auv <kamasamikon@gmail.com>",
        NULL
    };

    gtk_show_about_dialog(GTK_WINDOW(window),
            "name", "zuk! zuk!",
            "version", "0.0.1",
            "copyright", "(C) 1997-2009 DYI",
            "website", "http://www.zuk.org",
            "comments", "zuk.",
            "authors", authors,
            "documenters", documentors,
            "title", "About zuk!",
            NULL);
}

static void _show_info_window(GtkWidget *button, KIM *im)
{
    GtkWidget *window = (GtkWidget*)kim_getptr(im, "p.ui.ui.window.info", knil);
    gtk_widget_show(window);
}

static void _show_optn_window(GtkWidget *button, KIM *im)
{
    GtkWidget *window = (GtkWidget*)kim_getptr(im, "p.ui.ui.window.optn", knil);

    /* fill all the modules to the list */
    gtk_widget_show(window);
}

/////////////////////////////////////////////////////////////////////////////
// support routines
static GtkWidget *ui_ui_window_main_menubar_create(KIM *im)
{
    GtkWidget *menubar, *file, *tools, *help, *filemenu, *toolsmenu, *helpmenu;
    GtkAccelGroup *group;

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

    g_signal_connect((gpointer)about, "activate", G_CALLBACK(on_about_activate), NULL);

    return menubar;
}

static GtkWidget *ui_ui_window_main_create(KIM *im)
{
    GtkWidget *main_win, *button[3], *vbox, *ctrl;

    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "zuk! zuk!");
    gtk_widget_set_size_request(GTK_WIDGET(main_win), 320, 240);

    vbox = gtk_vbox_new(FALSE, 0);

    GtkWidget *win_main_menubar = ui_ui_window_main_menubar_create(im);
    kim_addptr(im, "p.ui.ui.window.main.menubar", (kvoid*)win_main_menubar, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_main_menubar, FALSE, FALSE, 0);

    ctrl = gtk_button_new_with_label("show info");
    gtk_box_pack_start(GTK_BOX(vbox), ctrl, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(ctrl), "clicked", G_CALLBACK(_show_info_window), im);

    ctrl = gtk_button_new_with_label("show optn");
    gtk_box_pack_start(GTK_BOX(vbox), ctrl, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(ctrl), "clicked", G_CALLBACK(_show_optn_window), im);

    for (int i = 0; i < G_N_ELEMENTS(button); i++) {
        kchar label[111];

        sprintf(label, "Shit %c", 'A' + i);
        button[i] = gtk_button_new_with_label(label);

        gtk_box_pack_start(GTK_BOX(vbox), button[i], FALSE, TRUE, 0);

        g_signal_connect(G_OBJECT(button[i]), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    }

    gtk_container_add(GTK_CONTAINER(main_win), vbox);

    g_signal_connect(G_OBJECT(main_win), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return main_win;
}

static GtkWidget *ui_ui_window_info_trigger_create(KIM *im)
{
    GtkWidget *trigger, *hbox;

    trigger = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    hbox = gtk_hbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(trigger), hbox);

    return trigger;
}

static GtkWidget *ui_ui_window_info_content_create(KIM *im)
{
    GtkWidget *content, *vbox;

    content = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    vbox = gtk_vbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(content), vbox);

    return content;
}

static GtkWidget *ui_ui_window_info_create(KIM *im)
{
    GtkWidget *win_info, *vbox;

    win_info = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win_info), "info");
    gtk_widget_set_size_request(GTK_WIDGET(win_info), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    /* add trigger bar */
    GtkWidget *win_info_trigger = ui_ui_window_info_trigger_create(im);
    kim_addptr(im, "p.ui.ui.window.info.trigger", (kvoid*)win_info_trigger, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_info_trigger, FALSE, FALSE, 0);

    /* add content bar */
    GtkWidget *win_info_content = ui_ui_window_info_content_create(im);
    kim_addptr(im, "p.ui.ui.window.info.content", (kvoid*)win_info_content, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_info_content, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(win_info), vbox);

    return win_info;
}

static GtkWidget *ui_ui_window_opt_trigger_create(KIM *im)
{
    GtkWidget *trigger, *hbox;

    trigger = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    hbox = gtk_hbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(trigger), hbox);

    return trigger;
}

static GtkWidget *ui_ui_window_opt_content_create(KIM *im)
{
    GtkWidget *content, *vbox;

    content = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    vbox = gtk_vbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(content), vbox);

    return content;
}


static GtkWidget *ui_ui_window_opt_create(KIM *im)
{
    GtkWidget *win_opt, *vbox;

    win_opt = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win_opt), "optn");
    gtk_widget_set_size_request(GTK_WIDGET(win_opt), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    /* add trigger bar */
    GtkWidget *win_opt_trigger = ui_ui_window_opt_trigger_create(im);
    kim_addptr(im, "p.ui.ui.window.optn.trigger", (kvoid*)win_opt_trigger, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_opt_trigger, FALSE, FALSE, 0);

    /* add content bar */
    GtkWidget *win_opt_content = ui_ui_window_opt_content_create(im);
    kim_addptr(im, "p.ui.ui.window.optn.content", (kvoid*)win_opt_content, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_opt_content, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(win_opt), vbox);

    return win_opt;
}

static GtkWidget *ui_ui_window_media_trigger_create(KIM *im)
{
    GtkWidget *trigger, *vbox;

    trigger = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(trigger), "media");
    gtk_widget_set_size_request(GTK_WIDGET(trigger), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    gtk_container_add(GTK_CONTAINER(trigger), vbox);

    return trigger;
}

static GtkWidget *ui_ui_window_media_content_create(KIM *im)
{
    GtkWidget *content, *vbox;

    content = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(content), "media");
    gtk_widget_set_size_request(GTK_WIDGET(content), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    gtk_container_add(GTK_CONTAINER(content), vbox);

    return content;
}


static GtkWidget *ui_ui_window_media_create(KIM *im)
{
    GtkWidget *win_media, *vbox;

    win_media = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win_media), "media");
    gtk_widget_set_size_request(GTK_WIDGET(win_media), 320, 240);

    vbox = gtk_vbox_new(TRUE, 1);

    /* add trigger bar */
    GtkWidget *win_media_trigger = ui_ui_window_media_trigger_create(im);
    kim_addptr(im, "p.ui.ui.window.media.trigger", (kvoid*)win_media_trigger, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_media_trigger, FALSE, FALSE, 0);

    /* add content bar */
    GtkWidget *win_media_content = ui_ui_window_media_content_create(im);
    kim_addptr(im, "p.ui.ui.window.media.content", (kvoid*)win_media_content, RF_AUTOSET, knil, knil);
    gtk_box_pack_start(GTK_BOX(vbox), win_media_content, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(win_media), vbox);

    return win_media;
}

/**
 * \brief Create all the skel or container windows for modules but ui.
 *
 * main window.             p.ui.ui.window.main
 *      menu bar            p.ui.ui.window.main.menubar
 * optn window.             p.ui.ui.window.optn
 *      trigger/tool bar    p.ui.ui.window.optn.trigger
 *      content             p.ui.ui.window.optn.content
 * info window.             p.ui.ui.window.info
 *      trigger/tool bar    p.ui.ui.window.info.trigger
 *      content             p.ui.ui.window.info.content
 * media window             p.ui.ui.window.media
 *      ctrl bar            p.ui.ui.window.media.ctrlbar
 */
static void ui_create_ui(KIM *im)
{
    GtkWidget *win_main = ui_ui_window_main_create(im);
    kim_addptr(im, "p.ui.ui.window.main", (kvoid*)win_main, RF_AUTOSET, knil, knil);

    GtkWidget *win_info = ui_ui_window_info_create(im);
    kim_addptr(im, "p.ui.ui.window.info", (kvoid*)win_info, RF_AUTOSET, knil, knil);

    GtkWidget *win_media = ui_ui_window_media_create(im);
    kim_addptr(im, "p.ui.ui.window.media", (kvoid*)win_media, RF_AUTOSET, knil, knil);

    GtkWidget *win_opt = ui_ui_window_opt_create(im);
    kim_addptr(im, "p.ui.ui.window.optn", (kvoid*)win_opt, RF_AUTOSET, knil, knil);
}

static void status_icon_popup_menu_cb(GtkStatusIcon *status_icon,
        guint button, guint activate_time, gpointer *icon)
{
    GtkWidget *menu_item;
    GtkWidget *submenu;
    gint i;

    submenu = gtk_menu_new();

    for (i = 0; i < 5; i++) {
        menu_item = gtk_menu_item_new_with_label("sfasdf");
        gtk_widget_show(menu_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_item);
    }

    gtk_menu_popup (GTK_MENU (submenu), NULL, NULL, NULL,
            NULL, NULL, activate_time);
}

static void ui_create_status_icon(KIM *im)
{
    GtkStatusIcon *trayIcon;

    trayIcon = gtk_status_icon_new_from_file("/home/auv/right.png");
    gtk_status_icon_set_visible(trayIcon, TRUE);
    gtk_status_icon_set_blinking(trayIcon, TRUE);
    gtk_status_icon_set_tooltip(trayIcon, "auv is a good v!");

    g_signal_connect(trayIcon, "popup-menu", G_CALLBACK (status_icon_popup_menu_cb), NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ktmr routines

/////////////////////////////////////////////////////////////////////////////
// kimat routines
static kint imat_ui_act_show(struct _KIM *im, struct _KRtiRec *rec, kuchar reason)
{
    kint show = (kint)REC_SET(rec);
    GtkWidget *window = (GtkWidget*)kim_getptr(im, "p.ui.ui.window.main", knil);

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

    ui_create_ui(im);
    ui_create_status_icon(im);

    kim_addint(im, "i.ui.act.show", 0, RF_AUTOSET, imat_ui_act_show, "(M)::show or hide UI");

    // kim_addint(im, "i.ui.evt.page.new", 0, 0, imat_ui_evt_page_new, "(M):val='NA', ua='page ime-name':set immediately after created");
    // kim_addint(im, "i.ui.evt.page.del", 0, 0, imat_ui_evt_page_del, "(M):val='NA', ua='page ime-name':set immediately before deleted");
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
}

