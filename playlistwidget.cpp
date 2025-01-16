#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlayListWidget::PlayListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayListWidget)
{
    ui->setupUi(this);

    // 设置默认条目的数据
    ui->listWidget->item(0)->setData(Qt::UserRole, QVariant("demo.mp4"));
    ui->listWidget->item(1)->setData(Qt::UserRole, QVariant("demo.mp4"));

    // 创建动作并连接到槽函数
    QAction *actionAdd = new QAction("添加", this);
    connect(actionAdd, &QAction::triggered, this, &PlayListWidget::onActionAddTriggered);

    QAction *actionRemove = new QAction("移除所选项", this);
    connect(actionRemove, &QAction::triggered, this, &PlayListWidget::onActionRemoveTriggered);

    QAction *actionClearList = new QAction("清空列表", this);
    connect(actionClearList, &QAction::triggered, ui->listWidget, &QListWidget::clear);

    QAction *actionExit = new QAction("Exit", this);
    connect(actionExit, &QAction::triggered, &QApplication::quit);

    // 创建一个上下文菜单，并添加动作
    contextMenu = new QMenu(this);
    contextMenu->addAction(actionAdd);
    contextMenu->addAction(actionRemove);
    contextMenu->addAction(actionClearList);
    contextMenu->addAction(actionExit);

}

QListWidget *PlayListWidget::getListWidget()
{
    return ui->listWidget;
}

void PlayListWidget::contextMenuEvent(QContextMenuEvent *event)  {
    // 在事件位置显示上下文菜单
    contextMenu->exec(event->globalPos());
}

void PlayListWidget::onActionAddTriggered() {
    // 获取用户选择的所有文件的路径
    QStringList filePathList = QFileDialog::getOpenFileNames(this, "打开文件", QDir::homePath(),
                                                             "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");
    // 遍历所有选择的文件绝对路径
    for (QString strFilePath : filePathList)
    {
        // 获取该路径对应的文件对象
        QFileInfo fileInfo(strFilePath);
        // 创建列表的Item对象
        QListWidgetItem *pItem = new QListWidgetItem(ui->listWidget);
        // 设置Data数据,用户自己保存自己使用的数据, Qt::UserRole 代表是用户自己的数据，也可以写其他的角色，获取的时候就要写其他角色来获取
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));
        // 设置条目显示的标题
        pItem->setText(fileInfo.fileName());
        // 设置鼠标放在条目上悬浮出来的文本
        pItem->setToolTip(fileInfo.filePath());
        // 将当前条目添加到列表中 —— 但是好像不需要，因为该条目创建的时候就绑定了该列表了，故这行不加也没影响
        ui->listWidget->addItem(pItem);
    }
}

void PlayListWidget::onActionRemoveTriggered() {
    // 删除当前选择的行【默认选择的是第一行】
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}


PlayListWidget::~PlayListWidget()
{
    delete ui;
}
