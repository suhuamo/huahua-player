#ifndef FFPLAY_H
#define FFPLAY_H

#include <QObject>
#include<QDebug>
#define cout qDebug()
#ifdef __cplusplus
extern "C"
{
// 包含ffmpeg头文件
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "SDL.h"
}
#endif
#include"mainwindow.h"

// 媒体播放类，用于操控播放视频和音频
class FFPlay : public QObject
{
    Q_OBJECT
public:
    // 单例模式
    static FFPlay *GetInstance();
    // explicit FFPlay(QObject *parent = nullptr);
    // 开始媒体播放
    void start_work(MainWindow &mainWindow);
private:
    explicit FFPlay(QObject *parent = nullptr);

signals:
};

#endif // FFPLAY_H
