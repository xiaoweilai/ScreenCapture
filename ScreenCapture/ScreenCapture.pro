#-------------------------------------------------
#
# Project created by QtCreator 2014-05-30T13:12:20
#
#-------------------------------------------------

QT       += core

TARGET = ScreenCapture
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    capthread.cpp

#ffmpeg and SDL header dir
INCLUDEPATH += ./ffmpeg-20150316-git-1e4d049-win32-dev/include

#ffmpeg lib
LIBS += -L./ffmpeg-20150316-git-1e4d049-win32-dev/lib -lavcodec.dll -lavdevice.dll -lavfilter.dll -lavformat.dll  -lavutil.dll -lpostproc.dll -lswresample.dll -lswscale.dll


#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/avcodec.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/avformat.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/avdevice.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/avfilter.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/avutil.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/postproc.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/swresample.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/swscale.lib

#SDL libs
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/SDL.lib
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/SDLmain.lib

LIBS += -L./ffmpeg-20150316-git-1e4d049-win32-dev/lib -lSDL.dll -lSDLmain
#LIBS += ./ffmpeg-20150316-git-1e4d049-win32-dev/lib/SDL.dll

HEADERS += \
    capthread.h
