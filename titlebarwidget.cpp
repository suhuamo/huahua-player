#include "titlebarwidget.h"
#include "ui_titlebarwidget.h"

TitleBarWidget::TitleBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBarWidget)
{
    ui->setupUi(this);
}

TitleBarWidget::~TitleBarWidget()
{
    delete ui;
}
