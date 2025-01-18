#include "ffplay.h"

// 当前帧音频PCM数据
static Uint8 *audio_pcm_g;
// 当前帧音频PCM数据的字节总大小长度
static Uint32 audio_len_g;

FFPlay::FFPlay(QObject *parent)
    : QObject{parent}
{
    //注册新类型，因为子线程和主线程传递数据的时候如果没有注册是无法传输的
    qRegisterMetaType<QImage>("QImage&");
    // 默认是运行状态
    abort_ = false;
}

FFPlay *FFPlay::GetInstance()
{
    static FFPlay *ffPlay = new FFPlay();
    // 这里可以加一个判断，ffplay是否初始化过
    return ffPlay;
}

// 填充PCM数据到SDL中
void fill_audio_pcm(void *udata, Uint8 *stream, int len) {
    // 清空上一帧的数据
    SDL_memset(stream, 0, len);
    // 如果外部线程【主线程读帧】还未读取到数据，那么无法填充PCM到SDL中进行播放
    if(audio_len_g == 0)
    {
        return ;
    }
    // 本次回调结束最多只能取len字节的数据
    // 如果外部读取的帧小于len字节，那么直接填充外部读取到的所有数据即可
    // 如果外部读取的帧大于len字节，那么本次填充len字节的数据，等下次回调再填充 audio_len_g - len字节的数据
    // 【如果audio_len_g - len 还是大于了len字节，那么继续取len填充即可】
    len = len > audio_len_g ? audio_len_g : len;
    //填充PCM数据到SDL中
    SDL_MixAudio(stream, audio_pcm_g, len, SDL_MIX_MAXVOLUME/2);// SDL_MIX_MAXVOLUME/2 为音频大小，在0-128之间调整
    // 更新pcm内存指针指向位置，已经【又】使用了len个字节空间，那么下次需要从当前位置+len的位置开始使用
    audio_pcm_g += len;
    // 更新剩余字节大小数量，已经读取了len个字节大小的数据，那么下次还剩 audio_len_g - len 个字节大小的数据可以使用
    audio_len_g -= len;

}

// 开启线程播放媒体
void FFPlay::thread_work(QString file_url)
{
    // 获取输入的文件
    const char *in_file_url = file_url.toStdString().c_str();
    int ret = 0;
    // 创建封装上下文对象
    AVFormatContext *in_fmt_ctx = avformat_alloc_context();
    // 加载视频信息到上下文中
    ret = avformat_open_input(&in_fmt_ctx, in_file_url, NULL, NULL);
    // 查询流信息
    ret = avformat_find_stream_info(in_fmt_ctx, NULL);
    // 测试输出视频信息
    av_dump_format(in_fmt_ctx, 0, in_file_url, 0);
    // 查找视频流、音频流的序号
    int video_idx = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    int audio_idx = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    // 视频时间
    int64_t time_second = in_fmt_ctx->duration / AV_TIME_BASE;
    int total_hour = time_second / 3600;
    int total_minute = time_second % 3600 / 60;
    int total_second = time_second % 60;
    // 设置进度条栏
    emit setPlaySliderMaximum(time_second);
    emit setTotalTimeEdit(QTime(total_hour, total_minute, total_second));
    // 视频解码器上下文
    AVCodecContext *video_codec_ctx = avcodec_alloc_context3(NULL);
    // 加载视频流中的参数到解码器上下文中
    ret = avcodec_parameters_to_context(video_codec_ctx, in_fmt_ctx->streams[video_idx]->codecpar);
    // 查找视频解码器
    AVCodec *video_codec = avcodec_find_decoder(video_codec_ctx->codec_id);
    // 指定解码器上下文使用该解码器
    ret = avcodec_open2(video_codec_ctx, video_codec, NULL);
    // 音频解码器上下文
    AVCodecContext *audio_codec_ctx = avcodec_alloc_context3(NULL);
    // 加载视频流中的参数到解码器上下文中
    ret = avcodec_parameters_to_context(audio_codec_ctx, in_fmt_ctx->streams[audio_idx]->codecpar);
    // 查找视频解码器
    AVCodec *audio_codec = avcodec_find_decoder(audio_codec_ctx->codec_id);
    // 指定解码器上下文使用该解码器
    ret = avcodec_open2(audio_codec_ctx, audio_codec, NULL);
    // 包数据
    AVPacket *pkt = av_packet_alloc();
    // 帧数据
    AVFrame *frame = av_frame_alloc();
    //存储解码后转换的RGB数据
    AVFrame *pFrameRGB32 = av_frame_alloc();
    // 给 pFrameRGB32 分配 Buffer内存空间
    ret = av_image_fill_arrays(pFrameRGB32->data,pFrameRGB32->linesize, (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, video_codec_ctx->width, video_codec_ctx->height,4)), AV_PIX_FMT_RGB32, video_codec_ctx->width, video_codec_ctx->height,1);
    // 视频画面重采样
    SwsContext *img_convert_ctx = sws_getContext(video_codec_ctx->width,
                                                 video_codec_ctx->height,
                                                 video_codec_ctx->pix_fmt,
                                                 video_codec_ctx->width,
                                                 video_codec_ctx->height,
                                                 AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    // SDL
    // 初始化音频
    if(SDL_Init(SDL_INIT_AUDIO)) {
        cout << "初始化SDL音频失败";
        return ;
    }
    // 音频播放上下文，音频播放只能通过这个结构体进行操作
    // 创建 SwrContext 只能使用 swr_alloc() 函数
    SwrContext *swrContext = swr_alloc();
    if(!swrContext)
    {
        cout << "初始化swrContext对象失败";
        return ;
    }
    // 设置具体参数来创建 SwrContext对象
    /* channel布局:如立体声、5.1声道、单声道等
     * 采样格式：不同音频格式的采样格式不同，如AAC的采样格式是 AV_SAMPLE_FMT_FLTP，
     *      而MP3的采样格式是 AV_SAMPLE_FMT_S16P
     * 采样率：一秒钟采集多少次样本
     * */
    swrContext = swr_alloc_set_opts(NULL,   //是否需要继承一个存在的SwrContext的内容
                                    AV_CH_LAYOUT_STEREO, //输出的channel布局
                                    AV_SAMPLE_FMT_S16, //输出的采样格式
                                    44100, //输出的采样率
                                    av_get_default_channel_layout(audio_codec_ctx->channels), //输入的channel布局
                                    audio_codec_ctx->sample_fmt, //输入的采用格式
                                    audio_codec_ctx->sample_rate, //输入的采用率
                                    0,
                                    NULL);
    /* 为什么要重采样？
     * 是因为输入的音频可能是mp4格式的，但是我们的电脑只能播放avi格式的音频，
     *  所以需要转换数据，转换为确保我们的电脑一定能播放的格式。
     * */
    // 初始化重采样上下文
    ret = swr_init(swrContext);
    // 初始化重采样失败，那么音频无法播放
    if(ret < 0)
    {
        cout << "初始化重采样上下文失败";
        return ;
    }
    // SDL_AudioSpc 是音频播放参数的结构体
    // 期望能够实现的音频参数
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = 44100; //期望的采样率
    wanted_spec.format = AUDIO_S16SYS; //期望的采样格式
    wanted_spec.channels = 2; //期望的通道格式
    wanted_spec.silence = 0; //期望中静音大小的值
    wanted_spec.samples = 1024; //期望中一帧的数据大小，即样本数
    wanted_spec.callback = fill_audio_pcm;//播放音频时会开启一个线程，反复调用这个回调函数，用来给音频填充PCM
    wanted_spec.userdata = audio_codec_ctx; //回调函数中第一个参数的对象
    // 按照指定参数打开真实的物理设备
    ret = SDL_OpenAudio(&wanted_spec, NULL);
    if(ret < 0)
    {
        cout << "打开音频设备失败";
        return ;
    }
    // 开始播放音频
    SDL_PauseAudio(0);
    // 分配输出音频数据
    Uint8 *out_buffer = nullptr;
    // 获取所有帧数据
    while(1)
    {
        if(abort_)
        {
            goto end_;
        }
        // 暂停中不需要播放
        if(State::play_state == 0)
        {
            SDL_Delay(10);
            continue;
        }
        // 读取包数据
        ret = av_read_frame(in_fmt_ctx, pkt);
        // 数据读取结束
        if(ret < 0)
        {
            cout << "play video finsh";
            break;
        }
        // 本次忽略音频
        if(pkt->stream_index == audio_idx)
        {
            // 将包加载到解码器上下文中进行解码
            ret = avcodec_send_packet(audio_codec_ctx, pkt);
            // 对应音频的包数据来说，一次包读取，可以获取到多个frame
            while(1)
            {
                // 读取解码后的包中的帧
                ret = avcodec_receive_frame(audio_codec_ctx, frame);
                // 如果所有的帧都读取完成了，那么开始读取下一个包
                if(AVERROR(EAGAIN) == ret)
                {
                    break;
                }
                // 获取输入的样本数
                int in_samples = frame->nb_samples;
                // 目标样本数【想要输出的样本数】
                int dst_samples = av_rescale_rnd(in_samples, wanted_spec.freq, frame->sample_rate, AV_ROUND_UP);
                // 计算需要输出的样本数内存空间大小
                int out_buffer_size = av_samples_get_buffer_size(NULL, wanted_spec.channels,
                                                                 dst_samples, AV_SAMPLE_FMT_S16, 0);
                // 如果输出的音频数据未开辟过空间，那么开辟空间
                if(!out_buffer)
                {
                    // 输出数据的空间大小即为计算出来需要输出的样本数大小
                    out_buffer = (Uint8 *)av_malloc(out_buffer_size);
                }
                // 返回每个通道需要输出的样本数，错误时返回负值
                int sample_count = swr_convert(swrContext, &out_buffer, dst_samples,
                                               (const Uint8 **)frame->data, in_samples);// frame->data 即为采样到的数据
                // 获取不到样本数了，那么进行下一个包数据的读取
                if(sample_count < 0)
                {
                    break;
                }
                // 计算这一帧的字节数大小/长度
                int out_size = sample_count * wanted_spec.channels *av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
                // 如果回调函数中的字节数还未处理完，那么不能进行下一个音频帧的处理
                while(audio_len_g > 0)
                    SDL_Delay(1);
                // 回调函数中的字节已经处理完了，那么可以填充下一个音频帧需要的数据了
                // 这一帧的字节长度
                audio_len_g = out_size;
                // 填充pcm数据
                audio_pcm_g = (Uint8 *)out_buffer;
            }
            // 释放buffer资源
            av_packet_unref(pkt);
        }
        else
        {
            // 将包数据发送到解码器上下文中进行解码
            ret = avcodec_send_packet(video_codec_ctx, pkt);
            // 对应包数据来说，一次包读取，可以获取到多个frame
            while(1)
            {
                // 获取解码器上下文界面后的帧数据
                ret = avcodec_receive_frame(video_codec_ctx, frame);
                // 如果解码器现在无法读取到一个完整的帧数据，那么再次循环发送包数据进行解码
                if(AVERROR(EAGAIN) == ret)
                {
                    break;
                }
                // codec_ctx->time_base 是编码时候的时间基， av_q2d(codec_ctx->time_base) 是 pts 转换为毫秒的单位， / 1000 转换为秒
                int64_t now_second = frame->pts * av_q2d(video_codec_ctx->time_base) / 1000;
                // 进度条走动
                emit setPlaySliderValue(now_second);
                emit setPlayTimeEdit(QTime(now_second / 3600, now_second % 3600 / 60, now_second % 60));
                /*
                     * sws_scale 用于将图像从一个像素格式转换为另一个像素格式。
                     * 可以在同一个函数里实现：1.图像色彩空间转换， 2:分辨率缩放，3:前后图像滤波处理
                    */
                sws_scale(img_convert_ctx,
                          (const uint8_t* const*)frame->data,
                          frame->linesize,
                          0,
                          video_codec_ctx->height,
                          pFrameRGB32->data,
                          pFrameRGB32->linesize);
                // 发送图片给界面显示
                QImage image((uchar*)pFrameRGB32->data[0], video_codec_ctx->width, video_codec_ctx->height, QImage::Format_RGB32);
                emit setImage(image);
                // 等待23ms一帧
                SDL_Delay(23);
                // 释放buffer资源
            }
            av_packet_unref(pkt);
        }
    }
end_:
    cout << "当前视频播放结束：" << file_url;
}

void FFPlay::stop_play()
{
    m_mutex.lock();
    // 通知当前播放结束
    abort_ = true;
    // 等待当前播放的视频线程销毁
    if(playLoopThread.joinable())
    {
        playLoopThread.join();
    }
    // 清空页面数据
    emit clear();
    m_mutex.unlock();
}

void FFPlay::updatePlayUrl(const QModelIndex &index)
{
    m_mutex.lock();
    // 通知当前播放结束
    abort_ = true;
    // 等待当前播放的视频线程销毁
    if(playLoopThread.joinable())
    {
        playLoopThread.join();
    }
    // 设置新的播放文件为当前选择的条目中的文件  [index.data(Qt::UserRole): 获取用户自己填充到条目中的数据]
    file_url = index.data(Qt::UserRole).value<QString>().toStdString().data();
    // 通知新的播放开始
    abort_ = false;
    // 开始播放新选择的文件
    playLoopThread = std::thread(&FFPlay::thread_work, this, index.data(Qt::UserRole).value<QString>());
    m_mutex.unlock();
}
