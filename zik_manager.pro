TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    libParrotZik

sailfish {
    message(SailfishOS build)
    SUBDIRS += SailfishOS/harbour-zik2-manager.pro
}else{
    message(Desktop build)
    SUBDIRS += Desktop
}
