#include <QCommandLineParser>
#include <QDateTime>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQuickView>

#include "kanatextedit.h"
#include "quiz.h"

static QString DefaultQuestionsPath()
{
    return QStringLiteral("questions");
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Quiz>("JQuiz", 1, 0, "Quiz");
    qmlRegisterType<KanaTextEdit>("JQuiz", 1, 0, "KanaTextEdit");

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption showMastered("m", "Show mastered cards.");
    parser.addOption(showMastered);

    QCommandLineOption questionsPath("p", "Questions path.", "path", DefaultQuestionsPath());
    parser.addOption(questionsPath);

    QCommandLineOption reviewOnly("r", "Only show cards marked for review.");
    parser.addOption(reviewOnly);

    QCommandLineOption examplesOnly("e", "Only show cards with examples.");
    parser.addOption(examplesOnly);

    QCommandLineOption kanjiQuiz("k", "Kanji quiz.");
    parser.addOption(kanjiQuiz);

    QCommandLineOption katakanaInput("o", "Katakana input.");
    parser.addOption(katakanaInput);

    parser.process(app);

    Quiz::CardFilters cardFilters = Quiz::CardFilter::None;
    if (parser.isSet(showMastered))
        cardFilters.setFlag(Quiz::CardFilter::ShowMastered);
    if (parser.isSet(reviewOnly))
        cardFilters.setFlag(Quiz::CardFilter::ReviewOnly);
    if (parser.isSet(examplesOnly))
        cardFilters.setFlag(Quiz::CardFilter::ExamplesOnly);

    Quiz quiz;
    quiz.setCardFilters(cardFilters);
    quiz.setKatakanaInput(parser.isSet(katakanaInput));
    if (!quiz.readCards(parser.value(questionsPath)))
        return -1;

    const QString cardSource = parser.isSet(kanjiQuiz) ? QStringLiteral("kanji.qml") : QStringLiteral("reading.qml");

    QQuickView view;
    view.engine()->rootContext()->setContextProperty(QStringLiteral("quiz"), &quiz);
    view.engine()->rootContext()->setContextProperty(QStringLiteral("cardSource"), cardSource);
    view.connect(view.engine(), &QQmlEngine::quit, &app, &QCoreApplication::quit);
    view.setSource(QUrl("qrc:/jquiz.qml"));
    if (view.status() == QQuickView::Error)
        return -1;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}
