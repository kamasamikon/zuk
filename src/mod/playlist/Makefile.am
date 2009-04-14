
lib_LTLIBRARIES = libplaylist.la

libplaylist_la_SOURCES = playlist_impl.cpp

GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

libplaylist_la_LDFLAGS = ${GTK_LIBS}
libplaylist_la_CFLAGS = ${GTK_CFLAGS}

libplaylist_la_CFLAGS += -Wall 
libplaylist_la_CFLAGS += -I../inc
libplaylist_la_CFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libplaylist_la_CFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libplaylist_la_CFLAGS += -D__UNIX__

libplaylist_la_CPPFLAGS = -Wall 
libplaylist_la_CPPFLAGS += -I../inc
libplaylist_la_CPPFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libplaylist_la_CPPFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libplaylist_la_CPPFLAGS += -D__UNIX__


