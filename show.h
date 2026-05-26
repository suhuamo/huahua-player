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
    bool Init();
    void OnPlay(QString strFile);
    void OnFrameDimensionsChanged(int nFrameWidth, int nFrameHeight);
    void ShowToast(const QString &text);
    void ShowShortcutHint(const QString &text); // 全屏快捷键提示（居中显示）
    void HideShortcutHint(); // 强制隐藏快捷键提示
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    void ChangeShow();
    bool initUi();
    bool connectionSignalSlots();

signals:
    void SigPlay(QString strFile);
    void SigOpenFile(QString strFile);
private slots:
    void OnToastTimeout();
    void OnShortcutHintTimeout();
private:
    Ui::Show *ui;

    int m_nLastFrameWidth;
    int m_nLastFrameHeight;
    
    QLabel *m_toastLabel;  // 提示标签（使用顶层窗口实现透明，右上角显示）
    QTimer *m_toastTimer;  // 提示定时器
    
    QLabel *m_shortcutHintLabel;  // 快捷键提示标签（居中显示）
    QTimer *m_shortcutHintTimer;  // 快捷键提示定时器
};

#endif // SHOW_H
