#ifndef STATE_H
#define STATE_H

// 全局状态
class State
{
public:
    State();
    // 播放状态，0代表暂停中，1代表正在播放
    static int play_state;
    // 声音状态，0代表静音中，1代表正常声音
    static int voice_state;
    // 声音的默认大小
    static int volume_value;
};

#endif // STATE_H
