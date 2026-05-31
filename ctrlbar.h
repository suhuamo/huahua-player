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
    void OnUserStopFinished(); // 用户主动停止
    void OnVideopVolume(double percent);
    void OnVideoTotalSeconds(int seconds);
    void OnVideoPlaySeconds(int seconds);
    void OnPlaySliderValueChanged();
    
    // 快捷键触发的 Toast 提示
    void OnSeekForward(int targetSeconds);
    void OnSeekBack(int targetSeconds);
    void OnVolumeChanged(double percent);  // 快捷键音量变化
private:
    bool initUi();
    void connectSignalSlots();
    void SlotOnVolumeBtnClicked();
    void OnVolumeSliderValueChanged();
signals:
    void SigPlayListCtlBtnClicked();
    void SigBackBtnClicked();
    void SigNextBtnClicked();
    void SigSpeed();
    void SigPlayOrPause();
    void SigStop();
    void SigPlayVolume(double percent);
    void SigPlaySeek(double percent);
    void SigShowToast(const QString &text);  // 显示提示信息的信号
private slots:
    void on_SpeedBtn_clicked();

    void on_PlayOrPauseBtn_clicked();

    void on_OverPlayBtn_clicked();

private:
    Ui::CtrlBar *ui;
    double _last_volume_percent;    //变为静音前的音量大小
    int m_total_play_seconds;
    int m_last_play_seconds; //上次播放时间
};

#endif // CTRLBAR_H
