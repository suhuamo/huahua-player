#include "displaywidget.h"
#include "ui_displaywidget.h"

DisplayWidget::DisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayWidget)
{
    ui->setupUi(this);
}

DisplayWidget::~DisplayWidget()
{
    delete ui;
}

void DisplayWidget::setImage(QImage &image)
{
    // 更新画布的图片
    pixmap_video = QPixmap::fromImage(image);
    // 调用paintEvent重绘页面
    update();
}

void DisplayWidget::clear()
{
    // 清空画布图片
    pixmap_video = QPixmap();
    // 调用paintEvent重绘页面
    update();
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    // 如果现在有画布内容了
    if(!pixmap_video.isNull())
    {
        m_mutex.lock();
        // 创建画笔对象
        QPainter painter(this);
        // 根据当前图片创建另一个图片对象【适配尺寸】
        QPixmap pixmap = pixmap_video.scaled(this->size(), Qt::KeepAspectRatio);
        // 设置画布宽高
        int x = (this->width() - pixmap.width()) / 2;
        int y = (this->height() - pixmap.height()) / 2;
        // 修改画布内容为当前 pixmap_video【即解析出来的帧图片】
        painter.drawPixmap(x, y, pixmap);
        m_mutex.unlock();
    }
    // 重绘页面
    QWidget::paintEvent(event);
}
