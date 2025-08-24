#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QListWidget>
#include<QMouseEvent>
#include<QPoint>
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

public:
    Ui::MainWindow *ui;
    Playlist _playlist;
    Title _title;
private:
    bool _move_drag; //移动窗口标志
    QPoint _drag_position; //在哪个位置开始移动
};

#endif // MAINWINDOW_H
