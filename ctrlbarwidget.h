#ifndef CTRLBARWIDGET_H
#define CTRLBARWIDGET_H

#include <QWidget>
#include"state.h"
#include"ffplay.h"

namespace Ui {
class CtrlBarWidget;
}

class CtrlBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CtrlBarWidget(QWidget *parent = 0);
    ~CtrlBarWidget();


public slots:
    void on_playOrPauseButton_clicked();
    void setPlaySliderValue(int value);
    void setPlaySliderMaximum(int value);
    void setPlayTimeEdit(const QTime& time);
    void setTotalTimeEdit(const QTime& time);

private slots:
    void on_stopButton_clicked();

private:
    Ui::CtrlBarWidget *ui;
};

#endif // CTRLBARWIDGET_H
