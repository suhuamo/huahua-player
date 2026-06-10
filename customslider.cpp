#include "customslider.h"
#include"globalhelper.h"

CustomSlider::CustomSlider(QWidget *parent)
    :QSlider(parent),
      m_pressed(false)
{
//    todo:这里就是给进度条设置刻度的地方
    this->setMaximum(MAX_SLIDER_VALUE);
}

CustomSlider::~CustomSlider()
{

}

void CustomSlider::mousePressEvent(QMouseEvent *ev)
{
    //    向上传递事件(先传递是因为上面的组件会改变进度条，而我们下面的逻辑也会改变进度条，所以我们的逻辑需要后执行)
    QSlider::mousePressEvent(ev);
    m_pressed = true;
//    获取鼠标的位置， 也可以取 ev->x()
    double pos = ev->pos().x() / (double)width();
//    pos 是现在点到了百分比多少的位置，maximum() - minimum() 是刻度一共有多少， + minimum() 是因为不能小于最低刻度
    setValue(pos * (maximum() - minimum()) + minimum());

    emit sigSliderValueChanged();
}

void CustomSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    //    向上传递事件(先传递是因为上面的组件会改变进度条，而我们下面的逻辑也会改变进度条，所以我们的逻辑需要后执行)
    QSlider::mouseReleaseEvent(ev);
    m_pressed = false;
//    todo：如果是视频播放的话，那么按道理应该是在松开的时候进行视频的seek，故视频的seek信号应该在这里触发，拖动和按下的时候不需要发送信号才对
//    emit sigSliderValueChanged();
}

void CustomSlider::mouseMoveEvent(QMouseEvent *ev)
{
    //    向上传递事件(先传递是因为上面的组件会改变进度条，而我们下面的逻辑也会改变进度条，所以我们的逻辑需要后执行)
    QSlider::mouseMoveEvent(ev);
//    如果处于按中状态，那么才需要修改进度条，不能鼠标在页面上滑来滑去进度条就猛猛变吧
    if(m_pressed) {
    //    获取鼠标的位置， 也可以取 ev->x()
        double pos = ev->pos().x() / (double)width();
    //    pos 是现在点到了百分比多少的位置，maximum() - minimum() 是刻度一共有多少， + minimum() 是因为不能小于最低刻度
        setValue(pos * (maximum() - minimum()) + minimum());

        emit sigSliderValueChanged();
    }
}
