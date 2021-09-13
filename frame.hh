#pragma once

#include <QPlainTextEdit>

class Frame;

class FrameLineNumberArea final : public QWidget
{
public:
    FrameLineNumberArea(Frame* frame);
    ~FrameLineNumberArea() = default;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Frame* m_frame;
};

class Frame : public QPlainTextEdit
{
    Q_OBJECT

public:
    Frame();
    virtual ~Frame() = default;

    int line_number_area_width() const;
    void line_number_area_paint_event(QPaintEvent* event);

signals:
    void gained_focus();

protected:
    void focusInEvent(QFocusEvent*) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void update_line_number_area_width(int);
    void update_line_number_area(QRect const& rect, int dy);
    void highlight_current_line();

private:
    FrameLineNumberArea* m_line_number_area;
};
