#pragma once

#include <QBuffer>
#include <QVariantMap>

class SynthThread;

class Quiz : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap card READ card NOTIFY cardChanged)
    Q_PROPERTY(QVariantMap example READ example NOTIFY exampleChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY statusLineChanged)
    Q_PROPERTY(bool katakanaInput READ katakanaInput WRITE setKatakanaInput NOTIFY katakanaInputChanged)
    Q_PROPERTY(SynthState synthState READ synthState NOTIFY synthStateChanged)

public:
    Quiz(QObject *parent = nullptr);
    ~Quiz();

    enum class SynthState {
        Idle,
        Loading,
        Playing
    };
    Q_ENUM(SynthState)

    void setShowMastered(bool showMastered);
    void setReviewOnly(bool reviewOnly);
    void setKatakanaInput(bool katakanaInput);

    bool readCards(const QString &path);

    Q_INVOKABLE void nextCard();
    Q_INVOKABLE void toggleCardReview();
    Q_INVOKABLE void setCardReview();
    Q_INVOKABLE void toggleCardMastered();
    Q_INVOKABLE void toggleReviewOnly();
    Q_INVOKABLE void sayExample();

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
    void synthStateChanged();

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

    bool m_showMastered = false;
    bool m_reviewOnly = false;
    bool m_katakanaInput = false;
    int m_viewedCards = 0;
    QVector<Card> m_cards;
    Card *m_curCard = nullptr;
    const Example *m_curExample = nullptr;
    QString m_deckPath;
    SynthThread *m_synthThread;
    QBuffer m_audioBuffer;
    SynthState m_synthState = SynthState::Idle;
};
