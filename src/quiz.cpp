#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTextStream>

#include "quiz.h"

Quiz::Quiz(bool showMastered, bool reviewOnly, bool katakanaInput, QObject *parent)
    : QObject(parent)
    , m_showMastered(showMastered)
    , m_reviewOnly(reviewOnly)
    , m_katakanaInput(katakanaInput)
    , m_viewedCards(0)
    , m_curCard(nullptr)
{
}

Quiz::~Quiz()
{
    writeDeck();
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
        const auto &card = cardValue.toObject();
        const auto eigo = card.value(QStringLiteral("eigo")).toString();
        const auto readingsArray = card.value(QStringLiteral("readings")).toArray();
        QStringList readings;
        readings.reserve(readingsArray.size());
        std::transform(readingsArray.begin(), readingsArray.end(), std::back_inserter(readings), [](const QJsonValue &value) {
            return value.toString();
        });
        const auto kanji = card.value(QStringLiteral("kanji")).toString();
        m_cards.append({ eigo, readings, kanji, Deck::Normal });
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
    if (m_reviewOnly)
        return c.deck == Deck::Review;
    else if (!m_showMastered)
        return c.deck != Deck::Mastered;
    else
        return true;
}

void Quiz::nextCard()
{
    Card *nextCard = nullptr;

    int index = 1;

    for (auto &card : m_cards) {
        if (m_curCard != &card && canShowCard(card)) {
            if (QRandomGenerator::global()->bounded(0, index) == 0) {
                nextCard = &card;
            }
            ++index;
        }
    }

    Q_ASSERT(nextCard != nullptr);

    m_curCard = nextCard;

    ++m_viewedCards;
    emit cardChanged();
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
    m_reviewOnly = !m_reviewOnly;
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
                         [](const Card &c) { return c.deck == Deck::Review; });
}
