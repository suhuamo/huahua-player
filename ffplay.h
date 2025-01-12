#ifndef FFPLAY_H
#define FFPLAY_H

#include <QObject>
#include<QImage>
#include<QThread>
#include<thread>
#include<QListWidget>

// 媒体播放类，用于操控播放视频和音频
class FFPlay : public QObject
{
    Q_OBJECT
public:
    // 单例模式
    static FFPlay *GetInstance();
    // 线程工作
    void thread_work();
public slots:
    void getPlayUrl(const QModelIndex &index);
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

signals:
};

#endif // FFPLAY_H
