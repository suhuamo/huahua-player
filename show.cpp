#include "show.h"
#include "ui_show.h"
#include"globalhelper.h"

Show::Show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Show)
{
    ui->setupUi(this);
//    接受拖拽事件，可以直接把视频文件拖动到播放界面开始播放
    setAcceptDrops(true);
    //    开启鼠标跟踪，用于播放时隐藏
    this->setMouseTracking(true);

}

Show::~Show()
{
    delete ui;
}

bool Show::Init()
{
    if(initUi() == false) {
        return false;
    }
    return true;
}

bool Show::initUi()
{
//    加载qss
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/show.css"));

    ui->label->clear();
    // 使用QPixmap加载资源系统中的图片
    ui->label->setPixmap(QPixmap(":/res/player.png"));

    // 可选：设置QLabel允许图片缩放以适应其大小
     ui->label->setScaledContents(true);

    return true;
}
