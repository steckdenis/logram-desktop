TEMPLATE = app
TARGET = ghetto
OBJECTS_DIR = ./obj/
MOC_DIR = ./obj/
DEPENDPATH += includes
INCLUDEPATH += includes /usr/include/logram

## SOURCES ##
HEADERS += includes/mwindow.h

SOURCES += main.cpp \
	   mwindow.cpp \
	   build.cpp

QT += phonon

target.path=/usr/bin

desktop.files=Ghetto.desktop
desktop.path=/usr/share/applications

icons.files = icons/
icons.path = /usr/share/logram/ghetto_blaster/

config.files = Ghetto_blaster.conf
config.path = ~/.config/Logram/

INSTALLS += target desktop config icons
