#pragma once

#include <QtQuick/private/qquicktextedit_p.h>

class KanaTextEdit : public QQuickTextEdit
{
    Q_OBJECT

public:
    explicit KanaTextEdit(QQuickItem *parent = nullptr);

    Q_INVOKABLE void convertToKana(bool fullText = false);

private:
    void keyPressEvent(QKeyEvent *event) override;
};
