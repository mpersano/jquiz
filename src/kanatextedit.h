#pragma once

#include <QtQuick/private/qquicktextedit_p.h>

class KanaTextEdit : public QQuickTextEdit
{
    Q_OBJECT
    Q_PROPERTY(bool katakanaInput READ katakanaInput WRITE setKatakanaInput NOTIFY katakanaInputChanged)

public:
    explicit KanaTextEdit(QQuickItem *parent = nullptr);

    Q_INVOKABLE void convertToKana(bool fullText = false);

    bool katakanaInput() const;
    void setKatakanaInput(bool value);

signals:
    void katakanaInputChanged(bool value);

private:
    void keyPressEvent(QKeyEvent *event) override;

    bool m_katakanaInput = false;
};
