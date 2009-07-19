CONFIG += debug
TEMPLATE = app
TARGET = startlogram
OBJECTS_DIR = ./obj/
MOC_DIR = ./obj/
DEPENDPATH += include
INCLUDEPATH += include /usr/include/logram
LIBS += -llwindowsystem -llmisc

# Input
HEADERS +=	include/App.h \
		include/Screen.h

SOURCES += 	App.cpp \
		Screen.cpp \
		main.cpp

target.path=/usr/bin

defconf.files=RunSession.conf
defconf.path=/etc/xdg/Logram/

INSTALLS += target defconf
