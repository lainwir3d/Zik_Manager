TEMPLATE = app

TARGET = zik-manager

ICON = ../icons/harbour-zik-manager.icns

QT += core qml quick bluetooth svg gui

unix:!android {
    QT += dbus
}

android {
    QT += androidextras
}

SOURCES += src/main.cpp \
    src/systraymanager.cpp

HEADERS += src/systraymanager.h

RESOURCES += \
    ../qml/common_ui.qrc \
    qml/ui.qrc
    
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

QMAKE_CXXFLAGS += -std=c++0x

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

# debug config
CONFIG(debug, debug|release) {
    message(debug build)
    DEFINES += DEBUG
}
else {
    message(release build)
    DEFINES += QT_NO_DEBUG_OUTPUT
}
