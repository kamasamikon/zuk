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

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include "gst-backend.h"

static GstSeekFlags seek_flags = GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT;

typedef struct _GstBackend GstBackend;

struct _GstBackend {
    GstElement *pipeline;
    GstElement *bin;
    GstElement *videosink;
    gpointer window;
};

static gboolean bus_cb(GstBus *bus, GstMessage *msg, gpointer data)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
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

gpointer backend_init(int *argc, char **argv[])
{
    GstBackend *be = g_new0(GstBackend, 1);

    gst_init(argc, argv);

    return (gpointer)be;
}

void backend_set_window(gpointer *handle, gpointer window_)
{
    GstBackend *be = (GstBackend*)handle;

    be->window = window_;
}

void backend_play(gpointer *handle, const gchar *filename)
{
    GstBackend *be = (GstBackend*)handle;

    backend_stop(handle);

    be->pipeline = gst_pipeline_new("gst-player");

    be->bin = gst_element_factory_make("playbin", "bin");
    be->videosink = gst_element_factory_make("ximagesink", "videosink");

    g_object_set(G_OBJECT(be->bin), "video-sink", be->videosink, NULL);

    gst_bin_add(GST_BIN(be->pipeline), be->bin);

    {
        GstBus *bus;
        bus = gst_pipeline_get_bus(GST_PIPELINE(be->pipeline));
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

    gst_element_set_state(be->pipeline, GST_STATE_PLAYING);
}

void backend_stop(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    if (be->pipeline) {
        gst_element_set_state(be->pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(be->pipeline));
        be->pipeline = NULL;
    }
}

void backend_pause(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    gst_element_set_state(be->pipeline, GST_STATE_PAUSED);
}

void backend_resume(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    gst_element_set_state(be->pipeline, GST_STATE_PLAYING);
}

void backend_reset(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    gst_element_seek(be->pipeline, 1.0,
            GST_FORMAT_TIME,
            seek_flags,
            GST_SEEK_TYPE_SET, 0,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void backend_seek(gpointer *handle, gint value)
{
    GstBackend *be = (GstBackend*)handle;

    gst_element_seek(be->pipeline, 1.0,
            GST_FORMAT_TIME,
            seek_flags,
            GST_SEEK_TYPE_CUR, value * GST_SECOND,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void backend_seek_absolute(gpointer *handle, guint64 value)
{
    GstBackend *be = (GstBackend*)handle;

    gst_element_seek(be->pipeline, 1.0,
            GST_FORMAT_TIME,
            seek_flags,
            GST_SEEK_TYPE_SET, value,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

guint64 backend_query_position(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    GstFormat format = GST_FORMAT_TIME;
    guint64 cur;

    gst_element_query_position(be->pipeline, &format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

guint64 backend_query_duration(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;

    GstFormat format = GST_FORMAT_TIME;
    guint64 cur;

    gst_element_query_duration(be->pipeline, &format, &cur);
    if (format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

void backend_deinit(gpointer *handle)
{
    GstBackend *be = (GstBackend*)handle;
    g_free(be);
}

