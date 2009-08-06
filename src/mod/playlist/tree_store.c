#include <gtk/gtk.h>

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
    {"National Tree Planting Day", FALSE, FALSE, FALSE, FALSE, FALSE,
     FALSE, NULL},
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
    {"National Day of Prayer", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     NULL},
    {"Mothers' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL},
    {"Armed Forces Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL},
    {"Memorial Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL},
    {NULL}
};

static TreeItem toplevel[] = {
    {"January", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, january},
    {"february february ", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
     february},
    {"March", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, march},
    {"April", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, april},
    {"May", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, may},
    {NULL}
};


static GtkTreeModel *create_model()
{
    GtkTreeStore *store;

    store = gtk_tree_store_new(BLIST_COLUMNS, G_TYPE_INT,   /* TYPE_COLUMN */
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
        gchar *title =
            g_strdup_printf
            ("<span color='%s'>%s</span>\n<span color='%s' size='smaller'>%s%d%s</span>",
             "red", month->label, "blue", " shit ", i, " oyes");

        GdkPixbuf *status;
        status =
            gtk_widget_render_icon(GTK_WIDGET(treeview),
                                   GTK_STOCK_DIRECTORY,
                                   GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);

        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                           STATUS_ICON_COLUMN, status,
                           STATUS_ICON_VISIBLE_COLUMN, TRUE,
                           TITLE_COLUMN, title,
                           RATE_ICON_COLUMN, status,
                           RATE_ICON_VISIBLE_COLUMN, TRUE,
                           REMIDER_COLUMN, NULL,
                           REMIDER_VISIBLE_COLUMN, NULL, -1);


        /* add children */
        while (holiday->label) {
            i++;

            GtkTreeIter child_iter;

            gchar *title =
                g_strdup_printf
                ("<span color='%s'>%s</span>\n<span color='%s' size='smaller'>%s%s%s</span>",
                 "red", month->label, "blue", " shit ",
                 holiday->alex ? "true" : "false", " oyes");

            GdkPixbuf *status;
            status =
                gtk_widget_render_icon(GTK_WIDGET(treeview),
                                       GTK_STOCK_NETWORK,
                                       GTK_ICON_SIZE_DIALOG, NULL);
            gtk_tree_store_append(store, &child_iter, &iter);
            gtk_tree_store_set(store, &child_iter,
                               STATUS_ICON_COLUMN, status,
                               STATUS_ICON_VISIBLE_COLUMN, TRUE,
                               TITLE_COLUMN, title,
                               RATE_ICON_COLUMN, status,
                               RATE_ICON_VISIBLE_COLUMN, TRUE,
                               REMIDER_COLUMN, NULL,
                               REMIDER_VISIBLE_COLUMN, NULL, -1);

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
                                        "pixbuf", STATUS_ICON_COLUMN,
                                        "visible",
                                        STATUS_ICON_VISIBLE_COLUMN, NULL);
    g_object_set(rend, "xalign", 0.0, "xpad", 6, "ypad", 0, NULL);

    text_rend = rend = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, rend, TRUE);
    gtk_tree_view_column_set_attributes(column, rend,
                                        "markup", TITLE_COLUMN, NULL);
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
                                        RATE_ICON_COLUMN, "visible",
                                        RATE_ICON_VISIBLE_COLUMN, NULL);
}

static gboolean filter_func(GtkTreeModel * model, GtkTreeIter * iter,
                            GtkEntry * entry)
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

static gboolean cb_entry_changed(GtkEditable * entry,
                                 GtkTreeView * treeview)
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

        /* create window, etc */
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_screen(GTK_WINDOW(window),
                              gtk_widget_get_screen(NULL));
        gtk_window_set_title(GTK_WINDOW(window), "list");
        g_signal_connect(window, "destroy",
                         G_CALLBACK(gtk_widget_destroyed), &window);

        vbox = gtk_vbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 1);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        tool_hbox = gtk_hbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(tool_hbox), 1);
        gtk_box_pack_start(GTK_BOX(vbox), tool_hbox, FALSE, FALSE, 0);

        scan_button = gtk_toggle_button_new_with_mnemonic("_Rescan");
        gtk_box_pack_start(GTK_BOX(tool_hbox), scan_button, FALSE, FALSE,
                           0);

        grep_button = gtk_toggle_button_new_with_mnemonic("_Search");
        gtk_box_pack_end(GTK_BOX(tool_hbox), grep_button, FALSE, FALSE, 0);

        grep_hbox = gtk_hbox_new(FALSE, 1);
        gtk_container_set_border_width(GTK_CONTAINER(grep_hbox), 1);
        gtk_box_pack_start(GTK_BOX(vbox), grep_hbox, FALSE, FALSE, 0);

        grep_entry = gtk_entry_new();
        gtk_box_pack_start(GTK_BOX(grep_hbox), grep_entry, TRUE, TRUE, 0);

        g_signal_connect(G_OBJECT(grep_button), "toggled",
                         G_CALLBACK(cb_search_toggled), GTK_ENTRY(grep_entry));

        sw = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw),
                                            GTK_SHADOW_ETCHED_IN);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
                                       GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_AUTOMATIC);
        gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);

        /* create model */
        model = create_model();

        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview),
                                GTK_TREE_MODEL(model));


        /* Create filter and set visible function */
        filter = gtk_tree_model_filter_new(model, NULL);
        gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER
                                               (filter),
                                               (GtkTreeModelFilterVisibleFunc)
                                               filter_func, grep_entry, NULL);

        /* Create treeview with model */
        treeview = gtk_tree_view_new_with_model(filter);

        fill_data(treeview, GTK_TREE_STORE(model));

        g_signal_connect(G_OBJECT(grep_entry), "changed",
                         G_CALLBACK(cb_entry_changed),
                         GTK_TREE_VIEW(treeview));

        g_object_unref(model);
        gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview), TRUE);
        gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(treeview), TRUE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
        gtk_tree_selection_set_mode(gtk_tree_view_get_selection
                                    (GTK_TREE_VIEW(treeview)),
                                    GTK_SELECTION_SINGLE);

        add_columns(GTK_TREE_VIEW(treeview));

        gtk_container_add(GTK_CONTAINER(sw), treeview);

        /* expand all rows after the treeview widget has been realized */
        g_signal_connect(treeview, "realize",
                         G_CALLBACK(gtk_tree_view_expand_all), NULL);
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

    return (gpointer)&__g_priv;
}

