#ifndef FFPLAY_H
#define FFPLAY_H

#include <QObject>
#include<QImage>
#include<QThread>
#include<thread>
#include<QListWidget>
#include"state.h"
#include <thread>
#include<QThread>
#include <QtWidgets/QTimeEdit>
#include"qvariant.h"
#include <QMutex>

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

// 媒体播放类，用于操控播放视频和音频
class FFPlay : public QObject
{
    Q_OBJECT
public:
    // 单例模式
    static FFPlay *GetInstance();
    // 线程工作
    void thread_work(QString in_file_url);
    // 停止播放
    void stop_play();
public slots:
    // 更新播放的文件
    void updatePlayUrl(const QModelIndex &index);
signals:
    // 发送图片给画面显示
    void putImage(QImage &image);
    void setPlaySliderValue(int value);
    void setPlaySliderMaximum(int value);
    void setPlayTimeEdit(const QTime& time);
    void setTotalTimeEdit(const QTime& time);
private:
    explicit FFPlay(QObject *parent = nullptr);
    // 播放循环线程
    std::thread playLoopThread;
    // 文件名称
    const char* file_url;
    // 中止状态
    bool abort_ = false;
    // 锁
    QMutex m_mutex;

signals:
};

#endif // FFPLAY_H
