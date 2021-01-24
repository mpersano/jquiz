#pragma once

#include <QVariantMap>

class Quiz : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap card READ card NOTIFY cardChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY statusLineChanged)
    Q_PROPERTY(bool katakanaInput READ katakanaInput CONSTANT)

public:
    Quiz(bool showMastered, bool reviewOnly, bool katakanaInput, QObject *parent = nullptr);
    ~Quiz();

    bool readCards(const QString& path);

    Q_INVOKABLE void nextCard();
    Q_INVOKABLE void toggleCardReview();
    Q_INVOKABLE void setCardReview();
    Q_INVOKABLE void toggleCardMastered();
    Q_INVOKABLE void toggleReviewOnly();

    QVariantMap card() const;
    QString statusLine() const;
    bool katakanaInput() const;

signals:
    void cardChanged();
    void statusLineChanged();

private:
    enum class Deck
    {
        Normal,
        Review,
        Mastered
    };

    struct Card
    {
        QString eigo;
        QStringList readings;
        QString kanji;
        Deck deck;
    };

    void readDeck();
    void writeDeck() const;
    bool canShowCard(const Card& c) const;
    int countVisibleCards() const;
    int countReviewCards() const;

    bool m_showMastered;
    bool m_reviewOnly;
    bool m_katakanaInput;
    int m_viewedCards;
    QList<Card> m_cards;
    Card *m_curCard;
};

