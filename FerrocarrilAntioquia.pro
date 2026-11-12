TEMPLATE = app
TARGET = FerrocarrilAntioquia

QT += core gui widgets multimedia

CONFIG += c++17
CONFIG -= console
CONFIG += app_bundle

SOURCES += \
    main.cpp \
    logica/Vegeta.cpp \
    gui/SpriteItem.cpp \
    gui/EscenaNivel1.cpp

HEADERS += \
    logica/EntidadLogica.h \
    logica/Vegeta.h \
    logica/ObstaculosNivel1.h \
    logica/ProyectilKi.h \
    gui/SpriteItem.h \
    gui/EscenaNivel1.h