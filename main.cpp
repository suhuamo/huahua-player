#include "mainwindow.h"
#include <QApplication>
#include<QFontDatabase>
#include"show.h"

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    使用第三方字库来作为UI图片
    QFontDatabase::addApplicationFont(":/res/fontawesome-webfont.ttf");

    MainWindow w;
    if(w.Init() == false) {
        return -1;
    }
    w.show();

    Show s;
    s.Init();
    s.show();

    return a.exec();
}
