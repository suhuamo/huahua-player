#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>

Playlist::Playlist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Playlist)
{
    ui->setupUi(this);
//    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/title.css"));
}

Playlist::~Playlist()
{
    delete ui;
}

bool Playlist::Init()
{
    if(initUi() == false) {
        return false;
    }
    return true;
}

bool Playlist::initUi()
{
//    todo：样式没生效，待后面排查
//    加载qss样式
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/title.css"));

    return true;
}
