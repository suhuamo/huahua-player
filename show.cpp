#include "show.h"
#include "ui_show.h"
#include"globalhelper.h"
#include"videoctl.h"
#include <QDebug>
#include <QtMath>
#include <QMutex>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

extern QMutex g_show_rect_mutex;

Show::Show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Show)
{
    ui->setupUi(this);
//    接受拖拽事件，可以直接把视频文件拖动到播放界面开始播放
    setAcceptDrops(true);

//    不做下面两个配置的添加，会导致：当窗口从后台恢复时，Qt 会清除 widget 的背景，造成黑屏。
    //防止过度刷新显示，告诉 Qt 这个 widget 会处理自己的所有绘制，Qt 不需要清除背景【即SDL去处理的绘制】
    this->setAttribute(Qt::WA_OpaquePaintEvent);
    //    防止 Qt 自动刷新 QLabel
    ui->label->setUpdatesEnabled(false);

    m_nLastFrameWidth = 0;
    m_nLastFrameHeight = 0;
    
    // 初始化提示标签（使用顶层窗口实现透明效果）
    m_toastLabel = new QLabel(this);
    m_toastLabel->setVisible(false);
    
    // 初始化定时器
    m_toastTimer = new QTimer(this);
    m_toastTimer->setSingleShot(true);
    
    // 初始化快捷键提示标签（居中显示）
    m_shortcutHintLabel = new QLabel(this);
    m_shortcutHintLabel->setVisible(false);
    
    // 初始化快捷键提示定时器
    m_shortcutHintTimer = new QTimer(this);
    m_shortcutHintTimer->setSingleShot(true);
}

Show::~Show()
{
    delete ui;
}

bool Show::Init()
{
    if(initUi() == false) {
        return false;
    }
    if(connectionSignalSlots() == false) {
        return false;
    }
    return true;
}

void Show::OnPlay(QString strFile)
{

//    todo：在这里或者在其他地方必须校验必须是可播放的视频文件，而且当前文件得存在，不能突然被删除了。防止打开失败导致程序崩溃
    VideoCtl::GetInstance()->start_play(strFile, ui->label->winId());
}

void Show::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty()) {
        return;
    }

    for(QUrl url: urls) {
        QString strFileName = url.toLocalFile();
        emit SigOpenFile(strFileName);
//        如果拖拽了多个，我们只播放第一个文件，不过这个合理吗？
        break;
    }
}

void Show::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

bool Show::initUi()
{
//    加载qss
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/show.css"));

    ui->label->clear();
//    没有视频的时候测试图片是否能正常显示
#if 0
    // 使用QPixmap加载资源系统中的图片
    ui->label->setPixmap(QPixmap(":/res/player.png"));

    // 可选：设置QLabel允许图片缩放以适应其大小
    ui->label->setScaledContents(true);
#endif

    // 设置样式 - 只设置文字颜色
    m_toastLabel->setStyleSheet(
        "QLabel {"
        "    color: #4169E1;"
        "    border: none;"
        "}"
    );
    
    // ⭐ 关键：设置为顶层窗口，避免继承父控件的样式表和背景
    m_toastLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_toastLabel->setAttribute(Qt::WA_TranslucentBackground, true);

    
    // 设置字体
    QFont font = m_toastLabel->font();
    font.setPointSize(14);
    m_toastLabel->setFont(font);
    
    // 设置快捷键提示标签样式（居中显示）
    m_shortcutHintLabel->setStyleSheet(
        "QLabel {"
        "    color: #1cdb20;"
        "    border: none;"
        "}"
    );
    
    // 设置为顶层窗口，避免继承父控件的样式表和背景
    m_shortcutHintLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_shortcutHintLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    
    // 设置字体
    m_shortcutHintLabel->setFont(font);

    return true;
}

bool Show::connectionSignalSlots()
{
    bool bRet = true;

//    todo:这是何意为？为什么自己链接自己，为什么外面连接 &Show::SigPlay 的时候不直接连接 &Show::OnPlay 得了
    bRet = connect(this, &Show::SigPlay, this, &Show::OnPlay);
    connect(m_toastTimer, &QTimer::timeout, this, &Show::OnToastTimeout);
    connect(m_shortcutHintTimer, &QTimer::timeout, this, &Show::OnShortcutHintTimeout);

    return bRet;
}

void Show::OnFrameDimensionsChanged(int nFrameWidth, int nFrameHeight)
{
    qDebug() << "Show::OnFrameDimensionsChanged" << nFrameWidth << nFrameHeight;

    // Only update if dimensions actually changed
    if (m_nLastFrameWidth == nFrameWidth && m_nLastFrameHeight == nFrameHeight) {
        return;
    }

    m_nLastFrameWidth = nFrameWidth;
    m_nLastFrameHeight = nFrameHeight;

    ChangeShow();
}

void Show::ChangeShow()
{
    g_show_rect_mutex.lock();
    qDebug() << "Show::ChangeShow - size:" << width() << "x" << height() << "frame:" << m_nLastFrameWidth << "x" << m_nLastFrameHeight;
    if(m_nLastFrameWidth == 0 && m_nLastFrameHeight == 0) {
        qDebug() << "Show::ChangeShow - no frame dimensions, setting label to full size";
        ui->label->setGeometry(0, 0, width(), height());
    } else {
        float aspect_ratio;
        int width, height, x, y;
        int scr_width = this->width();
        int scr_height = this->height();

        aspect_ratio = (float)m_nLastFrameWidth / (float)m_nLastFrameHeight;
        qDebug() << "Show::ChangeShow - aspect_ratio:" << aspect_ratio;

        height = scr_height;
        width = lrint(height * aspect_ratio) & ~1;
        if(width > scr_width) {
            width = scr_width;
            height = lrint(width / aspect_ratio) & ~1;
        }
        x = (scr_width - width) / 2;
        y = (scr_height - height) / 2;

        qDebug() << "Show::ChangeShow - final rect:" << x << y << width << height;
        ui->label->setGeometry(x, y, width, height);
    }
    g_show_rect_mutex.unlock();
}

void Show::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ChangeShow();
}

void Show::ShowToast(const QString &text)
{
    if (m_toastLabel == nullptr) {
        return;
    }

    // 设置文本
    m_toastLabel->setText(text);

    // 根据文本长度调整大小，添加一些边距
    QFontMetrics fm(m_toastLabel->font());
    m_toastLabel->setFixedSize(fm.width(text) + 30, fm.height() + 16);  // 左右各15px，上下各8px的边距

    // ⭐ 关键：对于顶层窗口，需要计算全局位置并调用 show()
    // 将局部坐标转换为全局坐标（左上角位置）
    QPoint globalPos = this->mapToGlobal(QPoint(10, 10));
    m_toastLabel->move(globalPos);
    
    // 显示标签并确保在最上层，但不获取焦点（避免影响键盘事件）
    m_toastLabel->show();
    m_toastLabel->raise();
    // 注意：不调用 activateWindow()，避免焦点转移

    // 启动定时器，2秒后隐藏
    m_toastTimer->start(2000);
}

void Show::OnToastTimeout()
{
    if (m_toastLabel) {
        m_toastLabel->hide();  // 顶层窗口使用 hide()
    }
}

void Show::ShowShortcutHint(const QString &text)
{
    if (m_shortcutHintLabel == nullptr) {
        return;
    }

    // 设置文本
    m_shortcutHintLabel->setText(text);

    // 根据文本长度调整大小，添加一些边距
    QFontMetrics fm(m_shortcutHintLabel->font());
    m_shortcutHintLabel->setFixedSize(fm.width(text) + 30, fm.height() + 16);  // 左右各15px，上下各8px的边距

    // 将文本显示在窗口正上方居中位置（距离顶部30px）
    int x = (this->width() - m_shortcutHintLabel->width()) / 2;
    int y = 30; // 距离顶部30px
    QPoint globalPos = this->mapToGlobal(QPoint(x, y));
    m_shortcutHintLabel->move(globalPos);
    
    // 显示标签并确保在最上层，但不获取焦点（避免影响键盘事件）
    m_shortcutHintLabel->show();
    m_shortcutHintLabel->raise();
    // 注意：不调用 activateWindow()，避免焦点转移

    // 启动定时器，2秒后隐藏
    m_shortcutHintTimer->start(2000);
}

void Show::HideShortcutHint()
{
    if (m_shortcutHintLabel) {
        m_shortcutHintLabel->hide();
        m_shortcutHintTimer->stop(); // 停止定时器
    }
}

void Show::OnShortcutHintTimeout()
{
    if (m_shortcutHintLabel) {
        m_shortcutHintLabel->hide();
    }
}

void Show::keyReleaseEvent(QKeyEvent *event)
{
    // 只有在全屏状态下才处理键盘事件--如果逻辑控制没问题的话，是不需要加这一行判断的，如果出现问题了，可以把这个打开
//    if (!isFullScreen()) {
//        QWidget::keyReleaseEvent(event);
//        return;
//    }
    
    int key = event->key();
    
    // 使用全局工具函数获取按键名称
    QString keyName = GlobalHelper::GetKeyName(key);
    
    qDebug() << "Show::keyReleaseEvent:" << keyName;
    
    // 发送相应的信号
    switch (key) {
        case Qt::Key_Space:
            emit SigPlayOrPause();
            break;
        case Qt::Key_Escape:
            emit SigExitFullScreen(); // ESC退出全屏
            break;
        case Qt::Key_Left:
            emit SigSeekBack();
            break;
        case Qt::Key_Right:
            emit SigSeekForward();
            break;
        case Qt::Key_Up:
            emit SigAddVolume();
            break;
        case Qt::Key_Down:
            emit SigSubVolume();
            break;
        case Qt::Key_S:
            emit SigStep();
            break;
        default:
            break;
    }
    
    QWidget::keyReleaseEvent(event);
}
