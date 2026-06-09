#include "vfilter.h"

VideoFilter::VideoFilter() 
    : m_filter_graph(nullptr),
      m_buffer_src_ctx(nullptr),
      m_buffer_sink_ctx(nullptr),
      m_width(0),
      m_height(0),
      m_format(AV_PIX_FMT_NONE),
      m_time_base({0, 1}),
      m_initialized(false) {
}

VideoFilter::~VideoFilter() {
    cleanup();
}

bool VideoFilter::init(int width, int height, AVPixelFormat format, AVRational time_base) {
    if (m_initialized) {
        cleanup();
    }

    m_width = width;
    m_height = height;
    m_format = format;
    m_time_base = time_base;

    return buildFilterGraph();
}

void VideoFilter::cleanup() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_filter_graph) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
    }
    m_buffer_src_ctx = nullptr;
    m_buffer_sink_ctx = nullptr;
    m_initialized = false;
}

bool VideoFilter::buildFilterGraph() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 释放旧的滤镜图
    if (m_filter_graph) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
    }

    m_filter_graph = avfilter_graph_alloc();
    if (!m_filter_graph) {
        return false;
    }

    // 获取滤镜名称(输入和输出滤镜上下文)
    const AVFilter *buffer_src = avfilter_get_by_name("buffer");
    const AVFilter *buffer_sink = avfilter_get_by_name("buffersink");
    if (!buffer_src || !buffer_sink) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    // 构建缓冲区源参数
    char args[512];
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=1/1",
             m_width, m_height, m_format,
             m_time_base.num, m_time_base.den);

    // 创建源滤镜上下文
    if (avfilter_graph_create_filter(&m_buffer_src_ctx, buffer_src, "in",
                                     args, nullptr, m_filter_graph) < 0) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    // 创建输出滤镜上下文
    if (avfilter_graph_create_filter(&m_buffer_sink_ctx, buffer_sink, "out",
                                     nullptr, nullptr, m_filter_graph) < 0) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    // 设置输出像素格式
    enum AVPixelFormat pix_fmts[] = {m_format, AV_PIX_FMT_NONE};
    av_opt_set_int_list(m_buffer_sink_ctx, "pix_fmts", pix_fmts,
                        AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

    // 构建滤镜链描述
    std::string filter_desc;

    if (m_params.brightness != FilterConstants::DEFAULT_BRIGHTNESS ||
        m_params.contrast != FilterConstants::DEFAULT_CONTRAST ||
        m_params.saturation != FilterConstants::DEFAULT_SATURATION) {
        char eq_args[256];
        snprintf(eq_args, sizeof(eq_args),
                 "eq=brightness=%f:contrast=%f:saturation=%f",
                 m_params.brightness, m_params.contrast, m_params.saturation);
        filter_desc += eq_args;
    }

    if (m_params.grayscale) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "hue=s=0";
    }

    if (m_params.sepia) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "colorchannelmixer=.393:.769:.189:0:.349:.686:.168:0:.272:.534:.131";
    }

    if (m_params.negative) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "negate";
    }

    if (m_params.sharpen) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "unsharp=5:5:1.0:5:5:0.0";
    }

    if (m_params.blur_radius > 0) {
        if (!filter_desc.empty()) filter_desc += ",";
        char blur_args[64];
        // 默认模糊两次，想要更模糊，可以增加steps
         snprintf(blur_args, sizeof(blur_args),
         "gblur=sigma=%f:steps=2",
         m_params.blur_radius);
        filter_desc += blur_args;
    }

    if (m_params.edge_detect) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "edgedetect";
    }

    if (m_params.hflip) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "hflip";
    }

    if (m_params.vflip) {
        if (!filter_desc.empty()) filter_desc += ",";
        filter_desc += "vflip";
    }

    // 解析并添加滤镜链
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    if (!outputs || !inputs) {
        avfilter_inout_free(&outputs);
        avfilter_inout_free(&inputs);
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = m_buffer_src_ctx;
    outputs->pad_idx = 0;
    outputs->next = nullptr;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = m_buffer_sink_ctx;
    inputs->pad_idx = 0;
    inputs->next = nullptr;

    int ret;
    if (!filter_desc.empty()) {
        ret = avfilter_graph_parse_ptr(m_filter_graph, filter_desc.c_str(),
                                       &inputs, &outputs, nullptr);
    } else {
        // 没有滤镜，直接连接
        ret = avfilter_link(m_buffer_src_ctx, 0, m_buffer_sink_ctx, 0);
    }

    avfilter_inout_free(&outputs);
    avfilter_inout_free(&inputs);

    if (ret < 0) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    // 配置滤镜图
    if (avfilter_graph_config(m_filter_graph, nullptr) < 0) {
        avfilter_graph_free(&m_filter_graph);
        m_filter_graph = nullptr;
        return false;
    }

    m_initialized = true;
    return true;
}

bool VideoFilter::applyFilter(AVFrame *src_frame, AVFrame *dst_frame) {
    if (!m_initialized || !m_params.isActive()) {
        // 没有激活滤镜，直接复制帧
        av_frame_move_ref(dst_frame, src_frame);
        return true;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_filter_graph || !m_buffer_src_ctx || !m_buffer_sink_ctx) {
        av_frame_move_ref(dst_frame, src_frame);
        return true;
    }

    // 发送帧到滤镜
    int ret = av_buffersrc_add_frame_flags(m_buffer_src_ctx, src_frame,
                                           AV_BUFFERSRC_FLAG_KEEP_REF);
    if (ret < 0) {
        return false;
    }

    // 获取处理后的帧
    ret = av_buffersink_get_frame(m_buffer_sink_ctx, dst_frame);
    if (ret < 0) {
        return false;
    }

    return true;
}

void VideoFilter::updateParams(const FilterParams &params) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_params = params;
    }
    
    // 如果有变化，重新构建滤镜图
    if (m_initialized) {
        reinitFilterGraph();
    }
}

bool VideoFilter::reinitFilterGraph() {
    return init(m_width, m_height, m_format, m_time_base);
}

FilterParams VideoFilter::getParams() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_params;
}

bool VideoFilter::isActive() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_params.isActive() && m_initialized;
}