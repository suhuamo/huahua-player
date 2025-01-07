#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QDebug>
#define cout qDebug()
#ifdef __cplusplus
extern "C"
{
// 包含ffmpeg头文件
#include "libavformat/avformat.h"
#include "SDL.h"


#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setImage(QImage image);
    void start_work();

public:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
