#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>
#include"medialist.h"
#include<QDir>

Playlist::Playlist(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Playlist),
    _current_play_list_index(0)
{
    ui->setupUi(this);
}

Playlist::~Playlist()
{
    QStringList strPlayList;
    for(int i = 0; i < ui->List->count(); i++) {
        strPlayList.append(ui->List->item(i)->toolTip());
    }
    GlobalHelper::SavePlaylist(strPlayList);

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

//    todo测试:目前测试为了方便，默认设置几个视频文件
    SlotOnAddFile("C:\\data\\video\\1-4min.mp4");
    SlotOnAddFile("C:\\data\\video\\2-4min.mp4");
    SlotOnAddFile("C:\\data\\video\\3-10s.mp4");
    SlotOnAddFile("C:\\data\\video\\time_counter_1.mp4");

//    清空播放列表，保证现在是干净的
    ui->List->clear();

    QStringList strPlayList;
//    从本地缓存读取文件列表
    GlobalHelper::GetPlaylist(strPlayList);
    for(QString videoFile : strPlayList) {
        SlotOnAddFile(videoFile);
    }
//    todo：应该默认选择上一次关闭前的播放文件才对吧
//    默认选择第一个视频作为默认播放文件
    if(strPlayList.length() > 0) {
        ui->List->setCurrentRow(0);
    }

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
    connect(ui->List, &MediaList::itemDoubleClicked, this, &Playlist::SlotOnListItemDoubleClicked);
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

void Playlist::SlotOnListItemDoubleClicked(QListWidgetItem *item)
{
    emit SigPlay(item->data(Qt::UserRole).toString());

    _current_play_list_index = ui->List->row(item);
}

void Playlist::SlotOnBackPlay()
{
    // 如果现在是第一个，还要回退，那么就变成最后一个
    if(_current_play_list_index == 0) {
        _current_play_list_index = ui->List->count() - 1;
//        发出信号通知其他组件
        SlotOnListItemDoubleClicked(ui->List->item(_current_play_list_index));
//        更新列表选择状态
        ui->List->setCurrentRow(_current_play_list_index);
    } else {
        _current_play_list_index--;
//        发出信号通知其他组件
        SlotOnListItemDoubleClicked(ui->List->item(_current_play_list_index));
//        更新列表选择状态
        ui->List->setCurrentRow(_current_play_list_index);
    }
}

void Playlist::SlotOnNextPlay()
{
    // 如果现在是最后一个，还要下一个，那么就变成第一个
    if(_current_play_list_index == ui->List->count() - 1) {
        _current_play_list_index = 0;
//        发出信号通知其他组件
        SlotOnListItemDoubleClicked(ui->List->item(_current_play_list_index));
//        更新列表选择状态
        ui->List->setCurrentRow(_current_play_list_index);
    } else {
        _current_play_list_index++;
//        发出信号通知其他组件
        SlotOnListItemDoubleClicked(ui->List->item(_current_play_list_index));
//        更新列表选择状态
        ui->List->setCurrentRow(_current_play_list_index);
    }
}
