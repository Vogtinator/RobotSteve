#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T20:25:03
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    CONFIG += c++11
}

lessThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    QMAKE_CXXFLAGS += -mmacoxs-version-min=10.7 -std=c++11 -stdlib=libc++
}

TARGET = RobotSteve
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    world.cpp \
    steveinterpreter.cpp \
    glbox.cpp \
    gldrawable.cpp \
    glquad.cpp \
    glworld.cpp \
    stevehighlighter.cpp \
    examplesdialog.cpp \
    worlddialog.cpp \
    steveedit.cpp \
    stevehelp.cpp \
    helpdialog.cpp

HEADERS  += mainwindow.h \
    world.h \
    steveinterpreter.h \
    gldrawable.h \
    glbox.h \
    glquad.h \
    glworld.h \
    stevehighlighter.h \
    examplesdialog.h \
    worlddialog.h \
    steveedit.h \
    stevehelp.h \
    helpdialog.h

FORMS    += mainwindow.ui \
    examplesdialog.ui \
    worlddialog.ui \
    helpdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    TODO.txt \
    Unterschiede.txt \
    RobotSteve.desktop \
    RobotSteve.spec \
    environment.png \
    char.png \
    RobotSteve.png \
    license.txt \
    help.xml \
    Examples.xml \
    Examples/laufen.steve \
    Examples/laufen.stworld \
    Examples/laufen2.steve \
    Examples/laufen2.stworld \
    Examples/fibonacci.steve \
    Examples/fibonacci.stworld

target.path = /usr/bin
desktop.path = /usr/share/applications
desktop.files += RobotSteve.desktop
icon.path = /usr/share/icons
icon.files += RobotSteve.png
mime.path = /usr/share/mime/packages
mime.files = RobotSteve.xml

INSTALLS += target desktop icon mime
