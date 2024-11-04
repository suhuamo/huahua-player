#include "ctrlbarwidget.h"
#include "ui_ctrlbarwidget.h"

CtrlBarWidget::CtrlBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBarWidget)
{
    ui->setupUi(this);
    // 设置播放图标
    QIcon play_icon(":/ctrl/icon/play.png");
    ui->playOrPauseButton->setIcon(play_icon);
    // 设置停止图标
    QIcon stop_icon(":/ctrl/icon/stop.png");
    ui->stopButton->setIcon(stop_icon);
    // 设置暂停图标
    QIcon pause_icon(":/ctrl/icon/pause.png");
}

CtrlBarWidget::~CtrlBarWidget()
{
    delete ui;
}
