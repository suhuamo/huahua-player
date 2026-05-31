#ifndef AUDIOSEPARATOR_H
#define AUDIOSEPARATOR_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QMap>

// 音频分离模式
enum AudioMode {
    AUDIO_ORIGINAL = 0,      // 原声
    AUDIO_VOCALS = 1,        // 仅人声
    AUDIO_ACCOMPANIMENT = 2, // 仅伴奏（drums + bass + other）
    AUDIO_DRUMS = 3,         // 仅鼓点
    AUDIO_BASS = 4,          // 仅贝斯
    AUDIO_OTHER = 5          // 仅其他
};

class AudioSeparator : public QObject
{
    Q_OBJECT
public:
    static AudioSeparator* GetInstance();
    ~AudioSeparator();

    // 开始分离（4-stem 模式：vocals/drums/bass/other）
    void startSeparation(const QString &filePath);
    // 是否已缓存
    bool isCached(const QString &filePath) const;
    // 获取指定模式的 stem 文件路径（空字符串表示不存在）
    QString getStemPath(const QString &filePath, AudioMode mode) const;
    // 取消正在进行的分离
    void cancelSeparation();
    // 是否正在分离
    bool isSeparating() const;
    // 检测 python + demucs 是否可用
    bool isDemucsAvailable() const;
    // 获取当前正在分离的文件
    QString currentSeparatingFile() const;

signals:
    void SigSeparationStarted();
    // modelIndex: 当前模型的索引(0~3)，percent: 该模型内的进度(0~100)
    // htdemucs_ft 是 4 个模型的 bag，会输出 4 个进度条
    void SigSeparationProgress(int modelIndex, int percent);
    void SigSeparationStemsReady(const QString &filePath);  // stem 文件已就绪，开始生成伴奏
    void SigSeparationCompleted(const QString &filePath);
    void SigSeparationFailed(const QString &error);
    void SigSeparationCancelled();

private slots:
    void OnProcessReadyReadStandardError();
    void OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    explicit AudioSeparator(QObject *parent = nullptr);

    // 获取缓存根目录
    QString getCacheDir() const;
    // 根据文件路径生成缓存子目录名（MD5 哈希）
    QString getFileHash(const QString &filePath) const;
    // 生成伴奏文件（drums + bass + other 混合）
    bool generateAccompaniment(const QString &cacheSubDir);

    static AudioSeparator* m_instance;
    QProcess *m_process;
    QString m_current_file;    // 当前正在分离的文件路径
    bool m_separating;
    QString m_cache_base_dir;  // 缓存根目录
    mutable QString m_python_cmd;  // 检测到的可用 python 命令（mutable 因为 isDemucsAvailable 是 const）
    int m_model_index;         // 当前进度条对应的模型索引(0~3)
    int m_last_percent;        // 上一次解析到的百分比，用于检测新进度条
};

#endif // AUDIOSEPARATOR_H
