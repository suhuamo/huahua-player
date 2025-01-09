#include "displaywidget.h"
#include "ui_displaywidget.h"

#include <QPainter>

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

void DisplayWidget::paintEvent(QPaintEvent *event)
{

    if(!pixmap_video.isNull())
    {

        QPainter painter(this);
        // 防止画布在同一时刻被两个地方操作
        // m_mutex.lock();
        QPixmap pixmap = pixmap_video.scaled(this->size(), Qt::KeepAspectRatio);
        // m_mutex.unlock();
        int x = (this->width() - pixmap.width()) / 2;
        int y = (this->height() - pixmap.height()) / 2;
        // 修改画布内容为当前 pixmap_video【即图片】
        painter.drawPixmap(x, y, pixmap);
    }
    // 重绘页面
    QWidget::paintEvent(event);
}
