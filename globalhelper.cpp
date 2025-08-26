#include "globalhelper.h"
#include<QFile>
#include<QDebug>
#include<QFont>

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

void GlobalHelper::SetIcon(QPushButton *btn, int iconSize, QChar icon)
{
    QFont font;
    font.setFamily("FontAwesome");
    font.setPointSize(iconSize);
    btn->setFont(font);
    btn->setText(icon);
}

GlobalHelper::GlobalHelper()
{

}
