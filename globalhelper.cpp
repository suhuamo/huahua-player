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

QString GlobalHelper::GetKeyName(int key)
{
    QString keyName;
    switch (key) {
        case Qt::Key_Space: keyName = "Space"; break;
        case Qt::Key_Escape: keyName = "ESC"; break;
        case Qt::Key_F1: keyName = "F1"; break;
        case Qt::Key_F2: keyName = "F2"; break;
        case Qt::Key_F3: keyName = "F3"; break;
        case Qt::Key_F4: keyName = "F4"; break;
        case Qt::Key_F5: keyName = "F5"; break;
        case Qt::Key_F6: keyName = "F6"; break;
        case Qt::Key_F7: keyName = "F7"; break;
        case Qt::Key_F8: keyName = "F8"; break;
        case Qt::Key_F9: keyName = "F9"; break;
        case Qt::Key_F10: keyName = "F10"; break;
        case Qt::Key_F11: keyName = "F11"; break;
        case Qt::Key_F12: keyName = "F12"; break;
        case Qt::Key_Left: keyName = "Left"; break;
        case Qt::Key_Right: keyName = "Right"; break;
        case Qt::Key_Up: keyName = "Up"; break;
        case Qt::Key_Down: keyName = "Down"; break;
        case Qt::Key_Enter: keyName = "Enter"; break;
        case Qt::Key_Return: keyName = "Return"; break;
        case Qt::Key_Backspace: keyName = "Backspace"; break;
        case Qt::Key_Tab: keyName = "Tab"; break;
        case Qt::Key_Delete: keyName = "Delete"; break;
        case Qt::Key_Insert: keyName = "Insert"; break;
        case Qt::Key_Home: keyName = "Home"; break;
        case Qt::Key_End: keyName = "End"; break;
        case Qt::Key_PageUp: keyName = "PageUp"; break;
        case Qt::Key_PageDown: keyName = "PageDown"; break;
        case Qt::Key_CapsLock: keyName = "CapsLock"; break;
        case Qt::Key_NumLock: keyName = "NumLock"; break;
        case Qt::Key_ScrollLock: keyName = "ScrollLock"; break;
        case Qt::Key_Print: keyName = "Print"; break;
        case Qt::Key_Pause: keyName = "Pause"; break;
        case Qt::Key_SysReq: keyName = "SysReq"; break;
        case Qt::Key_Clear: keyName = "Clear"; break;
        default: 
            if (key >= Qt::Key_A && key <= Qt::Key_Z) {
                keyName = QChar('A' + key - Qt::Key_A);
            } else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
                keyName = QChar('0' + key - Qt::Key_0);
            } else if (key >= Qt::Key_Asterisk && key <= Qt::Key_division) {
                // 小键盘数字
                keyName = QString("Num%1").arg(key - Qt::Key_0);
            } else {
                keyName = QString::number(key);
            }
            break;
    }
    return keyName;
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
