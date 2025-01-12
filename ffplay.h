#ifndef FFPLAY_H
#define FFPLAY_H

#include <QObject>
#include<QImage>
#include<QThread>
#include<thread>

// 媒体播放类，用于操控播放视频和音频
class FFPlay : public QObject
{
    Q_OBJECT
public:
    // 单例模式
    static FFPlay *GetInstance();
    // 开始媒体播放
    void start_work();
    // 线程工作
    void thread_work();
signals:
    // 发送图片给画面显示
    void putImage(QImage &image);
    void setPlaySliderValue(int value);
    void setPlaySliderMaximum(int value);
    void setPlayTimeEdit(const QTime& time);
    void setTotalTimeEdit(const QTime& time);
private:
    explicit FFPlay(QObject *parent = nullptr);
    std::thread m_tPlayLoopThread;

signals:
};

#endif // FFPLAY_H
