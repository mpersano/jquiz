#include "synththread.h"

#include <QDebug>

namespace {
constexpr auto DictionaryPath = "/var/lib/mecab/dic/open-jtalk/naist-jdic";
constexpr auto VoicePath = "/usr/share/hts-voice/nitech-jp-atr503-m001/nitech_jp_atr503_m001.htsvoice";
constexpr auto SampleRate = 48000;
} // namespace

SynthThread::SynthThread(QObject *parent)
    : QThread(parent)
{
    initializeSynth();
}

SynthThread::~SynthThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();
    wait();
}

int SynthThread::sampleRate() const
{
    return SampleRate;
}

void SynthThread::synthesize(const QString &text)
{
    QMutexLocker locker(&m_mutex);
    m_text = text;
    if (!isRunning()) {
        start(LowPriority);
    } else {
        m_restart = true;
        m_condition.wakeOne();
    }
}

void SynthThread::run()
{
    for (;;) {
        m_mutex.lock();
        const auto text = m_text;
        m_mutex.unlock();

        if (m_abort)
            break;

        QElapsedTimer timer;
        timer.start();
        const auto audioData = m_synth.synthesize(text.toUtf8().data());
        qDebug() << "Synthesized in" << timer.elapsed() << "ms";

        emit synthesizedAudio(audioData);

        m_mutex.lock();
        if (!m_restart) {
            m_condition.wait(&m_mutex);
        }
        m_restart = false;
        m_mutex.unlock();
    }
}

void SynthThread::initializeSynth()
{
    if (!m_synth.loadDictionary(DictionaryPath)) {
        qWarning("Failed to read dictionary file %s", DictionaryPath);
        return;
    }

    if (!m_synth.loadVoice(VoicePath)) {
        qWarning("Failed to read voice file %s", VoicePath);
    }

    m_synth.setSamplingFrequency(SampleRate);
    m_synth.setFramePeriod(240);
    m_synth.setAllPassConstant(0.55);
    m_synth.setPostfilteringCoefficient(0.0);
    m_synth.setSpeechSpeedRate(1.0);
    m_synth.setAdditionalHalfTone(0.0);
    m_synth.setVoiceUnvoicedThreshold(0.5);
    m_synth.setGVWeightForSpectrum(1.0);
    m_synth.setGVWeightForLogF0(1.0);
    m_synth.setVolume(1.0);
    m_synth.setAudioBufferSize(0);

    m_initialized = true;
}
