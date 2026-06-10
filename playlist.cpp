#include "playlist.h"
#include "ui_playlist.h"
#include<globalhelper.h>
#include"medialist.h"
#include<QDir>
#include<QMessageBox>

Playlist::Playlist(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Playlist),
    m_current_play_list_index(0)
{
    ui->setupUi(this);
}

Playlist::~Playlist()
{
    QStringList strPlayList;
    for(int i = 0; i < ui->list->count(); i++) {
        strPlayList.append(ui->list->item(i)->toolTip());
    }
    GlobalHelper::savePlaylist(strPlayList);

    delete ui;
}

bool Playlist::init()
{
    if(initUi() == false) {
        return false;
    }

    if(ui->list->init() == false) {
        return false;
    }

    connectSignalSlots();

//    清空播放列表，保证现在是干净的
    ui->list->clear();

    QStringList strPlayList;
//    从本地缓存读取文件列表
    GlobalHelper::getPlaylist(strPlayList);
    for(QString videoFile : strPlayList) {
        onAddFile(videoFile);
    }
//    todo：应该默认选择上一次关闭前的播放文件才对吧
//    默认选择第一个视频作为默认播放文件
    if(strPlayList.length() > 0) {
        ui->list->setCurrentRow(0);
    }

//    使得当前QT控制开启拖拽功能
    setAcceptDrops(true);

    return true;
}

bool Playlist::initUi()
{
//    加载qss样式
    setStyleSheet(GlobalHelper::getQssStr(":/res/qss/playlist.css"));

    return true;
}

void Playlist::connectSignalSlots()
{
    connect(ui->list, &MediaList::sigAddFile, this, &Playlist::onAddFile);
    connect(ui->list, &MediaList::itemDoubleClicked, this, &Playlist::onListItemDoubleClicked);
}

void Playlist::onAddFile(QString filePath)
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
    for(int i = 0; i < ui->list->count(); i++) {
        QListWidgetItem *item = ui->list->item(i);
        QString existingPath = item->data(Qt::UserRole).toString();
        if(existingPath == absolutePath) {
            isExist = true;
            break;
        }
    }

    //    如果没有添加过，那么添加到播放列表中
    if(!isExist) {
        QListWidgetItem *p_item = new QListWidgetItem(ui->list);
        p_item->setData(Qt::UserRole, QVariant(absolutePath));
        p_item->setText(fileInfo.fileName());
        p_item->setToolTip(absolutePath);
        ui->list->addItem(p_item);
    } else {
        QMessageBox::information(this, tr("重复添加"),
                                tr("文件 \"%1\" 已经在播放列表中存在。").arg(fileInfo.fileName()));
    }

}

void Playlist::onListItemDoubleClicked(QListWidgetItem *item)
{
    emit sigPlay(item->data(Qt::UserRole).toString());

    ui->list->setCurrentItem(item);
    m_current_play_list_index = ui->list->row(item);
}

void Playlist::onBackPlay()
{
    // 如果现在是第一个，还要回退，那么就变成最后一个
    if(m_current_play_list_index == 0) {
        m_current_play_list_index = ui->list->count() - 1;
//        发出信号通知其他组件
        onListItemDoubleClicked(ui->list->item(m_current_play_list_index));
//        更新列表选择状态
        ui->list->setCurrentRow(m_current_play_list_index);
    } else {
        m_current_play_list_index--;
//        发出信号通知其他组件
        onListItemDoubleClicked(ui->list->item(m_current_play_list_index));
//        更新列表选择状态
        ui->list->setCurrentRow(m_current_play_list_index);
    }
}

void Playlist::onNextPlay()
{
    // 如果现在是最后一个，还要下一个，那么就变成第一个
    if(m_current_play_list_index == ui->list->count() - 1) {
        m_current_play_list_index = 0;
//        发出信号通知其他组件
        onListItemDoubleClicked(ui->list->item(m_current_play_list_index));
//        更新列表选择状态
        ui->list->setCurrentRow(m_current_play_list_index);
    } else {
        m_current_play_list_index++;
//        发出信号通知其他组件
        onListItemDoubleClicked(ui->list->item(m_current_play_list_index));
//        更新列表选择状态
        ui->list->setCurrentRow(m_current_play_list_index);
    }
}

void Playlist::onAddFileAndPlay(QString strFileName)
{
    bool supportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive);
    if (!supportMovie)
    {
        return;
    }

    QFileInfo fileInfo(strFileName);
    if(!fileInfo.exists()) {
        QMessageBox::warning(this, tr("文件不存在"),
                            tr("文件 \"%1\" 不存在，请检查路径是否正确。").arg(fileInfo.fileName()));
        qDebug() << "文件不存在:" << strFileName;
        return;
    }

    QString absolutePath = fileInfo.absoluteFilePath();

    //    查找该文件是否已经添加过 - 通过 UserRole 中的完整路径判断唯一性
    QListWidgetItem *pItem = nullptr;
    bool isExist = false;
    for(int i = 0; i < ui->list->count(); i++) {
        QListWidgetItem *item = ui->list->item(i);
        QString existingPath = item->data(Qt::UserRole).toString();
        if(existingPath == absolutePath) {
            pItem = item;
            isExist = true;
            break;
        }
    }

    //    如果没有添加过，那么添加到播放列表中
    if(!isExist) {
        pItem = new QListWidgetItem(ui->list);
        pItem->setData(Qt::UserRole, QVariant(absolutePath));  // 用户数据
        pItem->setText(fileInfo.fileName());  // 显示文本
        pItem->setToolTip(absolutePath);
        ui->list->addItem(pItem);
    }

    onListItemDoubleClicked(pItem);
}

void Playlist::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty()) {
        return;
    }
    for(QUrl url: urls) {
        QString strFileName = url.toLocalFile();
        onAddFile(strFileName);
    }
}

void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
//    允许当前拖拽生效，使得事件往后续传递，即给 dropEvent 可以响应
    event->acceptProposedAction();
}
