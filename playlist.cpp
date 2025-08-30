#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>

Playlist::Playlist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Playlist)
{
    ui->setupUi(this);
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
//    todo：样式没生效，待后面排查【2025-08-28：tmd我真服了，这是playlist，写成了加载title.css样式了，挂不得一直不生效】
//    加载qss样式
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/playlist.css"));

    return true;
}
