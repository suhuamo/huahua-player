#include "globalhelper.h"
#include<QFile>
#include<QDebug>

QString GlobalHelper::GetQssStr(QString strQssPath)
{
    QString style;
    QFile qss(strQssPath);
    if(qss.open(QIODevice::ReadOnly)) {
        style = QLatin1String(qss.readAll());
        qss.close();
    } else {
        qDebug() << "读取样式表失败:" << strQssPath;
    }
    return style;
}

GlobalHelper::GlobalHelper()
{

}
