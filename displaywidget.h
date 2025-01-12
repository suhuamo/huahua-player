#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>

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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap pixmap_video;
    Ui::DisplayWidget *ui;
};

#endif // DISPLAYWIDGET_H
