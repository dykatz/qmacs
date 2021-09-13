#include <QColor>
#include <QPainter>
#include <QTextBlock>

#include "buffer.hh"
#include "frame.hh"

FrameLineNumberArea::FrameLineNumberArea(Frame* frame)
    : QWidget(frame)
    , m_frame(frame)
{}

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

    update_line_number_area_width(0);
    highlight_current_line();
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

    return 5 + digits * fontMetrics().horizontalAdvance(QLatin1Char('9'));
}

void Frame::line_number_area_paint_event(QPaintEvent* event)
{
    QPainter painter(m_line_number_area);
    painter.fillRect(event->rect(), Qt::lightGray);

    auto block = firstVisibleBlock();
    auto block_number = block.blockNumber();
    auto top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    auto bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            auto number = QString::number(block_number + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, m_line_number_area->width() - 2, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
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

    // TODO: detect dark mode
    // auto line_color = QColor(Qt::yellow).lighter(160);
    auto line_color = QColor(Qt::blue).darker(160);

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(line_color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> selections;
    selections.append(selection);

    setExtraSelections(selections);
}
