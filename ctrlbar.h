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
    void OnSpeed(float speed);
    void OnPauseStat(bool paused);
    void OnStopFinished();
private:
    bool initUi();
    void connectSignalSlots();
    void SlotOnVolumeBtnClicked();
signals:
    void SigPlayListCtlBtnClicked();
    void SigBackBtnClicked();
    void SigNextBtnClicked();
    void SigSpeed();
    void SigPlayOrPause();
    void SigStop();
private slots:
    void on_SpeedBtn_clicked();

    void on_PlayOrPauseBtn_clicked();

    void on_OverPlayBtn_clicked();

private:
    Ui::CtrlBar *ui;
    double _last_volume_percent;    //变为静音前的音量大小
};

#endif // CTRLBAR_H
