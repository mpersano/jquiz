#include "kanatextedit.h"

#include "kana.h"

KanaTextEdit::KanaTextEdit(QQuickItem *parent)
    : QQuickTextEdit(parent)
{
    connect(this, &QQuickTextEdit::textChanged, this, [this] { convertToKana(false); });
}

void KanaTextEdit::convertToKana(bool fullText)
{
    QString kanaText = romajiToKana(text(), fullText, m_katakanaInput);

    if (kanaText != text()) {
        const QSignalBlocker blocker(this);
        setText(kanaText);
        setCursorPosition(kanaText.size());
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

bool KanaTextEdit::katakanaInput() const
{
    return m_katakanaInput;
}

void KanaTextEdit::setKatakanaInput(bool value)
{
    if (m_katakanaInput == value)
        return;
    m_katakanaInput = value;
    emit katakanaInputChanged(value);
}
