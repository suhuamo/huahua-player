#ifndef SHOW_H
#define SHOW_H

#include <QWidget>
#include<QDropEvent>
#include<QDragEnterEvent>
#include<QMimeData>
#include<QResizeEvent>
#include<QLabel>
#include<QTimer>

namespace Ui {
class Show;
}

class Show : public QWidget
{
    Q_OBJECT

public:
    explicit Show(QWidget *parent = 0);
    ~Show();
    bool init();
    void onPlay(QString strFile);
    void onFrameDimensionsChanged(int nFrameWidth, int nFrameHeight);
    void showToast(const QString &text);
    void showShortcutHint(const QString &text); // 全屏快捷键提示（居中显示）
    void hideShortcutHint(); // 强制隐藏快捷键提示
    QString getCurrentFile();
    void onStartPlay(QString filename);
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event); // 键盘事件处理
private:
    void changeShow();
    bool initUi();
    bool connectSignalSlots();

signals:
    void sigPlay(QString strFile);
    void sigOpenFile(QString strFile);
    void sigExitFullScreen();
private slots:
    void onToastTimeout();
    void onShortcutHintTimeout();
private:
    Ui::Show *ui;

    int m_last_frame_width;
    int m_last_frame_height;
    QString m_current_file; // 当前播放的文件路径
    
    QLabel *m_toast_label;  // 提示标签（使用顶层窗口实现透明，右上角显示）
    QTimer *m_toast_timer;  // 提示定时器

    QLabel *m_shortcut_hint_label;  // 快捷键提示标签（居中显示）
    QTimer *m_shortcut_hint_timer;  // 快捷键提示定时器
};

#endif // SHOW_H
