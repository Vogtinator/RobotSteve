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
    glworld.cpp \
    stevehighlighter.cpp \
    examplesdialog.cpp

HEADERS  += mainwindow.h \
    world.h \
    steveinterpreter.h \
    gldrawable.h \
    glbox.h \
    glquad.h \
    glworld.h \
    stevehighlighter.h \
    examplesdialog.h

FORMS    += mainwindow.ui \
    examplesdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    TODO.txt \
    Unterschiede.txt \
    Beispiele.txt \
    RobotSteve.desktop \
    RobotSteve.spec \
    environment.png \
    char.png \
    RobotSteve.png \
    license.txt \
    Examples.xml \
    Examples/laufen.steve \
    Examples/laufen.stworld \
    Examples/laufen2.steve \
    Examples/laufen2.stworld

target.path = /usr/bin
desktop.path = /usr/share/applications
desktop.files += RobotSteve.desktop
icon.path = /usr/share/icons
icon.files += RobotSteve.png
mime.path = /usr/share/mime/packages
mime.files = RobotSteve.xml

INSTALLS += target desktop icon mime
