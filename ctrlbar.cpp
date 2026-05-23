#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include"globalhelper.h"

CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBar)
{
    ui->setupUi(this);
}

CtrlBar::~CtrlBar()
{
    delete ui;
}

bool CtrlBar::Init()
{
    if(initUi() == false) {
        return false;
    }

    connectSignalSlots();

//    设置初始音量
    double percent = -1.0;
//    从本地配置文件中读取
    GlobalHelper::GetPlayVolume(percent);
    if(percent != -1.0) {
        OnVideopVolume(percent);
        emit SigPlayVolume(percent);
    } else {
        OnVideopVolume(_last_volume_percent);
        emit SigPlayVolume(_last_volume_percent);
    }

    return true;
}

void CtrlBar::OnSpeed(float speed)
{
    ui->SpeedBtn->setText(QString("倍数:%1").arg(speed));
}

bool CtrlBar::initUi()
{
    //加载qss
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/ctrlbar.css"));
//    设置按钮图片
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    GlobalHelper::SetIcon(ui->OverPlayBtn, 12, QChar(0xf04d));
    GlobalHelper::SetIcon(ui->BackBtn, 12, QChar(0xf048));
    GlobalHelper::SetIcon(ui->NextBtn, 12, QChar(0xf051));
    GlobalHelper::SetIcon(ui->PlayListCtlBtn, 12, QChar(0xf036));
    GlobalHelper::SetIcon(ui->SettingBtn, 12, QChar(0xf013));
    GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
//    设置鼠标悬浮提示
    ui->PlayOrPauseBtn->setToolTip("点击播放");
    ui->OverPlayBtn->setToolTip("结束播放");
    ui->BackBtn->setToolTip("上一个");
    ui->NextBtn->setToolTip("下一个");
    ui->PlayListCtlBtn->setToolTip("播放列表");
    ui->SettingBtn->setToolTip("设置");
    ui->VolumeBtn->setToolTip("点击静音");
    ui->SpeedBtn->setToolTip("倍速");
//    设置进度条初始值
    _last_volume_percent = 0.5;

    return true;
}

void CtrlBar::connectSignalSlots()
{
//    todo：SettingBtn 是显示出来调节视频的参数，比如编码格式，编码效率等，等后续再开发
    connect(ui->PlayListCtlBtn, &QPushButton::clicked, this, &CtrlBar::SigPlayListCtlBtnClicked);
    connect(ui->VolumeBtn, &QPushButton::clicked, this, &CtrlBar::SlotOnVolumeBtnClicked);
    connect(ui->BackBtn, &QPushButton::clicked, this, &CtrlBar::SigBackBtnClicked);
    connect(ui->NextBtn, &QPushButton::clicked, this, &CtrlBar::SigNextBtnClicked);
    connect(ui->VolumeSlider, &CustomSlider::SigSliderValueChanged, this, &CtrlBar::OnVolumeSliderValueChanged);
}

void CtrlBar::OnPauseStat(bool paused)
{
    qDebug() << "CtrlBar::OnPauseStat" << paused;
//    需要变成暂停状态
    if (paused)
    {
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
        ui->PlayOrPauseBtn->setToolTip("点击播放");
    }
    else
    {
//        此时状态为播放状态，按钮变为暂停按钮，提示为点击暂停
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04c));
        ui->PlayOrPauseBtn->setToolTip("点击暂停");
    }
}

void CtrlBar::OnStopFinished()
{
    ui->PlaySlider->setValue(0);
    QTime stopTime(0, 0, 0);
    ui->VideoTotalTimeTimeEdit->setTime(stopTime);
    ui->VideoPlayTimeTimeEdit->setTime(stopTime);
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    ui->PlayOrPauseBtn->setToolTip("点击播放");
}

void CtrlBar::OnVideopVolume(double percent)
{
    ui->VolumeSlider->setValue(percent * MAX_SLIDER_VALUE);
    _last_volume_percent = percent;

    if (_last_volume_percent == 0)
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeBtn->setToolTip("点击恢复音量");
    }
    else
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeBtn->setToolTip("点击静音");
    }
    GlobalHelper::SavePlayVolume(percent);

}

void CtrlBar::SlotOnVolumeBtnClicked()
{
    // 静音和恢复静音并不会改变 _last_volume_percent 的值，因为 _last_volume_percent 只会被滑动条滑动改变
//    如果此时是非静音状态
    if(ui->VolumeBtn->text() == QChar(0xf028)) {
        ui->VolumeSlider->setValue(0);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeBtn->setToolTip("点击恢复音量");
        emit SigPlayVolume(0);
    } else {
//        恢复之前的音量百分比
        ui->VolumeSlider->setValue(_last_volume_percent * MAX_SLIDER_VALUE);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeBtn->setToolTip("点击静音");
        emit SigPlayVolume(_last_volume_percent);
    }
}

void CtrlBar::OnVolumeSliderValueChanged()
{
    double percent = ui->VolumeSlider->value() * 1.0 / ui->VolumeSlider->maximum();
    emit SigPlayVolume(percent);

    OnVideopVolume(percent);
}

void CtrlBar::on_SpeedBtn_clicked()
{
//    设置变速
    emit SigSpeed();
}

void CtrlBar::on_PlayOrPauseBtn_clicked()
{
    emit SigPlayOrPause();
}

void CtrlBar::on_OverPlayBtn_clicked()
{
    emit SigStop();
}
