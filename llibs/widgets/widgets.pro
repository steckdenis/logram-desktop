######################################################################
# Automatically generated by qmake (2.01a) ven. janv. 30 21:27:36 2009
######################################################################

CONFIG += debug
TEMPLATE = lib
TARGET = lwidgets
DEPENDPATH += include
INCLUDEPATH += include ../lio/include ../misc/include
OBJECTS_DIR = ./obj/
MOC_DIR = ./obj/
LIBS += -L../misc -L../lio -llmisc -llio
QT += network

# Input
HEADERS +=	include/LImageButton.h \
		include/LDirView.h \
		include/LDirBar.h \
		include/LDirTree.h \
		include/LDirActions.h

SOURCES +=	Misc/LImageButton.cpp \
		FileManagment/LDirView.cpp \
		FileManagment/LDirBar.cpp \
		FileManagment/LDirTree.cpp \
		FileManagment/LDirActions.cpp

target.path=/usr/lib

includes.files=./include/*.h
includes.path=/usr/include/logram

INSTALLS += target includes
