#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__)
#include <windows.h>
#endif

#include <QWaitCondition>
#include <QMutex>
#include <stdio.h>

#include <QtGui/QApplication>
#include <QTextCodec>

#include <ktypes.h>
#include <ksal.h>
#include <kdbg.h>

#include <kim.h>
#include <kini.h>

#if defined(__UNIX__)
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

#include "zuk_main.h"

int main(int argc, char **argv)
{
    int retval;

    printf("QApplication app(argc, argv);\n");
    QApplication app(argc, argv);

    printf("zuk_init(argc, argv);\n");
    zuk_init(argc, argv);

    printf("retval = app.exec();\n");
    retval = app.exec();

    printf("zuk_final(argc, argv);\n");
    zuk_final(argc, argv);

    printf("main:: return:%d\n", retval);
    return retval;
}

