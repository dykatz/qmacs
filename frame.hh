#pragma once

#include <QTextEdit>

class Frame : public QTextEdit
{
    Q_OBJECT

public:
    Frame();
    virtual ~Frame() = default;
};
