//
// Created by suhuamo on 2026/4/2.
//

#ifndef VIDEOCTL_H
#define VIDEOCTL_H

#include<QObject>
#include<QThread>
#include<QString>

#include <iostream>
#include <string>
#include"datactl.h"
#include"sonic.h"
#include"globalhelper.h"
#include"audioseparator.h"

using std::string;
using std::cout;
using std::endl;
using std::thread;

/*
 * mark：数字的宏定义最好用括号括起来
 */
#define PLAYBACK_RATE_MIN           (0.25)     // 最慢
#define PLAYBACK_RATE_MAX           (3.0)     // 最快
#define PLAYBACK_RATE_RESET         (1.0)     //默认播放速度
#define PLAYBACK_RATE_SCALE         (0.25)    // 变速刻度
#define NORMAL_SAMPLE_RATES         (44100)  // 默认采样率
#define NORMAL_CHANNELS             (2)     // 默认声道数


// Stem 音频源：用于播放分离后的人声/伴奏等 stem 文件
struct StemAudioSource {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
    int stream_index;
    AVStream *audio_st;

    PacketQueue audioq;     // stem 包队列
    FrameQueue sampq;       // stem 帧队列
    Decoder auddec;         // stem 解码器

    AudioParams audio_src;  // stem 源音频参数
    SwrContext *swr_ctx;    // stem 重采样器
    bool active;            // 是否激活（正在使用 stem 音频）

    SDL_cond *continue_read_thread; // 用于通知 stem 读取线程

    StemAudioSource() : fmt_ctx(nullptr), codec_ctx(nullptr), stream_index(-1),
        audio_st(nullptr), swr_ctx(nullptr), active(false), continue_read_thread(nullptr) {
        memset(&audio_src, 0, sizeof(AudioParams));
    }
};

class VideoCtl : public QObject{
    Q_OBJECT
public:

    ~VideoCtl();
    void start_play(QString filename, WId play_wid);
    static VideoCtl *GetInstance();
    int audio_decode_frame(VideoState *is);
    void set_clock_at(Clock *c, double pts, int serial, double time);
    double get_clock(Clock *c);
    bool get_playback_change();
    void set_playback_change(bool change);
    float get_playback_rate();
    int64_t get_target_frequency();
    int get_target_channels();
    bool is_normal_playback_rate();
    void OnSetSpeed(float speed); // 设置指定倍速
    void OnPause();
    void OnStop();
    void OnUserStop(); // 用户主动点击停止按钮
    void OnPlayVolume(double percent);
    void OnPlaySeek(double percent);
    void OnSeekForward();
    void OnSeekBack();
    void OnAddVolume();
    void OnSubVolume();
    void OnStep(); // 逐帧播放
    void OnSwitchAudioMode(int mode); // 切换音频模式（原声/人声/伴奏等）
    bool OpenStemSource(const QString &stemPath); // 打开 stem 音频源
    void CloseStemSource(); // 关闭 stem 音频源
signals:
    void SigStartPlay(QString strMsg);
    void SigSpeed(float speed);
    void SigPauseStat(bool paused);
    void SigStopFinished(); // 自然播放结束
    void SigUserStopFinished(); // 用户主动停止播放
    void SigStop();
    void SigVideoTotalSeconds(int seconds);
    void SigVideoPlaySeconds(int seconds);
    void SigVideoVolume(double percent);
    void SigFrameDimensionsChanged(int nFrameWidth, int nFrameHeight);
    void SigSeekForwardCompleted(int targetSeconds);  // 快进完成
    void SigSeekBackCompleted(int targetSeconds);     // 快退完成
    void SigAudioModeChanged(int mode);               // 音频模式切换完成
private:
    explicit VideoCtl(QObject *parent=nullptr);
    bool init();
    bool ConnectionSignalSlots();
    VideoState* stream_open(const char* filename);
    void init_clock(Clock *c, int *queueSerial);
    void set_clock(Clock *c, double pts, int serial);
    void set_clock_speed(Clock *c, double speed);
    double get_master_clock(VideoState *is);
    void read_thread(VideoState *is);
    int stream_component_open(VideoState *is, int stream_index);
    int audio_open(void *opaque, int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, struct AudioParams *audio_hw_params);
    int synchronize_audio(VideoState *is, int nb_samples);
    int get_master_sync_type(VideoState *is);
    int audio_thread(void *arg);
    int video_thread(void *arg);
    int get_video_frame(VideoState *is,AVFrame *frame);
    int queue_picture(VideoState *is, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial);
    void do_exit(VideoState *is);
    void stream_close(VideoState *is);
    void stream_component_close(VideoState *is, int stream_index);
    void loop_thread(VideoState *curStream);
    void refresh_loop_wait_event(VideoState *is, SDL_Event *event);
    void video_refresh(void *arg, double *remainingTime);
    double compute_target_delay(double delay, VideoState *is);
    double vp_duration(VideoState *is, Frame *vp, Frame *nextVp);
    void update_video_pts(VideoState *is, double pts, int64_t pos, int serial);
    void video_display(VideoState *is);
    void video_open();
    void video_image_display(VideoState *is);
    void calculate_display_rect(SDL_Rect *rect, int src_x_left, int src_y_top, int src_width, int src_height, int pic_width, int pic_height, AVRational pic_sar);
    int realloc_texture(SDL_Texture **texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blend_mode, int init_texture);
    int upload_texture(SDL_Texture *tex, AVFrame *frame, struct SwsContext **img_convert_ctx);
    int stream_has_enough_packets(AVStream *st, int stream_id, PacketQueue *queue);

    void stream_toggle_pause(VideoState *is);
    void toggle_pause(VideoState *is);
    void step_to_next_frame(VideoState *is);
    void stream_seek(VideoState *is, int64_t pos, int64_t rel);
    void stream_seek_incr(int incr);
    void stream_seek_back();
    void stream_seek_forward();
    void stream_cycle_channel(int media_type);
    int stem_audio_thread(void *arg);  // stem 解码线程
    void seekStemSource(int64_t pos);  // seek stem 源
    void stem_read_thread(StemAudioSource *stem); // stem 读取线程
    void toggle_full_screen();
    void update_volume(int sign, double step);
    void add_volume();
    void sub_volume();
    void update_speed(float speed);

private:
    static VideoCtl* m_instance;
    bool m_init;
    bool m_play_loop;
    std::thread m_play_loop_thread;
    VideoState* m_cur_stream;

    SDL_Renderer *m_renderer;
    SDL_Window *m_window;

    int m_screen_width;
    int m_screen_height;

    int m_frame_width;
    int m_frame_height;

    int m_startup_volume;
    bool m_is_full_screen;

    float m_playback_rate; //播放速度
    bool m_playback_changed; //播放速度是否改变
    WId m_play_wid;//播放窗口
    bool m_stop_emitted; //标记是否已经发送过停止信号
    QString m_current_file; // 当前播放文件路径
    bool m_video_open; // 视频窗口打开状态
    SDL_Texture* m_vid_texture;       // 视频纹理（由 VideoCtl 统一管理，播放结束后保留用于重新渲染）
    AVRational m_frame_sar;            // 当前帧的宽高比
    bool m_frame_flip_v;               // 当前帧是否垂直翻转
    AVFrame* m_last_frame;             // 最后一帧的引用（播放结束后用于重建纹理，av_frame_ref 保持数据有效）
    bool m_idle_loop;                   // 空闲事件循环运行标志（true=运行中，类似 m_play_loop）
    bool m_user_stop;                   // 用户主动停止标志（区分自然播放结束和用户点击停止）
    StemAudioSource m_stem;             // stem 音频源
    AudioMode m_audio_mode;             // 当前音频模式
    QString m_stem_file_path;           // 当前 stem 文件路径
    std::thread m_stem_read_thread;     // stem 读取线程
    bool m_stem_seek_req;               // stem 是否需要 seek
    int64_t m_stem_seek_pos;            // stem seek 目标位置（AV_TIME_BASE）
public:
    sonicStreamStruct* m_audio_speed_convert;
};


#endif //VIDEOCTL_H
