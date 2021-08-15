#pragma once

#include <QTextEdit>

class Frame : public QTextEdit
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
