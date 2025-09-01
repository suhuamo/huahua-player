#include "title.h"
#include "ui_title.h"
#include"globalhelper.h"
#include<QFileInfo>

Title::Title(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Title)
{
    ui->setupUi(this);
}

Title::~Title()
{
    delete ui;
}

bool Title::Init()
{
    if(initUi() == false) {
        return false;
    }

    connectSignalSlots();

    return true;
}

void Title::SlotOnPlay(QString filePath)
{
    QFileInfo fileInfo(filePath);
    if(fileInfo.isFile()) {
        ui->MovieNameLab->setText(fileInfo.fileName());
//        异常情况，即文件被删除了
    } else {
        //    todo:可以加个弹窗提示
        ui->MovieNameLab->setText("文件不存在");
    }

}

bool Title::initUi()
{
    //    鼠标悬浮在按钮上显示的文本
    ui->MenuBtn->setToolTip("显示主菜单");
    ui->MinBtn->setToolTip("最小化");
    ui->MaxBtn->setToolTip("最大化");
    ui->FullScreenBtn->setToolTip("全屏");
    ui->CloseBtn->setToolTip("关闭");
//    清空媒体视频名称
    ui->MovieNameLab->clear();
//     todo：目前没发现这个有什么用，等发现了再打开。保证窗口不被绘制上的部分透明
//    setAttribute(Qt::WA_TranslucentBackground);
//    设置样式表
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/title.css"));
//    设置按钮图标
    GlobalHelper::SetIcon(ui->MinBtn, 9, QChar(0xf2d1));
    GlobalHelper::SetIcon(ui->MaxBtn, 9, QChar(0xf2d0));
    GlobalHelper::SetIcon(ui->FullScreenBtn, 9, QChar(0xf065));
    GlobalHelper::SetIcon(ui->CloseBtn, 9, QChar(0xf00d));
    ui->MovieNameLab->setText("视频名称测试");

    return true;
}

void Title::connectSignalSlots()
{
    connect(ui->MinBtn, &QPushButton::clicked, this, &Title::SigMinBtnClicked);
    connect(ui->MaxBtn, &QPushButton::clicked, this, &Title::SigMaxBtnClicked);
    connect(ui->FullScreenBtn, &QPushButton::clicked, this, &Title::SigFullScreenBtnClicked);
    connect(ui->CloseBtn, &QPushButton::clicked, this, &Title::SigCloseBtnClicked);
    connect(ui->MenuBtn, &QPushButton::clicked, this, &Title::SigMenuBtnClicked);
}
