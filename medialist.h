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
    bool Init();
protected:
//    右键时触发，用来显示菜单的
    void contextMenuEvent(QContextMenuEvent *);
private:
    bool initUi();
    void addFile();
    void removeFile();
signals:
    void SigAddFile(QString filePath);
private:
    QMenu _menu;
    QAction _act_add_file;
    QAction _act_remove_file;
    QAction _act_clear_list;
};

#endif // MEDIALIST_H
