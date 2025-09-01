#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include<QListWidget>

namespace Ui {
class Playlist;
}

class Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit Playlist(QWidget *parent = 0);
    ~Playlist();
    bool Init();
    void SlotOnAddFile(QString filePath);
    void SlotOnListItemDoubleClicked(QListWidgetItem *item);
    void SlotOnBackPlay();
    void SlotOnNextPlay();

private:
    bool initUi();
    void connectSignalSlots();
signals:
    /*
     * 播放文件(其实没必要提取这个信号，直接使用QListWidget自带的itemDoubleClicked即可，只是为了方便外面使用，可以直接拿到FilePath，不需要再通过getData(Qt::UserRole)再多一步了)
     * 还有第二个作用：复用，双击可以调用这个，上一个和下一个也可以调用这个，只是filePath传不同的值就行了
     * */
    void SigPlay(QString filePath);
private:
    Ui::Playlist *ui;
    int _current_play_list_index;   // 记录当前选中的哪个item，才能实现上一个和下一个的切换(从0开始计数，0代表第一个)
};

#endif // PLAYLIST_H
