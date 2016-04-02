#-------------------------------------------------
#
# Project created by QtCreator 2014-11-21T23:24:48
#
#-------------------------------------------------

QT       += core opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGLQT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mygl.cpp \
    object3d.cpp \
    reader3d.cpp \
    neighbour3d.cpp \
    laplacianthing.cpp \
    gllightspos.cpp \
    laplaciankind.cpp \
    material.cpp \
    plotter.cpp

HEADERS  += mainwindow.h \
    mygl.h \
    object3d.h \
    reader3d.h \
    neighbour3d.h \
    laplacianthing.h \
    gllightspos.h \
    laplaciankind.h \
    material.h \
    plotter.h

FORMS    += mainwindow.ui \
    gllightspos.ui \
    laplaciankind.ui \
    material.ui \
    plotter.ui

RESOURCES += \
    images.qrc

