#include<QFile>
#include<QAction>
#include<QKeySequence>
#include<QDesktopWidget>
#include<QWindow>
#include<QShortcut>
#include<QLabel>
#include<QPropertyAnimation>
#include<QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"globalhelper.h"
#include"videoctl.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_playlist(this),
    m_move_drag(false),
    m_menu(this)
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

/*
mark：这Init为什么要让外面调用呢，在构造函数里面自己调用不行吗【可能是为了节省资源，因为有的窗口创建了对象但是不需要马上加载，那么Init延迟调用可以减少卡顿】
* 因为 Init 内部可能失败，如果失败了，代表存在某个地方报错了，比如资源不够等，所以我们需要将 init 函数提取出来，让外部调用，外部如果调用当前类的init方法失败了，就代表当前类无法使用。
外部自己再做其他处理，比如直接退出程序，或者申请另一个资源类，不使用当前类。
举个例子：要做 word
我们选择打开 wps，那么先创建 wsp 对象，发现 wps.init 报错了，说明 wps 无法使用，
那么我们选择打开 excel，如果 excel.init 成功了，那么我就选择使用 excel了。
就不会说 wps 明明无法使用，我们却还拿着 wps 使用，这样肯定会导致有问题的。
*/
bool MainWindow::Init()
{
    QWidget *play_list_bar_wid = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(play_list_bar_wid);
    ui->PlaylistWid->setWidget(&m_playlist);

    QWidget *title_bar_wid = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(title_bar_wid);
    ui->TitleWid->setWidget(&m_title);

//    mark: 通过设置 title.ui 的 maxHeight 可以让 Title 这个 DockWidget 不能被拖动

    if(m_title.Init() == false ||
       m_playlist.Init() == false ||
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
            m_move_drag = true;
            m_drag_position = event->globalPos() - this->pos();
        }
    }
//    执行原有的鼠标事件
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_move_drag = false;
    //    执行原有的鼠标事件
    QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_move_drag) {
        move(event->globalPos() - m_drag_position);
    }
    //    执行原有的鼠标事件
    QWidget::mouseMoveEvent(event);
}

//链接信号与槽
void MainWindow::connectSignalSlots()
{
//    标题栏的按钮功能
    connect(&m_title, &Title::SigMinBtnClicked, this, &MainWindow::SlotOnMinBtnClicked);
    connect(&m_title, &Title::SigMaxBtnClicked, this, &MainWindow::SlotOnMaxBtnClicked);
    connect(&m_title, &Title::SigFullScreenBtnClicked, this, &MainWindow::SlotOnFullScreenBtnClicked);
    connect(&m_title, &Title::SigCloseBtnClicked, this, &MainWindow::SlotOnCloseBtnClicked);
    connect(&m_title, &Title::SigMenuBtnClicked, this, &MainWindow::SlotOnMenuBtnClicked);

    /*
     * 开启视频播放
     * 逻辑是：双击播放列表或者点击播放按钮的时候，调用 Playlist::SigPlay，然后触发 Show::SigPlay，去调用 VideoCtl::start_play 播放视频
     * 然后 VideoCtl::start_play 触发时 又会去调用 &Title::SlotOnPlay 修改标签栏的视频文件名称
     *
     */
    connect(&m_playlist, &Playlist::SigPlay, ui->ShowWid, &Show::SigPlay);

//    图片显示窗口的事件功能，比如拖拽、快捷键按下等
    connect(ui->ShowWid, &Show::SigOpenFile, &m_playlist, &Playlist::OnAddFileAndPlay);
    
    // Show窗口的键盘事件处理（全屏时生效）--因为全屏后，show变成了顶层窗口，mainwindow 快捷键就无法生效了，只能给 show 又添加一次一模一样的快捷键事件。
    connect(ui->ShowWid, &Show::SigExitFullScreen, this, &MainWindow::SlotOnFullScreenBtnClicked);
    connect(ui->ShowWid, &Show::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
    connect(ui->ShowWid, &Show::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
    connect(ui->ShowWid, &Show::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
    connect(ui->ShowWid, &Show::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);
    connect(ui->ShowWid, &Show::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
    connect(ui->ShowWid, &Show::SigStep, VideoCtl::GetInstance(), &VideoCtl::OnStep);

//    mainwindow 的快捷键功能
    connect(this, &MainWindow::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
    connect(this, &MainWindow::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
    connect(this, &MainWindow::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
    connect(this, &MainWindow::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);
    connect(this, &MainWindow::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
    connect(this, &MainWindow::SigStep, VideoCtl::GetInstance(), &VideoCtl::OnStep);
    
    // 快捷键操作后显示 Toast（通过 VideoCtl 的状态信号）
    connect(VideoCtl::GetInstance(), &VideoCtl::SigSeekForwardCompleted, ui->CtrlBarWid, &CtrlBar::OnSeekForward);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigSeekBackCompleted, ui->CtrlBarWid, &CtrlBar::OnSeekBack);

//    状态控制栏的按钮功能
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayListCtlBtnClicked, this, &MainWindow::SlotOnPlayListCtrlBtnClicked);
    connect(ui->CtrlBarWid, &CtrlBar::SigBackBtnClicked, &m_playlist, &Playlist::SlotOnBackPlay);
    connect(ui->CtrlBarWid, &CtrlBar::SigNextBtnClicked, &m_playlist, &Playlist::SlotOnNextPlay);
    connect(ui->CtrlBarWid, &CtrlBar::SigSpeed, VideoCtl::GetInstance(), &VideoCtl::OnSpeed);
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
    connect(ui->CtrlBarWid, &CtrlBar::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnUserStop);
    connect(ui->CtrlBarWid, &CtrlBar::SigPlayVolume, VideoCtl::GetInstance(), &VideoCtl::OnPlayVolume);
    connect(ui->CtrlBarWid, &CtrlBar::SigPlaySeek, VideoCtl::GetInstance(), &VideoCtl::OnPlaySeek);
    connect(ui->CtrlBarWid, &CtrlBar::SigShowToast, ui->ShowWid, &Show::ShowToast);

    /*
     * 视频播放时，界面相关变化通知
     * - 有的是视频播放后才知道一点一点通知界面的，比如现在的播放时间；
     * - 有的是快捷键按下直接通知 VideoCtl的，所以 VideCtl 需要反馈给界面,比如左右按键
     * - 有的是窗口大小发生改变了，我们的 SDL 窗口大小也会改变，所以 VideCtl 需要反馈给界面
     * 使用 DirectConnection 是因为需要 signal 任务结束时马上通知 slot 任务，不能扔在队列里面等待，这样会慢不及时。故直接在发送者线程执行这两个方法，相当于直接回调，即 this.signal, receiver.slot
     * 使用 QueuedConnection 是因为双方不在同一个线程，防止出现数据竞争或者崩溃，故直接丢给将当前方法丢给接受者线程执行。相当于观察者模式，在 receiver 线程执行 sender.signal，执行完成后 notify 当前线程执行 slot 方法
     *
     */
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, &m_title, &Title::SlotOnPlay, Qt::DirectConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, ui->ShowWid, &Show::OnStartPlay, Qt::DirectConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigSpeed, ui->CtrlBarWid, &CtrlBar::OnSpeed);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigPauseStat, ui->CtrlBarWid, &CtrlBar::OnPauseStat, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->CtrlBarWid, &CtrlBar::OnStopFinished, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoTotalSeconds, ui->CtrlBarWid, &CtrlBar::OnVideoTotalSeconds, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoPlaySeconds, ui->CtrlBarWid, &CtrlBar::OnVideoPlaySeconds, Qt::QueuedConnection);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoVolume, ui->CtrlBarWid, &CtrlBar::OnVolumeChanged);
    connect(VideoCtl::GetInstance(), &VideoCtl::SigFrameDimensionsChanged, ui->ShowWid, &Show::OnFrameDimensionsChanged, Qt::QueuedConnection);

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
        // 退出全屏时显示 CtrlBar、PlaylistWid
        ui->CtrlBarWid->show();
        ui->PlaylistWid->show();
        // 退出全屏时立即隐藏快捷键提示
        ui->ShowWid->HideShortcutHint();
        // 强制将焦点转移回主窗口
        this->activateWindow();
        //    让当前窗口获取焦点
        this->setFocus();
    } else {
        // 全屏前隐藏 CtrlBar、PlaylistWid，避免它留在旧位置被渲染到视频画面上
        ui->CtrlBarWid->hide();
        ui->PlaylistWid->hide();
        //脱离父窗口后才能设置为全屏
        ui->ShowWid->setWindowFlags(Qt::Window);
        ui->ShowWid->showFullScreen();
        // 全屏后显示快捷键提示（居中显示）
        ui->ShowWid->ShowShortcutHint(tr("按下 ESC/F11 即可退出全屏"));
        //    让show窗口获取焦点
        ui->ShowWid->setFocus();
    }
}

void MainWindow::SlotOnCloseBtnClicked()
{
    this->close();
}

void MainWindow::SlotOnMenuBtnClicked()
{
//    在鼠标位置打开菜单
    m_menu.exec(cursor().pos());
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
    QAction *act_about = m_menu.addAction(tr("关于 \t Ctrl + A"));
    QMenu* open_menu = m_menu.addMenu(tr("打开"));
    QAction* act_open_file = open_menu->addAction(tr("打开文件 \t Ctrl + F"));
    QAction* act_open_stream = open_menu->addAction(tr("打开视频流 \t Ctrl + L"));
    QAction* act_full_screen = m_menu.addAction(tr("全屏/取消全屏 \t F11"));

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

    // 将行为设置为应用全局有效（F11需要全局生效，因为全屏时焦点在Show窗口上）
    act_full_screen->setShortcutContext(Qt::ApplicationShortcut);

    // 上下左右方向键设置为全局action（全屏时焦点在Show窗口上，也需要生效）
    QAction *act_seek_back = new QAction(this);
    act_seek_back->setShortcut(Qt::Key_Left);
    act_seek_back->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_seek_back, &QAction::triggered, this, [this]() {
        emit SigSeekBack();
    });
    this->addAction(act_seek_back);

    QAction *act_seek_forward = new QAction(this);
    act_seek_forward->setShortcut(Qt::Key_Right);
    act_seek_forward->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_seek_forward, &QAction::triggered, this, [this]() {
        emit SigSeekForward();
    });
    this->addAction(act_seek_forward);

    QAction *act_add_volume = new QAction(this);
    act_add_volume->setShortcut(Qt::Key_Up);
    act_add_volume->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_add_volume, &QAction::triggered, this, [this]() {
        emit SigAddVolume();
    });
    this->addAction(act_add_volume);

    QAction *act_sub_volume = new QAction(this);
    act_sub_volume->setShortcut(Qt::Key_Down);
    act_sub_volume->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_sub_volume, &QAction::triggered, this, [this]() {
        emit SigSubVolume();
    });
    this->addAction(act_sub_volume);

    QAction *act_play_or_pause = new QAction(this);
    act_play_or_pause->setShortcut(Qt::Key_Space);
    act_play_or_pause->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_play_or_pause, &QAction::triggered, this, [this]() {
        emit SigPlayOrPause();
    });
    this->addAction(act_play_or_pause);

    QAction *act_step = new QAction(this);
    act_step->setShortcut(Qt::Key_S);
    act_step->setShortcutContext(Qt::ApplicationShortcut);
    connect(act_step, &QAction::triggered, this, [this]() {
        emit SigStep();
    });
    this->addAction(act_step);

    // 把action添加到当前窗口上
    this->addAction(act_about);
    this->addAction(act_open_file);
    this->addAction(act_open_stream);
    this->addAction(act_full_screen);
}
