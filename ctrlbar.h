#ifndef CTRLBAR_H
#define CTRLBAR_H

#include <QWidget>

namespace Ui {
class CtrlBar;
}

class CtrlBar : public QWidget
{
    Q_OBJECT

public:
    explicit CtrlBar(QWidget *parent = 0);
    ~CtrlBar();
    bool Init();
private:
    bool initUi();
    void connectSignalSlots();
    void SlotOnPlayOrPauseBtnClicked();
    void SlotOnVolumeBtnClicked();
signals:
    void SigPlayListCtlBtnClicked();
    void SigBackBtnClicked();
    void SigNextBtnClicked();
private:
    Ui::CtrlBar *ui;
    bool _pause;    //是否处于暂停状态
    double _last_volume_percent;    //变为静音前的音量大小
};

#endif // CTRLBAR_H
