moddir = $(prefix)/zuk/modules/local
dist_mod_DATA = cfg.xml main.ini

mod_LTLIBRARIES = libmc-local.la

libmc_local_la_SOURCES = \
						 local_fw.cpp \
						 be/gst-backend.c be/ui.c \
						 mc/local-channel.cpp mc/local-device.cpp mc/local-protocol.cpp

GST_LIBS=`pkg-config --libs gstreamer-0.10` -lgstinterfaces-0.10
GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`
GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

libmc_local_la_LDFLAGS = ${GST_LIBS} ${GTK_LIBS} -module -lrt -lkacoo -lkmm -lkmc -ltextconv
libmc_local_la_CFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}

libmc_local_la_LDFLAGS += -L../../lib/kacoo -L../../lib/kmm -L../../lib/kmc -L../../lib/textconv
libmc_local_la_LDFLAGS += -lrt -lkacoo -lkmm -lkmc -ltextconv

libmc_local_la_CFLAGS += -Wall -g
libmc_local_la_CFLAGS += -I../inc
libmc_local_la_CFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libmc_local_la_CFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libmc_local_la_CFLAGS += -D__UNIX__

libmc_local_la_CPPFLAGS = -Wall -g
libmc_local_la_CPPFLAGS += -I../inc
libmc_local_la_CPPFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libmc_local_la_CPPFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libmc_local_la_CPPFLAGS += -D__UNIX__


