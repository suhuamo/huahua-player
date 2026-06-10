#ifndef MEDIALIST_H
#define MEDIALIST_H

#include<QListWidget>
#include<QMenu>
#include<QAction>
#include<QContextMenuEvent>

///
/// \brief 真正显示播放列表的组件
///
class MediaList: public QListWidget
{
    Q_OBJECT
public:
    MediaList(QWidget *parent=nullptr);
    ~MediaList();
    bool init();
protected:
//    右键时触发，用来显示菜单的
    void contextMenuEvent(QContextMenuEvent *);
private:
    bool initUi();
    void addFile();
    void removeFile();
signals:
    void sigAddFile(QString filePath);
private:
    QMenu m_menu;
    QAction m_act_add_file;
    QAction m_act_remove_file;
    QAction m_act_clear_list;
};

#endif // MEDIALIST_H
