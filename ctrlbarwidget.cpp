#include "ctrlbarwidget.h"
#include "ui_ctrlbarwidget.h"

CtrlBarWidget::CtrlBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBarWidget)
{
    ui->setupUi(this);
}

CtrlBarWidget::~CtrlBarWidget()
{
    delete ui;
}
