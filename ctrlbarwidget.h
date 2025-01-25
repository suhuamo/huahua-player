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
    // 初始化页面或者清理页面数据
    void initOrClear();
    void on_playOrPauseButton_clicked();
    void setPlaySliderValue(int value);
    void setPlaySliderMaximum(int value);
    void setPlayTimeEdit(const QTime& time);
    void setTotalTimeEdit(const QTime& time);

private slots:
    void on_stopButton_clicked();

    void on_volumeButton_clicked();
    // 改变声音大小
    void changeVolumeValue(int value);

private:
    Ui::CtrlBarWidget *ui;
    // 禁音之前的音量
    int volume_value_mute_before = 0;
};

#endif // CTRLBARWIDGET_H
