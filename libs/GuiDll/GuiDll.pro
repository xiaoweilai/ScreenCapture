#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T22:15:59
#
#-------------------------------------------------

QT       += network

TARGET = GuiDll
TEMPLATE = lib

DEFINES += GUIDLL_LIBRARY

SOURCES += guidll.cpp

HEADERS += guidll.h\
        GuiDll_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE62C63D0
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = GuiDll.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
