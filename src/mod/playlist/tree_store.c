/* Tree View/Tree Store
 *
 * The GtkTreeStore is used to store data in tree form, to be
 * used later on by a GtkTreeView to display it. This demo builds
 * a simple GtkTreeStore and displays it. If you're new to the
 * GtkTreeView widgets and associates, look into the GtkListStore
 * example first.
 *
 */

#include <gtk/gtk.h>

static GtkWidget *window = NULL;

/* TreeItem structure */
typedef struct _TreeItem TreeItem;
struct _TreeItem
{
  const gchar    *label;
  gboolean        alex;
  gboolean        havoc;
  gboolean        tim;
  gboolean        owen;
  gboolean        dave;
  gboolean        world_holiday; /* shared by the European hackers */
  TreeItem       *children;
};

/* columns */
enum
{
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
	STATUS_ICON_COLUMN,
	STATUS_ICON_VISIBLE_COLUMN,
	NAME_COLUMN,
	BUDDY_ICON_COLUMN,
	BUDDY_ICON_VISIBLE_COLUMN,
	NODE_COLUMN,
	BGCOLOR_COLUMN,
	GROUP_EXPANDER_COLUMN,
	GROUP_EXPANDER_VISIBLE_COLUMN,
	CONTACT_EXPANDER_COLUMN,
	CONTACT_EXPANDER_VISIBLE_COLUMN,
	EMBLEM_COLUMN,
	EMBLEM_VISIBLE_COLUMN,
	PROTOCOL_ICON_COLUMN,
	PROTOCOL_ICON_VISIBLE_COLUMN,
	BLIST_COLUMNS
};


/* tree data */
static TreeItem january[] =
{
  //{"New Years Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL },
  //{"Presidential Inauguration", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  //{"Martin Luther King Jr. day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem february[] =
{
  { "Presidents' Day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { "Groundhog Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Valentine's Day", FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, NULL },
  { NULL }
};

static TreeItem march[] =
{
  { "National Tree Planting Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "St Patrick's Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { NULL }
};
static TreeItem april[] =
{
  { "April Fools' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Army Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Earth Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Administrative Professionals' Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem may[] =
{
  { "Nurses' Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "National Day of Prayer", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Mothers' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Armed Forces Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Memorial Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL },
  { NULL }
};

static TreeItem june[] =
{
  { "June Fathers' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Juneteenth (Liberation of Slaves)", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Flag Day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem july[] =
{
  { "Parents' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Independence Day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem august[] =
{
  { "Air Force Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Coast Guard Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Friendship Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem september[] =
{
  { "Grandparents' Day", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { "Citizenship Day or Constitution Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Labor Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL },
  { NULL }
};

static TreeItem october[] =
{
  { "National Children's Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Bosses' Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Sweetest Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Mother-in-Law's Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Navy Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Columbus Day", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { "Halloween", FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, NULL },
  { NULL }
};

static TreeItem november[] =
{
  { "Marine Corps Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Veterans' Day", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL },
  { "Thanksgiving", FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NULL },
  { NULL }
};

static TreeItem december[] =
{
  { "Pearl Harbor Remembrance Day", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { "Christmas", TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, NULL },
  { "Kwanzaa", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NULL },
  { NULL }
};


static TreeItem toplevel[] =
{
  {"January", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, january},
  {"february february february february february february february february", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, february},
  {"March", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, march},
  {"April", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, april},
  {"May", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, may},
  {"June", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, june},
  {"July", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, july},
  {"August", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, august},
  {"September", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, september},
  {"October", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, october},
  {"November", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, november},
  {"December", FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, december},
  {NULL}
};


static GtkTreeModel *
create_model (GtkWidget *treeview)
{
  GtkTreeStore *model;
  GtkTreeIter iter;
  TreeItem *month = toplevel;

  model = gtk_tree_store_new(BLIST_COLUMNS,
						 GDK_TYPE_PIXBUF, /* Status icon */
						 G_TYPE_BOOLEAN,  /* Status icon visible */
						 G_TYPE_STRING,   /* Name */
						 GDK_TYPE_PIXBUF, /* Buddy icon */
						 G_TYPE_BOOLEAN,  /* Buddy icon visible */
						 G_TYPE_POINTER,  /* Node */
						 GDK_TYPE_COLOR,  /* bgcolor */
						 G_TYPE_BOOLEAN,  /* Group expander */
						 G_TYPE_BOOLEAN,  /* Group expander visible */
						 G_TYPE_BOOLEAN,  /* Contact expander */
						 G_TYPE_BOOLEAN,  /* Contact expander visible */
						 GDK_TYPE_PIXBUF, /* Emblem */
						 G_TYPE_BOOLEAN,  /* Emblem visible */
						 GDK_TYPE_PIXBUF, /* Protocol icon */
						 G_TYPE_BOOLEAN   /* Protocol visible */
						);

  /* add data to the tree store */
  while (month->label)
    {
      TreeItem *holiday = month->children;
      gchar *mark = "this is a very very very very long string\nxxxxxxxx<b>shit</b>";

      GdkPixbuf *status;
      status = gtk_widget_render_icon( GTK_WIDGET(treeview), GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL );

      gtk_tree_store_append (model, &iter, NULL);
	gtk_tree_store_set(model, &iter,
			   STATUS_ICON_COLUMN, status,
			   STATUS_ICON_VISIBLE_COLUMN, TRUE,
			   NAME_COLUMN, mark,
			   BUDDY_ICON_COLUMN, status,
			   BUDDY_ICON_VISIBLE_COLUMN, TRUE,
			   //EMBLEM_COLUMN, emblem,
			   EMBLEM_VISIBLE_COLUMN, TRUE,
			   PROTOCOL_ICON_COLUMN, status,
			   PROTOCOL_ICON_VISIBLE_COLUMN, TRUE,
			   BGCOLOR_COLUMN, NULL,
			   CONTACT_EXPANDER_COLUMN, NULL,
			   //CONTACT_EXPANDER_VISIBLE_COLUMN, expanded,
			   GROUP_EXPANDER_VISIBLE_COLUMN, FALSE,
			-1);


      /* add children */
      while (holiday->label)
	{
	  GtkTreeIter child_iter;
      gchar *mark = "child\nxxxxxxxx<b>shit</b>";


      GdkPixbuf *status;
      status = gtk_widget_render_icon( GTK_WIDGET(treeview), GTK_STOCK_NETWORK, GTK_ICON_SIZE_DIALOG, NULL );
	  gtk_tree_store_append (model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter,
			   STATUS_ICON_COLUMN, status,
			   STATUS_ICON_VISIBLE_COLUMN, TRUE,
			   NAME_COLUMN, mark,
			   BUDDY_ICON_COLUMN, status,
			   BUDDY_ICON_VISIBLE_COLUMN, TRUE,
			   //EMBLEM_COLUMN, emblem,
			   EMBLEM_VISIBLE_COLUMN, TRUE,
			   PROTOCOL_ICON_COLUMN, status,
			   PROTOCOL_ICON_VISIBLE_COLUMN, holiday->alex,
			   BGCOLOR_COLUMN, NULL,
			   CONTACT_EXPANDER_COLUMN, NULL,
			   //CONTACT_EXPANDER_VISIBLE_COLUMN, expanded,
			   GROUP_EXPANDER_VISIBLE_COLUMN, FALSE,
			-1);

	  holiday++;
	}

      month++;
    }

  return GTK_TREE_MODEL (model);
}

static void
item_toggled (GtkCellRendererToggle *cell,
	      gchar                 *path_str,
	      gpointer               data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  GtkTreeIter iter;
  gboolean toggle_item;

  gint *column;

  column = (gint*)g_object_get_data (G_OBJECT (cell), "column");

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, column, &toggle_item, -1);

  /* do something with the value */
  toggle_item ^= 1;

  /* set new value */
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter, column,
		      toggle_item, -1);

  /* clean up */
  gtk_tree_path_free (path);
}

static void
add_columns (GtkTreeView *treeview)
{
	GtkCellRenderer *rend;
	GtkTreeViewColumn *column;
	GtkTreeViewColumn *text_column; /**< Column */
    GtkCellRenderer *text_rend;

#if 1
	column = gtk_tree_view_column_new();
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	// auv gtk_tree_view_column_set_visible(column, FALSE);

	gtk_tree_view_column_set_visible(column, TRUE);
	gtk_tree_view_set_expander_column(GTK_TREE_VIEW(treeview), column);


	rend = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, rend, FALSE);
	gtk_tree_view_column_set_attributes(column, rend,
					    "pixbuf", STATUS_ICON_COLUMN,
					    "visible", STATUS_ICON_VISIBLE_COLUMN,
#if GTK_CHECK_VERSION(2,6,0)
					    "cell-background-gdk", BGCOLOR_COLUMN,
#endif
					    NULL);
	g_object_set(rend, "xalign", 0.0, "xpad", 6, "ypad", 0, NULL);

	text_rend = rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start (column, rend, TRUE);
	gtk_tree_view_column_set_attributes(column, rend,
#if GTK_CHECK_VERSION(2,6,0)
					    	    "cell-background-gdk", BGCOLOR_COLUMN,
#endif
										"markup", NAME_COLUMN,
										NULL);
#if GTK_CHECK_VERSION(2,6,0)
	// g_signal_connect(G_OBJECT(rend), "editing-started", G_CALLBACK(gtk_blist_renderer_editing_started_cb), NULL);
#endif
	g_object_set(rend, "ypad", 0, "yalign", 0.5, NULL);
#if GTK_CHECK_VERSION(2,6,0)
	g_object_set(rend, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
#endif
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);


	rend = gtk_cell_renderer_pixbuf_new();
	g_object_set(rend, "xalign", 1.0, "yalign", 0.5, "ypad", 0, "xpad", 3, NULL);
	gtk_tree_view_column_pack_start(column, rend, FALSE);
	gtk_tree_view_column_set_attributes(column, rend, "pixbuf", EMBLEM_COLUMN,
#if GTK_CHECK_VERSION(2,6,0)
							  "cell-background-gdk", BGCOLOR_COLUMN,
#endif
							  "visible", EMBLEM_VISIBLE_COLUMN, NULL);

	rend = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, rend, FALSE);
	gtk_tree_view_column_set_attributes(column, rend,
					   "pixbuf", PROTOCOL_ICON_COLUMN,
					   "visible", PROTOCOL_ICON_VISIBLE_COLUMN,
#if GTK_CHECK_VERSION(2,6,0)
					   "cell-background-gdk", BGCOLOR_COLUMN,
#endif
					  NULL);
	g_object_set(rend, "xalign", 0.0, "xpad", 3, "ypad", 0, NULL);

	rend = gtk_cell_renderer_pixbuf_new();
	g_object_set(rend, "xalign", 1.0, "ypad", 0, NULL);
	gtk_tree_view_column_pack_start(column, rend, FALSE);
	gtk_tree_view_column_set_attributes(column, rend, "pixbuf", BUDDY_ICON_COLUMN,
#if GTK_CHECK_VERSION(2,6,0)
					    "cell-background-gdk", BGCOLOR_COLUMN,
#endif
					    "visible", BUDDY_ICON_VISIBLE_COLUMN,
					    NULL);
#else
#endif
}

GtkWidget *
do_tree_store (GtkWidget *do_widget)
{
  if (!window)
    {
      GtkWidget *vbox, *tool_hbox, *grep_hbox;
      GtkWidget *button;
      GtkWidget *text;
      GtkWidget *sw;
      GtkWidget *treeview;
      GtkTreeModel *model;

      /* create window, etc */
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_screen (GTK_WINDOW (window),
			     gtk_widget_get_screen (do_widget));
      gtk_window_set_title (GTK_WINDOW (window), "list");
      g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_widget_destroyed), &window);

      vbox = gtk_vbox_new (FALSE, 1);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 1);
      gtk_container_add (GTK_CONTAINER (window), vbox);

      tool_hbox = gtk_hbox_new (FALSE, 1);
      gtk_container_set_border_width (GTK_CONTAINER (tool_hbox), 1);
      gtk_box_pack_start (GTK_BOX (vbox), tool_hbox, FALSE, FALSE, 0);

      button = gtk_button_new_with_label("search");
      gtk_box_pack_start (GTK_BOX (tool_hbox), button, FALSE, FALSE, 0);

      button = gtk_button_new_with_label("search1");
      gtk_box_pack_start (GTK_BOX (tool_hbox), button, FALSE, FALSE, 0);

      grep_hbox = gtk_hbox_new (FALSE, 1);
      gtk_container_set_border_width (GTK_CONTAINER (grep_hbox), 1);
      gtk_box_pack_start (GTK_BOX (vbox), grep_hbox, FALSE, FALSE, 0);

      text = gtk_entry_new();
      gtk_box_pack_start (GTK_BOX (grep_hbox), text, TRUE, TRUE, 0);

      sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
					   GTK_SHADOW_ETCHED_IN);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				      GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

      /* create tree view */
      treeview = gtk_tree_view_new();

      /* create model */
      model = create_model (treeview);

      gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(model));

      g_object_unref (model);
      gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
      gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(treeview), TRUE);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
				   GTK_SELECTION_SINGLE);

      add_columns (GTK_TREE_VIEW (treeview));

      gtk_container_add (GTK_CONTAINER (sw), treeview);

      /* expand all rows after the treeview widget has been realized */
      g_signal_connect (treeview, "realize",
			G_CALLBACK (gtk_tree_view_expand_all), NULL);
      gtk_window_set_default_size (GTK_WINDOW (window), 650, 400);
    }

  if (!GTK_WIDGET_VISIBLE (window))
    gtk_widget_show_all (window);
  else
    {
      gtk_widget_destroy (window);
      window = NULL;
    }

  return window;
}
