TEMPLATE = app
TARGET = magellan
OBJECTS_DIR = obj/
MOC_DIR = obj/
DEPENDPATH += includes/
INCLUDEPATH += . /usr/include/logram includes/
LIBS += -llmisc -llwidgets -llio

# Input
HEADERS += includes/mwindow.h 
SOURCES += main.cpp mwindow.cpp preferences.cpp history.cpp
QT+=webkit

target.path=/usr/bin/

config.files=Magellan.conf
config.path=~/.config/Logram/

icons.files=icons
icons.path=/usr/share/logram/magellan/

INSTALLS += target config icons
