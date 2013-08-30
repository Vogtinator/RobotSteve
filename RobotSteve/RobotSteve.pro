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
    world.cpp \
    steveinterpreter.cpp \
    glbox.cpp \
    gldrawable.cpp \
    glquad.cpp \
    glworld.cpp

HEADERS  += mainwindow.h \
    world.h \
    steveinterpreter.h \
    gldrawable.h \
    glbox.h \
    glquad.h \
    glworld.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    TODO.txt
