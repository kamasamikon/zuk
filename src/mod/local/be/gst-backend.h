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

#ifndef GST_BACKEND_H
#define GST_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

void *backend_init(int *argc, char **argv[]);
void backend_deinit(void *handle);
void backend_set_window(void *handle, void *window);
void backend_play(void *handle, const char *filename);
void backend_stop(void *handle);
void backend_seek(void *handle, int value);
void backend_seek_absolute(void *handle, unsigned int value);
void backend_reset(void *handle);
void backend_pause(void *handle);
void backend_resume(void *handle);
unsigned int backend_query_position(void *handle);
unsigned int backend_query_duration(void *handle);

#ifdef __cplusplus
}
#endif
#endif /* GST_BACKEND_H */
