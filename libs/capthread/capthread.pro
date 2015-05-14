#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T22:20:55
#
#-------------------------------------------------

QT       += network

TARGET = capthread
TEMPLATE = lib

DEFINES += CAPTHREAD_LIBRARY

SOURCES += capthread.cpp

HEADERS += capthread.h\
        capthread_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE12598AF
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = capthread.dll
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
