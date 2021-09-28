#pragma once

#include <QPlainTextEdit>

class Buffer;
class Frame;

class FrameLineNumberArea final : public QWidget
{
public:
    FrameLineNumberArea(Frame* frame);
    ~FrameLineNumberArea() = default;

    QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    Frame* m_frame;
};

class Frame : public QPlainTextEdit
{
    Q_OBJECT

public:
    Frame();
    virtual ~Frame() = default;

    Buffer* buffer() const { return qobject_cast<Buffer*>(document()); }
    void set_buffer(Buffer* buffer);
    int line_number_area_width() const;
    void line_number_area_paint_event(QPaintEvent* event);

public slots:
    void on_zoom(int);

signals:
    void gained_focus();

protected:
    virtual void focusInEvent(QFocusEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

private slots:
    void update_line_number_area_width(int);
    void update_line_number_area(QRect const& rect, int dy);
    void highlight_current_line();

private:
    FrameLineNumberArea* m_line_number_area;
};
