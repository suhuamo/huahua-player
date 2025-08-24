#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _playlist(this),
    _move_drag(false)
{
    ui->setupUi(this);
    //  Qt::FramelessWindowHint: 直接去掉菜单栏； Qt::WindowMinimizeButtonHint：窗口化和退出按钮均置灰不可点击，只能点击最小化
//    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
//    设置任务栏中显示的图片
    this->setWindowIcon(QIcon("://res//player.png"));
//    todo：加载样式

//    开启鼠标跟踪，用于播放时隐藏
    this->setMouseTracking(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::Init()
{
    QWidget *play_list_bar_wid = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(play_list_bar_wid);
    ui->PlaylistWid->setWidget(&_playlist);

    QWidget *title_bar_wid = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(title_bar_wid);
    ui->TitleWid->setWidget(&_title);

    return true;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() & Qt::LeftButton) {
//        只有鼠标在标题栏按下时，才可以拖动窗口
        if(ui->TitleWid->geometry().contains(event->pos())) {
            _move_drag = true;
            _drag_position = event->globalPos() - this->pos();
        }
    }
//    执行原有的鼠标事件
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    _move_drag = false;
    //    执行原有的鼠标事件
    QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(_move_drag) {
        move(event->globalPos() - _drag_position);
    }
    //    执行原有的鼠标事件
    QWidget::mouseMoveEvent(event);
}
