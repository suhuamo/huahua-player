#ifndef SHOW_H
#define SHOW_H

#include <QWidget>

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
private:
    bool initUi();

private:
    Ui::Show *ui; // lable
};

#endif // SHOW_H
