CONFIG += plugin
TEMPLATE = lib
INCLUDEPATH += ../../include /usr/include/logram
SOURCES += WikiSearch.cpp
HEADERS += WikiSearch.h
TARGET = WikiSearch
RESOURCES += QRessource.qrc
DESTDIR = ../..
DEPENDPATH += .