#include "synth.h"

#include <mecab2njd.h>
#include <njd2jpcommon.h>
#include <njd_set_accent_phrase.h>
#include <njd_set_accent_type.h>
#include <njd_set_digit.h>
#include <njd_set_long_vowel.h>
#include <njd_set_pronunciation.h>
#include <njd_set_unvoiced_vowel.h>
#include <text2mecab.h>

#include <QDebug>

#include <algorithm>
#include <clocale>

namespace {

class LocaleSetter
{
public:
    LocaleSetter(int category, const char *locale)
        : m_category(category)
        , m_oldLocale(std::setlocale(category, nullptr))
    {
        std::setlocale(category, locale);
    }

    ~LocaleSetter()
    {
        std::setlocale(m_category, m_oldLocale.c_str());
    }

    LocaleSetter(const LocaleSetter &) = delete;
    LocaleSetter &operator=(const LocaleSetter &) = delete;

private:
    int m_category;
    std::string m_oldLocale;
};

} // namespace

Synth::Synth()
{
    Mecab_initialize(&m_mecab);
    NJD_initialize(&m_njd);
    JPCommon_initialize(&m_jpcommon);
    HTS_Engine_initialize(&m_engine);
}

Synth::~Synth()
{
    HTS_Engine_clear(&m_engine);
    JPCommon_clear(&m_jpcommon);
    NJD_clear(&m_njd);
    Mecab_clear(&m_mecab);
}

bool Synth::loadDictionary(const char *dictionary)
{
    return Mecab_load(&m_mecab, dictionary) == TRUE;
}

bool Synth::loadVoice(const char *voice)
{
    // hts_engine uses atof, set locale to "C"
    LocaleSetter locale(LC_NUMERIC, "C");

    char *voices = const_cast<char *>(voice);
    if (HTS_Engine_load(&m_engine, &voices, 1) != TRUE) {
        return false;
    }
    qDebug() << "Loaded voice, label format:" << HTS_Engine_get_fullcontext_label_format(&m_engine);

    return true;
}

void Synth::setSamplingFrequency(size_t value)
{
    HTS_Engine_set_sampling_frequency(&m_engine, value);
}

void Synth::setFramePeriod(size_t value)
{
    HTS_Engine_set_fperiod(&m_engine, value);
}

void Synth::setAllPassConstant(double value)
{
    HTS_Engine_set_alpha(&m_engine, value);
}

void Synth::setPostfilteringCoefficient(double value)
{
    HTS_Engine_set_beta(&m_engine, value);
}

void Synth::setSpeechSpeedRate(double value)
{
    HTS_Engine_set_speed(&m_engine, value);
}

void Synth::setAdditionalHalfTone(double value)
{
    HTS_Engine_add_half_tone(&m_engine, value);
}

void Synth::setVoiceUnvoicedThreshold(double value)
{
    HTS_Engine_set_msd_threshold(&m_engine, 1, value);
}

void Synth::setGVWeightForSpectrum(double value)
{
    HTS_Engine_set_gv_weight(&m_engine, 0, value);
}

void Synth::setGVWeightForLogF0(double value)
{
    HTS_Engine_set_gv_weight(&m_engine, 1, value);
}

void Synth::setVolume(double value)
{
    HTS_Engine_set_volume(&m_engine, value);
}

void Synth::setAudioBufferSize(size_t value)
{
    HTS_Engine_set_audio_buff_size(&m_engine, value);
}

QByteArray Synth::synthesize(const char *text)
{
    char buf[1024];
    text2mecab(buf, text);
    Mecab_analysis(&m_mecab, buf);

    mecab2njd(&m_njd, Mecab_get_feature(&m_mecab), Mecab_get_size(&m_mecab));
    njd_set_pronunciation(&m_njd);
    njd_set_digit(&m_njd);
    njd_set_accent_phrase(&m_njd);
    njd_set_accent_type(&m_njd);
    njd_set_unvoiced_vowel(&m_njd);
    njd_set_long_vowel(&m_njd);
    njd2jpcommon(&m_jpcommon, &m_njd);
    JPCommon_make_label(&m_jpcommon);
    QByteArray result;
    if (JPCommon_get_label_size(&m_jpcommon) > 2) {
        if (HTS_Engine_synthesize_from_strings(&m_engine, JPCommon_get_label_feature(&m_jpcommon), JPCommon_get_label_size(&m_jpcommon)) == TRUE) {
            const auto sampleCount = HTS_Engine_get_nsamples(&m_engine);
            result.resize(sampleCount * sizeof(short));
            auto *data = result.data();
            for (int i = 0; i < sampleCount; ++i) {
                const auto sample = HTS_Engine_get_generated_speech(&m_engine, i);
                const short value = static_cast<short>(std::clamp(sample, -32768.0, 32767.0));
                *data++ = value & 0xff;
                *data++ = (value >> 8) & 0xff;
            }
        }
        HTS_Engine_refresh(&m_engine);
    }
    JPCommon_refresh(&m_jpcommon);
    NJD_refresh(&m_njd);
    Mecab_refresh(&m_mecab);

    return result;
}
