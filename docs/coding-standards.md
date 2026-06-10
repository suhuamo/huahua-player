# 项目代码规范

## 1. 命名规范

### 1.1 类名 (Class Names)
- **命名风格**: 大驼峰 (PascalCase)
- **示例**:
  ```cpp
  class VideoCtl { };
  class AudioSeparator { };
  class CtrlBar { };
  ```

### 1.2 函数/方法名 (Function/Method Names)
- **命名风格**: 小驼峰 (camelCase)
- **示例**:
  ```cpp
  bool init();
  void connectSignalSlots();
  bool applyFilter(AVFrame *src_frame, AVFrame *dst_frame);
  ```

### 1.3 信号名 (Signal Names)
- **命名风格**: 小驼峰 (camelCase)，以 `sig` 前缀开头
- **示例**:
  ```cpp
  signals:
      void sigPlay(QString filePath);
      void sigVideoVolume(double percent);
      void sigAudioModeChanged(int mode);
      void sigStop();
  ```

### 1.4 槽函数名 (Slot Function Names)
- **命名风格**: 小驼峰 (camelCase)，以 `on` 开头
- **示例**:
  ```cpp
  private slots:
      void onPlayOrPauseBtnClicked();
      void onSpeedMenuTriggered(QAction* action);
      void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
  ```

### 1.5 UI控件变量名 (UI Widget Variable Names)
- **命名风格**: 小驼峰 (camelCase)
- **说明**: Qt Designer 中定义的 UI 控件变量，由 `uic` 工具自动生成
- **示例**:
  ```cpp
  ui->playSlider;
  ui->volumeBtn;
  ui->videoPlayTimeEdit;
  ui->showWid;
  ui->ctrlBarWid;
  ```

### 1.6 成员变量名 (Member Variable Names)
- **命名风格**: 蛇型命名 (snake_case)，以 `m_` 前缀开头
- **示例**:
  ```cpp
  private:
      double m_last_volume_percent;
      QString m_current_file;
      bool m_separating;
      int m_model_index;
  ```

### 1.7 局部变量名 (Local Variable Names)
- **命名风格**: 小驼峰 (camelCase)
- **示例**:
  ```cpp
  int currentSeconds = 0;
  QString targetPath;
  bool isReady = false;
  ```

### 1.8 常量名 (Constant Names)
- **命名风格**: 全大写，单词间用下划线分隔
- **示例**:
  ```cpp
  const int MAX_SLIDER_VALUE = 100;
  const int MIN_FRAMES = 25;
  const double NORMAL_CHANNELS = 2.0;
  ```

### 1.9 宏定义名 (Macro Names)
- **命名风格**: 全大写，单词间用下划线分隔
- **示例**:
  ```cpp
  #define AUDIO_ORIGINAL 0
  #define AUDIO_VOCALS 1
  #define SEEK_INCR 10
  ```

## 2. 槽函数连接规范

### 2.1 禁止使用自动连接
- **不要使用** `on_<控件名>_<信号名>` 这种自动连接方式
- **原因**: 自动连接方式不够清晰，难以追踪信号与槽的对应关系

**错误示例**:
```cpp
// ❌ 错误：使用自动连接
private slots:
    void on_SpeedBtn_clicked();  // Qt 会自动连接
```

### 2.2 使用显式 connect 连接
- **必须使用** `connect()` 函数显式连接信号和槽
- **推荐**: 在 `connectSignalSlots()` 方法中集中管理所有信号连接

**正确示例**:
```cpp
// ✅ 正确：使用显式 connect
void CtrlBar::connectSignalSlots()
{
    connect(ui->SpeedBtn, &QPushButton::clicked, this, &CtrlBar::onSpeedBtnClicked);
    connect(ui->PlayOrPauseBtn, &QPushButton::clicked, this, &CtrlBar::onPlayOrPauseBtnClicked);
    connect(ui->VolumeSlider, &QSlider::valueChanged, this, &CtrlBar::onVolumeSliderValueChanged);
}

private slots:
    void onSpeedBtnClicked();
    void onPlayOrPauseBtnClicked();
    void onVolumeSliderValueChanged();
```

### 2.3 槽函数命名建议
- 槽函数名应清晰表达其功能
- 推荐格式：`on` + `控件名` + `动作`
- **示例**:
  ```cpp
  void onPlayBtnClicked();          // 播放按钮点击
  void onVolumeSliderValueChanged(); // 音量滑块值改变
  void onSpeedMenuTriggered(QAction* action); // 速度菜单触发
  void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus); // 进程结束
  ```

### 2.4 信号连接位置
- **推荐**: 在 `connectSignalSlots()` 方法中集中管理
- **调用时机**: 在 `InitUi()` 或构造函数中调用 `connectSignalSlots()`

**示例**:
```cpp
bool CtrlBar::InitUi()
{
    // UI 初始化代码
    // ...

    // 连接信号槽
    connectSignalSlots();

    return true;
}

void CtrlBar::connectSignalSlots()
{
    // 所有信号连接集中在这里
    connect(ui->PlayBtn, &QPushButton::clicked, this, &CtrlBar::onPlayBtnClicked);
    connect(ui->StopBtn, &QPushButton::clicked, this, &CtrlBar::onStopBtnClicked);
    // ...
}
```

## 3. 代码组织规范

### 3.1 头文件结构
```cpp
#ifndef CLASSNAME_H
#define CLASSNAME_H

#include <必要头文件>

class ClassName : public QObject
{
    Q_OBJECT

public:
    // 公有方法
    static ClassName* GetInstance();
    bool init();
    void cleanup();

signals:
    // 信号声明
    void sigSignalName();

public slots:
    // 公有槽函数

private slots:
    // 私有槽函数
    void onBtnClicked();

private:
    // 私有方法
    void connectSignalSlots();
    bool initInternal();

    // 成员变量
    int m_member_variable;
    QString m_another_var;
};

#endif // CLASSNAME_H
```

### 3.2 源文件结构
```cpp
#include "classname.h"

// 构造函数
ClassName::ClassName(QObject *parent)
    : QObject(parent),
      m_member_variable(0),
      m_another_var("")
{
}

// 析构函数
ClassName::~ClassName()
{
    cleanup();
}

// 公有方法实现
bool ClassName::init()
{
    if (connectSignalSlots() == false)
        return false;
    // ...
    return true;
}

// 私有方法实现
void ClassName::connectSignalSlots()
{
    // 信号连接
}

// 槽函数实现
void ClassName::onBtnClicked()
{
    // 处理逻辑
}
```

## 4. 注意事项

1. **一致性**: 整个项目必须严格遵循上述命名规范
2. **可读性**: 命名应清晰表达其用途，避免使用缩写（除通用缩写如 UI、URL、ID 等）
3. **避免匈牙利命名法**: 不需要使用类型前缀（如 `nCount`, `strName`）
4. **信号槽连接**: 必须使用显式 `connect()`，禁止自动连接
5. **代码审查**: 提交代码前应检查是否符合本规范

## 5. 迁移说明

本项目已从旧的命名风格迁移到新规范：

- ✅ 方法名已统一为小驼峰
- ✅ 信号名已统一为小驼峰（sig 前缀）
- ✅ 成员变量已统一为蛇型命名（m_ 前缀）
- ✅ 槽函数已改为显式 connect 模式

**迁移前**:
```cpp
// 旧代码
bool Init();                    // 大驼峰
void SigPlay();                 // 信号大驼峰
void on_SpeedBtn_clicked();     // 自动连接
double _lastVolumePercent;      // 下划线前缀
```

**迁移后**:
```cpp
// 新代码
bool init();                    // 小驼峰
void sigPlay();                 // 信号小驼峰
void onSpeedBtnClicked();       // 显式连接（小驼峰）
double m_last_volume_percent;   // m_ + 蛇型
```