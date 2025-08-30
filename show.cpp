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
    qDebug() <<GlobalHelper::GetQssStr(":/res/qss/show.css");
    return true;
}
