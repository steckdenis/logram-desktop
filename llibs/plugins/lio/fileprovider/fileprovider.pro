CONFIG += plugin
TEMPLATE = lib
INCLUDEPATH += ../include /usr/include/logram
SOURCES += fileprovider.cpp
HEADERS += fileprovider.h
TARGET = liofileprovider
DESTDIR = ..
DEPENDPATH += .
QT += network