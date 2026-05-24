#include "show.h"
#include "ui_show.h"
#include"globalhelper.h"
#include"videoctl.h"
#include <QDebug>
#include <QtMath>
#include <QMutex>

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

     return true;
}

bool Show::connectionSignalSlots()
{
    bool bRet;

//    todo:这是何意为？为什么自己链接自己，为什么外面连接 &Show::SigPlay 的时候不直接连接 &Show::OnPlay 得了
    bRet = connect(this, &Show::SigPlay, this, &Show::OnPlay);

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