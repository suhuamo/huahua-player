#include "mainwindow.h"
#include <QApplication>


#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.start_work();
    return a.exec();
}
