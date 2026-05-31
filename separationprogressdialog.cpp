#include "separationprogressdialog.h"
#include <QApplication>
#include <QFileInfo>
#include <QTimer>

// htdemucs_ft 的 4 个模型分别产出不同的 stem
// 模型 0 → 人声(vocals), 模型 1 → 鼓点(drums), 模型 2 → 贝斯(bass), 模型 3 → 其他(other)
static const int STEM_FROM_MODEL[] = { 0, 1, 2, 3 };

static const char *STEM_NAMES[] = {
    QT_TR_NOOP("人声"),
    QT_TR_NOOP("鼓点"),
    QT_TR_NOOP("贝斯"),
    QT_TR_NOOP("其他")
};

#define MODEL_COUNT 4

SeparationProgressDialog::SeparationProgressDialog(QWidget *parent)
    : QDialog(parent)
    , m_completed(false)
{
    setWindowTitle(tr("音频分离"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(360, 240);
    setModal(false);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(4);

    // 文件名
    m_file_label = new QLabel(tr("文件：--"), this);
    m_file_label->setWordWrap(true);
    mainLayout->addWidget(m_file_label);

    // 目标模式
    m_mode_label = new QLabel(tr("目标模式：--"), this);
    mainLayout->addWidget(m_mode_label);

    // 4 个产出状态标签
    for (int i = 0; i < 4; i++) {
        m_stem_status[i] = STEM_WAITING;
        m_stem_labels[i] = new QLabel(this);
        updateStemLabel(i);
        mainLayout->addWidget(m_stem_labels[i]);
    }

    // 进度条
    m_progress_bar = new QProgressBar(this);
    m_progress_bar->setRange(0, 100);
    m_progress_bar->setValue(0);
    m_progress_bar->setTextVisible(true);
    mainLayout->addWidget(m_progress_bar);

    // 进度文字
    m_progress_label = new QLabel(tr("正在准备..."), this);
    mainLayout->addWidget(m_progress_label);

    // 取消按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_cancel_btn = new QPushButton(tr("取消"), this);
    m_cancel_btn->setFixedSize(80, 28);
    connect(m_cancel_btn, &QPushButton::clicked, this, &SeparationProgressDialog::OnCancelClicked);
    btnLayout->addWidget(m_cancel_btn);
    mainLayout->addLayout(btnLayout);
}

void SeparationProgressDialog::setFileInfo(const QString &fileName, const QString &modeName)
{
    m_file_name = QFileInfo(fileName).fileName();
    m_mode_name = modeName;
    m_file_label->setText(tr("文件：%1").arg(m_file_name));
    m_mode_label->setText(tr("目标模式：%1").arg(m_mode_name));
}

void SeparationProgressDialog::setProgress(int modelIndex, int percent)
{
    if (m_completed) return;

    // 总进度 = (已完成模型数 * 100 + 当前模型进度) / 总模型数 * 85%
    // 85% 预留给 AI 分离，15% 预留给生成伴奏
    int totalPercent = (modelIndex * 100 + percent) * 85 / (MODEL_COUNT * 100);
    m_progress_bar->setValue(totalPercent);
    m_progress_label->setText(tr("AI 分离进度：%1/%2 — %3%")
        .arg(modelIndex + 1).arg(MODEL_COUNT).arg(percent));

    // 已完成的模型对应的 stem 标记为完成
    for (int i = 0; i < MODEL_COUNT; i++) {
        int stemIdx = STEM_FROM_MODEL[i];
        if (i < modelIndex) {
            if (m_stem_status[stemIdx] != STEM_DONE) {
                m_stem_status[stemIdx] = STEM_DONE;
                updateStemLabel(stemIdx);
            }
        } else if (i == modelIndex) {
            if (m_stem_status[stemIdx] != STEM_PROCESSING) {
                m_stem_status[stemIdx] = STEM_PROCESSING;
                updateStemLabel(stemIdx);
            }
        }
    }
}

void SeparationProgressDialog::setGeneratingAccompaniment()
{
    if (m_completed) return;
    // 所有 4 个 stem 已就绪
    for (int i = 0; i < MODEL_COUNT; i++) {
        int stemIdx = STEM_FROM_MODEL[i];
        if (m_stem_status[stemIdx] != STEM_DONE) {
            m_stem_status[stemIdx] = STEM_DONE;
            updateStemLabel(stemIdx);
        }
    }
    m_progress_bar->setValue(90);
    m_progress_label->setText(tr("正在生成伴奏..."));
}

void SeparationProgressDialog::setCompleted()
{
    m_completed = true;
    for (int i = 0; i < 4; i++) {
        m_stem_status[i] = STEM_DONE;
        updateStemLabel(i);
    }
    m_progress_bar->setValue(100);
    m_progress_label->setText(tr("全部完成！"));
    m_cancel_btn->setText(tr("关闭"));
    disconnect(m_cancel_btn, &QPushButton::clicked, this, &SeparationProgressDialog::OnCancelClicked);
    connect(m_cancel_btn, &QPushButton::clicked, this, &QDialog::close);
    QTimer::singleShot(1500, this, &QDialog::close);
}

void SeparationProgressDialog::setFailed(const QString &error)
{
    m_completed = true;
    m_progress_label->setText(tr("错误：%1").arg(error));
    m_progress_bar->setValue(0);
    m_cancel_btn->setText(tr("关闭"));
    disconnect(m_cancel_btn, &QPushButton::clicked, this, &SeparationProgressDialog::OnCancelClicked);
    connect(m_cancel_btn, &QPushButton::clicked, this, &QDialog::close);
}

void SeparationProgressDialog::OnCancelClicked()
{
    emit SigCancelRequested();
    close();
}

void SeparationProgressDialog::closeEvent(QCloseEvent *event)
{
    if (!m_completed) {
        emit SigCancelRequested();
    }
    event->accept();
}

void SeparationProgressDialog::updateStemLabel(int index)
{
    m_stem_labels[index]->setText(QString("%1 %2").arg(stemIcon(m_stem_status[index])).arg(tr(STEM_NAMES[index])));
}

QString SeparationProgressDialog::stemIcon(StemStatus status)
{
    switch (status) {
    case STEM_WAITING:    return QString::fromUtf8("⏳");
    case STEM_PROCESSING: return QString::fromUtf8("⏳");
    case STEM_DONE:       return QString::fromUtf8("✅");
    }
    return QString();
}
