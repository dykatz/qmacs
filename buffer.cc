#include "buffer.hh"

Buffer::Buffer(QString const& name, QWidget* parent)
    : QTextDocument(parent)
{
    setObjectName(name);
}
