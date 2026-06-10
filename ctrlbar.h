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
    bool init();
    void onSpeed(float speed);
    void onPauseStat(bool paused);
    void onStopFinished();
    void onUserStopFinished(); // 用户主动停止
    void onVideoVolume(double percent);
    void onVideoTotalSeconds(int seconds);
    void onVideoPlaySeconds(int seconds);
    void onPlaySliderValueChanged();

    // 快捷键触发的 Toast 提示
    void onSeekForward(int targetSeconds);
    void onSeekBack(int targetSeconds);
    void onVolumeChanged(double percent);  // 快捷键音量变化
    void onAudioModeChanged(int mode);     // 音频模式切换回调（更新按钮文字）
    void syncFilterMenuState();           // 同步滤镜菜单状态
private:
    bool initUi();
    void connectSignalSlots();
    void onVolumeBtnClicked();
    void onVolumeSliderValueChanged();
signals: 
    void sigPlayListCtlBtnClicked();
    void sigBackBtnClicked();
    void sigNextBtnClicked();
    void sigSpeedChanged(float speed); // 选择指定倍速
    void sigPlayOrPause();
    void sigStop();
    void sigPlayVolume(double percent);
    void sigPlaySeek(double percent);
    void sigShowToast(const QString &text);  // 显示提示信息的信号
    void sigAudioModeChanged(int mode);      // 音频模式切换信号
    void sigSetFilterBrightness(double value);
    void sigSetFilterContrast(double value);
    void sigSetFilterSaturation(double value);
    void sigSetFilterBlur(double value);
    void sigSetFilterGrayscale(bool enabled);
    void sigSetFilterEdgeDetect(bool enabled);
    void sigSetFilterHorizontalFlip(bool enabled);
    void sigSetFilterVerticalFlip(bool enabled);
    void sigSetFilterSepia(bool enabled);
    void sigSetFilterNegative(bool enabled);
    void sigSetFilterSharpen(bool enabled);
    void sigResetFilter();
private slots:
    void onSpeedMenuTriggered(QAction* action);
    void onAudioModeMenuTriggered(QAction* action);
    void onFilterMenuTriggered(QAction* action);
    void onSpeedBtnClicked();
    void onAudioModeBtnClicked();
    void onFilterBtnClicked();
    void onPlayOrPauseBtnClicked();
    void onOverPlayBtnClicked();

private:
    Ui::CtrlBar *ui;
    double m_last_volume_percent;    //变为静音前的音量大小
    int m_total_play_seconds;
    int m_last_play_seconds; //上次播放时间
    QMenu *m_speed_menu;     // 倍速下拉菜单
    QMenu *m_audio_mode_menu; // 音频模式下拉菜单
    QMenu *m_filter_menu;    // 滤镜下拉菜单
};

#endif // CTRLBAR_H
