#ifndef SHOW_H
#define SHOW_H

#include <QWidget>
#include<QDropEvent>
#include<QDragEnterEvent>
#include<QMimeData>
#include<QResizeEvent>

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
private:
    Ui::Show *ui;

    int m_nLastFrameWidth;
    int m_nLastFrameHeight;
};

#endif // SHOW_H
