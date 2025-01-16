#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include<QListWidget>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFileDialog>

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

protected:
    // 鼠标按下事件
    virtual void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onActionAddTriggered();
    void onActionRemoveTriggered();

private:
    Ui::PlayListWidget *ui;
    QMenu *contextMenu;
};

#endif // PLAYLISTWIDGET_H
