zukdir = $(prefix)/zuk
dist_zuk_DATA = cfg.xml zuk main.ini

zuk_PROGRAMS = zuk_bin
zuk_bin_SOURCES = options.c sysdeps.c zuk_fw.cpp zuk_main.cpp

GST_LIBS=`pkg-config --libs gstreamer-0.10` -lgstinterfaces-0.10
GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`
GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

zuk_bin_LDFLAGS = ${GST_LIBS} ${GTK_LIBS}

zuk_core_LDFLAGS = -L../lib/kacoo -L../lib/kmm -L../lib/kmc -L../lib/textconv
zuk_core_LDFLAGS += -lrt -lkacoo -lkmm -lkmc -ltextconv

zuk_bin_LDFLAGS += ${zuk_core_LDFLAGS}

zuk_bin_CFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}
zuk_bin_CPPFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}

zuk_bin_CFLAGS += -Wall -g
zuk_bin_CFLAGS += -I../lib/kacoo/inc

zuk_bin_CPPFLAGS += -Wall -g
zuk_bin_CPPFLAGS += -I../lib/kacoo/inc


zuk_bin_CFLAGS += -I../lib/kacoo/inc -I../lib/kacoo/lib/inc -I../lib/kacoo/lib/xmldoc
zuk_bin_CFLAGS += -I../lib/kmm -I../lib/kmc -I../lib/textconv

zuk_bin_CPPFLAGS += -I../lib/kacoo/inc -I../lib/kacoo/lib/inc -I../lib/kacoo/lib/xmldoc
zuk_bin_CPPFLAGS += -I../lib/kmm -I../lib/kmc -I../lib/textconv


