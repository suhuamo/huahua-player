#ifndef STATE_H
#define STATE_H

// 全局状态
class State
{
public:
    State();
    // 播放状态，0代表暂停中，1代表正在播放
    static int play_state;
};

#endif // STATE_H
