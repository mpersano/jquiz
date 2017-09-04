#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlContext>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDateTime>

#include "quiz.h"

namespace {
const QString DefaultQuestionsPath("questions");
}

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
