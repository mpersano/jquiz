#pragma once

#include "synth.h"

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class SynthThread : public QThread
{
    Q_OBJECT

public:
    explicit SynthThread(QObject *parent = nullptr);
    ~SynthThread();

    int sampleRate() const;
    void synthesize(const QString &text);

signals:
    void synthesizedAudio(const QByteArray &audioData);

protected:
    void run() override;

private:
    void initializeSynth();

    Synth m_synth;
    QMutex m_mutex;
    QWaitCondition m_condition;
    QString m_text;
    bool m_restart = false;
    bool m_abort = false;
    bool m_initialized = false;
};
