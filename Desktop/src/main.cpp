#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickView>
#include <QObject>
#include <QSystemTrayIcon>
#include <parrotziktweaker.h>
#include "systraymanager.h"

//#define DEBUG_VIEW

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));

    ParrotZikTweeker::declareQML();

    ParrotZikTweeker * tweaker;
    tweaker = new ParrotZikTweeker();
    //tweaker = new ParrotZikTweeker("a0:14:3d:6c:2a:3f");

    /*QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("zik",  tweaker);
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    */

    QQuickView view;
    view.setTitle("Zik Manager");

#if defined(DEBUG_VIEW)
    QQuickView debugview;
    debugview.setTitle("Zik Manager - debug");
#endif

    QPixmap pixmap = QPixmap(":/icon.png");
    view.setIcon(QIcon(pixmap));

   /*
    QSurfaceFormat format;
    format.setSamples(16);
    view.setFormat(format);
    */

    view.rootContext()->setContextProperty("zik",  tweaker);
    view.rootContext()->setContextProperty("presetModel",  tweaker->presetModel);

#if defined(DEBUG_VIEW)
    debugview.rootContext()->setContextProperty("zik",  tweaker);
    debugview.rootContext()->setContextProperty("presetModel",  tweaker->presetModel);
    debugview.setSource(QUrl(QStringLiteral("qrc:///debugmain.qml")));
#endif

    view.setSource(QUrl(QStringLiteral("qrc:///main.qml")));

    QObject::connect(&view, SIGNAL(visibleChanged(bool)), tweaker, SLOT(setApplicationVisible(bool)));
	QObject::connect(&view, SIGNAL(closing(QQuickCloseEvent*)), tweaker, SLOT(closeBTConnectionsBeforeQuit()));

#ifdef Q_OS_ANDROID
    view.show();
#else
    view.setWidth(275);
    view.setHeight(418);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

#if defined(DEBUG_VIEW)
    debugview.setWidth(275);
    debugview.setHeight(418);
    debugview.setResizeMode(QQuickView::SizeRootObjectToView);
    debugview.show();
#endif

    SysTrayManager * traymanager = new SysTrayManager(tweaker, &view);
    tweaker->setAlwaysVisible(true);

#endif

    return app.exec();
}
