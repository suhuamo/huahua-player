#include "mainwindow.h"
#include <QApplication>
#include"ffplay.h"

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    FFPlay::GetInstance()->start_work(w);

    return a.exec();
}
