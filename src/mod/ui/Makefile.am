moddir = $(prefix)/zuk/modules/ui
dist_mod_DATA = cfg.xml main.ini main.glade

mod_LTLIBRARIES = libui.la

libui_la_SOURCES = ui_fw.cpp glade-palette-box.c

GTK_LIBS=`pkg-config --libs gtk+-2.0`
GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`

GTK_LIBS += -lglade-2.0
GTK_CFLAGS += -I/usr/include/libglade-2.0 
libui_la_CPPFLAGS = ${GTK_CFLAGS}

zuk_core_LDFLAGS = -L../../lib/kacoo -L../../lib/kmm -L../../lib/kmc -L../../lib/textconv
zuk_core_LDFLAGS += -lrt -lkacoo -lkmm -lkmc -ltextconv

libui_la_LDFLAGS = ${GTK_LIBS} -module -rdynamic 
libui_la_LDFLAGS += ${zuk_core_LDFLAGS}
libui_la_CFLAGS = ${GTK_CFLAGS}

libui_la_CFLAGS += -Wall -g
libui_la_CFLAGS += -I../inc
libui_la_CFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libui_la_CFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libui_la_CFLAGS += -D__UNIX__

libui_la_CPPFLAGS += -Wall -g
libui_la_CPPFLAGS += -I../inc
libui_la_CPPFLAGS += -I../../lib/kacoo/inc -I../../lib/kacoo/lib/inc -I../../lib/kacoo/lib/xmldoc
libui_la_CPPFLAGS += -I../../lib/kmm -I../../lib/kmc -I../../lib/textconv
libui_la_CPPFLAGS += -D__UNIX__

