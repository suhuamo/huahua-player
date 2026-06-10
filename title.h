#ifndef TITLE_H
#define TITLE_H

#include <QWidget>

namespace Ui {
class Title;
}

class Title : public QWidget
{
    Q_OBJECT

public:
    explicit Title(QWidget *parent = 0);
    ~Title();
    bool init();
    void onPlay(QString filePath);
    void onStop(); // 用户主动停止时清空文件名
private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool initUi();
    void connectSignalSlots();
signals:
    void sigMinBtnClicked();
    void sigMaxBtnClicked();
    void sigFullScreenBtnClicked();
    void sigCloseBtnClicked();
    void sigMenuBtnClicked();

private:
    Ui::Title *ui;
};

#endif // TITLE_H
