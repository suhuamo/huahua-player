#ifndef SEPARATIONPROGRESSDIALOG_H
#define SEPARATIONPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

class SeparationProgressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SeparationProgressDialog(QWidget *parent = nullptr);

    // 设置文件和模式信息
    void setFileInfo(const QString &fileName, const QString &modeName);
    // 更新 AI 分离进度（modelIndex: 当前模型索引 0~3，percent: 该模型内的进度 0~100）
    void setProgress(int modelIndex, int percent);
    // 切换到"生成伴奏"步骤
    void setGeneratingAccompaniment();
    // 分离完成
    void setCompleted();
    // 分离失败
    void setFailed(const QString &error);

signals:
    void sigCancelRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onCancelClicked();

private:
    enum StemStatus { STEM_WAITING, STEM_PROCESSING, STEM_DONE };

    QLabel *m_file_label;          // 文件名
    QLabel *m_mode_label;          // 目标模式
    QLabel *m_stem_labels[4];      // 4个产出的状态标签：人声/鼓点/贝斯/其他
    StemStatus m_stem_status[4];   // 4个产出的状态
    QProgressBar *m_progress_bar;
    QLabel *m_progress_label;      // 进度文字
    QPushButton *m_cancel_btn;
    bool m_completed;

    QString m_file_name;
    QString m_mode_name;

    void updateStemLabel(int index);
    static QString stemIcon(StemStatus status);
};

#endif // SEPARATIONPROGRESSDIALOG_H
