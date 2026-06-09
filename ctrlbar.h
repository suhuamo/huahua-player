#ifndef CTRLBAR_H
#define CTRLBAR_H

#include <QWidget>
#include <QMenu>
#include "audioseparator.h"

// 倍速菜单参数
#define SPEED_MENU_MIN           (0.5)     // 菜单最小倍速
#define SPEED_MENU_MAX           (3.0)     // 菜单最大倍速
#define SPEED_MENU_SCALE         (0.5)     // 菜单倍速步长

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
    void OnAudioModeChanged(int mode);     // 音频模式切换回调（更新按钮文字）
    void SyncFilterMenuState();           // 同步滤镜菜单状态
private:
    bool initUi();
    void connectSignalSlots();
    void SlotOnVolumeBtnClicked();
    void OnVolumeSliderValueChanged();
signals:
    void SigPlayListCtlBtnClicked();
    void SigBackBtnClicked();
    void SigNextBtnClicked();
    void SigSpeedChanged(float speed); // 选择指定倍速
    void SigPlayOrPause();
    void SigStop();
    void SigPlayVolume(double percent);
    void SigPlaySeek(double percent);
    void SigShowToast(const QString &text);  // 显示提示信息的信号
    void SigAudioModeChanged(int mode);      // 音频模式切换信号
    
    // 视频滤镜相关信号
    void SigSetFilterBrightness(double value);
    void SigSetFilterContrast(double value);
    void SigSetFilterSaturation(double value);
    void SigSetFilterBlur(double value);
    void SigSetFilterGrayscale(bool enabled);
    void SigSetFilterEdgeDetect(bool enabled);
    void SigSetFilterHorizontalFlip(bool enabled);
    void SigSetFilterVerticalFlip(bool enabled);
    void SigSetFilterSepia(bool enabled);
    void SigSetFilterNegative(bool enabled);
    void SigSetFilterSharpen(bool enabled);
    void SigResetFilter();
private slots:
    void OnSpeedMenuTriggered(QAction* action);
    void OnAudioModeMenuTriggered(QAction* action);
    void OnFilterMenuTriggered(QAction* action);
    void OnSpeedBtnClicked();
    void OnAudioModeBtnClicked();
    void OnFilterBtnClicked();
    void OnPlayOrPauseBtnClicked();
    void OnOverPlayBtnClicked();

private:
    Ui::CtrlBar *ui;
    double _last_volume_percent;    //变为静音前的音量大小
    int m_total_play_seconds;
    int m_last_play_seconds; //上次播放时间
    QMenu *m_speed_menu;     // 倍速下拉菜单
    QMenu *m_audio_mode_menu; // 音频模式下拉菜单
    QMenu *m_filter_menu;    // 滤镜下拉菜单
};

#endif // CTRLBAR_H
