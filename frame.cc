#include "frame.hh"

Frame::Frame()
{
}

void Frame::focusInEvent(QFocusEvent* event)
{
    QTextEdit::focusInEvent(event);
    emit gained_focus();
}
