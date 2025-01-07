#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include<QDebug>
#define cout qDebug()

namespace Ui {
class DisplayWidget;
}

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayWidget(QWidget *parent = 0);
    ~DisplayWidget();
    void showImage(const QImage &image);
    QImage *image = new QImage();
protected:
    void paintEvent(QPaintEvent *event) override;

private:
     QPixmap pixmap_video;
    Ui::DisplayWidget *ui;
};

#endif // DISPLAYWIDGET_H
