#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include"globalhelper.h"

CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBar),
    _last_volume_percent(0.5),
    m_total_play_seconds(0),
    m_last_play_seconds(-1)
{
    ui->setupUi(this);
}

CtrlBar::~CtrlBar()
{
    delete ui;
}

bool CtrlBar::Init()
{
    if(initUi() == false) {
        return false;
    }

    connectSignalSlots();

//    设置初始音量
    double percent = -1.0;
//    从本地配置文件中读取
    GlobalHelper::GetPlayVolume(percent);
    if(percent != -1.0) {
        OnVideopVolume(percent);
        emit SigPlayVolume(percent);
    } else {
        OnVideopVolume(_last_volume_percent);
        emit SigPlayVolume(_last_volume_percent);
    }

    return true;
}

void CtrlBar::OnSpeed(float speed)
{
    ui->SpeedBtn->setText(QString("倍数:%1").arg(speed));

    // 同步菜单选中状态
    for (QAction *action : m_speed_menu->actions()) {
        action->setChecked(qFuzzyCompare(action->data().toFloat(), speed));
    }
}

bool CtrlBar::initUi()
{
    //加载qss
    setStyleSheet(GlobalHelper::GetQssStr(":/res/qss/ctrlbar.css"));
//    设置按钮图片
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    GlobalHelper::SetIcon(ui->OverPlayBtn, 12, QChar(0xf04d));
    GlobalHelper::SetIcon(ui->BackBtn, 12, QChar(0xf048));
    GlobalHelper::SetIcon(ui->NextBtn, 12, QChar(0xf051));
    GlobalHelper::SetIcon(ui->PlayListCtlBtn, 12, QChar(0xf036));
    GlobalHelper::SetIcon(ui->SettingBtn, 12, QChar(0xf013));
    GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
//    设置鼠标悬浮提示
    ui->PlayOrPauseBtn->setToolTip("点击播放");
    ui->OverPlayBtn->setToolTip("结束播放");
    ui->BackBtn->setToolTip("上一个");
    ui->NextBtn->setToolTip("下一个");
    ui->PlayListCtlBtn->setToolTip("播放列表");
    ui->SettingBtn->setToolTip("设置");
    ui->VolumeBtn->setToolTip("点击静音");
    ui->SpeedBtn->setToolTip("倍速");
    ui->AudioModeBtn->setToolTip("音频模式");

    // 创建倍速下拉菜单，步长由 SPEED_MENU_SCALE 控制
    m_speed_menu = new QMenu(this);
    m_speed_menu->setObjectName("SpeedMenu");
    // 使用 QActionGroup 实现互斥单选，防止点击已选中项时取消选中
    QActionGroup *speedGroup = new QActionGroup(this);
    for (float speed = SPEED_MENU_MIN; speed <= SPEED_MENU_MAX + 0.001f; speed += SPEED_MENU_SCALE) {
        QString text = QString("%1x").arg(speed, 0, 'f', 1);
        QAction *action = m_speed_menu->addAction(text);
        action->setData(speed);
        action->setCheckable(true);
        speedGroup->addAction(action);
        // 启动时默认选择1.0倍速
        if (qFuzzyCompare(speed, 1.0f)) {
            action->setChecked(true);
        }
    }
    connect(m_speed_menu, &QMenu::triggered, this, &CtrlBar::OnSpeedMenuTriggered);

    // 创建音频模式下拉菜单
    m_audio_mode_menu = new QMenu(this);
    m_audio_mode_menu->setObjectName("AudioModeMenu");

    // 使用 QActionGroup 实现互斥单选，防止点击已选中项时取消选中
    QActionGroup *audioModeGroup = new QActionGroup(this);

    struct AudioModeItem {
        QString text;
        int mode;
    };
    QVector<AudioModeItem> audioModes = {
        {tr("原声"), AUDIO_ORIGINAL},
        {tr("人声"), AUDIO_VOCALS},
        {tr("伴奏"), AUDIO_ACCOMPANIMENT},
        {tr("鼓点"), AUDIO_DRUMS},
        {tr("贝斯"), AUDIO_BASS},
        {tr("其他"), AUDIO_OTHER}
    };
    for (const auto &item : audioModes) {
        QAction *action = m_audio_mode_menu->addAction(item.text);
        action->setData(item.mode);
        action->setCheckable(true);
        audioModeGroup->addAction(action);
        if (item.mode == AUDIO_ORIGINAL) {
            action->setChecked(true);
        }
    }
    connect(m_audio_mode_menu, &QMenu::triggered, this, &CtrlBar::OnAudioModeMenuTriggered);

    return true;
}

void CtrlBar::connectSignalSlots()
{
//    todo：SettingBtn 是显示出来调节视频的参数，比如编码格式，编码效率等，等后续再开发
    connect(ui->PlayListCtlBtn, &QPushButton::clicked, this, &CtrlBar::SigPlayListCtlBtnClicked);
    connect(ui->VolumeBtn, &QPushButton::clicked, this, &CtrlBar::SlotOnVolumeBtnClicked);
    connect(ui->BackBtn, &QPushButton::clicked, this, &CtrlBar::SigBackBtnClicked);
    connect(ui->NextBtn, &QPushButton::clicked, this, &CtrlBar::SigNextBtnClicked);
    connect(ui->VolumeSlider, &CustomSlider::SigSliderValueChanged, this, &CtrlBar::OnVolumeSliderValueChanged);
    connect(ui->PlaySlider, &CustomSlider::SigSliderValueChanged, this, &CtrlBar::OnPlaySliderValueChanged);
}

void CtrlBar::OnPauseStat(bool paused)
{
    qDebug() << "CtrlBar::OnPauseStat" << paused;
//    需要变成暂停状态
    if (paused)
    {
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
        ui->PlayOrPauseBtn->setToolTip("点击播放");
    }
    else
    {
//        此时状态为播放状态，按钮变为暂停按钮，提示为点击暂停
        GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04c));
        ui->PlayOrPauseBtn->setToolTip("点击暂停");
    }
}

void CtrlBar::OnStopFinished()
{
    ui->PlaySlider->setValue(0);
    QTime stopTime(0, 0, 0);
    ui->VideoPlayTimeTimeEdit->setTime(stopTime);
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    ui->PlayOrPauseBtn->setToolTip("点击播放");

    m_last_play_seconds = -1;
}

void CtrlBar::OnUserStopFinished()
{
    ui->PlaySlider->setValue(0);
    QTime stopTime(0, 0, 0);
    ui->VideoTotalTimeTimeEdit->setTime(stopTime);
    ui->VideoPlayTimeTimeEdit->setTime(stopTime);
    GlobalHelper::SetIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    ui->PlayOrPauseBtn->setToolTip("点击播放");

    m_last_play_seconds = -1;
    m_total_play_seconds = 0;
}

void CtrlBar::OnVideopVolume(double percent)
{
    ui->VolumeSlider->setValue(percent * MAX_SLIDER_VALUE);
    _last_volume_percent = percent;

    if (_last_volume_percent == 0)
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeBtn->setToolTip("点击恢复音量");
    }
    else
    {
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeBtn->setToolTip("点击静音");
    }
    GlobalHelper::SavePlayVolume(percent);

    // 显示音量提示
    int volumePercent = static_cast<int>(percent * 100);
    emit SigShowToast(QString("声音：%1%").arg(volumePercent));

}

void CtrlBar::OnVideoTotalSeconds(int seconds)
{
    m_total_play_seconds = seconds;

    int thh, tmm, tss;
    thh = seconds / 3600;
    tmm = (seconds % 3600) / 60;
    tss = seconds % 60;
    QTime totalTime(thh, tmm, tss);
    ui->VideoTotalTimeTimeEdit->setTime(totalTime);
}

void CtrlBar::OnVideoPlaySeconds(int seconds)
{
    // 优化，如果当前 seconds 和上一次 seconds 一样，那么就不用更新ui了
    if(m_last_play_seconds == seconds) {
        return;
    }
    m_last_play_seconds = seconds;

    int thh, tmm, tss;
    thh = seconds / 3600;
    tmm = (seconds % 3600) / 60;
    tss = seconds % 60;
    QTime totalTime(thh, tmm, tss);
    ui->VideoPlayTimeTimeEdit->setTime(totalTime);

    if(seconds >= 0) {
        ui->PlaySlider->setValue(seconds * 1.0 / m_total_play_seconds * MAX_SLIDER_VALUE);
    }
}

void CtrlBar::OnPlaySliderValueChanged()
{
    double percent = ui->PlaySlider->value() * 1.0 / ui->PlaySlider->maximum();
    emit SigPlaySeek(percent);
    
    // 计算跳转的时间
    int seconds = static_cast<int>(percent * m_total_play_seconds);
    int thh = seconds / 3600;
    int tmm = (seconds % 3600) / 60;
    int tss = seconds % 60;
    QString timeStr = QString("%1:%2:%3")
        .arg(thh, 2, 10, QChar('0'))
        .arg(tmm, 2, 10, QChar('0'))
        .arg(tss, 2, 10, QChar('0'));
    
    emit SigShowToast(QString("跳转到 %1").arg(timeStr));
}

void CtrlBar::SlotOnVolumeBtnClicked()
{
    // 静音和恢复静音并不会改变 _last_volume_percent 的值，因为 _last_volume_percent 只会被滑动条滑动改变
//    如果此时是非静音状态
    if(ui->VolumeBtn->text() == QChar(0xf028)) {
        ui->VolumeSlider->setValue(0);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeBtn->setToolTip("点击恢复音量");
        emit SigPlayVolume(0);
        emit SigShowToast("声音: 0%");
    } else {
//        恢复之前的音量百分比
        ui->VolumeSlider->setValue(_last_volume_percent * MAX_SLIDER_VALUE);
        GlobalHelper::SetIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeBtn->setToolTip("点击静音");
        emit SigPlayVolume(_last_volume_percent);
        
        int volumePercent = static_cast<int>(_last_volume_percent * 100);
        emit SigShowToast(QString("声音: %1%").arg(volumePercent));
    }
}

void CtrlBar::OnVolumeSliderValueChanged()
{
    double percent = ui->VolumeSlider->value() * 1.0 / ui->VolumeSlider->maximum();
    emit SigPlayVolume(percent);

    OnVideopVolume(percent);
    
    // 显示音量提示
    int volumePercent = static_cast<int>(percent * 100);
    emit SigShowToast(QString("声音：%1%").arg(volumePercent));
}

void CtrlBar::on_SpeedBtn_clicked()
{
    // 弹出倍速下拉菜单，显示在按钮上方
    QPoint pos = ui->SpeedBtn->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() - m_speed_menu->sizeHint().height());
    m_speed_menu->exec(pos);
}

void CtrlBar::OnSpeedMenuTriggered(QAction* action)
{
    float speed = action->data().toFloat();
    emit SigSpeedChanged(speed);
    emit SigShowToast(QString("倍速：%1x").arg(speed, 0, 'f', speed == int(speed) ? 1 : 2));
}

void CtrlBar::on_PlayOrPauseBtn_clicked()
{
    emit SigPlayOrPause();
}

void CtrlBar::on_OverPlayBtn_clicked()
{
    emit SigStop();
}

// 快捷键：快进
void CtrlBar::OnSeekForward(int targetSeconds)
{
    int hours = targetSeconds / 3600;
    int minutes = (targetSeconds % 3600) / 60;
    int seconds = targetSeconds % 60;
    emit SigShowToast(QString("快进: %1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
}

// 快捷键：快退
void CtrlBar::OnSeekBack(int targetSeconds)
{
    int hours = targetSeconds / 3600;
    int minutes = (targetSeconds % 3600) / 60;
    int seconds = targetSeconds % 60;
    emit SigShowToast(QString("快退: %1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
}

// 快捷键：音量变化
void CtrlBar::OnVolumeChanged(double percent)
{
    // 更新 UI
    OnVideopVolume(percent);
    
    // 显示音量提示
    int volumePercent = static_cast<int>(percent * 100);
    emit SigShowToast(QString("声音：%1%").arg(volumePercent));
}

void CtrlBar::on_AudioModeBtn_clicked()
{
    // 弹出音频模式下拉菜单，显示在按钮上方
    QPoint pos = ui->AudioModeBtn->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() - m_audio_mode_menu->sizeHint().height());
    m_audio_mode_menu->exec(pos);
}

void CtrlBar::OnAudioModeMenuTriggered(QAction* action)
{
    int mode = action->data().toInt();
    emit SigAudioModeChanged(mode);
}

void CtrlBar::OnAudioModeChanged(int mode)
{
    // 更新按钮文字
    QString text;
    switch ((AudioMode)mode) {
    case AUDIO_ORIGINAL:      text = tr("原声"); break;
    case AUDIO_VOCALS:        text = tr("人声"); break;
    case AUDIO_ACCOMPANIMENT: text = tr("伴奏"); break;
    case AUDIO_DRUMS:         text = tr("鼓点"); break;
    case AUDIO_BASS:          text = tr("贝斯"); break;
    case AUDIO_OTHER:         text = tr("其他"); break;
    default:                  text = tr("原声"); break;
    }
    ui->AudioModeBtn->setText(text);

    // 同步菜单选中状态
    for (QAction *action : m_audio_mode_menu->actions()) {
        action->setChecked(action->data().toInt() == mode);
    }
}
