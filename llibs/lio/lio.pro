CONFIG += debug
TEMPLATE = lib
TARGET = lio
DEPENDPATH += include
INCLUDEPATH += include ../plugins/lio/include /usr/include/logram ../misc/include
OBJECTS_DIR = ./obj/
MOC_DIR = ./obj/
QT += network
LIBS += -L../misc -llmisc

# Input
HEADERS +=	include/LIOJob.h \
		include/LDirModel.h

SOURCES +=	LIOJob.cpp \
		LDirModel.cpp

target.path=/usr/lib

includes.files=./include/*.h
includes.path=/usr/include/logram

INSTALLS += target includes
