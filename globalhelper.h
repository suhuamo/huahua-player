#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

#include<QString>
#include<QPushButton>
#include<QDebug>
#include<QKeyEvent>

class GlobalHelper
{
public:
    /**
     * @brief 获取Qss样式
     * @param strQssPath
     * @return
     */
    static QString GetQssStr(QString strQssPath);
    
    /**
     * @brief 将按键代码转换为可读的按键名称
     * @param key 按键代码
     * @return 可读的按键名称字符串
     */
    static QString GetKeyName(int key);
    /**
     * @brief 为按钮设置图片（通过第三方字库来设置的）
     * @param btn
     * @param iconSize
     * @param icon
     */
    static void SetIcon(QPushButton *btn, int iconSize, QChar icon);
//    设置音量同步到本地文件配置文件中
    static void SavePlayVolume(double &volume);
//    从本地文件配置文件获取音量
    static void GetPlayVolume(double& volume);
    static void SavePlaylist(QStringList& playList);    // 保存播放列表
    static void GetPlaylist(QStringList& playList);     // 获取播放列表
private:
    GlobalHelper();

};

const int MAX_SLIDER_VALUE = 65536;

#endif // GLOBALHELPER_H
