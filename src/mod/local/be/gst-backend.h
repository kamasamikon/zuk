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

#ifndef GST_BACKEND_H
#define GST_BACKEND_H

gpointer backend_init(int *argc, char **argv[]);
void backend_deinit(gpointer *handle);
void backend_set_window(gpointer *handle, gpointer window);
void backend_play(gpointer *handle, const gchar *filename);
void backend_stop(gpointer *handle);
void backend_seek(gpointer *handle, gint value);
void backend_seek_absolute(gpointer *handle, guint64 value);
void backend_reset(gpointer *handle);
void backend_pause(gpointer *handle);
void backend_resume(gpointer *handle);
guint64 backend_query_position(gpointer *handle);
guint64 backend_query_duration(gpointer *handle);

#endif /* GST_BACKEND_H */
