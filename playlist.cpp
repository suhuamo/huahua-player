#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>
#include"medialist.h"
#include<QDir>

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

    if(ui->List->Init() == false) {
        return false;
    }

    connectSignalSlots();

    return true;
}

bool Playlist::initUi()
{
//    加载qss样式
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/playlist.css"));

    return true;
}

void Playlist::connectSignalSlots()
{
    connect(ui->List, &MediaList::SigAddFile, this, &Playlist::SlotOnAddFile);
}

void Playlist::SlotOnAddFile(QString filePath)
{
    QFileInfo fileInfo(filePath);
//    查找该文件是否已经添加过
    QList<QListWidgetItem*>  itemList = ui->List->findItems(fileInfo.fileName(), Qt::MatchExactly);
//    如果没有添加过，那么添加到播放列表中
//    todo:不能直接通过这样判断，因为findItem是通过text来判断的，我们确定视频的唯一应该是通过UserRole的Data来确定的
    if(itemList.isEmpty()) {
        QListWidgetItem *p_item = new QListWidgetItem(ui->List);
        p_item->setData(Qt::UserRole, QVariant(fileInfo.filePath()));// 自定义用户数据，可在其他地方取出来使用
        p_item->setText(fileInfo.fileName());   // 显示文本
        p_item->setToolTip(fileInfo.filePath());
        ui->List->addItem(p_item);
    } else {
//        todo：如果已经添加过，那么将该元素移动到第一位
    }

}
