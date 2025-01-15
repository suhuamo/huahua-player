#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include<QListWidget>
#include<QMenu>
#include<QAction>
#include <QMouseEvent>

#include<QDebug>
#define cout qDebug()

#include"ffplay.h"

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
    virtual void mousePressEvent(QMouseEvent* event);

private:
    Ui::PlayListWidget *ui;

    QMenu menu;
    QAction actAdd;
    QAction actRemove;
    QAction actClearList;
};

#endif // PLAYLISTWIDGET_H
