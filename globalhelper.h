#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

#include<QString>
#include<QPushButton>
#include<QDebug>

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
     * @brief 为按钮设置图片（通过第三方字库来设置的）
     * @param btn
     * @param iconSize
     * @param icon
     */
    static void SetIcon(QPushButton *btn, int iconSize, QChar icon);
private:
    GlobalHelper();
};

#endif // GLOBALHELPER_H
