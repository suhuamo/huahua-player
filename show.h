#ifndef SHOW_H
#define SHOW_H

#include <QWidget>
#include<QDropEvent>
#include<QDragEnterEvent>
#include<QMimeData>

namespace Ui {
class Show;
}

//
///
/// \brief 视频界面显示类
///
class Show : public QWidget
{
    Q_OBJECT

public:
    explicit Show(QWidget *parent = 0);
    ~Show();
    bool Init();
    void OnPlay(QString strFile);
protected:
    //    鼠标拖拽放下事件
    void dropEvent(QDropEvent *event);
    //    鼠标拖动事件
    void dragEnterEvent(QDragEnterEvent *event);
signals:
    void SigPlay(QString strFile);
    void SigOpenFile(QString strFile);
private:
    bool initUi();
    bool connectionSignalSlots();

private:
    Ui::Show *ui; // lable
};

#endif // SHOW_H
