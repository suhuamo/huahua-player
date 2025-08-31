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
signals:
    void SigPlayListCtlBtnClicked();
private:
    Ui::CtrlBar *ui;
    bool _pause;    //是否处于暂停状态
};

#endif // CTRLBAR_H
