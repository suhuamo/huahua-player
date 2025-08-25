#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

#include<QString>

class GlobalHelper
{
public:
    /**
     * @brief 获取Qss样式
     * @param strQssPath
     * @return
     */
    static QString GetQssStr(QString strQssPath);
private:
    GlobalHelper();
};

#endif // GLOBALHELPER_H
