#include "systraymanager.h"

//#define PATH_DRAWING_METHOD 1

#if defined(Q_OS_OSX)
    void qt_mac_set_dock_menu(QMenu *menu);
#endif

SysTrayManager::SysTrayManager(ParrotZikTweeker * z, QQuickView * mainview, QObject *parent) : QObject(parent)
{
    view = mainview;
    zik = z;

    QPixmap pixmap = QPixmap(":/icon.png");
    systray = new QSystemTrayIcon(QIcon(pixmap));
    repaintTray();

    QObject::connect(systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activatedCallback(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu();

    concertHallAction = menu->addAction("Concert Hall", this, SLOT(concertHallActionCallback()));
    concertHallAction->setCheckable(true);

    equalizerAction = menu->addAction("Equalizer", this, SLOT(equalizerActionCallback()));
    equalizerAction->setCheckable(true);

    ncSubMenu = menu->addMenu("Noise control");
    setAncAction = ncSubMenu->addAction("Noise cancellation", this, SLOT(switchToAncActionCallback()));
    switchOffNcAction = ncSubMenu->addAction("Off", this, SLOT(switchOffNcActionCallback()));
    setAocAction = ncSubMenu->addAction("Street mode", this, SLOT(switchToAocActionCallback()));
    menu->addSeparator();
    menu->addAction("Quit", this, SLOT(quitActionCallback()));


    systray->setContextMenu(menu);
    determineVisibleActions();
    systray->show();

#if defined(Q_OS_OSX)
    osxDockMenu = new QMenu();
    osxDockMenu->addAction(concertHallAction);
    osxDockMenu->addAction(equalizerAction);

    osxDockNcSubMenu = osxDockMenu->addMenu("Noise control");
    osxDockNcSubMenu->addAction(setAncAction);
    osxDockNcSubMenu->addAction(switchOffNcAction);
    osxDockNcSubMenu->addAction(setAocAction);

    qt_mac_set_dock_menu(osxDockMenu);
#endif

    connect(zik, SIGNAL(connected()), this, SLOT(repaintTray()));
    connect(zik, SIGNAL(disconnected()), this, SLOT(repaintTray()));
    connect(zik, SIGNAL(batteryChanged()), this, SLOT(repaintTray()));

    connect(zik, SIGNAL(ncTypeChanged()), this, SLOT(determineVisibleActions()));
    connect(zik, SIGNAL(ncEnabledChanged()), this, SLOT(determineVisibleActions()));
    connect(zik, SIGNAL(ancChanged()), this, SLOT(determineVisibleActions()));

    connect(zik, SIGNAL(equalizerChanged()), this, SLOT(determineVisibleActions()));
    connect(zik, SIGNAL(concertHallChanged()), this, SLOT(determineVisibleActions()));

    connect(zik, SIGNAL(connected()), this, SLOT(determineVisibleActions()));
    connect(zik, SIGNAL(disconnected()), this, SLOT(determineVisibleActions()));
}

void SysTrayManager::repaintTray()
{
    QPixmap pixmap = QPixmap(":/icon-tray-300x300.png");

    QPainter painter( &pixmap );
    QPainterPath path;
    QPen pen;
    QFont font = painter.font();

    font.setBold(true);
    painter.setFont(font);

    QString text;
    if(zik->isConnected()){
        text = (zik->battery() != "-1") ? zik->battery() : "?";
    }else{
        text = "N/A";
    }

    float factor = (pixmap.width()-2) / painter.fontMetrics().width((text.length() == 1) ? "00" : text);
    font.setPointSizeF(font.pointSizeF()*factor);

    painter.setFont(font);
    painter.setPen(pen);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

#if !defined(PATH_DRAWING_METHOD)
    pen.setColor(Qt::black);
    painter.drawText( QRect(0,0,pixmap.width(),pixmap.height()), Qt::AlignCenter, text );
#else
    path.addText((pixmap.width() - painter.fontMetrics().width(text)) / 2, (pixmap.height() / 2) + (painter.fontMetrics().height() / 4), font, text); //Adjust the position

    painter.fillPath(path, QBrush(Qt::black));

    //pen.setWidth(4);
    //pen.setColor(Qt::white);
    //painter.setPen(pen);
    //painter.drawPath(path);
#endif


    systray->setIcon(QIcon(pixmap));
}

void SysTrayManager::activatedCallback(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::Unknown){

#if !defined(Q_OS_OSX)
        if(view->isVisible()){
            position = view->framePosition();
            view->hide();
        }else{
            view->show();
            view->setFramePosition(position);
        }
#endif

    }else if(reason == QSystemTrayIcon::Context){
        qDebug("Context");
    }else if(reason == QSystemTrayIcon::DoubleClick){
        qDebug("Double click");
    }else if(reason == QSystemTrayIcon::MiddleClick){
         qDebug("Middle Click");
    }
}

void SysTrayManager::quitActionCallback()
{
    if(view->isVisible()){
        view->close();
    }else{
        view->show();
        view->close();
    }
}

void SysTrayManager::switchToAncActionCallback()
{
    if(zik->capabilities() & ParrotZikTweeker::StreetMode){
        zik->setNcType("anc");
    }else{
        zik->setAnc("true");
    }
}

void SysTrayManager::switchOffNcActionCallback()
{
    if(zik->capabilities() & ParrotZikTweeker::StreetMode){
        zik->setNcEnabled("false");
    }else{
        zik->setAnc("false");
    }
}

void SysTrayManager::switchToAocActionCallback()
{
    zik->setNcType("aoc");
}

void SysTrayManager::concertHallActionCallback()
{
    if(zik->concertHall() == "false"){
        zik->setConcertHall("true");
    }else{
        zik->setConcertHall("false");
    }
}

void SysTrayManager::equalizerActionCallback()
{
    if(zik->equalizer() == "false"){
        zik->setEqualizer("true");
    }else{
        zik->setEqualizer("false");
    }
}

void SysTrayManager::determineVisibleActions()
{
    if(zik->isConnected()){
        if(zik->capabilities() & ParrotZikTweeker::StreetMode){
              if(zik->ncEnabled() == "false"){
                  setAncAction->setDisabled(false);
                  switchOffNcAction->setDisabled(true);
                  setAocAction->setDisabled(false);
              }else if(zik->ncType() == "anc"){
                  setAncAction->setDisabled(true);
                  switchOffNcAction->setDisabled(false);
                  setAocAction->setDisabled(false);
              }else if(zik->ncType() == "aoc"){
                  setAncAction->setDisabled(false);
                  switchOffNcAction->setDisabled(false);
                  setAocAction->setDisabled(true);
              }
        }else if(zik->capabilities() & ParrotZikTweeker::Anc){
              if(zik->anc() == "false"){
                  setAncAction->setDisabled(false);
                  switchOffNcAction->setDisabled(true);
                  setAocAction->setDisabled(true);
              }else{
                  setAncAction->setDisabled(true);
                  switchOffNcAction->setDisabled(false);
                  setAocAction->setDisabled(true);
              }
        }else{
            setAncAction->setDisabled(true);
            switchOffNcAction->setDisabled(true);
            setAocAction->setDisabled(true);
        }

        if(zik->capabilities() & ParrotZikTweeker::ConcertHall){
            concertHallAction->setEnabled(true);
            concertHallAction->setChecked((zik->concertHall() == "true") ? true : false);
        }

        if(zik->capabilities() & ParrotZikTweeker::BasicSettings){
            equalizerAction->setEnabled(true);
            equalizerAction->setChecked((zik->equalizer() == "true") ? true : false);
        }

    }else{
        setAncAction->setDisabled(true);
        setAocAction->setDisabled(true);
        switchOffNcAction->setDisabled(true);


        concertHallAction->setEnabled(false);
        equalizerAction->setEnabled(false);
    }

#if defined(Q_OS_LINUX)
    //systray->hide();
    //systray->show();
#endif

}
