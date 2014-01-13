#-------------------------------------------------
#
# Project created by QtCreator 2011-02-22T22:03:51
#
#-------------------------------------------------
#include(packages/packages.pri)

QT       += core gui webkit network xml

TARGET = Lightweight2
TEMPLATE = app

SOURCES += src/main.cpp\
        src/lightweight.cpp \
    src/browsertab.cpp \
    src/bookmark.cpp \
    src/webview.cpp \
    src/dialogs/passworddialog.cpp \
    src/download.cpp \
    src/downloadlist.cpp \
    src/searchbar.cpp \
    src/bookmarkfolder.cpp \
    src/dialogs/bookmarkmanager.cpp \
    src/dialogs/bookmarkbutton.cpp \
    src/dialogs/bookmarkfolderbutton.cpp \
    src/cookiejar.cpp \
    src/pluginfactory.cpp \
    src/enableplugin.cpp \
    src/dialogs/pluginmanager.cpp \
    src/dialogs/cookiemanager.cpp \
    src/dialogs/askdownload.cpp \
    src/bookmarkitem.cpp \
    src/dialogs/bookmarkitembutton.cpp

HEADERS  += src/lightweight.h \
    src/browsertab.h \
    src/bookmark.h \
    src/webview.h \
    src/dialogs/passworddialog.h \
    src/download.h \
    src/downloadlist.h \
    src/searchbar.h \
    src/bookmarkfolder.h \
    src/dialogs/bookmarkmanager.h \
    src/dialogs/bookmarkbutton.h \
    src/dialogs/bookmarkfolderbutton.h \
    src/cookiejar.h \
    src/pluginfactory.h \
    src/enableplugin.h \
    src/dialogs/pluginmanager.h \
    src/dialogs/cookiemanager.h \
    src/dialogs/askdownload.h \
    src/bookmarkitem.h \
    src/dialogs/bookmarkitembutton.h

FORMS    += src/lightweight.ui \
    src/dialogs/passworddialog.ui \
    src/download.ui \
    src/downloadlist.ui \
    src/searchbar.ui \
    src/dialogs/bookmarkmanager.ui \
    src/dialogs/bookmarkitembutton.ui \
    src/enableplugin.ui \
    src/dialogs/pluginmanager.ui \
    src/dialogs/cookiemanager.ui \
    src/dialogs/askdownload.ui

RESOURCES += \
    ressources.qrc

TRANSLATIONS += ts/Lightweight2_de.ts \
    ts/Lightweight2_es.ts \
    ts/Lightweight2_el.ts

os2: RC_FILE  = Lightweight2_os2.rc

win32 {
	RC_FILE  = Lightweight2_win.rc
	HEADERS += src/os/qtwin.h
	SOURCES += src/os/qtwin.cpp
}

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR = $$PREFIX/bin

    DATADIR = $$PREFIX/share
    PKGDATADIR = $$DATADIR/lightweight2
    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
}
