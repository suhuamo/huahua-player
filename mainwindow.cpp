#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFile>
#include<QDebug>
#include"globalhelper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _playlist(this),
    _move_drag(false)
{
    ui->setupUi(this);
    /*
     * Qt::FramelessWindowHint: 直接去掉菜单栏； Qt::WindowMinimizeButtonHint：窗口化和退出按钮均置灰不可点击，只能点击最小化
     */
//    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
//    todo：这个类为什么不提取一个 initUi的方法呢，可能是因为本类马上就需要加载页面，故不需要提取方法给别人调用吧？
//    设置任务栏中显示的图片
    this->setWindowIcon(QIcon(":/res//player.png"));
//    加载样式
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/mainwid.css"));
//    开启鼠标跟踪，用于播放时隐藏
    this->setMouseTracking(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//todo：这Init为什么要让外面调用呢，在构造函数里面自己调用不行吗【可能是为了节省资源，因为有的窗口创建了对象但是不需要马上加载，那么Init延迟调用可以减少卡顿】
bool MainWindow::Init()
{
    QWidget *play_list_bar_wid = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(play_list_bar_wid);
    ui->PlaylistWid->setWidget(&_playlist);

    QWidget *title_bar_wid = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(title_bar_wid);
    ui->TitleWid->setWidget(&_title);

    if(_title.Init() == false) {
        return false;
    }

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
