#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"ffplay.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 开启媒体播放，后续应该在选择文件的时候初始化
    FFPlay::GetInstance()->start_work();
    // 传递界面显示的图片，后续使用队列
    connect(FFPlay::GetInstance(), &FFPlay::putImage, ui->showWidget, &DisplayWidget::setImage);
}

MainWindow::~MainWindow()
{
    delete ui;
}
