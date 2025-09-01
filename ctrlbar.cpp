#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include"globalhelper.h"

CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBar),
    _pause(true)
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

    return true;
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
    ui->VolumeBtn->setToolTip("静音");
    return true;
}

void CtrlBar::connectSignalSlots()
{
//    todo：SettingBtn 是显示出来调节视频的参数，比如编码格式，编码效率等，等后续再开发
    connect(ui->PlayListCtlBtn, &QPushButton::clicked, this, &CtrlBar::SigPlayListCtlBtnClicked);
    connect(ui->PlayOrPauseBtn, &QPushButton::clicked, this, &CtrlBar::SlotOnPlayOrPauseBtnClicked);
    connect(ui->VolumeBtn, &QPushButton::clicked, this, &CtrlBar::SlotOnVolumeBtnClicked);
    connect(ui->BackBtn, &QPushButton::clicked, this, &CtrlBar::SigBackBtnClicked);
    connect(ui->NextBtn, &QPushButton::clicked, this, &CtrlBar::SigNextBtnClicked);
}

void CtrlBar::SlotOnPlayOrPauseBtnClicked()
{
//    暂停状态下，图标显示为播放按钮
    if (_pause)
    {
//        点击后视频开始播放，此时状态为播放状态，按钮变为暂停按钮，提示为点击暂停
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04c));
        ui->PlayOrPauseBtn->setToolTip("点击暂停");
        _pause = false;
    }
    else
    {
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
        ui->PlayOrPauseBtn->setToolTip("点击播放");
        _pause = true;
    }
}

void CtrlBar::SlotOnVolumeBtnClicked()
{
//    todo:如果是拖放进度，变为0和变为非0也要改变图标状态
//    如果此时是非静音状态
    if(ui->VolumeBtn->text() == QChar(0xf028)) {
        _last_volume_percent = ui->VolumeSlider->value();
        ui->VolumeSlider->setValue(0);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
    } else {
        ui->VolumeSlider->setValue(_last_volume_percent);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
    }
}
