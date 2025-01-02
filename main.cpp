#include "mainwindow.h"
#include <QApplication>
#ifdef __cplusplus
extern "C"
{
// 包含ffmpeg头文件
#include "libavutil/avutil.h"
}
#endif
#include<QDebug>

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qDebug() << av_version_info();
    return a.exec();
}
