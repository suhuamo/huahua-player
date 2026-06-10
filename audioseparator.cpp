#include "audioseparator.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDebug>
#include <QRegularExpression>

AudioSeparator* AudioSeparator::m_instance = nullptr;

AudioSeparator::AudioSeparator(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_separating(false)
    , m_model_index(0)
    , m_last_percent(0)
{
    m_cache_base_dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/huahua-stems";
    QDir().mkpath(m_cache_base_dir);
}

AudioSeparator::~AudioSeparator()
{
    cancelSeparation();
}

AudioSeparator* AudioSeparator::GetInstance()
{
    if (m_instance == nullptr) {
        m_instance = new AudioSeparator();
    }
    return m_instance;
}

QString AudioSeparator::getCacheDir() const
{
    return m_cache_base_dir;
}

QString AudioSeparator::getFileHash(const QString &filePath) const
{
    // 使用文件路径 + 文件大小生成哈希，避免同名不同内容的文件冲突
    QFile file(filePath);
    qint64 fileSize = 0;
    if (file.open(QIODevice::ReadOnly)) {
        fileSize = file.size();
        file.close();
    }
    QString key = filePath + QString::number(fileSize);
    return QString(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex());
}

bool AudioSeparator::isCached(const QString &filePath) const
{
    QString hash = getFileHash(filePath);
    QString cacheSubDir = m_cache_base_dir + "/" + hash;
    // 检查 4 个 stem 文件是否都存在
    QStringList stems = {"vocals", "drums", "bass", "other"};
    for (const QString &stem : stems) {
        QString stemPath = cacheSubDir + "/" + stem + ".wav";
        if (!QFile::exists(stemPath)) {
            return false;
        }
    }
    return true;
}

QString AudioSeparator::getStemPath(const QString &filePath, AudioMode mode) const
{
    if (mode == AUDIO_ORIGINAL) {
        return QString();
    }

    QString hash = getFileHash(filePath);
    QString cacheSubDir = m_cache_base_dir + "/" + hash;

    switch (mode) {
    case AUDIO_VOCALS:
        return cacheSubDir + "/vocals.wav";
    case AUDIO_ACCOMPANIMENT:
        return cacheSubDir + "/accompaniment.wav";
    case AUDIO_DRUMS:
        return cacheSubDir + "/drums.wav";
    case AUDIO_BASS:
        return cacheSubDir + "/bass.wav";
    case AUDIO_OTHER:
        return cacheSubDir + "/other.wav";
    default:
        return QString();
    }
}

bool AudioSeparator::isDemucsAvailable() const
{
    // 尝试多种 python 命令名，兼容不同 Windows 安装方式
    QStringList pythonNames = {"python", "python3", "py"};
    for (const QString &pythonName : pythonNames) {
        QProcess testProcess;
        testProcess.start(pythonName, QStringList() << "-m" << "demucs" << "--help");
        bool finished = testProcess.waitForFinished(5000);
        if (!finished) {
            testProcess.kill();
            testProcess.waitForFinished(1000);
        }
        if (finished && testProcess.exitCode() == 0) {
            // 记住可用的 python 命令，后续 startSeparation 使用
            m_python_cmd = pythonName;
            return true;
        }
    }

    // 尝试从常见安装路径查找
    QStringList searchPaths;
    QString localAppDir = qgetenv("LOCALAPPDATA");
    if (!localAppDir.isEmpty()) {
        QDir pyDir(localAppDir + "/Programs/Python");
        QStringList pyVersions = pyDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &ver : pyVersions) {
            searchPaths << pyDir.absoluteFilePath(ver) + "/python.exe";
        }
    }
    for (const QString &pythonPath : searchPaths) {
        if (!QFile::exists(pythonPath)) continue;
        QProcess testProcess;
        testProcess.start(pythonPath, QStringList() << "-m" << "demucs" << "--help");
        bool finished = testProcess.waitForFinished(5000);
        if (!finished) {
            testProcess.kill();
            testProcess.waitForFinished(1000);
        }
        if (finished && testProcess.exitCode() == 0) {
            m_python_cmd = pythonPath;
            return true;
        }
    }

    return false;
}

bool AudioSeparator::isSeparating() const
{
    return m_separating;
}

QString AudioSeparator::currentSeparatingFile() const
{
    return m_current_file;
}

void AudioSeparator::startSeparation(const QString &filePath)
{
    if (m_separating) {
        emit sigSeparationFailed(tr("已有分离任务正在进行"));
        return;
    }

    // 如果已经缓存了，直接返回
    if (isCached(filePath)) {
        emit sigSeparationCompleted(filePath);
        return;
    }

    m_current_file = filePath;
    m_separating = true;
    m_model_index = 0;
    m_last_percent = 0;

    QString hash = getFileHash(filePath);
    QString cacheSubDir = m_cache_base_dir + "/" + hash;
    QDir().mkpath(cacheSubDir);

    // 构建 demucs 命令
    // -n htdemucs_ft: 使用 htdemucs_ft 模型（4-stem: vocals/drums/bass/other）
    // -o: 指定输出目录
    // --filename "{stem}.{ext}": 简化输出文件名，避免创建以文件名命名的子目录
    QStringList args;
    args << "-m" << "demucs"
         << "-n" << "htdemucs_ft"
         << "-o" << cacheSubDir
         << "--filename" << "{stem}.{ext}"
         << filePath;

    // 使用检测到的 python 命令（若尚未检测则用默认值）
    QString pythonCmd = m_python_cmd.isEmpty() ? "python" : m_python_cmd;

    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &AudioSeparator::onProcessReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &AudioSeparator::onProcessFinished);

    qDebug() << "AudioSeparator: starting demucs with:" << pythonCmd << args.join(" ");
    m_process->start(pythonCmd, args);

    if (!m_process->waitForStarted(3000)) {
        m_separating = false;
        delete m_process;
        m_process = nullptr;
        emit sigSeparationFailed(tr("无法启动 demucs，请确认已安装 Python 和 demucs"));
        return;
    }

    emit sigSeparationStarted();
}

void AudioSeparator::cancelSeparation()
{
    if (m_process && m_separating) {
        m_process->kill();
        m_process->waitForFinished(3000);
        m_process->deleteLater();
        m_process = nullptr;
        m_separating = false;
        m_current_file.clear();
        emit sigSeparationCancelled();
    }
}

void AudioSeparator::onProcessReadyReadStandardError()
{
    if (!m_process) return;

    QString output = QString::fromUtf8(m_process->readAllStandardError());

    // htdemucs_ft 是 4 个模型的 bag，会输出 4 个进度条
    // 每个进度条从 0% 跑到 100%，然后下一个从 0% 开始
    // 通过检测百分比从高值回落到低值来判断新进度条开始
    QRegularExpression re("(\\d+)%\\|");
    QRegularExpressionMatchIterator it = re.globalMatch(output);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int percent = match.captured(1).toInt();

        // 检测新进度条：上次>=50% 且 这次<上次→说明新进度条开始了
        if (m_last_percent >= 50 && percent < m_last_percent && m_model_index < 3) {
            m_model_index++;
        }
        m_last_percent = percent;

        emit sigSeparationProgress(m_model_index, percent);
    }
}

void AudioSeparator::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString filePath = m_current_file;
    m_separating = false;
    m_current_file.clear();

    if (m_process) {
        m_process->deleteLater();
        m_process = nullptr;
    }

    if (exitStatus == QProcess::CrashExit) {
        emit sigSeparationFailed(tr("demucs 进程异常终止"));
        return;
    }

    if (exitCode != 0) {
        emit sigSeparationFailed(tr("demucs 分离失败，退出码: %1").arg(exitCode));
        return;
    }

    // demucs 使用 --filename "{stem}.{ext}" 后仍会在 -o 目录下创建模型名子目录（如 htdemucs_ft/）
    // 需要先从子目录移动文件到缓存根目录，再生成伴奏
    QString hash = getFileHash(filePath);
    QString cacheSubDir = m_cache_base_dir + "/" + hash;

    QStringList stems = {"vocals", "drums", "bass", "other"};
    for (const QString &stem : stems) {
        QString expectedPath = cacheSubDir + "/" + stem + ".wav";
        if (!QFile::exists(expectedPath)) {
            // 从 demucs 创建的子目录（如 htdemucs_ft/）移动文件
            QDir cacheDir(cacheSubDir);
            QStringList subDirs = cacheDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QString &subDir : subDirs) {
                QString candidatePath = cacheSubDir + "/" + subDir + "/" + stem + ".wav";
                if (QFile::exists(candidatePath)) {
                    QFile::rename(candidatePath, expectedPath);
                }
            }
        }
    }

    // 清理可能存在的空子目录
    QDir cacheDir(cacheSubDir);
    QStringList subDirs = cacheDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subDir : subDirs) {
        QDir subDirFull(cacheSubDir + "/" + subDir);
        subDirFull.rmdir(cacheSubDir + "/" + subDir);
    }

    // 通知 UI：stem 文件已就绪，即将生成伴奏
    emit sigSeparationStemsReady(filePath);

    // 生成伴奏文件（drums + bass + other 混合）
    generateAccompaniment(cacheSubDir);

    emit sigSeparationCompleted(filePath);
}

bool AudioSeparator::generateAccompaniment(const QString &cacheSubDir)
{
    // 伴奏 = drums + bass + other，使用 FFmpeg amix 混合
    QString drumsPath = cacheSubDir + "/drums.wav";
    QString bassPath = cacheSubDir + "/bass.wav";
    QString otherPath = cacheSubDir + "/other.wav";
    QString accompPath = cacheSubDir + "/accompaniment.wav";

    // 如果伴奏文件已存在则跳过
    if (QFile::exists(accompPath)) {
        return true;
    }

    // 检查三个源文件是否存在
    if (!QFile::exists(drumsPath) || !QFile::exists(bassPath) || !QFile::exists(otherPath)) {
        qWarning() << "AudioSeparator: missing stem files for accompaniment generation";
        return false;
    }

    // 使用 FFmpeg 混合: ffmpeg -i drums.wav -i bass.wav -i other.wav
    //   -filter_complex "[0:a][1:a][2:a]amix=inputs=3:duration=longest" accompaniment.wav
    QStringList args;
    args << "-y"  // 覆盖已有文件
         << "-i" << drumsPath
         << "-i" << bassPath
         << "-i" << otherPath
         << "-filter_complex" << "[0:a][1:a][2:a]amix=inputs=3:duration=longest"
         << accompPath;

    QProcess ffmpeg;
    ffmpeg.start("ffmpeg", args);
    if (!ffmpeg.waitForFinished(30000)) {
        ffmpeg.kill();
        ffmpeg.waitForFinished(3000);
        qWarning() << "AudioSeparator: FFmpeg accompaniment generation timed out";
        return false;
    }

    if (ffmpeg.exitCode() != 0) {
        qWarning() << "AudioSeparator: FFmpeg accompaniment generation failed:" << ffmpeg.readAllStandardError();
        // 如果 ffmpeg 不可用，尝试简单方案：复制 other.wav 作为伴奏（至少有点声音）
        if (!QFile::copy(otherPath, accompPath)) {
            return false;
        }
    }

    return QFile::exists(accompPath);
}
