#pragma once

#include <QTextDocument>
#include <QWidget>

class Buffer : public QTextDocument
{
    Q_OBJECT

public:
    Buffer(QWidget*);
    virtual ~Buffer() = default;
};
