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
    show.cpp \
    medialist.cpp \
    customslider.cpp

HEADERS  += mainwindow.h \
    playlist.h \
    title.h \
    globalhelper.h \
    ctrlbar.h \
    show.h \
    medialist.h \
    customslider.h

FORMS    += mainwindow.ui \
    playlist.ui \
    title.ui \
    ctrlbar.ui \
    show.ui


win32 {
    # 根据编译器位数设置架构
    win32-g++ {
        contains(QMAKE_TARGET.arch, x86_64) {
            ARCH = x64
        } else {
            ARCH = x86
        }
    } else:win32-msvc* {
        contains(QMAKE_HOST.arch, x86_64) {
            ARCH = x64
        } else {
            ARCH = x86
        }
    }

    message("Target architecture: $$ARCH")

    LIBS += -L$$PWD/lib/SDL2/lib/$$ARCH \
        -L$$PWD/lib/ffmpeg-4.2.1/lib/$$ARCH \
        -L$$PWD/lib/windows-kits/lib/$$ARCH \
        -lSDL2 \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale \
        -lOle32

    INCLUDEPATH += lib/SDL2/include \
        lib/ffmpeg-4.2.1/include
}

win32 {
    # 指定要拷贝的DLL文件目录（根据架构）
    DllSourceDir = $${PWD}/dll/$$ARCH
    # 将输入目录中的"/"替换为"\"
    DllSourceDir = $$replace(DllSourceDir, /, \\)

    # Debug模式：exe在debug子目录下
    CONFIG(debug, debug|release) {
        OutputDir = $${OUT_PWD}/debug
    } else {
        # Release模式：exe在release子目录下
        OutputDir = $${OUT_PWD}/release
    }
    # 将输出目录中的"/"替换为"\"
    OutputDir = $$replace(OutputDir, /, \\)

    # 执行copy命令，复制所有DLL文件到exe所在目录
    QMAKE_POST_LINK += copy /Y \"$$DllSourceDir\*.dll\" \"$$OutputDir\"
}



###cmd install lib
#sudo apt-get install ffmpeg
#sudo apt-get install libavformat-dev
#sudo apt-get install libavutil-dev
#sudo apt-get install libavcodec-dev
#sudo apt-get install libswscale-dev
#sudo apt-get install libsdl2-dev
###
unix {
LIBS += \
    -lSDL2 \
    -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lswresample \
    -lswscale
}

mac {

}

RESOURCES += \
    res.qrc

DISTFILES +=
