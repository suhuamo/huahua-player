#ifndef VFILTER_H
#define VFILTER_H

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

#include <string>
#include <mutex>

// 滤镜类型枚举
enum VideoFilterType {
    FILTER_NONE = 0,
    FILTER_BRIGHTNESS,      // 亮度
    FILTER_CONTRAST,        // 对比度
    FILTER_SATURATION,      // 饱和度
    FILTER_GRAYSCALE,       // 灰度
    FILTER_BLUR,            // 模糊
    FILTER_EDGE_DETECT,     // 边缘检测
    FILTER_HORIZONTAL_FLIP, // 水平翻转
    FILTER_VERTICAL_FLIP,   // 垂直翻转
    FILTER_SEPIA,           // 复古褐色
    FILTER_NEGATIVE,        // 负片效果
    FILTER_SHARPEN,         // 锐化
};

// 滤镜常量命名空间
namespace FilterConstants {
    // 默认值常量
    constexpr double DEFAULT_BRIGHTNESS = 0.0;
    constexpr double DEFAULT_CONTRAST = 1.0;
    constexpr double DEFAULT_SATURATION = 1.0;
    constexpr double DEFAULT_BLUR_RADIUS = 0.0;

    // 参数范围常量
    constexpr double MIN_BRIGHTNESS = -1.0;
    constexpr double MAX_BRIGHTNESS = 1.0;
    constexpr double MIN_CONTRAST = 0.0;
    constexpr double MAX_CONTRAST = 2.0;
    constexpr double MIN_SATURATION = 0.0;
    constexpr double MAX_SATURATION = 2.0;
    constexpr double MIN_BLUR_RADIUS = 0.0;
    constexpr double MAX_BLUR_RADIUS = 10.0;
}

// 滤镜参数结构
struct FilterParams {
    double brightness;      // 亮度 [MIN_BRIGHTNESS, MAX_BRIGHTNESS], 默认 DEFAULT_BRIGHTNESS
    double contrast;        // 对比度 [MIN_CONTRAST, MAX_CONTRAST], 默认 DEFAULT_CONTRAST
    double saturation;      // 饱和度 [MIN_SATURATION, MAX_SATURATION], 默认 DEFAULT_SATURATION
    double blur_radius;     // 模糊半径 [MIN_BLUR_RADIUS, MAX_BLUR_RADIUS], 默认 DEFAULT_BLUR_RADIUS
    bool grayscale;         // 灰度开关
    bool edge_detect;       // 边缘检测开关
    bool hflip;             // 水平翻转开关
    bool vflip;             // 垂直翻转开关
    bool sepia;             // 复古褐色开关
    bool negative;          // 负片效果开关
    bool sharpen;           // 锐化开关

    FilterParams() {
        reset();
    }

    void reset() {
        brightness = FilterConstants::DEFAULT_BRIGHTNESS;
        contrast = FilterConstants::DEFAULT_CONTRAST;
        saturation = FilterConstants::DEFAULT_SATURATION;
        blur_radius = FilterConstants::DEFAULT_BLUR_RADIUS;
        grayscale = false;
        edge_detect = false;
        hflip = false;
        vflip = false;
        sepia = false;
        negative = false;
        sharpen = false;
    }

    bool isActive() const {
        return brightness != FilterConstants::DEFAULT_BRIGHTNESS ||
               contrast != FilterConstants::DEFAULT_CONTRAST ||
               saturation != FilterConstants::DEFAULT_SATURATION ||
               blur_radius > FilterConstants::DEFAULT_BLUR_RADIUS ||
               grayscale || edge_detect || hflip ||
               vflip || sepia || negative || sharpen;
    }
};

class VideoFilter {
public:
    VideoFilter();
    ~VideoFilter();

    // 初始化滤镜
    bool init(int width, int height, AVPixelFormat format, AVRational time_base);

    // 释放滤镜资源
    void cleanup();

    // 应用滤镜到帧
    bool applyFilter(AVFrame *src_frame, AVFrame *dst_frame);

    // 更新滤镜参数
    void updateParams(const FilterParams &params);

    // 获取当前参数
    FilterParams getParams() const;

    // 检查滤镜是否激活
    bool isActive() const;

private:
    // 构建滤镜链
    bool buildFilterGraph();

    // 重新初始化滤镜图
    bool reinitFilterGraph();

    AVFilterGraph *m_filter_graph;
    AVFilterContext *m_buffer_src_ctx;
    AVFilterContext *m_buffer_sink_ctx;
    FilterParams m_params;
    mutable std::mutex m_mutex;

    // 输入帧参数
    int m_width;
    int m_height;
    AVPixelFormat m_format;
    AVRational m_time_base;
    bool m_initialized;
};

#endif // VFILTER_H