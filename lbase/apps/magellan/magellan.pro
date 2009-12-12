TEMPLATE = app
TARGET = magellan
OBJECTS_DIR = obj/
MOC_DIR = obj/
DEPENDPATH += includes/
INCLUDEPATH += . \
    /usr/include/logram \
    includes/
LIBS += -llmisc \
    -llwidgets \
    -llio

# Input
HEADERS += includes/mwindow.h \
    googlecompleter.h
SOURCES += main.cpp \
    mwindow.cpp \
    preferences.cpp \
    history.cpp \
    navigation.cpp \
    edition.cpp \
    downloads.cpp \
    googlecompleter.cpp
QT += webkit \
    network
target.path = /usr/bin/
config.files = Magellan.conf
config.path = /etc/xdg/Logram/
icons.files = icons
icons.path = /usr/share/logram/magellan/
INSTALLS += target \
    config \
    icons
