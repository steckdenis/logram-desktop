TEMPLATE = app
TARGET = panache-desktop
OBJECTS_DIR = ./obj/
MOC_DIR = ./obj/
DEPENDPATH += include
INCLUDEPATH += include /usr/include/logram
LIBS += -llwindowsystem -llmisc -llwidgets -llio
QMAKE_CXXFLAGS_DEBUG += -Wl,--export-dynamic
QMAKE_LFLAGS += -Wl,--export-dynamic
QT += dbus

# Input
HEADERS +=	include/App.h \
		include/DesktopPluginContainer.h \
		include/DesktopManager.h \
		include/TabsManager.h \
		include/TabPluginContainer.h \
		include/SubTab.h \
		include/MenuManager.h \
		include/StartMenu.h \
		include/TaskBar.h

SOURCES += 	App.cpp \
		main.cpp \
		DesktopManager.cpp \
		DesktopPluginContainer.cpp \
		TabsManager.cpp \
		TabPluginContainer.cpp \
		SubTab.cpp \
		MenuManager.cpp \
		StartMenu.cpp \
		TaskBar.cpp

target.path=/usr/bin

include.files=includes/IPanache*Plugin.h
include.path=/usr/include/logram

INSTALLS += target include
