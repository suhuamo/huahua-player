#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMutex>

namespace Ui {
class DisplayWidget;
}

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayWidget(QWidget *parent = 0);
    ~DisplayWidget();

public slots:
    // 设置当前帧的图片
    void setImage(QImage &image);
    // 清理画面
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // 图片对象
    QPixmap pixmap_video;
    Ui::DisplayWidget *ui;
    // 锁
    QMutex m_mutex;
};

#endif // DISPLAYWIDGET_H
