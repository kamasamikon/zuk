#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__)
#include <windows.h>
#endif

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QUiLoader>
#include <QtScript>

#include <ktypes.h>
#include <ksal.h>
#include <kdbg.h>

#include <kim.h>
#include <kmm.h>
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
    KMM *mm;
    int retval;

    kchar *str_js, *str_ui, *str_theme;

    kdbg_init();

    klog(("QApplication app(argc, argv);\n"));
    QApplication app(argc, argv);

    klog(("kim_new\n"));
    KIM* im = kim_new(knil);
    kim_start(im);

    klog(("zuk_init(argc, argv);\n"));
    zuk_init(im, argc, argv);

    mm = (KMM*)kim_getptr(im, "p.sys.kmm", knil);

#define PLAYLIST_GUID "7D378382-9351-4f4e-BF83-4FF20C456B6D"
    kmm_jc_cmd(mm, "jc_playlist_get_ui", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_ui);
    kmm_jc_cmd(mm, "jc_playlist_get_script", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_js);
    kmm_jc_cmd(mm, "jc_playlist_get_theme", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_theme);

    QScriptEngine engine;

    engine.evaluate(str_js);

    QTemporaryFile uifile;
    if (!uifile.open())
        return -1;

    uifile.write(str_ui);
    QUiLoader loader;
    QWidget *ui = loader.load(&uifile);
    uifile.close();

    QScriptValue ctor = engine.evaluate("Calculator");
    QScriptValue scriptUi = engine.newQObject(ui, QScriptEngine::ScriptOwnership);
    QScriptValue calc = ctor.construct(QScriptValueList() << scriptUi);

    qApp->setStyleSheet(str_theme);

    ui->show();

    klog(("retval = app.exec();\n"));
    retval = app.exec();

    klog(("zuk_final(argc, argv);\n"));
    zuk_final(im, argc, argv);

    klog(("kim_del\n"));
    kim_del(im);

    klog(("main:: return:%d\n", retval));
    return retval;
}

