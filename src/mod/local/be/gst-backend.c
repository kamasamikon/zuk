/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

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

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "gst-backend.h"

static GstSeekFlags seek_flags = GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT;

typedef struct _GstBackend GstBackend;

struct _GstBackend {
    GstElement *playElement;
    GstElement *bin;
    GstElement *videosink;
    gpointer window;
};

static gboolean bus_cb(GstBus *bus, GstMessage *msg, gpointer data)
{
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        {
            g_debug("end-of-stream");
            break;
        }
    case GST_MESSAGE_ERROR:
        {
            gchar *debug;
            GError *err;

            gst_message_parse_error(msg, &err, &debug);
            g_free(debug);

            g_warning("Error: %s", err->message);
            g_error_free(err);
            break;
        }
    default:
        break;
    }

    return TRUE;
}

void *backend_init(int *argc, char **argv[])
{
    GstBackend *be = g_new0(GstBackend, 1);

    gst_init(argc, argv);

    return (void *) be;
}

void backend_set_window(void *handle, void *a_window)
{
    GstBackend *be = (GstBackend *) handle;

    be->window = GINT_TO_POINTER(GDK_WINDOW_XWINDOW(a_window));
}

void backend_play(void *handle, const char *filename)
{
    GstBackend *be = (GstBackend *) handle;

    backend_stop(handle);

    be->playElement = gst_pipeline_new("gst-player");

    be->bin = gst_element_factory_make("playbin", "bin");
    be->videosink = gst_element_factory_make("ximagesink", "videosink");

    g_object_set(G_OBJECT(be->bin), "video-sink", be->videosink, NULL);

    gst_bin_add(GST_BIN(be->playElement), be->bin);

    {
        GstBus *bus;
        bus = gst_pipeline_get_bus(GST_PIPELINE(be->playElement));
        gst_bus_add_watch(bus, bus_cb, NULL);
        gst_object_unref(bus);
    }

    {
        gchar *uri;

        if (gst_uri_is_valid(filename))
            uri = g_strdup(filename);
        else if (g_path_is_absolute(filename))
            uri = g_filename_to_uri(filename, NULL, NULL);
        else {
            gchar *tmp;
            tmp = g_build_filename(g_get_current_dir(), filename, NULL);
            uri = g_filename_to_uri(tmp, NULL, NULL);
            g_free(tmp);
        }

        g_debug("%s", uri);
        g_object_set(G_OBJECT(be->bin), "uri", uri, NULL);
        g_free(uri);
    }

    g_object_set(G_OBJECT(be->videosink), "force-aspect-ratio", TRUE, NULL);

    if (GST_IS_X_OVERLAY(be->videosink))
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(be->videosink), GPOINTER_TO_INT(be->window));

    gst_element_set_state(be->playElement, GST_STATE_PLAYING);
}

void backend_stop(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    if (be->playElement) {
        gst_element_set_state(be->playElement, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(be->playElement));
        be->playElement = NULL;
    }
}

void backend_pause(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    gst_element_set_state(be->playElement, GST_STATE_PAUSED);
}

void backend_resume(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    gst_element_set_state(be->playElement, GST_STATE_PLAYING);
}

void backend_reset(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    gst_element_seek(be->playElement, 1.0,
            GST_FORMAT_TIME, seek_flags, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void backend_seek(void *handle, int value)
{
    GstBackend *be = (GstBackend *) handle;

    gst_element_seek(be->playElement, 1.0,
            GST_FORMAT_TIME,
            seek_flags, GST_SEEK_TYPE_CUR, value * GST_SECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void backend_seek_absolute(void *handle, unsigned int value)
{
    GstBackend *be = (GstBackend *) handle;

    gst_element_seek(be->playElement, 1.0,
            GST_FORMAT_TIME, seek_flags, GST_SEEK_TYPE_SET, value, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

unsigned int backend_query_position(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    GstFormat format = GST_FORMAT_TIME;
    guint64 cur;

    gst_element_query_position(be->playElement, &format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return (unsigned int) cur;
}

unsigned int backend_query_duration(void *handle)
{
    GstBackend *be = (GstBackend *) handle;

    GstFormat format = GST_FORMAT_TIME;
    guint64 cur;

    gst_element_query_duration(be->playElement, &format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return (unsigned int) cur;
}

void backend_deinit(void *handle)
{
    GstBackend *be = (GstBackend *) handle;
    g_free(be);
}
