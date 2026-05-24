#include "globalhelper.h"
#include<QFile>
#include<QDebug>
#include<QFont>
#include<QDir>
#include<QSettings>

/**
  操作系统的缓存目录
    Windows: C:\Users\{当前用户名}\AppData\Local\Temp\flower_player_config.ini
    macOS/Linux: /tmp/flower_player_config.ini
 * */
const QString PLAYER_CONFIG_BASEDIR = QDir::tempPath();

const QString PLAYER_CONFIG_FILENAME = "flower_player_config.ini";

const QString APP_VERSION = "0.1.0";

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

void GlobalHelper::SavePlayVolume(double& volume)
{
    QString strPlayerConfigFileName = PLAYER_CONFIG_BASEDIR + QDir::separator() + PLAYER_CONFIG_FILENAME;
    QSettings settings(strPlayerConfigFileName, QSettings::IniFormat);
    settings.setValue("volume/size", volume);
}

void GlobalHelper::GetPlayVolume(double& volume)
{
    QString strPlayerConfigFileName = PLAYER_CONFIG_BASEDIR + QDir::separator() + PLAYER_CONFIG_FILENAME;
    QSettings settings(strPlayerConfigFileName, QSettings::IniFormat);
    volume = settings.value("volume/size", volume).toDouble();
}

void GlobalHelper::SavePlaylist(QStringList& playList)
{
    QString strPlayerConfigFileName = PLAYER_CONFIG_BASEDIR + QDir::separator() + PLAYER_CONFIG_FILENAME;
    QSettings settings(strPlayerConfigFileName, QSettings::IniFormat);
    // 先移除旧的播放列表数组
    settings.remove("playlist");

    // 再写入新的播放列表
    settings.beginWriteArray("playlist");
    for (int i = 0; i < playList.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("movie", playList.at(i));
    }
    settings.endArray();
}

void GlobalHelper::GetPlaylist(QStringList& playList)
{
    QString strPlayerConfigFileName = PLAYER_CONFIG_BASEDIR + QDir::separator() + PLAYER_CONFIG_FILENAME;
    QSettings settings(strPlayerConfigFileName, QSettings::IniFormat);

    int size = settings.beginReadArray("playlist");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        playList.append(settings.value("movie").toString());
    }
    settings.endArray();
}


GlobalHelper::GlobalHelper()
{

}
