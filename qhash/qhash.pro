#-------------------------------------------------
#
# Project created by QtCreator 2015-07-27T13:27:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qhash
TEMPLATE = app

# make app as exec not shared library
QMAKE_LFLAGS += -no-pie

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/hasherthread.cpp \
    src/progressbardelegate.cpp

HEADERS  += src/mainwindow.h \
    src/hasherthread.h \
    src/progressbardelegate.h \
    src/customRoles.h

FORMS    += src/mainwindow.ui \
    src/options.ui \
    src/tools.ui

RESOURCES += \
    src/qhash.qrc

OTHER_FILES += \
    qhash-action.desktop

target.path = /usr/bin/
actions.files = qhash-action.desktop
desktop.path = /usr/share/applications/
desktop.files += qhash.desktop
images.path = /usr/share/pixmaps/
images.files += pixmaps/qhash.png

INSTALLS += target desktop images
