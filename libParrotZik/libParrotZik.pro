#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T12:29:05
#
#-------------------------------------------------

#QT += core bluetooth charts
QT += core bluetooth

unix:!android:!winrt {
    QT += dbus
}

android {
    QT += androidextras
}

TARGET = ParrotZik
TEMPLATE = lib
CONFIG += staticlib

SOURCES += src/gain.cpp \
    src/presetmodel.cpp \
    src/parrotziktweaker.cpp \
    src/parrotziktweaker_presets.cpp \
    src/parrotziktweaker_firmwareupdate.cpp \
    src/parrotziktweaker_generalsettings.cpp \

#winrt {
#    SOURCES +=
#}

HEADERS += include/gain.h \
    include/presetmodel.h \
    include/parrotziktweaker.h

INCLUDEPATH += include

QMAKE_CXXFLAGS += -std=c++0x

# debug config
CONFIG(debug, debug|release) {
    message(debug build)
    DEFINES += DEBUG
}
else {
    message(release build)
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# NO INSTALL
#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
