lib_LTLIBRARIES = libmclocal.la

libmclocal_la_SOURCES = \
						local_fw.cpp \
						be/gst-backend.c be/ui.c \
						mc/local-channel.cpp mc/local-device.cpp mc/local-protocol.cpp

GST_LIBS=`pkg-config --libs gstreamer-0.10` -lgstinterfaces-0.10
GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`
GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

libmclocal_la_LDFLAGS = ${GST_LIBS} ${GTK_LIBS}
libmclocal_la_CFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}

