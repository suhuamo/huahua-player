#-------------------------------------------------
#
# Project created by QtCreator 2024-10-31T22:55:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flower-player
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    playlist.cpp \
    title.cpp \
    globalhelper.cpp \
    ctrlbar.cpp \
    show.cpp

HEADERS  += mainwindow.h \
    playlist.h \
    title.h \
    globalhelper.h \
    ctrlbar.h \
    show.h

FORMS    += mainwindow.ui \
    playlist.ui \
    title.ui \
    ctrlbar.ui \
    show.ui


win32 {
    FFMPEG_PATH = D:\\env\\lib2h\\ffmpeg\\4.2.1
    FFMPEG_LIB_PATH = $$FFMPEG_PATH\\lib\\x86
    SDL_PATH = D:\\env\\lib2h\\SDL2
    SDL_LIB_PATH = $$SDL_PATH\\lib\\x86
}

win64 {
    FFMPEG_PATH = D:\\env\\lib2h\\ffmpeg\\4.2.1
    FFMPEG_LIB_PATH = $$FFMPEG_PATH\\lib\\x64
    SDL_PATH = D:\\env\\lib2h\\SDL2
    SDL_LIB_PATH = $$SDL_PATH\\lib\\x64
}

unix {
    FFMPEG_PATH = /usr/local/ffmpeg/4.2.1
    FFMPEG_LIB_PATH = $$FFMPEG_PATH
    SDL_PATH = /usr/local/SDL2
    SDL_LIB_PATH = $$SDL_PATH
}

# 通用配置
INCLUDEPATH += $$FFMPEG_PATH\\include
LIBS += $$FFMPEG_LIB_PATH\\avformat.lib \
        $$FFMPEG_LIB_PATH\\avcodec.lib \
        $$FFMPEG_LIB_PATH\\avdevice.lib \
        $$FFMPEG_LIB_PATH\\avfilter.lib \
        $$FFMPEG_LIB_PATH\\avutil.lib \
        $$FFMPEG_LIB_PATH\\postproc.lib \
        $$FFMPEG_LIB_PATH\\swresample.lib \
        $$FFMPEG_LIB_PATH\\swscale.lib

INCLUDEPATH += $$SDL_PATH\\include
LIBS += $$SDL_LIB_PATH\\SDL2.lib

RESOURCES += \
    res.qrc

DISTFILES +=
