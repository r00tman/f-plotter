#-------------------------------------------------
#
# Project created by QtCreator 2014-04-29T08:46:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plotter1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    plotwidget.cpp \
    coordinatesystem.cpp

HEADERS  += widget.h \
    plotwidget.h \
    coordinatesystem.h \
    common.h

QMAKE_CXXFLAGS += -std=c++11
