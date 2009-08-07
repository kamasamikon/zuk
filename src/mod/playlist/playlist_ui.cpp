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

#include <textconv.h>

#include <gtk/gtk.h>

#include "playlistdefine.h"

static char guid[] = "7D378382-9351-4f4e-BF83-4FF20C456B6D";

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
// watches

/////////////////////////////////////////////////////////////////////////////
// trees
typedef struct _priv priv;
struct _priv {
    GtkWidget *window;

    GtkWidget *treeview;
    GtkTreeModel *model;
    GtkTreeModel *filter;

    GtkWidget *scan_button;
    GtkWidget *grep_button;
    GtkWidget *grep_entry;
};

priv __g_priv = { 0 };

/* TreeItem structure */
typedef struct _TreeItem TreeItem;
struct _TreeItem {
    const gchar *label;
    gboolean alex;
    gboolean havoc;
    gboolean tim;
    gboolean owen;
    gboolean dave;
    gboolean world_holiday;     /* shared by the European hackers */
    TreeItem *children;
};

/* columns */
enum {
    HOLIDAY_NAME_COLUMN = 0,
    ALEX_COLUMN,
    HAVOC_COLUMN,
    TIM_COLUMN,
    OWEN_COLUMN,
    DAVE_COLUMN,

    VISIBLE_COLUMN,
    WORLD_COLUMN,
    NUM_COLUMNS
};

enum {
    TYPE_COLUMN,                        /**< ??? */

    HASH_COLUMN,                        /**< channel hash */

    STATUS_ICON_COLUMN,                 /**< go or stop */
    STATUS_ICON_VISIBLE_COLUMN,

    TITLE_COLUMN,

    RATE_ICON_COLUMN,                   /**< star */
    RATE_ICON_VISIBLE_COLUMN,

    REMIDER_COLUMN,
    REMIDER_VISIBLE_COLUMN,

    BLIST_COLUMNS
};


/* tree data */
static TreeItem january[] = {
    {NULL}
};

static TreeItem february[] = {
    {"Presidents' Day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL},
    {"Groundhog Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"Valentine's Day", FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, NULL},
    {NULL}
};

static TreeItem march[] = {
    {"National Tree Planting Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"St Patrick's Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL},
    {NULL}
};
static TreeItem april[] = {
    {"April Fools' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL},
    {"Army Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"Earth Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL},
    {"Administrative Professionals' Day", FALSE, FALSE, FALSE, FALSE,
     FALSE, FALSE, NULL},
    {NULL}
};

static TreeItem may[] = {
    {"Nurses' Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"National Day of Prayer", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"Mothers' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL},
    {"Armed Forces Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"Memorial Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL},
    {NULL}
};

static TreeItem toplevel[] = {
    {"January", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, january},
    {"february february ", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, february},
    {"March", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, march},
    {"April", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, april},
    {"May", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, may},
    {NULL}
};


static GtkTreeModel *create_model()
{
    GtkTreeStore *store;

    store = gtk_tree_store_new(BLIST_COLUMNS, G_TYPE_INT,       /* TYPE_COLUMN */
                               G_TYPE_STRING,   /* HASH_COLUMN */
                               GDK_TYPE_PIXBUF, /* STATUS_ICON_COLUMN */
                               G_TYPE_BOOLEAN,  /* STATUS_ICON_VISIBLE_COLUMN */
                               G_TYPE_STRING,   /* TITLE_COLUMN */
                               GDK_TYPE_PIXBUF, /* RATE_ICON_COLUMN */
                               G_TYPE_BOOLEAN,  /* RATE_ICON_VISIBLE_COLUMN */
                               GDK_TYPE_PIXBUF, /* REMIDER_COLUMN */
                               G_TYPE_BOOLEAN   /* RATE_ICON_VISIBLE_COLUMN */
        );

    return GTK_TREE_MODEL(store);
}

static void fill_data(GtkWidget * treeview, GtkTreeStore * store)
{
    int i = 34;
    GtkTreeIter iter;
    GtkTreeModel *filter;
    TreeItem *month = toplevel;
    /* add data to the tree store */
    while (month->label) {
        i++;

        TreeItem *holiday = month->children;
        gchar *title = g_strdup_printf("<span color='%s'>%s</span>\n<span color='%s' size='smaller'>%s%d%s</span>",
                                       "red", month->label, "blue", " shit ", i, " oyes");

        GdkPixbuf *status;
        status = gtk_widget_render_icon(GTK_WIDGET(treeview), GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);

        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                           STATUS_ICON_COLUMN, status,
                           STATUS_ICON_VISIBLE_COLUMN, TRUE,
                           TITLE_COLUMN, title,
                           RATE_ICON_COLUMN, status,
                           RATE_ICON_VISIBLE_COLUMN, TRUE, REMIDER_COLUMN, NULL, REMIDER_VISIBLE_COLUMN, NULL, -1);


        /* add children */
        while (holiday->label) {
            i++;

            GtkTreeIter child_iter;

            gchar *title = g_strdup_printf("<span color='%s'>%s</span>\n<span color='%s' size='smaller'>%s%s%s</span>",
                                           "red", month->label, "blue", " shit ",
                                           holiday->alex ? "true" : "false", " oyes");

            GdkPixbuf *status;
            status = gtk_widget_render_icon(GTK_WIDGET(treeview), GTK_STOCK_NETWORK, GTK_ICON_SIZE_DIALOG, NULL);
            gtk_tree_store_append(store, &child_iter, &iter);
            gtk_tree_store_set(store, &child_iter,
                               STATUS_ICON_COLUMN, status,
                               STATUS_ICON_VISIBLE_COLUMN, TRUE,
                               TITLE_COLUMN, title,
                               RATE_ICON_COLUMN, status,
                               RATE_ICON_VISIBLE_COLUMN, TRUE, REMIDER_COLUMN, NULL, REMIDER_VISIBLE_COLUMN, NULL, -1);

            holiday++;
        }

        month++;
    }

}

static void add_columns(GtkTreeView * treeview)
{
    GtkCellRenderer *rend;
    GtkTreeViewColumn *column;
    GtkTreeViewColumn *text_column; /**< Column */
    GtkCellRenderer *text_rend;

    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    // auv gtk_tree_view_column_set_visible(column, FALSE);

    gtk_tree_view_column_set_visible(column, TRUE);
    gtk_tree_view_set_expander_column(GTK_TREE_VIEW(treeview), column);

    rend = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(column, rend, FALSE);
    gtk_tree_view_column_set_attributes(column, rend,
                                        "pixbuf", STATUS_ICON_COLUMN, "visible", STATUS_ICON_VISIBLE_COLUMN, NULL);
    g_object_set(rend, "xalign", 0.0, "xpad", 6, "ypad", 0, NULL);

    text_rend = rend = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, rend, TRUE);
    gtk_tree_view_column_set_attributes(column, rend, "markup", TITLE_COLUMN, NULL);
#if GTK_CHECK_VERSION(2,6,0)
    // g_signal_connect(G_OBJECT(rend), "editing-started", G_CALLBACK(gtk_blist_renderer_editing_started_cb), NULL);
#endif

    g_object_set(rend, "ypad", 0, "yalign", 0.5, NULL);
    g_object_set(rend, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);


    rend = gtk_cell_renderer_pixbuf_new();
    g_object_set(rend, "xalign", 1.0, "ypad", 0, NULL);
    gtk_tree_view_column_pack_start(column, rend, FALSE);
    gtk_tree_view_column_set_attributes(column, rend, "pixbuf",
                                        RATE_ICON_COLUMN, "visible", RATE_ICON_VISIBLE_COLUMN, NULL);
}

static gboolean filter_func(GtkTreeModel * model, GtkTreeIter * iter, GtkEntry * entry)
{
    const gchar *needle;
    gchar *haystack;
    gboolean ret;

    gtk_tree_model_get(model, iter, TITLE_COLUMN, &haystack, -1);
    needle = gtk_entry_get_text(entry);

    printf("haystack: %s\n", haystack);
    printf("needle: %s\n", needle);

    if (strstr(haystack, needle) != NULL)
        ret = TRUE;
    else
        ret = FALSE;

    printf("ret: %d\n", ret);
    return ret;
}

static void cb_search_toggled(GtkToggleButton * button, GtkEntry * entry)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
        gtk_widget_show(GTK_WIDGET(entry));
    else
        gtk_widget_hide(GTK_WIDGET(entry));
}

GtkWidget *pidgin_new_item_from_stock(GtkWidget * menu, const char *str, const char *icon, GtkSignalFunc sf,
                                      gpointer data, guint accel_key, guint accel_mods, char *mod)
{
    GtkWidget *menuitem;
    /*
     * GtkWidget *hbox;
     * GtkWidget *label;
     */
    GtkWidget *image;

    if (icon == NULL)
        menuitem = gtk_menu_item_new_with_mnemonic(str);
    else
        menuitem = gtk_image_menu_item_new_with_mnemonic(str);

    if (menu)
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

    if (sf)
        g_signal_connect(G_OBJECT(menuitem), "activate", sf, data);

    if (icon != NULL) {
        //image = gtk_image_new_from_stock(icon, gtk_icon_size_from_name(PIDGIN_ICON_SIZE_TANGO_EXTRA_SMALL));
        //gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
    }

    gtk_widget_show_all(menuitem);

    return menuitem;
}

void
pidgin_treeview_popup_menu_position_func(GtkMenu *menu,
										   gint *x,
										   gint *y,
										   gboolean *push_in,
										   gpointer data)
{
	GtkWidget *widget = GTK_WIDGET(data);
	GtkTreeView *tv = GTK_TREE_VIEW(data);
	GtkTreePath *path;
	GtkTreeViewColumn *col;
	GdkRectangle rect;
	gint ythickness = GTK_WIDGET(menu)->style->ythickness;

	gdk_window_get_origin (widget->window, x, y);
	gtk_tree_view_get_cursor (tv, &path, &col);
	gtk_tree_view_get_cell_area (tv, path, col, &rect);

	*x += rect.x+(rect.width / 2);
	*y += rect.y+(rect.height / 2)+ythickness;
	// pidgin_menu_position_func_helper(menu, x, y, push_in, data);
}



void create_group_menu(guint32 time)
{
    GtkWidget *menu = 0;
    GtkWidget *item;
#if 1
    menu = gtk_menu_new();
    item = pidgin_new_item_from_stock(menu, ("Add _Buddy..."), GTK_STOCK_ADD, NULL, NULL, 0, 0, NULL);
    item = pidgin_new_item_from_stock(menu, ("Add C_hat..."), GTK_STOCK_ADD, NULL, NULL, 0, 0, NULL);
    pidgin_new_item_from_stock(menu, ("_Delete Group"), GTK_STOCK_REMOVE, NULL, NULL, 0, 0, NULL);
    pidgin_new_item_from_stock(menu, ("_Rename"), NULL, NULL, NULL, 0, 0, NULL);

    gtk_widget_show_all(menu);
    //gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, __g_priv.treeview, 0, 0);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, pidgin_treeview_popup_menu_position_func, __g_priv.treeview, NULL, time);
#else

    if (!menu) {
        menu = gtk_menu_new();

        item = gtk_check_menu_item_new_with_mnemonic(("_Scan"));
        //g_signal_connect(G_OBJECT(item), "toggled", G_CALLBACK(MenuScanCallback), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

        GtkWidget *menuitem;
        menuitem = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

        menuitem = gtk_image_menu_item_new_with_mnemonic(("_Quit"));
        GtkWidget *image;
        image = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
        //g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(MenuQuitCallback), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

        gtk_widget_show_all(menu);
    }
    // gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(scan_menuitem), gpAppFrame->oAppCore.oSelection.bEnable);

    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
#endif
}

static gboolean cb_entry_changed(GtkEditable * entry, GtkTreeView * treeview)
{
    GtkTreeModelFilter *filter;

    filter = GTK_TREE_MODEL_FILTER(gtk_tree_view_get_model(treeview));
    gtk_tree_model_filter_refilter(filter);

    gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));
    return (FALSE);
}

int update_channel(gpointer handle, char *hash, int *type, int *state, char **title, int *rate, int *alarm)
{
    GValue val;
    GtkTreeIter iter;
    gtk_tree_model_get_value(GTK_TREE_MODEL(__g_priv.model), &iter, HASH_COLUMN, &val);
}

static gboolean gtk_blist_key_press_cb(GtkWidget * treeview, GdkEventKey * event, gpointer data)
{
    GValue val;
    GtkTreeIter iter;
    GtkTreeSelection *sel;

    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    if (!gtk_tree_selection_get_selected(sel, NULL, &iter))
        return FALSE;

    //val.g_type = 0;
    //gtk_tree_model_get_value(GTK_TREE_MODEL(__g_priv.model), &iter, HASH_COLUMN, &val);

#define GDK_Return 0xff0d
    if (event->keyval == GDK_Return) {
        klog(("ctrl+return\n"));
        create_group_menu(event->time);
    }

    return FALSE;
}

static gboolean gtk_blist_button_press_cb(GtkWidget * treeview, GdkEventButton * event, gpointer user_data)
{
    GtkTreePath *path;
    GValue val;
    GtkTreeIter iter;
    GtkTreeSelection *sel;
    gboolean handled = FALSE;

    /* Here we figure out which node was clicked */
    if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y, &path, NULL, NULL, NULL))
        return FALSE;

    gtk_tree_model_get_iter(GTK_TREE_MODEL(__g_priv.model), &iter, path);
    val.g_type = 0;
    gtk_tree_model_get_value(GTK_TREE_MODEL(__g_priv.model), &iter, HASH_COLUMN, &val);

    if ((event->button == 3) && (event->type == GDK_BUTTON_PRESS)) {

        // create_group_menu(NULL, NULL, NULL, 3, event->time);
        create_group_menu(event->time);

        klog(("right clicked\n"));
    } else if ((event->button == 2) && (event->type == GDK_2BUTTON_PRESS)) {
        klog(("double clicked, play it\n"));
    }

    gtk_tree_path_free(path);

    return FALSE;
}

static void pidgin_blist_selection_changed(GtkTreeSelection * selection, gpointer data)
{
    char *new_selection = NULL;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, NULL, &iter))
        klog(("selection changed\n"));

    //gtk_tree_model_get(GTK_TREE_MODEL(__g_priv.model), &iter,
    //HASH_COLUMN, &new_selection, -1);
}


gpointer do_tree_store()
{
    GtkWidget *window = __g_priv.window;

    if (!window) {
        GtkWidget *vbox, *tool_hbox, *grep_hbox;
        GtkWidget *scan_button;
        GtkWidget *grep_button;
        GtkWidget *grep_entry;
        GtkWidget *sw;
        GtkWidget *treeview;
        GtkTreeModel *model;
        GtkTreeModel *filter;
        GtkTreeSelection *selection;

        /* create window, etc */
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(NULL));
        gtk_window_set_title(GTK_WINDOW(window), "list");
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

        vbox = gtk_vbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 1);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        tool_hbox = gtk_hbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(tool_hbox), 1);
        gtk_box_pack_start(GTK_BOX(vbox), tool_hbox, FALSE, FALSE, 0);

        scan_button = gtk_toggle_button_new_with_mnemonic("_Rescan");
        gtk_box_pack_start(GTK_BOX(tool_hbox), scan_button, FALSE, FALSE, 0);

        grep_button = gtk_toggle_button_new_with_mnemonic("_Search");
        gtk_box_pack_end(GTK_BOX(tool_hbox), grep_button, FALSE, FALSE, 0);

        grep_hbox = gtk_hbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(grep_hbox), 1);
        gtk_box_pack_start(GTK_BOX(vbox), grep_hbox, FALSE, FALSE, 0);

        grep_entry = gtk_entry_new();
        gtk_box_pack_start(GTK_BOX(grep_hbox), grep_entry, TRUE, TRUE, 0);

        g_signal_connect(G_OBJECT(grep_button), "toggled", G_CALLBACK(cb_search_toggled), GTK_ENTRY(grep_entry));

        sw = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);

        /* create model */
        model = create_model();

        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(model));


        /* Create filter and set visible function */
        filter = gtk_tree_model_filter_new(model, NULL);
        gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(filter), (GtkTreeModelFilterVisibleFunc)
                                               filter_func, grep_entry, NULL);

        /* Create treeview with model */
        treeview = gtk_tree_view_new_with_model(filter);

        /* Set up selection stuff */
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
        g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(pidgin_blist_selection_changed), NULL);

        g_signal_connect(G_OBJECT(treeview), "button-press-event", G_CALLBACK(gtk_blist_button_press_cb), NULL);
        g_signal_connect(G_OBJECT(treeview), "key-press-event", G_CALLBACK(gtk_blist_key_press_cb), NULL);

        fill_data(treeview, GTK_TREE_STORE(model));

        g_signal_connect(G_OBJECT(grep_entry), "changed", G_CALLBACK(cb_entry_changed), GTK_TREE_VIEW(treeview));

        g_object_unref(model);
        gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview), TRUE);
        gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(treeview), TRUE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
        gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), GTK_SELECTION_SINGLE);

        add_columns(GTK_TREE_VIEW(treeview));

        gtk_container_add(GTK_CONTAINER(sw), treeview);

        /* expand all rows after the treeview widget has been realized */
        g_signal_connect(treeview, "realize", G_CALLBACK(gtk_tree_view_expand_all), NULL);
        gtk_window_set_default_size(GTK_WINDOW(window), 650, 400);


        /* set priv */
        __g_priv.window = window;
        __g_priv.treeview = treeview;
        __g_priv.model = model;
        __g_priv.filter = filter;
        __g_priv.scan_button = scan_button;
        __g_priv.grep_button = grep_button;
        __g_priv.grep_entry = grep_entry;
    }

    if (!GTK_WIDGET_VISIBLE(window))
        gtk_widget_show_all(window);
    else {
        gtk_widget_destroy(window);
        window = NULL;
    }

    return (gpointer) & __g_priv;
}

/////////////////////////////////////////////////////////////////////////////
// standard KMM routines

static GtkWidget *create_media_window()
{
    GtkWidget *mediaWindow;

    mediaWindow = gtk_drawing_area_new();
    return mediaWindow;
}

static GtkWidget *create_playlist_window()
{
    GtkWidget *playlistWindow;

    do_tree_store();
}

int add_playitem(KMediaChannel * channel)
{
}

void create_ui(KIM * im)
{
    SET_GLOBALS(im);

    GtkWidget *mediaWindow = create_media_window();
    kim_addptr(im, "p.playlist.ui.mediaWindow", mediaWindow, 0, knil, knil);

    GtkWidget *playlistWindow = create_playlist_window();
    kim_addptr(im, "p.playlist.ui.playlistWindow", playlistWindow, 0, knil, knil);
}
