#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 传递界面显示的图片，后续使用队列
    connect(FFPlay::GetInstance(), &FFPlay::setImage, ui->showWidget, &DisplayWidget::setImage);
    // 传递进度条数据
    connect(FFPlay::GetInstance(), &FFPlay::setPlaySliderMaximum, ui->ctrlBarWidget, &CtrlBarWidget::setPlaySliderMaximum);
    connect(FFPlay::GetInstance(), &FFPlay::setPlaySliderValue, ui->ctrlBarWidget, &CtrlBarWidget::setPlaySliderValue);
    // 传递时间戳数据
    connect(FFPlay::GetInstance(), &FFPlay::setPlayTimeEdit, ui->ctrlBarWidget, &CtrlBarWidget::setPlayTimeEdit);
    connect(FFPlay::GetInstance(), &FFPlay::setTotalTimeEdit, ui->ctrlBarWidget, &CtrlBarWidget::setTotalTimeEdit);
    // 更换播放的文件
    connect(ui->playListContents->getListWidget(), &QListWidget::clicked, FFPlay::GetInstance(), &FFPlay::updatePlayUrl);
    // 清空页面数据
    connect(FFPlay::GetInstance(), &FFPlay::clear, ui->ctrlBarWidget, &CtrlBarWidget::initOrClear);
    connect(FFPlay::GetInstance(), &FFPlay::clear, ui->showWidget, &DisplayWidget::clear);
}

MainWindow::~MainWindow()
{
    delete ui;
}
