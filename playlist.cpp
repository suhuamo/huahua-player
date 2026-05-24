#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>
#include"medialist.h"
#include<QDir>
#include<QMessageBox>

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

//    使得当前QT控制开启拖拽功能
    setAcceptDrops(true);

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
    if(filePath.isEmpty()) {
        qDebug() << "filePath is empty";
        return;
    }

    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists()) {
        QMessageBox::warning(this, tr("文件不存在"),
                                  tr("文件 \"%1\" 不存在，请检查路径是否正确。").arg(fileInfo.fileName()));
        qDebug() << "文件不存在:" << filePath;
        return;
    }

    QString absolutePath = fileInfo.absoluteFilePath();

    //    查找该文件是否已经添加过 - 通过 UserRole 中的完整路径判断唯一性
    bool isExist = false;
    for(int i = 0; i < ui->List->count(); i++) {
        QListWidgetItem *item = ui->List->item(i);
        QString existingPath = item->data(Qt::UserRole).toString();
        if(existingPath == absolutePath) {
            isExist = true;
            break;
        }
    }

    //    如果没有添加过，那么添加到播放列表中
    if(!isExist) {
        QListWidgetItem *p_item = new QListWidgetItem(ui->List);
        p_item->setData(Qt::UserRole, QVariant(absolutePath));
        p_item->setText(fileInfo.fileName());
        p_item->setToolTip(absolutePath);
        ui->List->addItem(p_item);
    } else {
        QMessageBox::information(this, tr("重复添加"),
                                tr("文件 \"%1\" 已经在播放列表中存在。").arg(fileInfo.fileName()));
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

void Playlist::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty()) {
        return;
    }
    for(QUrl url: urls) {
        QString strFileName = url.toLocalFile();
        SlotOnAddFile(strFileName);
    }
}

void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
//    允许当前拖拽生效，使得事件往后续传递，即给 dropEvent 可以响应
    event->acceptProposedAction();
}
