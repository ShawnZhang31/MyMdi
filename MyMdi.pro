#-------------------------------------------------
#
# Project created by QtCreator 2017-07-20T01:41:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyMdi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp

HEADERS  += mainwindow.h \
    mdichild.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    myimage.qrc
