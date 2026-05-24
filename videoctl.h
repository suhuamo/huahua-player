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
    void OnSpeed();
    void OnPause();
    void OnStop();
    void OnPlayVolume(double percent);
    void OnPlaySeek(double percent);
    void OnSeekForward();
    void OnSeekBack();
    void OnAddVolume();
    void OnSubVolume();
signals:
    void SigStartPlay(QString strMsg);
    void SigSpeed(float speed);
    void SigPauseStat(bool paused);
    void SigStopFinished(); // 结束播放
    void SigStop();
    void SigVideoTotalSeconds(int seconds);
    void SigVideoPlaySeconds(int seconds);
    void SigVideoVolume(double percent);
    void SigFrameDimensionsChanged(int nFrameWidth, int nFrameHeight);
private:
    explicit VideoCtl(QObject *parent=nullptr);
    bool init();
    bool ConnectionSignalSlots();
    VideoState* stream_open(const char* filename);
    void init_clock(Clock *c, int *queueSerial);
    void set_clock(Clock *c, double pts, int serial);
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
    void toggle_full_screen();
    void update_volume(int sign, double step);
    void add_volume();
    void sub_volume();
    void update_speed(float speed);
    void add_speed();
    void sub_speed();
    void reset_speed();

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
    QString m_current_file; //当前播放的文件路径
    bool m_stop_emitted; //标记是否已经发送过停止信号
public:
    sonicStreamStruct* m_audio_speed_convert;
};


#endif //VIDEOCTL_H
