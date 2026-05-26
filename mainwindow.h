#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QListWidget>
#include<QMouseEvent>
#include<QPoint>
#include<QMenu>
#include<QShortcut>

#include"playlist.h"
#include"title.h"


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
//    键盘事件
    void keyReleaseEvent(QKeyEvent *event);
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
signals:
    void SigSeekForward();
    void SigSeekBack();
    void SigAddVolume();
    void SigSubVolume();
    void SigPlayOrPause();
    void SigStep(); // 逐帧播放
public:
    Ui::MainWindow *ui;
    Playlist m_playlist;
    Title m_title;
private:
    bool m_move_drag; //移动窗口标志
    QPoint m_drag_position; //在哪个位置开始移动
    QMenu m_menu;    //菜单，不是通过.ui文件加载的，而是代码里面直接渲染的
};

#endif // MAINWINDOW_H
