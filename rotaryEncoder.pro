#-------------------------------------------------
#
# Project created by QtCreator 2016-07-10T21:39:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rotaryEncoder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rotaryEncoder.cpp \
    gpiocontrol.cpp

HEADERS  += mainwindow.h \
    rotaryEncoder.h \
    gpiocontrol.h

FORMS    += mainwindow.ui

LIBS += -L/usr/local/lib -lwiringPi
