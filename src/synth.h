#pragma once

#include <HTS_engine.h>
#include <jpcommon.h>
#include <mecab.h>
#include <njd.h>

#include <QByteArray>

class Synth
{
public:
    Synth();
    ~Synth();

    Synth(const Synth &) = delete;
    Synth &operator=(const Synth &) = delete;

    bool loadDictionary(const char *dictionary);
    bool loadVoice(const char *voice);

    void setSamplingFrequency(size_t value);
    void setFramePeriod(size_t value);
    void setAllPassConstant(double value);
    void setPostfilteringCoefficient(double value);
    void setSpeechSpeedRate(double value);
    void setAdditionalHalfTone(double value);
    void setVoiceUnvoicedThreshold(double value);
    void setGVWeightForSpectrum(double value);
    void setGVWeightForLogF0(double value);
    void setVolume(double value);
    void setAudioBufferSize(size_t value);

    QByteArray synthesize(const char *text);

private:
    HTS_Engine m_engine;
    NJD m_njd;
    JPCommon m_jpcommon;
    Mecab m_mecab;
};
