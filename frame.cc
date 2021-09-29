#include <QColor>
#include <QPainter>
#include <QTextBlock>

#include "buffer.hh"
#include "frame.hh"

FrameLineNumberArea::FrameLineNumberArea(Frame* frame)
    : QWidget(frame)
    , m_frame(frame)
{
}

QSize FrameLineNumberArea::sizeHint() const
{
    return QSize(m_frame->line_number_area_width(), 0);
}

void FrameLineNumberArea::paintEvent(QPaintEvent* event)
{
    m_frame->line_number_area_paint_event(event);
}

Frame::Frame()
{
    m_line_number_area = new FrameLineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &Frame::update_line_number_area_width);
    connect(this, &QPlainTextEdit::updateRequest, this, &Frame::update_line_number_area);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &Frame::highlight_current_line);
}

void Frame::set_buffer(Buffer* buffer)
{
    setDocument(buffer);
    update_line_number_area_width(0);
}

int Frame::line_number_area_width() const
{
    auto digits = 1;
    auto lines = qMax(1, blockCount());

    while (lines >= 10) {
        lines /= 10;
        ++digits;
    }

    return 5 + (digits + 3) * fontMetrics().horizontalAdvance(QLatin1Char('9'));
}

void Frame::line_number_area_paint_event(QPaintEvent* event)
{
    QPainter painter(m_line_number_area);
    painter.fillRect(event->rect(), QPalette().color(QPalette::ToolTipBase));

    auto block = firstVisibleBlock();
    auto block_number = block.blockNumber();
    auto top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    auto bottom = top + qRound(blockBoundingRect(block).height());
    auto width = m_line_number_area->width() - 2 - fontMetrics().horizontalAdvance(QLatin1Char('9'));
    auto height = fontMetrics().height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            auto number = QString::number(block_number + 1);
            painter.setPen(QPalette().color(QPalette::ToolTipText));
            painter.drawText(0, top, width, height, Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
}

void Frame::set_font_point_size(int font_point_size)
{
    QFont frame_font("");
    frame_font.setStyleHint(QFont::Monospace);
    frame_font.setFixedPitch(true);
    frame_font.setPointSize(font_point_size);

    setFont(frame_font);
    m_line_number_area->setFont(frame_font);

    update_line_number_area_width(0);
    highlight_current_line();
}

void Frame::focusInEvent(QFocusEvent* event)
{
    QPlainTextEdit::focusInEvent(event);
    emit gained_focus();
}

void Frame::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    auto cr = contentsRect();
    m_line_number_area->setGeometry(cr.left(), cr.top(), line_number_area_width(), cr.height());
}

void Frame::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        event->ignore();
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}

void Frame::update_line_number_area_width(int)
{
    setViewportMargins(line_number_area_width(), 0, 0, 0);
}

void Frame::update_line_number_area(QRect const& rect, int dy)
{
    if (dy)
        m_line_number_area->scroll(0, dy);
    else
        m_line_number_area->update(0, rect.y(), m_line_number_area->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        update_line_number_area_width(0);
}

void Frame::highlight_current_line()
{
    if (isReadOnly())
        return;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QPalette().color(QPalette::Button));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> selections;
    selections.append(selection);

    setExtraSelections(selections);
}
