#ifndef CTRLBARWIDGET_H
#define CTRLBARWIDGET_H

#include <QWidget>

namespace Ui {
class CtrlBarWidget;
}

class CtrlBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CtrlBarWidget(QWidget *parent = 0);
    ~CtrlBarWidget();


private slots:
    void on_playOrPauseButton_clicked();

private:
    Ui::CtrlBarWidget *ui;
};

#endif // CTRLBARWIDGET_H
