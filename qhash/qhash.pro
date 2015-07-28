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
    hasherthread.cpp

HEADERS  += mainwindow.h \
    hasherthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    qhash.qrc
