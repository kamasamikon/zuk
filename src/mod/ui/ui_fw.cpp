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
#include <glade/glade.h>

static char guid[] = "52727E3B-08FD-4664-97B1-4CBABD17C985";

static kchar *__g_mod_dir = knil;

/////////////////////////////////////////////////////////////////////////////
// defines
extern "C" void on_winmain_menu_item_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *window_main = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.main", knil);

    const gchar *authors[] = {
        "auv <kamasamikon@gmail.com>",
        NULL
    };

    const gchar *documentors[] = {
        "auv <kamasamikon@gmail.com>",
        NULL
    };

    gtk_show_about_dialog(GTK_WINDOW(window_main),
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

extern "C" void on_winmain_menu_item_tool_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *window_tool = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.tool", knil);
    gtk_widget_show(window_tool);
}

extern "C" void on_winmain_menu_item_pref_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *window_pref = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.optn", knil);

    /* fill all the modules to the list */
    gtk_widget_show(window_pref);
}

/////////////////////////////////////////////////////////////////////////////
// support routines

/**
 * \brief Create all the skel or container windows for modules but ui.
 *
 * main window.             p.ui.ui.window.main
 * optn window.             p.ui.ui.window.optn
 * info window.             p.ui.ui.window.info
 * media window             p.ui.ui.window.media
 */
static void ui_create_ui(KIM *im)
{
    char xmlpath[1024];
    sprintf(xmlpath, "%s\\ui\\main.glade", kim_getstr(im, "s.env.path.moduleDir", knil));
    kstr_subs(xmlpath, '\\', kvfs_path_sep());

    GladeXML *gxml = glade_xml_new (xmlpath, NULL, NULL);

    GtkWidget *window_main = glade_xml_get_widget (gxml, "window_main");
    kim_addptr(im, "p.ui.ui.window.main", (kvoid*)window_main, RF_AUTOSET, knil, knil);

    GtkWidget *window_info = glade_xml_get_widget (gxml, "window_info");
    kim_addptr(im, "p.ui.ui.window.info", (kvoid*)window_info, RF_AUTOSET, knil, knil);

    GtkWidget *window_media = glade_xml_get_widget (gxml, "window_media");
    kim_addptr(im, "p.ui.ui.window.media", (kvoid*)window_media, RF_AUTOSET, knil, knil);

    GtkWidget *window_pref = glade_xml_get_widget (gxml, "window_pref");
    kim_addptr(im, "p.ui.ui.window.optn", (kvoid*)window_pref, RF_AUTOSET, knil, knil);

    GtkWidget *window_tool = glade_xml_get_widget (gxml, "window_tool");
    kim_addptr(im, "p.ui.ui.window.optn", (kvoid*)window_tool, RF_AUTOSET, knil, knil);

    glade_xml_signal_autoconnect(gxml);

    glade_xml_signal_connect_data(gxml, "on_winmain_menu_item_about_activate", G_CALLBACK(on_winmain_menu_item_about_activate), NULL);
    glade_xml_signal_connect_data(gxml, "on_winmain_menu_item_tool_activate", G_CALLBACK(on_winmain_menu_item_tool_activate), NULL);
    glade_xml_signal_connect_data(gxml, "on_winmain_menu_item_pref_activate", G_CALLBACK(on_winmain_menu_item_pref_activate), NULL);
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

