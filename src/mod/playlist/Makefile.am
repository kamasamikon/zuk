moddir = $(prefix)/zuk/modules/playlist

mod_LTLIBRARIES = libplaylist.la

libplaylist_la_SOURCES = playlist_impl.cpp playlist_ui.cpp

dist_mod_DATA = cfg.xml main.ini

GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

libplaylist_la_LDFLAGS = ${GTK_LIBS} -module
libplaylist_la_CFLAGS = ${GTK_CFLAGS}
libplaylist_la_CPPFLAGS = ${GTK_CFLAGS}

libplaylist_la_LDFLAGS += -L../../lib/kacoo -L../../lib/kmm -L../../lib/kmc -L../../lib/textconv
libplaylist_la_LDFLAGS += -lrt -lkacoo -lkmm -lkmc -ltextconv

libplaylist_la_CFLAGS += -Wall -g
libplaylist_la_CFLAGS += -I../inc
libplaylist_la_CFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libplaylist_la_CFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libplaylist_la_CFLAGS += -D__UNIX__

libplaylist_la_CPPFLAGS += -Wall -g
libplaylist_la_CPPFLAGS += -I../inc
libplaylist_la_CPPFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libplaylist_la_CPPFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libplaylist_la_CPPFLAGS += -D__UNIX__


