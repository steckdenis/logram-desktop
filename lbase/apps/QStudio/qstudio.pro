# -------------------------------------------------
# Project created by QtCreator 2008-11-23T12:37:49
# -------------------------------------------------
TARGET = qstudio
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/wizard.cpp \
    src/compilateur.cpp \
    src/seek.cpp \
    src/addtext.cpp
HEADERS += src/mainwindow.h \
    src/wizard.h \
    src/compilateur.h \
    src/seek.h \
    src/addtext.h \
    src/plugininterface.h
FORMS += ui/mainwindow.ui \
    ui/wizard.ui \
    ui/seek.ui \
    ui/addtext.ui
LIBS += -lqscintilla2
RESOURCES += ressources.qrc
TRANSLATIONS = traduction/qstudio_en.ts
QT += xml
