#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlContext>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDateTime>

#include "quiz.h"
#include "kanatextedit.h"

namespace {
const QString DefaultQuestionsPath("questions");
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<KanaTextEdit>("JQuiz", 1, 0, "KanaTextEdit");

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption showMastered("m", "Show mastered cards.");
    parser.addOption(showMastered);

    QCommandLineOption questionsPath("p", "Questions path.", "path", DefaultQuestionsPath);
    parser.addOption(questionsPath);

    QCommandLineOption reviewOnly("r", "Only show cards marked for review.");
    parser.addOption(reviewOnly);

    QCommandLineOption kanjiQuiz("k", "Kanji quiz.");
    parser.addOption(kanjiQuiz);

    parser.process(app);

    qsrand(QDateTime::currentDateTime().toTime_t());

    Quiz quiz(parser.isSet(showMastered), parser.isSet(reviewOnly));
    if (!quiz.readCards(parser.value(questionsPath)))
        return -1;

    QString cardSource;
    if (parser.isSet(kanjiQuiz))
        cardSource = QStringLiteral("kanji.qml");
    else
        cardSource = QStringLiteral("reading.qml");

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
