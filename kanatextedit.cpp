#include "kanatextedit.h"

#include "kana.h"

KanaTextEdit::KanaTextEdit(QQuickItem *parent)
    : QQuickTextEdit(parent)
{
    connect(this, &QQuickTextEdit::textChanged, [this] { convertToKana(false); });
}

void KanaTextEdit::convertToKana(bool fullText)
{
    QString kanaText = romajiToKana(text(), fullText);

    if (kanaText != text()) {
        bool block = blockSignals(true);

        setText(kanaText);
        setCursorPosition(kanaText.size());

        blockSignals(block);
    }
}

void KanaTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        event->accept();
        convertToKana(true);
        return;
    }

    QQuickTextEdit::keyPressEvent(event);
}
