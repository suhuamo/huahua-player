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

void Title::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}

void Title::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

bool Title::init()
{
    if(initUi() == false) {
        return false;
    }

    connectSignalSlots();

    return true;
}

void Title::onPlay(QString filePath)
{
    qDebug() << "Title::onPlay";
    QFileInfo fileInfo(filePath);
    if(fileInfo.isFile()) {
        ui->movieNameLab->setText(fileInfo.fileName());
//        todo：加一个异常情况判断，即文件被删除了
    } else {
        //    todo:可以加个弹窗提示
        ui->movieNameLab->setText("文件不存在");
    }

}

void Title::onStop()
{
    ui->movieNameLab->clear();
}
bool Title::initUi()
{
    //    鼠标悬浮在按钮上显示的文本
    ui->menuBtn->setToolTip("显示主菜单");
    ui->minBtn->setToolTip("最小化");
    ui->maxBtn->setToolTip("最大化");
    ui->fullScreenBtn->setToolTip("全屏");
    ui->closeBtn->setToolTip("关闭");
//    清空媒体视频名称
    ui->movieNameLab->clear();
//     todo：目前没发现这个有什么用，等发现了再打开。保证窗口不被绘制上的部分透明
//    setAttribute(Qt::WA_TranslucentBackground);
//    设置样式表
    setStyleSheet(GlobalHelper::getQssStr(":/res/qss/title.css"));
//    设置按钮图标
    GlobalHelper::setIcon(ui->minBtn, 9, QChar(0xf2d1));
    GlobalHelper::setIcon(ui->maxBtn, 9, QChar(0xf2d0));
    GlobalHelper::setIcon(ui->fullScreenBtn, 9, QChar(0xf065));
    GlobalHelper::setIcon(ui->closeBtn, 9, QChar(0xf00d));

    return true;
}

void Title::connectSignalSlots()
{
    connect(ui->minBtn, &QPushButton::clicked, this, &Title::sigMinBtnClicked);
    connect(ui->maxBtn, &QPushButton::clicked, this, &Title::sigMaxBtnClicked);
    connect(ui->fullScreenBtn, &QPushButton::clicked, this, &Title::sigFullScreenBtnClicked);
    connect(ui->closeBtn, &QPushButton::clicked, this, &Title::sigCloseBtnClicked);
    connect(ui->menuBtn, &QPushButton::clicked, this, &Title::sigMenuBtnClicked);
}
