#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFile>
#include"globalhelper.h"
#include<QAction>
#include<QKeySequence>
#include<QDesktopWidget>
#include<QWindow>
#include<QShortcut>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _playlist(this),
    _move_drag(false),
    _menu(this)
{
    ui->setupUi(this);
    /*
     * Qt::FramelessWindowHint: 直接去掉菜单栏； Qt::WindowMinimizeButtonHint：窗口化和退出按钮均置灰不可点击，只能点击最小化
     */
    setWindowFlags(Qt::FramelessWindowHint);
//    todo：这个类为什么不提取一个 initUi的方法呢，可能是因为本类马上就需要加载页面，故不需要提取方法给别人调用吧？
//    设置任务栏中显示的图片
    this->setWindowIcon(QIcon(":/res/player.png"));
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

//    todo:怎么设置Title不可上下拖动？

    if(_title.Init() == false ||
       _playlist.Init() == false ||
            ui->CtrlBarWid->Init() == false ||
            ui->ShowWid->Init() == false) {
        return false;
    }

    initMenu();

    connectSignalSlots();

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

void MainWindow::connectSignalSlots()
{
    connect(&_title, &Title::SigMinBtnClicked, this, &MainWindow::SlotOnMinBtnClicked);
    connect(&_title, &Title::SigMaxBtnClicked, this, &MainWindow::SlotOnMaxBtnClicked);
    connect(&_title, &Title::SigFullScreenBtnClicked, this, &MainWindow::SlotOnFullScreenBtnClicked);
    connect(&_title, &Title::SigCloseBtnClicked, this, &MainWindow::SlotOnCloseBtnClicked);
    connect(&_title, &Title::SigMenuBtnClicked, this, &MainWindow::SlotOnMenuBtnClicked);
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayListCtlBtnClicked, this, &MainWindow::SlotOnPlayListCtrlBtnClicked);
    connect(&_playlist, &Playlist::SigPlay, &_title, &Title::SlotOnPlay);
    connect(ui->CtrlBarWid, &CtrlBar::SigBackBtnClicked, &_playlist, &Playlist::SlotOnBackPlay);
    connect(ui->CtrlBarWid, &CtrlBar::SigNextBtnClicked, &_playlist, &Playlist::SlotOnNextPlay);
}

void MainWindow::SlotOnMinBtnClicked()
{
    this->showMinimized();
}

void MainWindow::SlotOnMaxBtnClicked()
{
    if(isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void MainWindow::SlotOnFullScreenBtnClicked()
{
//    如果当前是全屏状态
    if(ui->ShowWid->isFullScreen()) {
//        需要先将当前窗口设置为子窗口，才能取消全屏
        ui->ShowWid->setWindowFlags(Qt::SubWindow);
        ui->ShowWid->showNormal();
    } else {
        //脱离父窗口后才能设置为全屏
        ui->ShowWid->setWindowFlags(Qt::Window);
        ui->ShowWid->showFullScreen();
    }
//    让当前窗口获取焦点
    this->setFocus();
}

void MainWindow::SlotOnCloseBtnClicked()
{
    this->close();
}

void MainWindow::SlotOnMenuBtnClicked()
{
//    在鼠标位置打开菜单
    _menu.exec(cursor().pos());
}

void MainWindow::SlotOnPlayListCtrlBtnClicked()
{
    if(ui->PlaylistWid->isHidden()) {
        ui->PlaylistWid->show();
    } else {
        ui->PlaylistWid->hide();
    }
}

void MainWindow::initMenu()
{
//    添加菜单和行为
    QAction *act_about = _menu.addAction(tr("关于 \t Ctrl + A"));
    QMenu* open_menu = _menu.addMenu(tr("打开"));
    QAction* act_open_file = open_menu->addAction(tr("打开文件 \t Ctrl + F"));
    QAction* act_open_stream = open_menu->addAction(tr("打开视频流 \t Ctrl + L"));
    QAction* act_full_screen = _menu.addAction(tr("全屏/取消全屏 \t F11"));
//    添加槽函数
    connect(act_about, &QAction::triggered, this, []() {
        qDebug() << "关于 \t Ctrl + A 被触发";
    });
    connect(act_open_file, &QAction::triggered, this, []() {
        qDebug() << "打开文件 \t Ctrl + F  被触发";
    });
    connect(act_open_stream, &QAction::triggered, this, []() {
        qDebug() << "打开视频流 \t Ctrl + L  被触发";
    });
    connect(act_full_screen, &QAction::triggered, this, [this]() {
        qDebug() << "全屏/取消全屏 \t F11 被触发";
        this->SlotOnFullScreenBtnClicked();
    });

    // 添加快捷键(由于QMenu没有焦点，故只能再次添加到当前窗口上)
    act_about->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    act_open_file->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    act_open_stream->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    act_full_screen->setShortcut(QKeySequence(Qt::Key_F11));
    // 设置为应用全局有效
    act_full_screen->setShortcutContext(Qt::ApplicationShortcut); // 因为全屏的时候焦点是在show的Lable上，不在当前窗口上，那么当前窗口的快捷键就无法生效了

    // 把action添加到当前窗口上
    this->addAction(act_about);
    this->addAction(act_open_file);
    this->addAction(act_open_stream);
    this->addAction(act_full_screen);

}
