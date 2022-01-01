#ifdef ENABLE_SPEECH_SYNTH
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>
#endif
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTextStream>

#include "quiz.h"
#ifdef ENABLE_SPEECH_SYNTH
#include "synththread.h"
#endif

Quiz::Quiz(QObject *parent)
    : QObject(parent)
#ifdef ENABLE_SPEECH_SYNTH
    , m_synthThread(new SynthThread(this))
    , m_synthState(m_synthThread->initialized() && initializeAudio() ? SynthState::Idle : SynthState::Error)
#endif
{
#ifdef ENABLE_SPEECH_SYNTH
    connect(m_synthThread, &SynthThread::synthesizedAudio, this, [this](const QByteArray &audioData) {
        Q_ASSERT(m_audioOutput);
        m_audioBuffer.setData(audioData);
        m_audioBuffer.open(QIODevice::ReadOnly);
        m_audioOutput->start(&m_audioBuffer);
        m_synthState = SynthState::Playing;
        emit synthStateChanged();
    });
#endif
}

Quiz::~Quiz()
{
    writeDeck();
}

void Quiz::setCardFilters(CardFilters cardFilters)
{
    m_cardFilters = cardFilters;
}

void Quiz::setKatakanaInput(bool katakanaInput)
{
    if (katakanaInput == m_katakanaInput) {
        return;
    }
    m_katakanaInput = katakanaInput;
    emit katakanaInputChanged(katakanaInput);
}

bool Quiz::katakanaInput() const
{
    return m_katakanaInput;
}

QVariantMap Quiz::card() const
{
    QVariantMap data;

    data.insert("kanji", m_curCard->kanji);
    data.insert("readings", m_curCard->readings);
    data.insert("eigo", m_curCard->eigo);
    data.insert("deck", [this]() -> QChar {
        switch (m_curCard->deck) {
        case Deck::Review:
            return 'R';
        case Deck::Mastered:
            return 'M';
        default:
            return 'N';
        }
    }());

    return data;
}

QVariantMap Quiz::example() const
{
    QVariantMap data;
    data.insert("isValid", m_curExample != nullptr);
    data.insert("en", m_curExample ? m_curExample->eigo : "");
    data.insert("jp", m_curExample ? m_curExample->nihongo : "");
    return data;
}

bool Quiz::readCards(const QString &path)
{
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << path;
        return false;
    }

    QJsonParseError error;
    const auto cardsArray = QJsonDocument::fromJson(in.readAll(), &error).array();
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse" << path;
        return false;
    }

    m_cards.reserve(cardsArray.size());

    for (const auto &cardValue : cardsArray) {
        const auto card = cardValue.toObject();
        const auto eigo = card.value(QStringLiteral("eigo")).toString();
        const auto readingsArray = card.value(QStringLiteral("readings")).toArray();
        QStringList readings;
        readings.reserve(readingsArray.size());
        std::transform(readingsArray.begin(), readingsArray.end(), std::back_inserter(readings), [](const QJsonValue &value) {
            return value.toString();
        });
        const auto kanji = card.value(QStringLiteral("kanji")).toString();
        QVector<Example> examples;
        const auto examplesValue = card.value(QStringLiteral("examples"));
        if (!examplesValue.isUndefined()) {
            const auto examplesArray = examplesValue.toArray();
            examples.reserve(examplesArray.size());
            std::transform(examplesArray.begin(), examplesArray.end(), std::back_inserter(examples), [](const QJsonValue &exampleValue) {
                const auto example = exampleValue.toObject();
                const auto nihongo = example.value(QStringLiteral("jp")).toString();
                const auto eigo = example.value(QStringLiteral("en")).toString();
                return Example { eigo, nihongo };
            });
        }
        m_cards.append({ eigo, readings, kanji, examples, Deck::Normal });
    }

    m_deckPath = QStringLiteral("%1.deck").arg(QFileInfo(path).baseName());

    readDeck();

    if (m_cards.count() < 2) {
        qWarning() << "Need at least two cards";
        return false;
    }

    nextCard();

    return true;
}

void Quiz::readDeck()
{
    QFile file(m_deckPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList parts = line.split(QChar(':'));
        if (parts.size() == 2) {
            const auto &kanji = parts[0];
            const auto &deck = parts[1];

            auto it = std::find_if(std::begin(m_cards), std::end(m_cards),
                                   [&](const Card &c) { return c.kanji == kanji; });

            if (it != std::end(m_cards)) {
                if (deck == QLatin1String("R"))
                    it->deck = Deck::Review;
                else if (deck == QLatin1String("M"))
                    it->deck = Deck::Mastered;
            }
        }
    }
}

void Quiz::writeDeck() const
{
    QFile file(m_deckPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);

    for (const auto &card : qAsConst(m_cards)) {
        if (card.deck != Deck::Normal) {
            stream << card.kanji << QChar(':')
                   << (card.deck == Deck::Review ? QChar('R') : QChar('M')) << QChar('\n');
        }
    }
}

bool Quiz::canShowCard(const Card &c) const
{
    if (m_cardFilters.testFlag(CardFilter::ExamplesOnly)) {
        if (c.examples.empty())
            return false;
    }

    if (m_cardFilters.testFlag(CardFilter::ReviewOnly)) {
        return c.deck == Deck::Review;
    }

    if (!m_cardFilters.testFlag(CardFilter::ShowMastered)) {
        return c.deck != Deck::Mastered;
    }

    return true;
}

void Quiz::nextCard()
{
#ifdef ENABLE_SPEECH_SYNTH
    stopSynth();
#endif

    auto *randomGenerator = QRandomGenerator::global();

    Card *nextCard = nullptr;

    int index = 1;

    for (auto &card : m_cards) {
        if (m_curCard != &card && canShowCard(card)) {
            if (randomGenerator->bounded(0, index) == 0) {
                nextCard = &card;
            }
            ++index;
        }
    }

    Q_ASSERT(nextCard != nullptr);

    m_curCard = nextCard;

    m_curExample = [this, randomGenerator]() -> const Example * {
        const auto &examples = m_curCard->examples;
        if (examples.empty()) {
            return nullptr;
        }
        const auto index = randomGenerator->bounded(0, examples.size());
        return &examples[index];
    }();

    ++m_viewedCards;
    emit cardChanged();
    emit exampleChanged();
    emit statusLineChanged();
}

void Quiz::toggleCardReview()
{
    if (m_curCard->deck == Deck::Review)
        m_curCard->deck = Deck::Normal;
    else
        m_curCard->deck = Deck::Review;
    emit cardChanged();
    emit statusLineChanged();
}

void Quiz::setCardReview()
{
    m_curCard->deck = Deck::Review;
    emit cardChanged();
    emit statusLineChanged();
}

void Quiz::toggleCardMastered()
{
    if (m_curCard->deck == Deck::Mastered)
        m_curCard->deck = Deck::Normal;
    else
        m_curCard->deck = Deck::Mastered;
    emit cardChanged();
    emit statusLineChanged();
}

void Quiz::toggleReviewOnly()
{
    m_cardFilters ^= CardFilter::ReviewOnly;
    emit statusLineChanged();
}

QString Quiz::statusLine() const
{
    return QStringLiteral("%1 [/%2 ?=%3])")
            .arg(m_viewedCards)
            .arg(countVisibleCards())
            .arg(countReviewCards());
}

int Quiz::countVisibleCards() const
{
    return std::count_if(std::begin(m_cards), std::end(m_cards),
                         std::bind(&Quiz::canShowCard, this, std::placeholders::_1));
}

int Quiz::countReviewCards() const
{
    return std::count_if(std::begin(m_cards), std::end(m_cards),
                         [this](const Card &c) { return c.deck == Deck::Review && canShowCard(c); });
}

#ifdef ENABLE_SPEECH_SYNTH
void Quiz::sayExample()
{
    if (m_curExample && m_synthState == SynthState::Idle) {
        m_synthThread->synthesize(m_curExample->nihongo);
        m_synthState = SynthState::Loading;
        emit synthStateChanged();
    }
}

void Quiz::stopSynth()
{
    if (m_synthState == SynthState::Playing) {
        Q_ASSERT(m_audioOutput);
        m_audioOutput->stop();
    }
}

Quiz::SynthState Quiz::synthState() const
{
    return m_synthState;
}

bool Quiz::initializeAudio()
{
    QAudioFormat format;
    format.setSampleRate(m_synthThread->sampleRate());
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        return false;
    }

    m_audioOutput = new QAudioOutput(format, this);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State newState) {
        switch (newState) {
        case QAudio::IdleState:
            m_audioOutput->stop();
            break;
        case QAudio::StoppedState:
            if (m_audioOutput->error() != QAudio::NoError) {
                qWarning() << "Error playing audio:" << m_audioOutput->error();
            }
            m_audioBuffer.close();
            m_synthState = SynthState::Idle;
            emit synthStateChanged();
            break;
        default:
            break;
        }
    });

    return true;
}
#endif
