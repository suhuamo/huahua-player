#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include "globalhelper.h"
#include "vfilter.h"
#include"videoctl.h"

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
    ui->FilterBtn->setToolTip("滤镜");

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

    // 创建滤镜下拉菜单
    m_filter_menu = new QMenu(this);
    m_filter_menu->setObjectName("FilterMenu");

    // 颜色调整子菜单
    QMenu *color_submenu = m_filter_menu->addMenu(tr("颜色调整"));
    QAction *act_brightness_up = color_submenu->addAction(tr("亮度 +"));
    act_brightness_up->setData(QVariant::fromValue(QPair<int, double>(0, 0.1)));
    QAction *act_brightness_down = color_submenu->addAction(tr("亮度 -"));
    act_brightness_down->setData(QVariant::fromValue(QPair<int, double>(0, -0.1)));
    QAction *act_contrast_up = color_submenu->addAction(tr("对比度 +"));
    act_contrast_up->setData(QVariant::fromValue(QPair<int, double>(1, 0.1)));
    QAction *act_contrast_down = color_submenu->addAction(tr("对比度 -"));
    act_contrast_down->setData(QVariant::fromValue(QPair<int, double>(1, -0.1)));
    QAction *act_saturation_up = color_submenu->addAction(tr("饱和度 +"));
    act_saturation_up->setData(QVariant::fromValue(QPair<int, double>(2, 0.1)));
    QAction *act_saturation_down = color_submenu->addAction(tr("饱和度 -"));
    act_saturation_down->setData(QVariant::fromValue(QPair<int, double>(2, -0.1)));

    // 特效子菜单
    QMenu *effect_submenu = m_filter_menu->addMenu(tr("特效"));
    QAction *act_grayscale = effect_submenu->addAction(tr("灰度"));
    act_grayscale->setData(QVariant::fromValue(QPair<int, bool>(3, true)));
    act_grayscale->setCheckable(true);
    QAction *act_sepia = effect_submenu->addAction(tr("复古褐色"));
    act_sepia->setData(QVariant::fromValue(QPair<int, bool>(4, true)));
    act_sepia->setCheckable(true);
    QAction *act_negative = effect_submenu->addAction(tr("负片效果"));
    act_negative->setData(QVariant::fromValue(QPair<int, bool>(5, true)));
    act_negative->setCheckable(true);
    QAction *act_sharpen = effect_submenu->addAction(tr("锐化"));
    act_sharpen->setData(QVariant::fromValue(QPair<int, bool>(6, true)));
    act_sharpen->setCheckable(true);
    QAction *act_blur = effect_submenu->addAction(tr("模糊"));
    act_blur->setData(QVariant::fromValue(QPair<int, bool>(7, true)));
    act_blur->setCheckable(true);
    QAction *act_edge_detect = effect_submenu->addAction(tr("边缘检测"));
    act_edge_detect->setData(QVariant::fromValue(QPair<int, bool>(8, true)));
    act_edge_detect->setCheckable(true);

    // 几何变换子菜单
    QMenu *transform_submenu = m_filter_menu->addMenu(tr("几何变换"));
    QAction *act_hflip = transform_submenu->addAction(tr("水平翻转"));
    act_hflip->setData(QVariant::fromValue(QPair<int, bool>(9, true)));
    act_hflip->setCheckable(true);
    QAction *act_vflip = transform_submenu->addAction(tr("垂直翻转"));
    act_vflip->setData(QVariant::fromValue(QPair<int, bool>(10, true)));
    act_vflip->setCheckable(true);

    // 重置滤镜
    m_filter_menu->addSeparator();
    QAction *act_reset_filter = m_filter_menu->addAction(tr("重置滤镜"));
    act_reset_filter->setData(QVariant::fromValue(QPair<int, int>(11, 0)));

    connect(m_filter_menu, &QMenu::triggered, this, &CtrlBar::OnFilterMenuTriggered);

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

void CtrlBar::on_FilterBtn_clicked()
{
    // 同步滤镜菜单状态
    SyncFilterMenuState();
    
    // 弹出滤镜下拉菜单，显示在按钮上方
    QPoint pos = ui->FilterBtn->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() - m_filter_menu->sizeHint().height());
    m_filter_menu->exec(pos);
}

void CtrlBar::SyncFilterMenuState()
{
    // 获取当前滤镜参数
    FilterParams params = VideoCtl::GetInstance()->GetVideoFilterParams();
    
    // 更新所有可勾选的菜单选项状态
    for (QAction *action : m_filter_menu->findChildren<QAction*>()) {
        if (!action->isCheckable()) {
            continue;
        }
        
        QVariant data = action->data();
        if (data.canConvert<QPair<int, bool>>()) {
            QPair<int, bool> pair = data.value<QPair<int, bool>>();
            int type = pair.first;
            bool checked = false;
            
            switch (type) {
            case 3: // 灰度
                checked = params.grayscale;
                break;
            case 4: // 复古褐色
                checked = params.sepia;
                break;
            case 5: // 负片效果
                checked = params.negative;
                break;
            case 6: // 锐化
                checked = params.sharpen;
                break;
            case 7: // 模糊
                checked = params.blur_radius > 0;
                break;
            case 8: // 边缘检测
                checked = params.edge_detect;
                break;
            case 9: // 水平翻转
                checked = params.hflip;
                break;
            case 10: // 垂直翻转
                checked = params.vflip;
                break;
            }
            
            action->setChecked(checked);
        }
    }
}

void CtrlBar::OnFilterMenuTriggered(QAction* action)
{
    QVariant data = action->data();
    if (data.canConvert<QPair<int, double>>()) {
        QPair<int, double> pair = data.value<QPair<int, double>>();
        int type = pair.first;
        double value = pair.second;
        // 先发送参数更新信号
        switch (type) {
        case 0: // 亮度
            emit SigSetFilterBrightness(value);
            break;
        case 1: // 对比度
            emit SigSetFilterContrast(value);
            break;
        case 2: // 饱和度
            emit SigSetFilterSaturation(value);
            break;
        }
        // 获取更新后的参数并显示
        FilterParams params = VideoCtl::GetInstance()->GetVideoFilterParams();
        QString toastText;
        switch (type) {
        case 0: // 亮度
            toastText = QString(tr("亮度: %1")).arg(QString::number(params.brightness, 'f', 2));
            break;
        case 1: // 对比度
            toastText = QString(tr("对比度: %1")).arg(QString::number(params.contrast, 'f', 2));
            break;
        case 2: // 饱和度
            toastText = QString(tr("饱和度: %1")).arg(QString::number(params.saturation, 'f', 2));
            break;
        }
        emit SigShowToast(toastText);
    } else if (data.canConvert<QPair<int, bool>>()) {
        QPair<int, bool> pair = data.value<QPair<int, bool>>();
        int type = pair.first;
        bool enabled = action->isChecked(); // Qt会自动切换状态，直接获取即可
        switch (type) {
        case 3: // 灰度
            emit SigSetFilterGrayscale(enabled);
            emit SigShowToast(enabled ? tr("已启用灰度") : tr("已禁用灰度"));
            break;
        case 4: // 复古褐色
            emit SigSetFilterSepia(enabled);
            emit SigShowToast(enabled ? tr("已启用复古褐色") : tr("已禁用复古褐色"));
            break;
        case 5: // 负片效果
            emit SigSetFilterNegative(enabled);
            emit SigShowToast(enabled ? tr("已启用负片效果") : tr("已禁用负片效果"));
            break;
        case 6: // 锐化
            emit SigSetFilterSharpen(enabled);
            emit SigShowToast(enabled ? tr("已启用锐化") : tr("已禁用锐化"));
            break;
        case 7: // 模糊
            emit SigSetFilterBlur(enabled ? 2.0 : 0.0);
            emit SigShowToast(enabled ? tr("已启用模糊") : tr("已禁用模糊"));
            break;
        case 8: // 边缘检测
            emit SigSetFilterEdgeDetect(enabled);
            emit SigShowToast(enabled ? tr("已启用边缘检测") : tr("已禁用边缘检测"));
            break;
        case 9: // 水平翻转
            emit SigSetFilterHorizontalFlip(enabled);
            emit SigShowToast(enabled ? tr("已启用水平翻转") : tr("已禁用水平翻转"));
            break;
        case 10: // 垂直翻转
            emit SigSetFilterVerticalFlip(enabled);
            emit SigShowToast(enabled ? tr("已启用垂直翻转") : tr("已禁用垂直翻转"));
            break;
        }
    } else if (data.canConvert<QPair<int, int>>()) {
        QPair<int, int> pair = data.value<QPair<int, int>>();
        int type = pair.first;
        if (type == 11) { // 重置滤镜
            emit SigResetFilter();
            // 取消所有勾选状态
            for (QAction *act : m_filter_menu->findChildren<QAction*>()) {
                if (act->isCheckable()) {
                    act->setChecked(false);
                }
            }
            emit SigShowToast(tr("滤镜已重置"));
        }
    }
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
