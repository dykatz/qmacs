#pragma once

#include <QPlainTextEdit>

class Frame : public QPlainTextEdit
{
    Q_OBJECT

public:
    Frame();
    virtual ~Frame() = default;

signals:
    void gained_focus();

protected:
    void focusInEvent(QFocusEvent*) override;
};
