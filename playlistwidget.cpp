#include "playlistwidget.h"
#include "ui_playlistwidget.h"
#include<QDebug>
#define cout qDebug()
#include"ffplay.h"

PlayListWidget::PlayListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListWidget)
{
    ui->setupUi(this);
}

QListWidget *PlayListWidget::getListWidget()
{
    return ui->listWidget;
}

PlayListWidget::~PlayListWidget()
{
    delete ui;
}
