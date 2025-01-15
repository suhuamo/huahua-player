#include "ffplay.h"

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

// 开启线程播放媒体
void FFPlay::thread_work()
{
    const char *in_file_url = file_url;
    cout << "in_file: " << in_file_url;
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
    cout << "video:" << video_idx << " audio:" << audio_idx;
    // 视频时间
    int64_t time_second = in_fmt_ctx->duration / AV_TIME_BASE;
    int total_hour = time_second / 3600;
    int total_minute = time_second % 3600 / 60;
    int total_second = time_second % 60;
    // 设置进度条栏
    emit setPlaySliderMaximum(time_second);
    emit setTotalTimeEdit(QTime(total_hour, total_minute, total_second));
    // 解码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    // 加载视频流中的参数到解码器上下文中
    ret = avcodec_parameters_to_context(codec_ctx, in_fmt_ctx->streams[video_idx]->codecpar);
    // 查找解码器
    AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
    // 指定解码器上下文使用该解码器
    ret = avcodec_open2(codec_ctx, codec, NULL);
    // 包数据
    AVPacket *pkt = av_packet_alloc();
    // 帧数据
    AVFrame *frame = av_frame_alloc();
    //存储解码后转换的RGB数据
    AVFrame *pFrameRGB32 = av_frame_alloc();
    int size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height,4);
    uint8_t *out_buffer = (uint8_t *)av_malloc(size);
    ret = av_image_fill_arrays(pFrameRGB32->data,pFrameRGB32->linesize, out_buffer, AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height,1);

    SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width,
                                                 codec_ctx->height,
                                                 codec_ctx->pix_fmt,
                                                 codec_ctx->width,
                                                 codec_ctx->height,
                                                 AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
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
            continue;
        }
        // 将包数据发送到解码器上下文中进行解码
        ret = avcodec_send_packet(codec_ctx, pkt);
        // 获取解码器上下文界面后的帧数据
        ret = avcodec_receive_frame(codec_ctx, frame);
        // 如果解码器现在无法读取到一个完整的帧数据，那么再次循环发送包数据进行解码
        if(AVERROR(EAGAIN) == ret)
        {
            continue;
        }
        // codec_ctx->time_base 是编码时候的时间基， av_q2d(codec_ctx->time_base) 是 pts 转换为毫秒的单位， / 1000 转换为秒
        int64_t now_second = frame->pts * av_q2d(codec_ctx->time_base) / 1000;
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
                  codec_ctx->height,
                  pFrameRGB32->data,
                  pFrameRGB32->linesize);
        // 发送图片给界面显示
        QImage image((uchar*)pFrameRGB32->data[0], codec_ctx->width, codec_ctx->height, QImage::Format_RGB32);
        emit putImage(image);
        // 等待23ms一帧
        SDL_Delay(23);
        // 释放buffer资源
        av_packet_unref(pkt);
    }
end_:
    cout << "当前播放结束";
}

void FFPlay::getPlayUrl(const QModelIndex &index)

{
    // 通知当前播放结束
    abort_ = true;
    // 等待当前播放的视频线程销毁
    if(playLoopThread.joinable())
    {
        playLoopThread.join();
    }
    // 更新输入的文件
    if(index.row() == 0)
    {
        file_url = "1.mp4";
    }else
    {
        file_url = "2.mp4";
    }
    // 通知新的播放开始
    abort_ = false;
    // 开始播放新选择的文件
    playLoopThread = std::thread(&FFPlay::thread_work, this);
}
