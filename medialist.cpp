#include "medialist.h"
#include<QFileDialog>
#include"globalhelper.h"

MediaList::MediaList(QWidget *parent)
    : QListWidget(parent),
      m_menu(this),
      m_act_add_file(this),
      m_act_remove_file(this),
      m_act_clear_list(this)
{

}

MediaList::~MediaList()
{

}

bool MediaList::init()
{
    if(initUi() == false) {
        return false;
    }
//    将行为添加到菜单上
    m_menu.addAction(&m_act_add_file);
    m_menu.addAction(&m_act_remove_file);
    m_menu.addAction(&m_act_clear_list);
//    添加信号槽
    connect(&m_act_add_file, &QAction::triggered, this, &MediaList::addFile);
    connect(&m_act_remove_file, &QAction::triggered, this, &MediaList::removeFile);
    connect(&m_act_clear_list, &QAction::triggered, this, &QListWidget::clear);
    return true;
}

void MediaList::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu.exec(event->globalPos());
    // 接受事件，阻止继续传播（目前父组件没有相关事件，故这个可写可不写）
    event->accept();
}

bool MediaList::initUi()
{
    // 设置文本
    m_act_add_file.setText("添加");
    m_act_remove_file.setText("移除所选项");
    m_act_clear_list.setText("清空列表");
    return true;
}

void MediaList::addFile()
{
    //todo：这里的 QDir::homePath() 可以修改为指定路径，比如 C:\\data\\video ，或者修改为上次打开的目录
    QStringList filePathList = QFileDialog::getOpenFileNames(this, "打开文件", QDir::homePath(), "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");
   
    for(QString filePath : filePathList) {
        emit sigAddFile(filePath);
    }
}

void MediaList::removeFile()
{
    takeItem(currentRow());
}
