#pragma once

#include <QVariantMap>

class Quiz : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap card READ card NOTIFY cardChanged)
    Q_PROPERTY(QString statusLine READ statusLine NOTIFY statusLineChanged)

public:
    Quiz(bool showMastered, bool reviewOnly, QObject *parent = nullptr);
    ~Quiz();

    bool readCards(const QString& path);

    Q_INVOKABLE void nextCard();
    Q_INVOKABLE void toggleCardReview();
    Q_INVOKABLE void setCardReview();
    Q_INVOKABLE void toggleCardMastered();
    Q_INVOKABLE void toggleReviewOnly();

    QVariantMap card() const;
    QString statusLine() const;

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
        QString reading;
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
    int m_viewedCards;
    QList<Card> m_cards;
    Card *m_curCard;
};

