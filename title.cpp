#include "title.h"
#include "ui_title.h"
#include"globalhelper.h"

Title::Title(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Title)
{
    ui->setupUi(this);

//    鼠标悬浮在按钮上显示的文本
    ui->MenuBtn->setToolTip("显示主菜单");
    ui->MinBtn->setToolTip("最小化");
    ui->MaxBtn->setToolTip("最大化");
    ui->FullScreenBtn->setToolTip("全屏");
    ui->CloseBtn->setToolTip("关闭");
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
    return true;
}

bool Title::initUi()
{
//    清空媒体视频名称
    ui->MovieNameLab->clear();
    // todo：目前没发现这个有什么用，等发现了再打开。保证窗口不被绘制上的部分透明
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
