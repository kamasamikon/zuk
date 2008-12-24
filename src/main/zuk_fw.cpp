#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__)
#include <windows.h>
#endif

#include <QtGui/QApplication>
#include <QtGui/QDialog>

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

    klog(("QApplication app(argc, argv);\n"));
    QApplication app(argc, argv);

    QDialog dlg;

    klog(("kim_new\n"));
    KIM* im = kim_new(knil);
    kim_start(im);

    klog(("zuk_init(argc, argv);\n"));
    zuk_init(im, argc, argv);

    dlg.show();

    klog(("retval = app.exec();\n"));
    retval = app.exec();

    klog(("zuk_final(argc, argv);\n"));
    zuk_final(im, argc, argv);

    klog(("kim_del\n"));
    kim_del(im);

    klog(("main:: return:%d\n", retval));
    return retval;
}

