#include "ctrlbarwidget.h"
#include "ui_ctrlbarwidget.h"
#include"state.h"

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

    // 进度条初始数据
    ui->playSlider->setValue(0);
    ui->playSlider->setMinimum(0);
    ui->playSlider->setSingleStep(1);
    // 设置界面不允许操作
    ui->playTimeEdit->setEnabled(false);
    ui->totalTimeEdit->setEnabled(false);
}

CtrlBarWidget::~CtrlBarWidget()
{
    delete ui;
}

void CtrlBarWidget::on_playOrPauseButton_clicked()
{
    // 如果当前是暂停，那么开启播放视频，然后按钮变成点击暂停按钮
    if(State::play_state == 0)
    {
        // 设置暂停图标
        QIcon pause_icon(":/ctrl/icon/pause.png");
        ui->playOrPauseButton->setIcon(pause_icon);
    }
    // 如果当前是播放状态，那么把媒体暂停，然后按钮变成点击播放按钮
    else
    {
        // 设置播放图标
        QIcon play_icon(":/ctrl/icon/play.png");
        ui->playOrPauseButton->setIcon(play_icon);
    }
    //修改播放状态
    State::play_state = ~State::play_state;
}

void CtrlBarWidget::setPlaySliderValue(int value)
{
    ui->playSlider->setValue(value);
}

void CtrlBarWidget::setPlaySliderMaximum(int value)
{
    ui->playSlider->setMaximum(value);
}

void CtrlBarWidget::setPlayTimeEdit(const QTime &time)
{
    ui->playTimeEdit->setTime(time);
}

void CtrlBarWidget::setTotalTimeEdit(const QTime &time)
{
    QTime time1(0, 0, 12);
    ui->totalTimeEdit->setTime(time);
}

