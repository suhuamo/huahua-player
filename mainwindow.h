#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QListWidget>
#include<QMouseEvent>
#include<QPoint>
#include<QMenu>
#include"playlist.h"
#include"title.h"
#include<QShortcut>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool Init();
protected:
//    解决窗口无法拖动
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    void connectSignalSlots();
    void SlotOnMinBtnClicked();
    void SlotOnMaxBtnClicked();
    void SlotOnFullScreenBtnClicked();
    void SlotOnCloseBtnClicked();
    void SlotOnMenuBtnClicked();
    void SlotOnPlayListCtrlBtnClicked();
//    初始化菜单，并添加 action 的槽函数
    void initMenu();
public:
    Ui::MainWindow *ui;
    Playlist _playlist;
    Title _title;
private:
    bool _move_drag; //移动窗口标志
    QPoint _drag_position; //在哪个位置开始移动
    QMenu _menu;    //菜单，不是通过.ui文件加载的，而是代码里面直接渲染的
    QShortcut* _esc_shortcut_showWid; // 用于ESC退出全屏的快捷键
};

#endif // MAINWINDOW_H
