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

static KIM *__g_im = knil;

kint turtle_frontend_start_for_qt(KIM *im, kint argc, kchar **argv)
{
    /* XXX move to install dir for QT load resource */
    kvfs_chdir(kim_getstr(im, "s.env.path.instDir", knil));

    QApplication app(argc, argv);
    return app.exec();
}

