pkglib_LTLIBRARIES = libmclocal.la

libmclocal_la_SOURCES = \
						local_fw.cpp \
						be/gst-backend.c be/ui.c \
						mc/local-channel.cpp mc/local-device.cpp mc/local-protocol.cpp

GST_LIBS=`pkg-config --libs gstreamer-0.10` -lgstinterfaces-0.10
GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`
GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

libmclocal_la_LDFLAGS = ${GST_LIBS} ${GTK_LIBS} -module
libmclocal_la_CFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}


libmclocal_la_CFLAGS += -Wall 
libmclocal_la_CFLAGS += -I../inc
libmclocal_la_CFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libmclocal_la_CFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libmclocal_la_CFLAGS += -D__UNIX__

libmclocal_la_CPPFLAGS = -Wall 
libmclocal_la_CPPFLAGS += -I../inc
libmclocal_la_CPPFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libmclocal_la_CPPFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libmclocal_la_CPPFLAGS += -D__UNIX__


