#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

int thread_work(void *arg)
{
    MainWindow *w = (MainWindow*)arg;
    const char *in_file_url = "1.mp4";
    cout << "in_file:";
    cout << in_file_url;
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
        QImage image((uchar*)pFrameRGB32->data[0], codec_ctx->width, codec_ctx->height, QImage::Format_RGB32);
        w->setImage(image);
        SDL_Delay(23);
        av_packet_unref(pkt);
    }
    return 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setImage(QImage image)
{
    ui->showWidget->showImage(image);
}

void MainWindow::start_work()
{
    SDL_CreateThread(thread_work,"thread_work",this);
}
