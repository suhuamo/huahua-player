#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include<QListWidget>

namespace Ui {
class PlayListWidget;
}

class PlayListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayListWidget(QWidget *parent = 0);
    QListWidget *getListWidget();
    ~PlayListWidget();

private:
    Ui::PlayListWidget *ui;
};

#endif // PLAYLISTWIDGET_H
