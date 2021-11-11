#ifndef SYSTRAYMANAGER_H
#define SYSTRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QQuickView>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <parrotziktweaker.h>

class SysTrayManager : public QObject
{
    Q_OBJECT
public:
    explicit SysTrayManager(ParrotZikTweeker * z, QQuickView * mainview, QObject *parent = 0);

signals:

public slots:
    void repaintTray();

private slots:
    void activatedCallback(QSystemTrayIcon::ActivationReason reason);
    void quitActionCallback();
    void switchToAncActionCallback();
    void switchToAocActionCallback();
    void switchOffNcActionCallback();
    void concertHallActionCallback();
    void equalizerActionCallback();
    void determineVisibleActions();

private:
    QSystemTrayIcon * systray;

    QQuickView * view;
    QPoint position;

    ParrotZikTweeker * zik;

    QMenu * menu;
    QMenu * ncSubMenu;
    QAction * setAncAction;
    QAction * switchOffNcAction;
    QAction * setAocAction;

    QAction * concertHallAction;
    QAction * equalizerAction;

#if defined(Q_OS_OSX)
    QMenu * osxDockMenu;
    QMenu * osxDockNcSubMenu;
#endif

};

#endif // SYSTRAYMANAGER_H
