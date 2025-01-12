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
    connect(FFPlay::GetInstance(), &FFPlay::setPlaySliderMaximum, ui->ctrlBarWidget, &CtrlBarWidget::setPlaySliderMaximum);
    connect(FFPlay::GetInstance(), &FFPlay::setPlaySliderValue, ui->ctrlBarWidget, &CtrlBarWidget::setPlaySliderValue);
    connect(FFPlay::GetInstance(), &FFPlay::setPlayTimeEdit, ui->ctrlBarWidget, &CtrlBarWidget::setPlayTimeEdit);
    connect(FFPlay::GetInstance(), &FFPlay::setTotalTimeEdit, ui->ctrlBarWidget, &CtrlBarWidget::setTotalTimeEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
}
