#include "frame.hh"

Frame::Frame()
{
}

void Frame::focusInEvent(QFocusEvent* event)
{
    QPlainTextEdit::focusInEvent(event);
    emit gained_focus();
}
