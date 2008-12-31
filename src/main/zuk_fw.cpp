#include <stdio.h>
#include <stdlib.h>

#if defined(__WIN32__)
#include <windows.h>
#endif

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QUiLoader>
#include <QtScript>

#include <QVBoxLayout>
#include <QPushButton>

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

QScriptValue mySpecialQObjectConstructor(QScriptContext *context, QScriptEngine *engine)
{
    static int i = 0;
    QWidget *parent = (QWidget*)context->argument(0).toQObject();
    printf("mySpecialQObjectConstructor:parent:%x\n", (unsigned int)parent);

    char label[222];
    sprintf(label, "shit:%d\n", i++);

    QPushButton *object = new QPushButton(QString(label), (QWidget*)parent);
    printf("mySpecialQObjectConstructor:object:%x\n", (unsigned int)object);

    object->setVisible(true);
    parent->layout()->addWidget(object);

    /* ScriptOwnership and QtOwnership all works */
    // return engine->newQObject(object, QScriptEngine::ScriptOwnership);
    return engine->newQObject(object, QScriptEngine::QtOwnership);
}


class mwi {
    kchar *modname;     /* playlist, ui, etc */

    QScriptEngine engine;
    QUiLoader uiLoader;

    QString uiText;
    QString scriptText;
    QString themeText;

    kuint type;         /* popup, main, float */

    QWidget *widget;

    QWidget *newWidget(kchar *name);
};

// kim_addint("i.wmi.evt.win", add_or_del, [name of entry, e.g. "i.mod.mwi.win"], knil)

QWidget *playlist_ui(QObject *p, KMM *mm)
{
    kchar *str_js, *str_ui, *str_theme;

#define PLAYLIST_GUID "7D378382-9351-4f4e-BF83-4FF20C456B6D"
    kmm_jc_cmd(mm, "jc_playlist_get_ui", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_ui);
    kmm_jc_cmd(mm, "jc_playlist_get_script", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_js);
    kmm_jc_cmd(mm, "jc_playlist_get_theme", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_theme);

    QScriptEngine *engine = new QScriptEngine();
    engine->evaluate("print('hello', 'world')");
    engine->evaluate(str_js);

    QTemporaryFile uifile;
    if (!uifile.open())
        return 0;

#if 0
    uifile.write(str_ui);
    QUiLoader loader;
    QWidget *ui = loader.load(&uifile);
    qDebug() << "uifile:" << uifile.fileName() << endl;
    uifile.close();
#else
    QUiLoader loader;
    QFile uiFile("/home/auv/work/zuk/installdir/modules/playlist/pl.ui");
    uiFile.open(QIODevice::ReadOnly);
    QWidget *ui = loader.load(&uiFile);
    uiFile.close();

    if (!ui) {
        klog(("no ui created\n"));
        return 0;
    }
#endif


    QVBoxLayout *layout = new QVBoxLayout();
    ui->setLayout(layout);

    QScriptValue ctor = engine->evaluate("Calculator");
    QScriptValue scriptUi = engine->newQObject(ui, QScriptEngine::ScriptOwnership);
    QScriptValue calc = ctor.construct(QScriptValueList() << scriptUi);

    ctor = engine->newFunction(mySpecialQObjectConstructor);
    QScriptValue metaObject = engine->newQMetaObject(&QObject::staticMetaObject, ctor);
    engine->globalObject().setProperty("QObject", metaObject);

    // QScriptValue result = engine->evaluate("new QObject()");
    // printf("xxxxxxxxxxxxxxxxxxxxx\n");


    return ui;
}


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

    QDialog dlg;

    QVBoxLayout *layout = new QVBoxLayout;

    mm = (KMM*)kim_getptr(im, "p.sys.kmm", knil);

#define PLAYLIST_GUID "7D378382-9351-4f4e-BF83-4FF20C456B6D"
    kmm_jc_cmd(mm, "jc_playlist_get_ui", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_ui);
    kmm_jc_cmd(mm, "jc_playlist_get_script", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_js);
    kmm_jc_cmd(mm, "jc_playlist_get_theme", PLAYLIST_GUID, knil, knil, knil, knil, (kchar**)&str_theme);

    /////////////////////////////////////
    //
    QScriptEngine engine;

    engine.evaluate(str_js);

#if 0
    QTemporaryFile uifile;
    if (!uifile.open())
        return -1;

    uifile.write(str_ui);
    QUiLoader loader;
    QWidget *ui = loader.load(&uifile);
    qDebug() << "uifile:" << uifile.fileName() << endl;
    uifile.close();
#else
    QUiLoader loader;
    QFile uiFile("/home/auv/work/zuk/installdir/modules/playlist/pl.ui");
    uiFile.open(QIODevice::ReadOnly);
    QWidget *ui = loader.load(&uiFile);
    uiFile.close();

    if (!ui) {
        klog(("no ui created\n"));
        return -1;
    }
#endif

    QScriptValue ctor = engine.evaluate("Calculator");
    QScriptValue scriptUi = engine.newQObject(ui, QScriptEngine::ScriptOwnership);
    QScriptValue calc = ctor.construct(QScriptValueList() << scriptUi);

    ctor = engine.newFunction(mySpecialQObjectConstructor);
    QScriptValue metaObject = engine.newQMetaObject(&QObject::staticMetaObject, ctor);
    engine.globalObject().setProperty("QObject", metaObject);

    // QScriptValue result = engine.evaluate("new QObject()");
    // printf("xxxxxxxxxxxxxxxxxxxxx\n");


    /////////////////////////////////////////////////
    QScriptEngine engggg;

    engggg.evaluate(str_js);

#if 0
    uifileeeeeeeeee.write(str_ui);
    QUiLoader loaderdddd;
    QWidget *uiiiiiiii = loaderdddd.load(&uifileeeeeeeeee);
    qDebug() << "uifileeeeeeeeee:" << uifileeeeeeeeee.fileName() << endl;
    uifileeeeeeeeee.close();
#else
    QUiLoader loaderdddd;
    QFile uifileeeeeeeeee("/home/auv/work/zuk/installdir/modules/playlist/pl.ui");
    uifileeeeeeeeee.open(QIODevice::ReadOnly);
    QWidget *uiiiiiiii = loaderdddd.load(&uifileeeeeeeeee);
    uifileeeeeeeeee.close();

    if (!uiiiiiiii) {
        klog(("no uiiiiiiii created\n"));
        return -1;
    }
#endif

    QScriptValue ctorsss = engggg.evaluate("Calculator");
    QScriptValue scriptUiiiii = engggg.newQObject(uiiiiiiii, QScriptEngine::ScriptOwnership);
    QScriptValue calcxxxxxxx = ctorsss.construct(QScriptValueList() << scriptUiiiii);

/////////////////////////////////////






    qApp->setStyleSheet(str_theme);

    // ui = new QPushButton("sdfasd");

    // ui->show();
    //ui->setParent(&dlg);

    // ui = playlist_ui(&dlg, mm);
    //      layout->addWidget(ui);
    // layout->addWidget(uiiiiiiii);

    ui = playlist_ui(&dlg, mm);
    layout->addWidget(ui);

    ui = playlist_ui(&dlg, mm);
    layout->addWidget(ui);

    ui = playlist_ui(&dlg, mm);
    layout->addWidget(ui);

    //
    //ui = new QPushButton("sdfasd");

    dlg.setLayout(layout);
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

