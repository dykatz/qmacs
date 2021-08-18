#pragma once

#include <QTextDocument>
#include <QWidget>

class Buffer : public QTextDocument
{
    Q_OBJECT

public:
    Buffer(QString const&, QWidget*);
    virtual ~Buffer() = default;
};
