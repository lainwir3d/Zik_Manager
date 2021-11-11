# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-zik2-manager

CONFIG += sailfishapp

QT += bluetooth dbus

SOURCES += \
    src/harbour-zik2-manager.cpp

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages_zik2/FirstPage.qml \
    qml/pages_zik2/Settings.qml \
    qml/pages_zik2/LoadingPage.qml \
    qml/pages_zik2/ConcertHallPage.qml \
    qml/pages_zik2/EqualizerPage.qml \
    qml/pages_zik2/PresetPage.qml \
    qml/pages_zik2/NoiseControlPage.qml \
    qml/pages_zik2/QuickPresetPage.qml \
    qml/pages_zik2/AboutPage.qml \
    lib/libQt5Bluetooth.so.5.2.0 \
    harbour-zik2-manager.desktop \
    ../rpm/harbour-zik2-manager.spec \
    ../rpm/harbour-zik2-manager.yaml \
    harbour-zik2-manager.png \
    qml/harbour-zik2-manager.qml \
    qml/LoadingPage.qml \
    qml/pages_zik2/WideLandscape.qml \
    icons/86x86/apps/harbour-zik2-manager.png \
    icons/108x108/apps/harbour-zik2-manager.png \
    icons/128x128/apps/harbour-zik2-manager.png \
    icons/256x256/apps/harbour-zik2-manager.png


RESOURCES += \
    ../qml/common_ui.qrc
#    qml/sailfish_ui.qrc



# libParrotZik config
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libParrotZik/release/ -lParrotZik
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libParrotZik/debug/ -lParrotZik
else:unix: LIBS += -L$$OUT_PWD/../libParrotZik/ -lParrotZik

INCLUDEPATH += $$PWD/../libParrotZik/include
DEPENDPATH += $$PWD/../libParrotZik

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libParrotZik/release/libParrotZik.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libParrotZik/debug/libParrotZik.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libParrotZik/release/ParrotZik.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libParrotZik/debug/ParrotZik.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libParrotZik/libParrotZik.a

# Include locally shipped QtBluetooth
lib.files = lib
lib.path = /usr/share/$${TARGET}

# Include locally shipped QtBluetooth sdpscanner
bin.files = bin
bin.path = /usr/share/$${TARGET}

appicons.path = /usr/share/icons/hicolor
appicons.files = icons/*

INSTALLS += lib appicons
#INSTALLS += lib bin appicons

CONFIG(debug, debug|release) {
    DEFINES += DEBUG
}
else {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

DEFINES += QTBLUETOOTH_BACKPORT

#unix:!macx: LIBS += -L$$PWD/../../build-qtconnectivity-MerSDK_SailfishOS_armv7hl-Release/lib/ -lQt5Bluetooth

#INCLUDEPATH += $$PWD/../../build-qtconnectivity-MerSDK_SailfishOS_armv7hl-Release/include
#DEPENDPATH += $$PWD/../../build-qtconnectivity-MerSDK_SailfishOS_armv7hl-Release/include


