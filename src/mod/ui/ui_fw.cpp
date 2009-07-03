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

#include "glade-palette-box.h"

typedef struct _ToolEntry ToolEntry;
struct _ToolEntry {
    GtkWidget* (*create)();
    void (*destroy)(GtkWidget*);

    kuint type;         /* trigger, page, tool */
    kchar *tool_tip;
    kchar *name;
    kchar *icon;
};

static void register_tool(ToolEntry *te);

static char guid[] = "52727E3B-08FD-4664-97B1-4CBABD17C985";

static kchar *__g_mod_dir = knil;

enum {
    TARGET_STRING,
    TARGET_ROOTWIN
};
static GtkTargetEntry target_table[] = {
    { "STRING",     0, TARGET_STRING },
    { "text/plain", 0, TARGET_STRING },
    { "application/x-rootwindow-drop", 0, TARGET_ROOTWIN }
};

static guint n_targets = sizeof(target_table) / sizeof(target_table[0]);

gboolean
target_drag_drop(GtkWidget	       *widget,
        GdkDragContext     *context,
        gint                x,
        gint                y,
        guint               time) ;
    void
target_drag_data_received  (GtkWidget          *widget,
        GdkDragContext     *context,
        gint                x,
        gint                y,
        GtkSelectionData   *data,
        guint               info,
        guint               time) ;

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
            "title", "About zuk! zuk!",
            NULL);
}

extern "C" void on_winmain_menu_item_tool_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *window_tool = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.tool", knil);
    gtk_widget_show_all(window_tool);
}

extern "C" void on_winmain_menu_item_pref_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *window_pref = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.optn", knil);

    /* fill all the modules to the list */
    gtk_widget_show(window_pref);
}

/////////////////////////////////////////////////////////////////////////////
// support routines

typedef struct _K_tool_button K_tool_button;
struct _K_tool_button
{
    gchar *text;
    gchar *tool_tip;

    guint type;             /**< trigger? container? */
    gboolean multi_instance;    /**< allow multi instance */
    GtkWidget *(fun_init)(void);
};

static GtkWidget *create_tool_widget()
{
    static gint index = 0;
    gchar *labels[5] = { "a", "aa", "aaaaaaa", "sdfss", "x" };

    GtkWidget *tool, *button, *vbox, *entry;

    tool = gtk_button_new();
    button = gtk_button_new_with_label(labels[index++ % 5]);
    vbox = gtk_vbox_new(TRUE, TRUE);
    entry = gtk_entry_new();

    gtk_container_add(GTK_CONTAINER(tool), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

    return tool;
}

GtkWidget* htoolbar_create()
{
    GtkWidget *htbar, *button;
    htbar = gtk_hbox_new(TRUE, 1);

    button = gtk_button_new_with_label("htbar0");
    gtk_box_pack_start(GTK_BOX(htbar), button, TRUE, FALSE, 0);

    button = gtk_button_new_with_label("htbar1");
    gtk_box_pack_start(GTK_BOX(htbar), button, TRUE, FALSE, 0);

    gtk_drag_dest_set (htbar,
            GTK_DEST_DEFAULT_ALL,
            target_table, n_targets, /* no rootwin */
            GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
    g_signal_connect (htbar, "drag_drop",
            G_CALLBACK (target_drag_drop), htbar);

    g_signal_connect (htbar, "drag_data_received",
            G_CALLBACK (target_drag_data_received), NULL);


    gtk_widget_show_all(htbar);

    return htbar;
}

void htoolbar_destroy(GtkWidget*)
{
}

/*---------------------------------------------------------------------------------
 * button_play tool
 */

#define IMWCH(f) (f)(KIM *im, KRtiRec *rec, void *ua, void *ub, kuchar type)
static kint IMWCH(imwch_button_play_clicked)
{
    /* toggle to pause or play the current media */
}

static void on_button_play_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *but = (GtkWidget*)user_data;
    g_assert((void*)but == (void*)button);

    kim_setint(__g_im, "i.tool.evt.clicked", 1, (void**)but, knil);
}

/* create the play button */
GtkWidget* button_play_create()
{
    GtkWidget *button_play;

    button_play = gtk_button_new_with_label("button");

    /* set tool tips */
    /* set icon */

    /* g_signal -> g_callback -> im_event */
    g_signal_connect(G_OBJECT(button_play), "clicked", G_CALLBACK(on_button_play_clicked), button_play);
    kim_addawch(__g_im, "i.tool.evt.clicked", imwch_button_play_clicked, button_play, knil, knil);

    gtk_widget_show(button_play);

    return button_play;
}

void button_play_destroy(GtkWidget*)
{
}

static ToolEntry* mk_button_play(kbool pub)
{
    ToolEntry *te = (ToolEntry*)kmem_alloz(sizeof(ToolEntry));
    te->create = button_play_create;
    te->destroy = button_play_destroy;

    te->type = 'T';
    te->tool_tip = "Create play button";
    te->name = "xbut";

    if (pub)
        register_tool(te);

    return te;
}

/*---------------------------------------------------------------------------------
 * button tool
 */

static ToolEntry* mk_htoolbar2(kchar *clsid, kbool pub)
{
}

static ToolEntry* mk_htoolbar(kbool pub)
{
    ToolEntry *te = (ToolEntry*)kmem_alloz(sizeof(ToolEntry));
    te->create = htoolbar_create;
    te->destroy = htoolbar_destroy;

    te->type = 'L';
    te->tool_tip = "Create vertical tool bar";
    te->name = "VerticalToolBar";

    if (pub)
        register_tool(te);

    return te;
}

#if 0
void flow()
{
    GtkWidget *vol = gtk_progress_bar_new();
    g_signal_connect(G_OBJECT(vol), "changed", G_CALLBACK(vol), vol);
    kim_addawch(__g_im, "vol.enable", vol_enable, vol, ...);



    GtkWidget *vol = gtk_progress_bar_new();
    g_signal_connect(G_OBJECT(vol), "changed", G_CALLBACK(vol), vol);
    kim_addawch(__g_im, "vol.enable", vol_enable, vol, ...);



    GtkWidget *vol = gtk_progress_bar_new();
    g_signal_connect(G_OBJECT(vol), "changed", G_CALLBACK(vol), vol);
    kim_addawch(__g_im, "vol.enable", vol_enable, vol, ...);
}
#endif

GList *__g_tool_list = NULL;
static void register_tool(ToolEntry *te)
{
    __g_tool_list = g_list_append(__g_tool_list, te);
}

static GtkWidget *create_tool_widget2(ToolEntry *te)
{
    static gint index = 0;
    gchar *labels[5] = { "a", "aa", "aaaaaaa", "sdfss", "x" };

    GtkWidget *tool, *button, *vbox, *entry;

    tool = gtk_toggle_button_new();
    gtk_widget_set_size_request(GTK_WIDGET(tool), 80, 80);
    gtk_widget_set_tooltip_text(GTK_WIDGET(tool), te->tool_tip);

    button = gtk_button_new_with_label(te->name);
    vbox = gtk_vbox_new(TRUE, TRUE);
    entry = gtk_entry_new();

    gtk_container_add(GTK_CONTAINER(tool), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

    return tool;
}
    void
source_drag_data_get  (GtkWidget          *widget,
        GdkDragContext     *context,
        GtkSelectionData   *selection_data,
        guint               info,
        guint               time,
        gpointer            data)
{
    printf("--------- source_drag_data_get \n");
    g_print("source_drag_data_get: info : %x\n", info);
    if (info == TARGET_ROOTWIN)
        g_print ("I was dropped on the rootwin\n");
    else
        gtk_selection_data_set (selection_data,
                selection_data->target,
                // 8, (const guchar*)"I'm Data!", 9);
                8, (const guchar*)data, 4);
}


static void fill_tool_window()
{
    GtkWidget *window_tool_pool = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.tool_pool", knil);
    GtkWidget *window_layout_tool_pool = (GtkWidget*)kim_getptr(__g_im, "p.ui.ui.window.layout_tool_pool", knil);
    window_tool_pool = window_layout_tool_pool;

    GtkWidget *tool_box;
    GtkWidget *button;

    char label[22];
    gint i;
    GList *l;
    ToolEntry *te;

    mk_htoolbar(ktrue);
    mk_button_play(ktrue);

    mk_htoolbar(ktrue);
    mk_button_play(ktrue);

    mk_htoolbar(ktrue);
    mk_button_play(ktrue);
    mk_button_play(ktrue);
    mk_button_play(ktrue);
    mk_htoolbar(ktrue);
    mk_htoolbar(ktrue);

    gtk_widget_set_size_request(GTK_WIDGET(window_tool_pool), 200, 200);

	tool_box = glade_palette_box_new ();

    /* FIXME: set border other than 0 can make layout mess some time */
    gtk_container_set_border_width(GTK_CONTAINER(tool_box), 0);

    for (l = __g_tool_list; l; l = l->next) {
        te = (ToolEntry*)l->data;
        button = create_tool_widget2(te);

        printf("fill_tool_window: button: %x\n", button);

#if 10 /* TODO */
        gtk_drag_source_set (button, GdkModifierType(GDK_BUTTON1_MASK | GDK_BUTTON3_MASK),
                target_table, n_targets,
                GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
#endif

        g_signal_connect (button, "drag_data_get",
                G_CALLBACK (source_drag_data_get), te);

        gtk_container_add (GTK_CONTAINER (tool_box), button);
    }

#if 0
    for (i = 0; i < 15; i++) {
        sprintf(label, "NIU-NIU-NIU:%X\n", i);
        button = gtk_button_new_with_label(label);
        button = create_tool_widget();

#if 10 /* TODO */
        gtk_drag_source_set (button, GdkModifierType(GDK_BUTTON1_MASK | GDK_BUTTON3_MASK),
                target_table, n_targets,
                GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
#endif

        gtk_container_add (GTK_CONTAINER (tool_box), button);
    }
#endif

    gtk_container_add(GTK_CONTAINER(window_tool_pool), tool_box);

    gtk_widget_show_all(GTK_WIDGET(window_tool_pool));
}

gboolean
target_drag_drop(GtkWidget	       *widget,
        GdkDragContext     *context,
        gint                x,
        gint                y,
        guint               time)
{
    return FALSE;
#if 0
    GtkWidget *button;
    ToolEntry *te = (ToolEntry*)data->data;

    printf("--------- target_drag_drop \n");
    printf("target_drag_drop: widget: %x\n", widget);

    button = gtk_button_new_with_label("sdfsdfas");
    button = te->create();
    gtk_widget_show(GTK_WIDGET(button));
    gtk_box_pack_start(GTK_BOX(widget), button, TRUE, FALSE, 0);
    return FALSE;
#endif
}
    void
target_drag_data_received  (GtkWidget          *widget,
        GdkDragContext     *context,
        gint                x,
        gint                y,
        GtkSelectionData   *data,
        guint               info,
        guint               time)
{
    printf("--------- target_drag_data_received \n");
    if ((data->length >= 0) && (data->format == 8))
    {
        g_print ("Received \"%x\" in trashcan\n", (gchar *)data->data);
        if (1) {
            GtkWidget *button;
            ToolEntry *te = (ToolEntry*)data->data;

            printf("--------- target_drag_drop \n");
            printf("target_drag_drop: widget: %x\n", widget);

            button = gtk_button_new_with_label("sdfsdfas");
            button = te->create();
            gtk_widget_show(GTK_WIDGET(button));
            gtk_box_pack_start(GTK_BOX(widget), button, TRUE, FALSE, 0);
        }

        gtk_drag_finish (context, TRUE, FALSE, time);
        return;
    }

    gtk_drag_finish (context, FALSE, FALSE, time);
}



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
    kim_addptr(im, "p.ui.ui.glade", (kvoid*)gxml, RF_AUTOSET, knil, knil);

    GtkWidget *window_main = glade_xml_get_widget (gxml, "window_main");
    kim_addptr(im, "p.ui.ui.window.main", (kvoid*)window_main, RF_AUTOSET, knil, knil);
    klog(("window_main: %s\n", GTK_OBJECT_TYPE_NAME(window_main)));

    GtkWidget *vbox_main = glade_xml_get_widget(gxml, "winmain_vbox_main");
    printf("ui_create_ui: vbox_main: %x\n", vbox_main);
    klog(("vbox_main: %s\n", GTK_OBJECT_TYPE_NAME(vbox_main)));

    gtk_drag_dest_set (vbox_main,
            GTK_DEST_DEFAULT_ALL,
            target_table, n_targets, /* no rootwin */
            GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE));
    g_signal_connect (vbox_main, "drag_drop",
            G_CALLBACK (target_drag_drop), vbox_main);

    g_signal_connect (vbox_main, "drag_data_received",
            G_CALLBACK (target_drag_data_received), NULL);


    if (1) {
        GtkWidget *button;
        button = gtk_button_new_with_label("xxyyyx");
        gtk_box_pack_start(GTK_BOX(vbox_main), button, TRUE, FALSE, 0);
    }

    GtkWidget *window_info = glade_xml_get_widget (gxml, "window_info");
    kim_addptr(im, "p.ui.ui.window.info", (kvoid*)window_info, RF_AUTOSET, knil, knil);

    GtkWidget *window_media = glade_xml_get_widget (gxml, "window_media");
    kim_addptr(im, "p.ui.ui.window.media", (kvoid*)window_media, RF_AUTOSET, knil, knil);

    GtkWidget *window_pref = glade_xml_get_widget (gxml, "window_pref");
    kim_addptr(im, "p.ui.ui.window.optn", (kvoid*)window_pref, RF_AUTOSET, knil, knil);

    GtkWidget *window_tool = glade_xml_get_widget (gxml, "window_tool");
    kim_addptr(im, "p.ui.ui.window.tool", (kvoid*)window_tool, RF_AUTOSET, knil, knil);

    GtkWidget *window_tool_pool = glade_xml_get_widget (gxml, "wintool_viewport_tool_pool");
    kim_addptr(im, "p.ui.ui.window.tool_pool", (kvoid*)window_tool_pool, RF_AUTOSET, knil, knil);

    GtkWidget *window_layout_tool_pool = glade_xml_get_widget (gxml, "wintool_viewport_layout_tool_pool");
    kim_addptr(im, "p.ui.ui.window.layout_tool_pool", (kvoid*)window_layout_tool_pool, RF_AUTOSET, knil, knil);

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

    fill_tool_window();

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

