#pragma once

#include <QList>
#include <QMainWindow>

#include "buffer.hh"
#include "buffer-model.hh"
#include "frame.hh"

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();
    virtual ~Window() = default;

private slots:
    void horizontal_split_frame();
    void vertical_split_frame();
    void remove_this_frame();
    void remove_other_frames();
    void move_to_next_frame();
    void move_to_previous_frame();
    void new_buffer();
    void open_buffer();
    void save_buffer();
    void save_buffer_as();

private:
    QString create_untitled_name() const;
    void update_window_title();
    Buffer* create_buffer();
    Frame* create_frame();
    void set_active_frame(Frame*);
    void set_active_buffer(Buffer*);

    Buffer* m_active_buffer;
    Frame* m_active_frame;
    BufferModel* m_buffer_model;
};
