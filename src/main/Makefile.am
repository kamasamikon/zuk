bin_PROGRAMS = zuk
zuk_SOURCES = options.c sysdeps.c zuk_fw.cpp zuk_main.cpp

GST_LIBS=`pkg-config --libs gstreamer-0.10` -lgstinterfaces-0.10
GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`
GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

zuk_LDFLAGS = ${GST_LIBS} ${GTK_LIBS}
zuk_LDFLAGS += -L../lib/kacoo -L../lib/kmm -L../lib/kmc -L../lib/textconv
zuk_LDFLAGS += -lrt -lkacoo -lkmm -lkmc -ltextconv

zuk_CFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}
zuk_CPPFLAGS = ${GST_CFLAGS} ${GTK_CFLAGS}

zuk_CFLAGS += -Wall
zuk_CFLAGS += -I../lib/kacoo/inc

zuk_CPPFLAGS += -Wall
zuk_CPPFLAGS += -I../lib/kacoo/inc


zuk_CFLAGS += -I../lib/kacoo/inc -I../lib/kacoo/lib/inc -I../lib/kacoo/lib/xmldoc
zuk_CFLAGS += -I../lib/kmm -I../lib/kmc -I../lib/textconv

zuk_CPPFLAGS += -I../lib/kacoo/inc -I../lib/kacoo/lib/inc -I../lib/kacoo/lib/xmldoc
zuk_CPPFLAGS += -I../lib/kmm -I../lib/kmc -I../lib/textconv


