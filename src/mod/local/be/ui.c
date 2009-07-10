/* vim:set et sw=4 sts=4 ff=unix: */
/*
 * Copyright(C) 2008 Felipe Contreras.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include <string.h>

#include "gst-backend.h"

static gchar *__g_filename;
static GtkWidget *__g_video_output;
static GtkWidget *__g_pause_button;
static GtkWidget *__g_scale;
static guint64 __g_duration;
static GtkWidget *__g_window;

static gpointer __g_backend;

#define DURATION_IS_VALID(x)(x != 0 && x !=(guint64) -1)

static void toggle_paused(void)
{
    static gboolean paused = FALSE;

    if (paused) {
        backend_resume(__g_backend);
        gtk_button_set_label(GTK_BUTTON(__g_pause_button), "Pause");
        paused = FALSE;
    } else {
        backend_pause(__g_backend);
        gtk_button_set_label(GTK_BUTTON(__g_pause_button), "Resume");
        paused = TRUE;
    }
}

static void toggle_fullscreen(void)
{
    static gboolean fullscreen = TRUE;

    fullscreen = !fullscreen;

    if (fullscreen)
        gtk_window_fullscreen(__g_window);
    else
        gtk_window_unfullscreen(__g_window);
}

static void pause_cb(GtkWidget *widget, gpointer data)
{
    toggle_paused();
}

static void reset_cb(GtkWidget *widget, gpointer data)
{
    backend_reset(__g_backend);
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

static gboolean key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    switch(event->keyval)
    {
        case GDK_P:
        case GDK_p:
        case GDK_space:
            toggle_paused();
            break;
        case GDK_F:
        case GDK_f:
            toggle_fullscreen();
            break;
        case GDK_R:
        case GDK_r:
            backend_reset(__g_backend);
            break;
        case GDK_Right:
            backend_seek(__g_backend, 10);
            break;
        case GDK_Left:
            backend_seek(__g_backend, -10);
            break;
        case GDK_Q:
        case GDK_q:
            gtk_main_quit();
            break;
        default:
            break;
    }

    return TRUE;
}

static void seek_cb(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer data)
{
    guint64 to_seek;

    if (!DURATION_IS_VALID(__g_duration))
        __g_duration = backend_query_duration(__g_backend);

    if (!DURATION_IS_VALID(__g_duration))
        return;

    to_seek = (value / 100) * __g_duration;

#if 0
    g_print("value: %f\n", value);
    g_print("__g_duration: %llu\n", __g_duration);
    g_print("seek: %llu\n", to_seek);
#endif

    backend_seek_absolute(__g_backend, to_seek);
}

static void start(void)
{
    GtkWidget *button;
    GtkWidget *hbox;
    GtkWidget *vbox;

    __g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(G_OBJECT(__g_window), "delete_event", G_CALLBACK(delete_event), NULL);

    g_signal_connect(G_OBJECT(__g_window), "destroy", G_CALLBACK(destroy), NULL);

    g_signal_connect(G_OBJECT(__g_window), "key-press-event", G_CALLBACK(key_press), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(__g_window), 0);

    vbox = gtk_vbox_new(FALSE, 0);

    gtk_container_add(GTK_CONTAINER(__g_window), vbox);

    gtk_widget_show(vbox);

    hbox = gtk_hbox_new(FALSE, 0);

    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

    gtk_widget_show(hbox);

    {
        __g_video_output = gtk_drawing_area_new();

        gtk_box_pack_start(GTK_BOX(vbox), __g_video_output, TRUE, TRUE, 0);

        gtk_widget_set_size_request(__g_video_output, 0x200, 0x100);

        gtk_widget_show(__g_video_output);
    }

    {
        button = gtk_button_new_with_label("Pause");

        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_cb), NULL);

        gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

        gtk_widget_show(button);

        __g_pause_button = button;
    }

    {
        button = gtk_button_new_with_label("Reset");

        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_cb), NULL);

        gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

        gtk_widget_show(button);
    }

    {
        GtkObject *adjustment;
        adjustment = gtk_adjustment_new(0, 0, 101, 1, 5, 1);
        __g_scale = gtk_hscale_new(GTK_ADJUSTMENT(adjustment));

        gtk_box_pack_end(GTK_BOX(hbox), __g_scale, TRUE, TRUE, 2);

        g_signal_connect(G_OBJECT(__g_scale), "change-value", G_CALLBACK(seek_cb), NULL);

        gtk_widget_show(__g_scale);
    }

    // gtk_widget_show(__g_window);
}

static gboolean init(gpointer data)
{
    backend_set_window(__g_backend, GINT_TO_POINTER(GDK_WINDOW_XWINDOW(__g_video_output->window)));

    if (__g_filename)
        backend_play(__g_backend, __g_filename);

    return FALSE;
}

static gboolean timeout(gpointer data)
{
    guint64 pos;

    pos = backend_query_position(__g_backend);
    if (!DURATION_IS_VALID(__g_duration))
        __g_duration = backend_query_duration(__g_backend);

    if (!DURATION_IS_VALID(__g_duration))
        return TRUE;

#if 0
    g_print("__g_duration=%f\n", __g_duration /((double) 60 * 1000 * 1000 * 1000));
    g_print("position=%llu\n", pos);
#endif

    /** @todo use events for seeking instead of checking for bad positions. */
    if (pos != 0)
    {
        double value;
        value = (pos * (((double) 100) / __g_duration));
        gtk_range_set_value(GTK_RANGE(__g_scale), value);
    }

    return TRUE;
}

extern void *mediaWindow;
int __not_main(int argc, char *argv[])
{
    // gtk_init(&argc, &argv);
    __g_backend = backend_init(&argc, &argv);

    start();
    __g_video_output = (GtkWidget*)mediaWindow;

    __g_filename = g_strdup("/home/auv/Desktop/niaochao/MVI_0032.avi");

    toggle_fullscreen();
    g_idle_add(init, NULL);
    g_timeout_add(500, timeout, NULL);

    return 0;
}

