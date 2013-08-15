#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T20:25:03
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RobotSteve
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    glview.cpp \
    world.cpp \
    steveinterpreter.cpp

HEADERS  += mainwindow.h \
    glview.h \
    world.h \
    steveinterpreter.h

FORMS    += mainwindow.ui
