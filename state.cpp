#include "state.h"
//默认是暂停中
int State::play_state = 0;
// 默认是正常中
int State::voice_state = 1;
// 初始声音在UI的初始化配置
int State::volume_value = 0;

State::State() {}
