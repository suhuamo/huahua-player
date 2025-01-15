#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlayListWidget::PlayListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListWidget)
{
    ui->setupUi(this);
    // 将菜单与播放列表UI绑定起来
    // menu.setParent(ui->listWidget);
    // actAdd.setParent(ui->listWidget);
    // actRemove.setParent(ui->listWidget);
    // actClearList.setParent(ui->listWidget);

    // menu.setParent(this);
    // actAdd.setParent(this);
    // actRemove.setParent(this);
    // actClearList.setParent(this);

    // // 设置菜单栏内容
    // actAdd.setText("添加");
    // actRemove.setText("移除所选项");
    // actClearList.setText("清空列表");
    // // 将菜单栏添加到菜单界面上
    // menu.addAction(&actRemove);
    // menu.addAction(&actAdd);
    // menu.addAction(&actClearList);
}

QListWidget *PlayListWidget::getListWidget()
{
    return ui->listWidget;
}

PlayListWidget::~PlayListWidget()
{
    delete ui;
}

void PlayListWidget::mousePressEvent(QMouseEvent *event)
{
    cout << "点击按钮";
    if (event->button() == Qt::RightButton)
    {
        cout << "点击右键";
        menu.exec(QCursor::pos());
    }
}
