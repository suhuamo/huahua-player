#include "medialist.h"
#include<QFileDialog>
#include"globalhelper.h"

MediaList::MediaList(QWidget *parent)
    : QListWidget(parent),
      _menu(this),
      _act_add_file(this),
      _act_remove_file(this),
      _act_clear_list(this)
{

}

MediaList::~MediaList()
{

}

bool MediaList::Init()
{
    if(initUi() == false) {
        return false;
    }
//    将行为添加到菜单上
    _menu.addAction(&_act_add_file);
    _menu.addAction(&_act_remove_file);
    _menu.addAction(&_act_clear_list);
//    添加信号槽
    connect(&_act_add_file, &QAction::triggered, this, &MediaList::addFile);
    connect(&_act_remove_file, &QAction::triggered, this, &MediaList::removeFile);
    connect(&_act_clear_list, &QAction::triggered, this, &QListWidget::clear);
    return true;
}

void MediaList::contextMenuEvent(QContextMenuEvent *event)
{
    _menu.exec(event->globalPos());
    // 接受事件，阻止继续传播（目前父组件没有相关事件，故这个可写可不写）
    event->accept();
}

bool MediaList::initUi()
{
    // 设置文本
    _act_add_file.setText("添加");
    _act_remove_file.setText("移除所选项");
    _act_clear_list.setText("清空列表");
    return true;
}

void MediaList::addFile()
{
//    QStringList fileNameList = QFileDialog::getOpenFileNames(this, "打开文件", QDir::homePath(), "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");
//    todo测试:目前测试为了方便，直接指定为有视频的文件夹
    QStringList filePathList = QFileDialog::getOpenFileNames(this, "打开文件", "C:\\data\\video", "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");
    for(QString filePath : filePathList) {
        emit SigAddFile(filePath);
    }
}

void MediaList::removeFile()
{
    takeItem(currentRow());
}
