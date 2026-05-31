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
#include"audioseparator.h"
#include"separationprogressdialog.h"


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
//    键盘事件已通过全局QAction处理
private:
    void connectSignalSlots();
    void SlotOnMinBtnClicked();
    void SlotOnMaxBtnClicked();
    void SlotOnFullScreenBtnClicked();
    void SlotOnCloseBtnClicked();
    void SlotOnMenuBtnClicked();
    void SlotOnPlayListCtrlBtnClicked();
    void SlotOnAudioModeChanged(int mode);
    void SlotOnSeparationCompleted(const QString &filePath);
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
    int m_pending_audio_mode; // 等待分离完成后切换的音频模式
    SeparationProgressDialog *m_separation_dialog; // 音频分离进度对话框
};

#endif // MAINWINDOW_H
