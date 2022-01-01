#pragma once

#include <QBuffer>
#include <QVariantMap>

class QAudioOutput;
class SynthThread;

class Quiz : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap card READ card NOTIFY cardChanged)
    Q_PROPERTY(QVariantMap example READ example NOTIFY exampleChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY statusLineChanged)
    Q_PROPERTY(bool katakanaInput READ katakanaInput WRITE setKatakanaInput NOTIFY katakanaInputChanged)
#ifdef ENABLE_SPEECH_SYNTH
    Q_PROPERTY(SynthState synthState READ synthState NOTIFY synthStateChanged)
#endif

public:
    Quiz(QObject *parent = nullptr);
    ~Quiz();

    enum class SynthState {
        Idle,
        Loading,
        Playing,
        Error
    };
    Q_ENUM(SynthState)

    enum class CardFilter {
        None = 0,
        ShowMastered = 1 << 0,
        ReviewOnly = 1 << 1,
        ExamplesOnly = 1 << 2,
    };
    Q_ENUM(CardFilter)
    Q_DECLARE_FLAGS(CardFilters, CardFilter)

    void setCardFilters(CardFilters cardFilters);
    void setKatakanaInput(bool katakanaInput);

    bool readCards(const QString &path);

    Q_INVOKABLE void nextCard();
    Q_INVOKABLE void toggleCardReview();
    Q_INVOKABLE void setCardReview();
    Q_INVOKABLE void toggleCardMastered();
    Q_INVOKABLE void toggleReviewOnly();
#ifdef ENABLE_SPEECH_SYNTH
    Q_INVOKABLE void sayExample();
    Q_INVOKABLE void stopSynth();
#endif

    QVariantMap card() const;
    QVariantMap example() const;
    QString statusLine() const;
    bool katakanaInput() const;
    SynthState synthState() const;

signals:
    void cardChanged();
    void exampleChanged();
    void statusLineChanged();
    void katakanaInputChanged(bool katakanaInput);
#ifdef ENABLE_SPEECH_SYNTH
    void synthStateChanged();
#endif

private:
    enum class Deck {
        Normal,
        Review,
        Mastered
    };

    struct Example {
        QString eigo;
        QString nihongo;
    };

    struct Card {
        QString eigo;
        QStringList readings;
        QString kanji;
        QVector<Example> examples;
        Deck deck;
    };

    void readDeck();
    void writeDeck() const;
    bool canShowCard(const Card &c) const;
    int countVisibleCards() const;
    int countReviewCards() const;
#ifdef ENABLE_SPEECH_SYNTH
    bool initializeAudio();
#endif

    CardFilters m_cardFilters = CardFilter::None;
    bool m_katakanaInput = false;
    int m_viewedCards = 0;
    QVector<Card> m_cards;
    Card *m_curCard = nullptr;
    const Example *m_curExample = nullptr;
    QString m_deckPath;
#ifdef ENABLE_SPEECH_SYNTH
    SynthThread *m_synthThread;
    QBuffer m_audioBuffer;
    QAudioOutput *m_audioOutput = nullptr;
    SynthState m_synthState;
#endif
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Quiz::CardFilters)
