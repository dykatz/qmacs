#pragma once

#include <QAction>
#include <QFileSystemWatcher>
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
    bool save_buffer();
    bool save_buffer_as();
    bool save_all_buffers();
    void switch_buffer();
    void close_buffer();
    void externally_modified_buffer(QString const&);
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();

protected:
    virtual void closeEvent(QCloseEvent*) override;

private:
    QString create_untitled_name() const;
    void update_window_title();
    void enable_frame_actions(bool enabled = true);
    void connect_buffer(Buffer*);
    Frame* create_frame();
    void replace_buffers_in_frames(Buffer*, Buffer*);
    QString unwatched_buffer_write(Buffer*);
    void set_active_frame(Frame*);
    void set_active_buffer(Buffer*);

    Buffer* m_active_buffer;
    Frame* m_active_frame;
    BufferModel* m_buffer_model;
    QList<Frame*> m_frames;
    QFileSystemWatcher* m_watcher;

    QAction* m_remove_other_frames_action;
    QAction* m_remove_this_frame_action;
    QAction* m_move_to_next_frame_action;
    QAction* m_move_to_previous_frame_action;
};
