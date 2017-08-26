#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlContext>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDateTime>

static const QString DeckPath("deck");
static const QString DefaultQuestionsPath("questions");

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

Quiz::Quiz(bool showMastered, bool reviewOnly, QObject *parent)
    : QObject(parent)
    , m_showMastered(showMastered)
    , m_reviewOnly(reviewOnly)
    , m_viewedCards(0)
    , m_curCard(nullptr)
{
}

Quiz::~Quiz()
{
    writeDeck();
}

QVariantMap Quiz::card() const
{
    QVariantMap data;

    data.insert("kanji", m_curCard->kanji);
    data.insert("reading", m_curCard->reading);
    data.insert("eigo", m_curCard->eigo);
    data.insert("deck", [&]() -> QChar {
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

bool Quiz::readCards(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qDebug() << "Failed to open" << path;
        return false;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList parts = line.split(QChar('\t'));
        if (parts.size() == 3) {
            m_cards.append({ parts[0], parts[1], parts[2], Deck::Normal });
        }
    }

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
    QFile file(DeckPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList parts = line.split(QChar(':'));
        if (parts.size() == 2) {
            const auto& kanji = parts[0];
            const auto& deck = parts[1];

            auto it = std::find_if(std::begin(m_cards), std::end(m_cards),
                                   [&](const Card& c) { return c.kanji == kanji; });

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
    QFile file(DeckPath);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return;

    QTextStream stream(&file);

    for (const auto& card : qAsConst(m_cards)) {
        if (card.deck != Deck::Normal) {
            stream << card.kanji << QChar(':')
                   << (card.deck == Deck::Review ? QChar('R') : QChar('M')) << QChar('\n');
        }
    }
}

bool Quiz::canShowCard(const Card& c) const
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

    for (auto& card : m_cards) {
        if (m_curCard != &card && canShowCard(card)) {
            if ((qrand() % index) == 0) {
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
                         [](const Card& c) { return c.deck == Deck::Review; });
}

#include "main.moc"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption showMastered("m", "Show mastered cards.");
    parser.addOption(showMastered);

    QCommandLineOption questionsPath("p", "Questions path.", "path", DefaultQuestionsPath);
    parser.addOption(questionsPath);

    parser.process(app);

    qsrand(QDateTime::currentDateTime().toTime_t());

    Quiz quiz(parser.isSet(showMastered), false);
    if (!quiz.readCards(parser.value(questionsPath)))
        return -1;

    QQuickView view;
    view.engine()->rootContext()->setContextProperty(QStringLiteral("quiz"), &quiz);
    view.connect(view.engine(), &QQmlEngine::quit, &app, &QCoreApplication::quit);
    view.setSource(QUrl("qrc:/jquiz.qml"));
    if (view.status() == QQuickView::Error)
        return -1;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}
