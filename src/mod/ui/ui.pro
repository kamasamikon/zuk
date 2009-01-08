######################################################################
# Automatically generated by qmake (2.01a) Fri Jun 20 13:18:33 2008
######################################################################

TEMPLATE = lib
CONFIG += debug_and_release
CONFIG += dll

QMAKE_LFLAGS += -Wl,-rpath,"'\$\${ORIGIN}'"
QMAKE_LFLAGS += -Wl,-rpath,"'\$\${ORIGIN}/../../lib'"

QMAKE_CXXFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0 libglade-2.0`
QMAKE_CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0 libglade-2.0`

DEFINES -= UNICODE

win32:DEFINES += __WIN32__ WIN32
unix:DEFINES += __UNIX__

unix:LIBS += `pkg-config --libs gtk+-2.0 libglade-2.0`
LIBS += -L../../../output -lkacoo -lkmc -ltextconv

DESTDIR = ../../../installdir/modules/ui
QMAKE_POST_LINK =

INCLUDEPATH += . ../inc ../../lib/textconv ../../lib/kacoo/inc ../../lib/kacoo/lib/inc ../../lib/kmm ../../lib/kmc ../../lib/inc ../../lib/xmldoc
DEPENDPATH += . ../inc ../../lib/textconv ../../lib/kacoo/inc ../../lib/kacoo/lib/inc ../../lib/kmm ../../lib/kmc ../../lib/inc ../../lib/xmldoc

# Input
HEADERS +=
SOURCES += ui_impl.cpp 

