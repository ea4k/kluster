#-------------------------------------------------
#
# Project created by QtCreator 2015-01-23T13:46:01
#
#-------------------------------------------------

QT += core \
    gui \
    network \
    sql

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}
unix {
    macx {
        TARGET = Kluster
    } else {
        TARGET = kluster
    }
} else {
    TARGET = kluster
}

CONFIG += app_bundle
CONFIG += static

TARGET = kluster
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    setupdialog.cpp \
    dxclustert.cpp \
    database.cpp \
    world.cpp \
    locator.cpp

HEADERS  += mainwindow.h \
    setupdialog.h \
    dxclustert.h \
    database.h \
    world.h \
    locator.h

CODECFORTR = UTF-8
FORMS +=

OTHER_FILES += \
    TODO \
    README \
    NEWS \
    INSTALL.txt \
    INSTALL-win.txt \
    INSTALL-osx.txt \
    INSTALL-linux \
    COPYING \
    Changelog \
    AUTHORS
