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
// standard KMM routines

static GtkWidget* create_media_window()
{
    GtkWidget *mediaWindow;

    mediaWindow = gtk_drawing_area_new();
    return mediaWindow;
}

static GtkWidget* create_playlist_window()
{
    GtkWidget *playlistWindow;

    // playlistWindow = gtk_tree_view_collapse_all();
}

int add_playitem(KMediaChannel *channel)
{
}

void create_ui(KIM *im)
{
    SET_GLOBALS(im);

    GtkWidget *mediaWindow = create_media_window();
    kim_addptr(im, "p.playlist.ui.mediaWindow", mediaWindow, 0, knil, knil);
}

