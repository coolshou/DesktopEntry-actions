#-------------------------------------------------
#
# Project created by QtCreator 2015-07-27T13:27:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qhash
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hasherthread.cpp \
    progressbardelegate.cpp

HEADERS  += mainwindow.h \
    hasherthread.h \
    progressbardelegate.h \
    customRoles.h

FORMS    += mainwindow.ui \
    options.ui

RESOURCES += \
    qhash.qrc

OTHER_FILES +=
