# 花花播放器 (Huahua Player)

<div align="center">

![Qt](https://img.shields.io/badge/Qt-5.x-green.svg)
![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)
![FFmpeg](https://img.shields.io/badge/FFmpeg-4.2.1-red.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

一个基于 Qt + FFmpeg + SDL 的跨平台多媒体播放器，支持视频播放、音频解码、播放列表管理等功能。

**本项目参考于**: [playerdemo](https://github.com/itisyang/playerdemo)

</div>

---

## 📋 目录

- [项目简介](#项目简介)
- [主要功能](#主要功能)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [环境要求](#环境要求)
- [编译与构建](#编译与构建)
- [使用说明](#使用说明)
- [核心架构](#核心架构)
- [开发指南](#开发指南)
- [常见问题](#常见问题)
- [许可证](#许可证)
- [致谢](#致谢)

---

## 📖 项目简介

花花播放器是一个用于学习 FFmpeg 和 Qt 开发的入门级多媒体播放器项目。该项目实现了基本的视频播放功能，包括视频解码、音频播放、播放控制、播放列表管理等核心功能。

本项目适合作为：
- FFmpeg 入门学习项目
- Qt 多媒体应用开发参考
- 音视频同步机制学习案例
- C++ 多线程编程实践

---

## ✨ 主要功能

### 基础播放功能
- ✅ 视频文件播放（支持常见格式：MP4, AVI, MKV, FLV 等）
- ✅ 音频解码与播放
- ✅ 播放/暂停控制
- ✅ 停止播放
- ✅ 进度条拖拽跳转
- ✅ 音量调节
- ✅ 静音切换

### 高级功能
- 🎯 播放速度调节（0.25x - 3.0x）
- 🎯 上一曲/下一曲切换
- 🎯 全屏模式切换
- 🎯 窗口最大化/最小化
- 🎯 键盘快捷键支持
- 🎯 鼠标拖拽移动窗口
- 🎯 **操作提示显示** - 音量和进度调整时显示实时反馈
- 🎯 **逐帧播放** - 按下 `S` 键逐帧播放视频

### 播放列表管理
- 📝 支持文件拖拽添加到播放列表
- 📝 双击播放列表项播放
- 📝 播放状态显示

### 界面特性
- 🎨 自定义 UI 样式（QSS 样式表）
- 🎨 无边框窗口设计
- 🎨 响应式布局
- 🎨 字体图标支持（Font Awesome）

---

## 🛠 技术栈

### 核心框架
- **Qt 5.x** - GUI 应用框架
- **C++11** - 编程语言
- **FFmpeg 4.2.1** - 音视频编解码库
- **SDL2** - 音频输出和渲染引擎

### 关键技术
- 多线程编程（std::thread）
- 音视频同步机制
- 生产者-消费者队列模型
- 信号槽机制（Qt Signal-Slot）
- 事件驱动编程

### 依赖库
```
FFmpeg 组件:
├── avcodec      - 编解码器
├── avformat     - 容器格式处理
├── avutil       - 工具函数
├── swscale      - 图像缩放和格式转换
├── swresample   - 音频重采样
└── avdevice     - 设备输入输出

SDL2:
└── SDL2         - 音频播放和纹理渲染
```

---

## 📁 项目结构

```
huahua-player/
├── dll/                    # 动态链接库文件
│   ├── x64/               # 64位 DLL 文件
│   └── x86/               # 32位 DLL 文件
├── lib/                    # 第三方库文件
│   ├── SDL2/              # SDL2 库
│   ├── ffmpeg-4.2.1/      # FFmpeg 库
│   └── windows-kits/      # Windows SDK
├── res/                    # 资源文件
│   ├── qss/               # Qt 样式表
│   │   ├── ctrlbar.css    # 控制栏样式
│   │   ├── mainwid.css    # 主窗口样式
│   │   ├── playlist.css   # 播放列表样式
│   │   ├── show.css       # 显示区域样式
│   │   └── title.css      # 标题栏样式
│   ├── Player.ico         # 应用图标
│   ├── fontawesome-webfont.ttf  # 字体图标
│   └── menu.json          # 菜单配置
├── *.h                     # 头文件
├── *.cpp                   # 源文件
├── *.ui                    # Qt UI 设计文件
├── *.qrc                   # Qt 资源文件
└── flower-player.pro       # Qt 项目配置文件
```

### 核心模块说明

| 模块 | 文件 | 功能描述 |
|------|------|----------|
| 主窗口 | `mainwindow.h/cpp` | 应用程序主窗口，整合所有子模块 |
| 视频控制 | `videoctl.h/cpp` | FFmpeg 解码核心，音视频同步控制 |
| 播放列表 | `playlist.h/cpp` | 播放列表管理，支持拖拽添加文件 |
| 控制栏 | `ctrlbar.h/cpp` | 播放控制界面（播放/暂停/进度/音量） |
| 显示区域 | `show.h/cpp` | 视频渲染显示区域，包含 Toast 提示功能 |
| 标题栏 | `title.h/cpp` | 自定义标题栏（最小化/最大化/关闭） |
| 数据控制 | `datactl.h` | 数据结构定义（队列、帧、解码器等） |
| 音频变速 | `sonic.h/cpp` | 音频变速不变调算法实现 |
| 全局助手 | `globalhelper.h/cpp` | 全局辅助函数 |

---

## 💻 环境要求

### 开发环境
- **操作系统**: Windows 7/10/11, Linux, macOS
- **编译器**: GCC 5+, MSVC 2015+, Clang 3.8+
- **Qt 版本**: Qt 5.9 或更高版本
- **CMake**: 3.10+（可选，用于构建）

### 运行时依赖
- Qt 5.x 运行时库
- FFmpeg 4.2.1 动态库
- SDL2 运行时库

### Windows 特定要求
- Visual Studio 2015 或更高版本（如使用 MSVC）
- MinGW-w64（如使用 GCC）
- Windows SDK

---

## 🔧 编译与构建

### Windows (Qt Creator)

1. **克隆项目**
   ```bash
   git clone <repository-url>
   cd huahua-player
   ```

2. **准备依赖库**
   - 确保 `dll/` 目录下包含正确的 FFmpeg 和 SDL2 DLL 文件
   - 确保 `lib/` 目录下包含对应的 .lib 文件和头文件

3. **打开项目**
   - 使用 Qt Creator 打开 `flower-player.pro`
   - Qt Creator 会自动检测编译器（MSVC 或 MinGW）

4. **配置构建套件**
   - 选择 Desktop Qt 5.x MSVC/MinGW 64-bit 或 32-bit
   - 确保与 DLL 文件架构匹配（x64 或 x86）

5. **构建项目**
   ```
   点击 Qt Creator 左下角的 "构建" 按钮
   或使用快捷键: Ctrl+B
   ```

6. **运行程序**
   ```
   点击 "运行" 按钮或按 Ctrl+R
   ```

### Windows (命令行)

```bash
# 进入项目目录
cd huahua-player

# 使用 qmake 生成 Makefile
qmake flower-player.pro

# 编译项目
mingw32-make    # MinGW
# 或
nmake           # MSVC

# 运行程序
debug\flower-player.exe    # Debug 版本
# 或
release\flower-player.exe  # Release 版本
```

### Linux

1. **安装依赖**
   ```bash
   sudo apt-get install qt5-default libqt5widgets5 libqt5gui5 libqt5core5a
   sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev
   sudo apt-get install libswscale-dev libswresample-dev
   sudo apt-get install libsdl2-dev
   ```

2. **编译项目**
   ```bash
   qmake flower-player.pro
   make
   ./flower-player
   ```

### macOS

```bash
brew install qt@5 ffmpeg sdl2
qmake flower-player.pro
make
open flower-player.app
```

---

## 📖 使用说明

### 基本操作

#### 播放视频
1. **方法一**: 点击菜单栏 "文件" -> "打开文件"
2. **方法二**: 直接将视频文件拖拽到播放窗口
3. **方法三**: 拖拽到播放列表区域

#### 播放控制
- **播放/暂停**: 点击控制栏播放按钮或按 `空格键`
- **停止**: 点击停止按钮

#### 进度控制
- **拖拽进度条**: 鼠标左键拖拽进度滑块
- **快进**: 按 `→` 键
- **快退**: 按 `←` 键
- **逐帧播放**: 按 `S` 键逐帧播放视频
- **实时反馈**: 调整进度时左上角显示跳转目标时间

#### 音量控制
- **调节音量**: 拖拽音量滑块或按 `↑/↓` 键
- **静音切换**: 点击音量图标
- **实时反馈**: 调整音量时左上角显示当前音量百分比

#### 播放速度
- **加速**: 点击速度按钮，选择更快的速度
- **减速**: 点击速度按钮，选择更慢的速度
- **恢复正常**: 再次点击恢复到 1.0x

#### 窗口操作
- **全屏**: 点击全屏按钮或按 `F11` 键 / `Esc` 退出
- **最大化**: 点击最大化按钮
- **最小化**: 点击最小化按钮
- **关闭窗口**: 点击关闭按钮

#### 播放列表
- **添加文件**: 拖拽文件到播放列表区域
- **删除项目**: 右键点击列表项
- **切换歌曲**: 双击列表项

### 快捷键列表

| 快捷键     | 功能 |
|---------|------|
| `Space` | 播放/暂停 |
| `←`     | 快退 10 秒 |
| `→`     | 快进 10 秒 |
| `↑`     | 增加音量 |
| `↓`     | 减小音量 |
| `S`     | 逐帧播放 |
| `F11`   | 全屏切换 |
| `Esc`   | 退出全屏 |

---

## 🏗 核心架构

### 系统架构图

```
┌─────────────────────────────────────────────────┐
│                 MainWindow                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │
│  │  Title   │  │  Show    │  │  Playlist    │  │
│  └──────────┘  └──────────┘  └──────────────┘  │
│                  ┌──────────┐                   │
│                  │ CtrlBar  │                   │
│                  └──────────┘                   │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│                VideoCtl (核心控制器)              │
│  ┌──────────────┐  ┌──────────────────────┐    │
│  │ Read Thread  │  │  Loop Thread         │    │
│  └──────────────┘  └──────────────────────┘    │
│  ┌──────────────┐  ┌──────────┐  ┌──────────┐ │
│  │ Video Decoder│  │Audio Dec │  │Sub Dec   │ │
│  └──────────────┘  └──────────┘  └──────────┘ │
│  ┌──────────────┐  ┌──────────┐  ┌──────────┐ │
│  │ Video Queue  │  │Audio Q   │  │Sub Queue │ │
│  └──────────────┘  └──────────┘  └──────────┘ │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│              FFmpeg + SDL2                       │
│  ┌──────────────┐  ┌──────────────────────┐    │
│  │  AVCodec     │  │  SDL Audio/Render    │    │
│  └──────────────┘  └──────────────────────┘    │
└─────────────────────────────────────────────────┘
```

### 数据流图

```
视频文件
   │
   ▼
┌─────────────┐
│ Read Thread │ ──→ 读取数据包 (AVPacket)
└─────────────┘
   │
   ├──────────────┬──────────────┐
   ▼              ▼              ▼
┌────────┐   ┌──────────┐  ┌────────┐
│Video Q │   │ Audio Q  │  │ Sub Q  │
└────────┘   └──────────┘  └────────┘
   │              │
   ▼              ▼
┌────────┐   ┌──────────┐
│Video D │   │ Audio D  │
└────────┘   └──────────┘
   │              │
   ▼              ▼
┌────────┐   ┌──────────┐
│Frame Q │   │ Frame Q  │
└────────┘   └──────────┘
   │              │
   ▼              ▼
┌────────┐   ┌──────────┐
│SDL Render│  │SDL Audio │
└────────┘   └──────────┘
   │              │
   ▼              ▼
 屏幕显示      扬声器输出
```

### 关键类说明

#### VideoCtl (视频控制器)
核心解码和播放控制类，负责：
- 启动和管理读取线程
- 音视频解码线程管理
- 音视频同步控制
- 播放状态管理（播放/暂停/停止/跳转）
- 音量控制和速度调节

主要方法：
```cpp
void start_play(QString filename, WId play_wid);  // 开始播放
void OnPause();                                    // 暂停/继续
void OnStop();                                     // 停止播放
void OnPlaySeek(double percent);                   // 跳转到指定位置
void OnPlayVolume(double percent);                 // 设置音量
void update_speed(float speed);                    // 调整播放速度
```

#### VideoState (视频状态)
管理单个媒体文件的完整状态：
- 解复用器上下文 (AVFormatContext)
- 音视频流信息
- 数据包队列
- 帧队列
- 时钟同步信息
- 解码器状态

#### PacketQueue (数据包队列)
线程安全的生产者-消费者队列：
- 存储未解码的 AVPacket
- 提供阻塞式读写接口
- 支持队列清空和销毁
- 序列号管理（用于流切换）

#### FrameQueue (帧队列)
存储解码后的帧数据：
- 预分配固定大小的帧缓存
- 读写索引分离
- 支持 peek 操作（不消费数据）
- 与 PacketQueue 关联

#### Clock (时钟)
音视频同步的核心：
- 维护 PTS (Presentation Time Stamp)
- 计算时钟漂移
- 支持暂停/恢复
- 三种同步模式：音频主时钟、视频主时钟、外部时钟

---

## 👨‍💻 开发指南

### 添加新功能

#### 示例：添加新的快捷键

1. 在 `mainwindow.h` 中添加信号：
```cpp
signals:
    void SigNewFunction();
```

2. 在 `mainwindow.cpp` 的 `keyReleaseEvent` 中处理按键：
```cpp
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_X:  // 假设 X 键触发新功能
            emit SigNewFunction();
            break;
        // ... 其他按键
    }
}
```

3. 连接信号到槽函数：
```cpp
connect(this, &MainWindow::SigNewFunction,
        videoCtl, &VideoCtl::OnNewFunction);
```

4. 在 `VideoCtl` 中实现功能：
```cpp
void VideoCtl::OnNewFunction()
{
    // 实现新功能逻辑
}
```

#### 示例：显示操作提示（Toast）

项目已内置 Toast 提示功能，用于在用户操作时显示实时反馈：

**使用方式：**
```cpp
// 在任何地方通过信号触发提示
emit SigShowToast("提示信息");
```

**实现原理：**
- `CtrlBar` 发送 `SigShowToast` 信号
- `MainWindow` 将信号转发到 `Show` 组件
- `Show` 组件在左上角显示半透明提示框，2秒后自动消失

**自定义样式：**
在 `show.cpp` 中修改 `m_toastLabel` 的样式表：
```cpp
m_toastLabel->setStyleSheet(
    "QLabel#toastLabel {"
    "    background-color: transparent;"  // 背景颜色
    "    color: #4169E1;"                  // 字体颜色
    "    padding: 8px 15px;"               // 内边距
    "    font-size: 14px;"                 // 字体大小
    "}"
);
```

### 修改 UI 样式

所有 UI 样式通过 QSS 文件管理，位于 `res/qss/` 目录：

```css
/* 示例：修改播放按钮样式 */
#PlayOrPauseBtn {
    background-color: transparent;
    border: none;
    color: #ffffff;
    font-size: 24px;
}

#PlayOrPauseBtn:hover {
    color: #4CAF50;
}
```

加载样式表：
```cpp
QFile styleFile(":/res/qss/ctrlbar.css");
styleFile.open(QFile::ReadOnly);
QString styleSheet = QLatin1String(styleFile.readAll());
widget->setStyleSheet(styleSheet);
```

### 调试技巧

#### 启用 FFmpeg 日志
```cpp
// 在 videoctl.cpp 中添加
av_log_set_level(AV_LOG_DEBUG);  // 设置为 DEBUG 级别
```

#### 查看队列状态
```cpp
// 在适当位置打印队列信息
qDebug() << "Video Queue Size:" << videoq.nb_packets;
qDebug() << "Audio Queue Size:" << audioq.nb_packets;
```

#### 性能分析
```cpp
// 测量帧处理时间
auto start = std::chrono::high_resolution_clock::now();
// ... 处理代码 ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
qDebug() << "Frame processing time:" << duration.count() << "ms";
```

### 常见问题排查

#### 问题 1: 无法播放视频
**可能原因**:
- DLL 文件缺失或版本不匹配
- 视频格式不支持
- 解码器未正确初始化

**解决方法**:
1. 检查 `debug/` 或 `release/` 目录是否包含所有 DLL 文件
2. 使用 FFmpeg 命令行测试视频文件：`ffprobe your_video.mp4`
3. 查看控制台输出的错误信息

#### 问题 2: 有画面无声音
**可能原因**:
- SDL 音频初始化失败
- 音频格式不支持
- 音量设置为 0

**解决方法**:
1. 检查 SDL 初始化日志
2. 确认系统音频输出正常
3. 检查音量控制状态

#### 问题 3: 音视频不同步
**可能原因**:
- 时钟同步算法问题
- 系统性能不足
- 视频帧率异常

**解决方法**:
1. 检查 `get_master_clock()` 返回值
2. 降低视频分辨率或码率测试
3. 调整 `AV_SYNC_THRESHOLD` 参数

---

## ❓ 常见问题

### Q1: 支持哪些视频格式？
A: 理论上支持 FFmpeg 支持的所有格式，包括但不限于：
- MP4, AVI, MKV, FLV, MOV, WMV, WebM
- MPEG-1/2/4, H.264, H.265/HEVC, VP8, VP9
- 具体支持情况取决于 FFmpeg 编译时启用的解码器

### Q2: 如何添加字幕支持？
A: 当前版本已包含字幕解码框架（`subdec`, `subpq`），但 UI 层未完全实现。可以参考视频解码流程添加字幕渲染功能。

### Q3: 能否在网络环境下使用？
A: 可以播放网络流媒体（RTMP, HTTP, HLS 等），只需将文件路径替换为 URL 即可。FFmpeg 原生支持多种网络协议。

### Q4: 如何优化播放性能？
A: 
- 启用硬件解码（需要修改 FFmpeg 配置）
- 调整队列大小（`MAX_QUEUE_SIZE`, `FRAME_QUEUE_SIZE`）
- 降低视频分辨率
- 使用更高效的视频编码格式（如 H.264）

### Q5: 是否支持 GPU 加速？
A: 当前版本使用软件解码。如需 GPU 加速，可以：
- 使用 FFmpeg 的 NVDEC/CUVID（NVIDIA）
- 使用 VAAPI（Intel/AMD）
- 使用 DXVA2（Windows）

---

## 📄 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

FFmpeg 遵循 LGPL/GPL 许可证，使用时请注意合规性。

---

## 🙏 致谢

- **[playerdemo](https://github.com/itisyang/playerdemo)** - 本项目的主要参考和灵感来源
- **[FFmpeg](https://ffmpeg.org/)** - 强大的音视频处理库
- **[SDL2](https://www.libsdl.org/)** - 跨平台多媒体库
- **[Qt](https://www.qt.io/)** - 优秀的 GUI 框架
- **[Font Awesome](https://fontawesome.com/)** - 图标字体库

---

## 📞 联系方式

如有问题或建议，欢迎提交 Issue 或 Pull Request。

---

<div align="center">

**⭐ 如果这个项目对你有帮助，请给个 Star 支持一下！⭐**

Made with ❤️ by Huahua Player Team

</div>
