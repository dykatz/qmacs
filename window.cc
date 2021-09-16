#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>

#include "picker.hh"
#include "window.hh"

Window::Window()
    : QMainWindow(nullptr)
{
    auto file_menu = menuBar()->addMenu("&File");

    auto new_buffer_action = new QAction("&New Buffer", this);
    new_buffer_action->setShortcut(QKeySequence::New);
    connect(new_buffer_action, &QAction::triggered, this, &Window::new_buffer);
    file_menu->addAction(new_buffer_action);

    auto open_buffer_action = new QAction("&Open File", this);
    open_buffer_action->setShortcut(QKeySequence::Open);
    connect(open_buffer_action, &QAction::triggered, this, &Window::open_buffer);
    file_menu->addAction(open_buffer_action);

    auto save_buffer_action = new QAction("&Save File", this);
    save_buffer_action->setShortcut(QKeySequence::Save);
    connect(save_buffer_action, &QAction::triggered, this, &Window::save_buffer);
    file_menu->addAction(save_buffer_action);

    auto save_buffer_as_action = new QAction("S&ave File As", this);
    save_buffer_as_action->setShortcut(QKeySequence::SaveAs);
    connect(save_buffer_as_action, &QAction::triggered, this, &Window::save_buffer_as);
    file_menu->addAction(save_buffer_as_action);

    auto save_all_buffers_action = new QAction("Save All Files", this);
    save_all_buffers_action->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_S);
    connect(save_all_buffers_action, &QAction::triggered, this, &Window::save_all_buffers);
    file_menu->addAction(save_all_buffers_action);

    auto switch_buffer_action = new QAction("Switch &Buffer", this);
    switch_buffer_action->setShortcut(Qt::CTRL | Qt::Key_B);
    connect(switch_buffer_action, &QAction::triggered, this, &Window::switch_buffer);
    file_menu->addAction(switch_buffer_action);

    auto close_buffer_action = new QAction("&Close Buffer", this);
    close_buffer_action->setShortcut(Qt::CTRL | Qt::Key_K);
    connect(close_buffer_action, &QAction::triggered, this, &Window::close_buffer);
    file_menu->addAction(close_buffer_action);

    auto frame_menu = menuBar()->addMenu("F&rame");

    auto horizontal_split_frame_action = new QAction("Split &Horizontally", this);
    horizontal_split_frame_action->setShortcut(Qt::CTRL | Qt::Key_3);
    connect(horizontal_split_frame_action, &QAction::triggered, this, &Window::horizontal_split_frame);
    frame_menu->addAction(horizontal_split_frame_action);

    auto vertical_split_frame_action = new QAction("Split &Vertically", this);
    vertical_split_frame_action->setShortcut(Qt::CTRL | Qt::Key_2);
    connect(vertical_split_frame_action, &QAction::triggered, this, &Window::vertical_split_frame);
    frame_menu->addAction(vertical_split_frame_action);

    m_remove_other_frames_action = new QAction("Remove &Other", this);
    m_remove_other_frames_action->setShortcut(Qt::CTRL | Qt::Key_1);
    m_remove_other_frames_action->setEnabled(false);
    connect(m_remove_other_frames_action, &QAction::triggered, this, &Window::remove_other_frames);
    frame_menu->addAction(m_remove_other_frames_action);

    m_remove_this_frame_action = new QAction("Remove &Current", this);
    m_remove_this_frame_action->setShortcut(Qt::CTRL | Qt::Key_0);
    m_remove_this_frame_action->setEnabled(false);
    connect(m_remove_this_frame_action, &QAction::triggered, this, &Window::remove_this_frame);
    frame_menu->addAction(m_remove_this_frame_action);

    m_move_to_next_frame_action = new QAction("Move to &Next Frame", this);
    m_move_to_next_frame_action->setShortcut(Qt::CTRL | Qt::Key_Tab);
    m_move_to_next_frame_action->setEnabled(false);
    connect(m_move_to_next_frame_action, &QAction::triggered, this, &Window::move_to_next_frame);
    frame_menu->addAction(m_move_to_next_frame_action);

    m_move_to_previous_frame_action = new QAction("Move to &Previous Frame", this);
    m_move_to_previous_frame_action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab);
    m_move_to_previous_frame_action->setEnabled(false);
    connect(m_move_to_previous_frame_action, &QAction::triggered, this, &Window::move_to_previous_frame);
    frame_menu->addAction(m_move_to_previous_frame_action);

    m_watcher = new QFileSystemWatcher;
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &Window::externally_modified_buffer);

    m_buffer_model = new BufferModel(this);
    m_active_buffer = new Buffer(create_untitled_name(), this);
    connect_buffer(m_active_buffer);

    auto first_frame = create_frame();
    m_active_frame = first_frame;
    setCentralWidget(first_frame);

    update_window_title();
}

void Window::horizontal_split_frame()
{
    auto parent = m_active_frame->parent();
    auto splitter = qobject_cast<QSplitter*>(parent);
    if (!splitter) {
        splitter = new QSplitter(this);
        splitter->addWidget(m_active_frame);
        splitter->setSizes(QList<int>({INT_MAX}));
        setCentralWidget(splitter);
    } else if (splitter->orientation() != Qt::Horizontal) {
        auto index_of_current_frame = splitter->indexOf(m_active_frame);
        auto new_splitter = new QSplitter;
        splitter->replaceWidget(index_of_current_frame, new_splitter);
        new_splitter->addWidget(m_active_frame);
        new_splitter->setSizes(QList<int>({INT_MAX}));
        splitter = new_splitter;
    }

    splitter->insertWidget(splitter->indexOf(m_active_frame) + 1, create_frame());
    m_active_frame->setFocus();
    enable_frame_actions();
}

void Window::vertical_split_frame()
{
    auto parent = m_active_frame->parent();
    auto splitter = qobject_cast<QSplitter*>(parent);
    if (!splitter) {
        splitter = new QSplitter(this);
        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(m_active_frame);
        splitter->setSizes(QList<int>({INT_MAX}));
        setCentralWidget(splitter);
    } else if (splitter->orientation() != Qt::Vertical) {
        auto index_of_current_frame = splitter->indexOf(m_active_frame);
        auto new_splitter = new QSplitter;
        new_splitter->setOrientation(Qt::Vertical);
        splitter->replaceWidget(index_of_current_frame, new_splitter);
        new_splitter->addWidget(m_active_frame);
        new_splitter->setSizes(QList<int>({INT_MAX}));
        splitter = new_splitter;
    }

    splitter->insertWidget(splitter->indexOf(m_active_frame) + 1, create_frame());
    m_active_frame->setFocus();
    enable_frame_actions();
}

void Window::remove_other_frames()
{
    auto central_widget = centralWidget();
    if (central_widget == m_active_frame)
        return;
    m_frames.clear();
    setCentralWidget(m_active_frame);
    delete central_widget;
    m_active_frame->setFocus();
    m_frames.append(m_active_frame);
    enable_frame_actions(false);
}

void Window::remove_this_frame()
{
    auto splitter = qobject_cast<QSplitter*>(m_active_frame->parent());
    if (!splitter)
        return;

    auto index_of_active_frame = splitter->indexOf(m_active_frame);
    m_frames.removeAll(m_active_frame);
    delete m_active_frame;

    if (splitter->count() == 1) {
        auto splitter_child = splitter->findChild<QWidget*>();
        auto splitter_parent = qobject_cast<QSplitter*>(splitter->parent());
        if (!splitter_parent) {
            setCentralWidget(splitter_child);
            splitter_parent = qobject_cast<QSplitter*>(splitter_child);
            index_of_active_frame = 0;
        } else {
            auto splitter_index = splitter_parent->indexOf(splitter);
            splitter_parent->replaceWidget(splitter_index, splitter_child);
            index_of_active_frame = splitter_index;
        }
        delete splitter;
        splitter = splitter_parent;
    }

    if (splitter) {
        while (true) {
            auto widget = splitter->widget(index_of_active_frame);
            if (!widget) {
                --index_of_active_frame;
                continue;
            }

            auto widget_as_frame = qobject_cast<Frame*>(widget);
            if (!widget_as_frame) {
                index_of_active_frame = 0;
                splitter = qobject_cast<QSplitter*>(widget);
                continue;
            }

            m_active_frame = widget_as_frame;
            break;
        }
    } else {
        m_active_frame = qobject_cast<Frame*>(centralWidget());
        enable_frame_actions(false);
    }

    m_active_frame->setFocus();
}

void Window::move_to_next_frame()
{
    auto splitter = qobject_cast<QSplitter*>(m_active_frame->parent());
    if (!splitter)
        return;

    auto index_of_active_frame = splitter->indexOf(m_active_frame);
    auto start_from_top = false;
    while (index_of_active_frame == splitter->count() - 1) {
        auto next_splitter = qobject_cast<QSplitter*>(splitter->parent());
        if (!next_splitter) {
            start_from_top = true;
            break;
        }

        index_of_active_frame = next_splitter->indexOf(splitter);
        splitter = next_splitter;
    }

    auto next_element = start_from_top ? centralWidget() : splitter->widget(index_of_active_frame + 1);
    auto next_element_as_frame = qobject_cast<Frame*>(next_element);

    while (!next_element_as_frame) {
        auto next_element_as_splitter = qobject_cast<QSplitter*>(next_element);
        next_element = next_element_as_splitter->widget(0);
        next_element_as_frame = qobject_cast<Frame*>(next_element);
    }

    set_active_frame(next_element_as_frame);
    m_active_frame->setFocus();
}

void Window::move_to_previous_frame()
{
    auto splitter = qobject_cast<QSplitter*>(m_active_frame->parent());
    if (!splitter)
        return;

    auto index_of_active_frame = splitter->indexOf(m_active_frame);
    auto start_from_top = false;
    while (index_of_active_frame == 0) {
        auto next_splitter = qobject_cast<QSplitter*>(splitter->parent());
        if (!next_splitter) {
            start_from_top = true;
            break;
        }

        index_of_active_frame = next_splitter->indexOf(splitter);
        splitter = next_splitter;
    }

    auto next_element = start_from_top ? centralWidget() : splitter->widget(index_of_active_frame - 1);
    auto next_element_as_frame = qobject_cast<Frame*>(next_element);

    while (!next_element_as_frame) {
        auto next_element_as_splitter = qobject_cast<QSplitter*>(next_element);
        next_element = next_element_as_splitter->widget(next_element_as_splitter->count() - 1);
        next_element_as_frame = qobject_cast<Frame*>(next_element);
    }

    set_active_frame(next_element_as_frame);
    m_active_frame->setFocus();
}

void Window::new_buffer()
{
    auto buffer = new Buffer(create_untitled_name(), this);
    connect_buffer(buffer);
    set_active_buffer(buffer);
}

void Window::open_buffer()
{
    auto file_paths = QFileDialog::getOpenFileNames(this, "Select files to open...");
    if (file_paths.count() == 0)
        return;

    Buffer* new_active_buffer = nullptr;
    QStringList error_messages;
    for (auto file_path : file_paths) {
        auto file_already_open = false;
        for (int row = 0; row < m_buffer_model->rowCount(); ++row) {
            if (m_buffer_model->index(row, 1).data() == file_path) {
                file_already_open = true;
                new_active_buffer = m_buffer_model->buffer_from_row(row);
                break;
            }
        }
        if (file_already_open)
            continue;

        auto file_buffer = new Buffer("", this);
        file_buffer->set_file_path(file_path);

        auto read_error = file_buffer->read();
        if (!read_error.isEmpty()) {
            error_messages.append(read_error);
            delete file_buffer;
            continue;
        }

        connect_buffer(file_buffer);
        new_active_buffer = file_buffer;
    }
    if (new_active_buffer != nullptr)
        set_active_buffer(new_active_buffer);
    if (error_messages.count() > 0)
        QMessageBox::warning(this, "Warning", "Could not open files:\n" + error_messages.join("\n"));
}

void Window::save_buffer()
{
    auto file_path = m_active_buffer->file_path();
    if (file_path.isEmpty()) {
        save_buffer_as();
        return;
    }

    auto write_error = unwatched_buffer_write(m_active_buffer);
    if (!write_error.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Could not save file: " + write_error);
        return;
    }
}

void Window::save_buffer_as()
{
    auto new_file_path = QFileDialog::getSaveFileName(this, "Save file as...");
    if (new_file_path.isEmpty())
        return;

    auto old_file_name = m_active_buffer->objectName();
    auto old_file_path = m_active_buffer->file_path();
    m_active_buffer->set_file_path(new_file_path);

    auto write_error = m_active_buffer->write();
    if (!write_error.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Could not save file: " + write_error);
        m_active_buffer->set_file_path(old_file_path);
        m_active_buffer->setObjectName(old_file_name);
        return;
    }

    m_watcher->removePath(old_file_path);
    m_watcher->addPath(new_file_path);
    set_active_buffer(m_active_buffer);
}

void Window::save_all_buffers()
{
    QList<Buffer*> untracked_buffers;
    for (int row = 0; row < m_buffer_model->rowCount(); ++row) {
        auto buffer = m_buffer_model->buffer_from_row(row);
        auto file_path = buffer->file_path();

        if (!buffer->isModified())
            continue;

        if (file_path.isEmpty()) {
            untracked_buffers.push_front(buffer);
            continue;
        }

        auto write_error = unwatched_buffer_write(buffer);
        if (!write_error.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Could not save file: " + write_error);
            continue;
        }
    }

    if (untracked_buffers.count() > 0) {
        auto result = QMessageBox::question(this, "Warning", "You have untracked files. Save them?");
        if (result == QMessageBox::No)
            return;

        auto original_buffer = m_active_buffer;

        for (auto buffer : untracked_buffers) {
            set_active_buffer(buffer);
            save_buffer_as();
        }

        set_active_buffer(original_buffer);
    }
}

void Window::switch_buffer()
{
    auto buffer_picker = new Picker(m_buffer_model, 1, this);

    connect(buffer_picker, &QDialog::accepted, this, [=] {
        auto buffer_row = buffer_picker->result_row();
        set_active_buffer(m_buffer_model->buffer_from_row(buffer_row));
        m_buffer_model->move_buffer_to_top(buffer_row);
    });

    buffer_picker->open();
}

void Window::close_buffer()
{
    auto buffer_picker = new Picker(m_buffer_model, m_buffer_model->row_from_buffer(m_active_buffer), this);

    connect(buffer_picker, &QDialog::accepted, this, [=] {
        auto buffer_row = buffer_picker->result_row();
        auto original_buffer = m_buffer_model->buffer_from_row(buffer_row);

        if (original_buffer->isModified()) {
            auto result = QMessageBox::question(this, "Warning", "This file is unsaved. Save it?");
            if (result == QMessageBox::Yes) {
                auto active_buffer = m_active_buffer;
                set_active_buffer(original_buffer);
                save_buffer();
                set_active_buffer(active_buffer);
            }
        }

        m_buffer_model->remove_buffer(buffer_row);
        if (!original_buffer->file_path().isEmpty())
            m_watcher->removePath(original_buffer->file_path());
        auto replacement_buffer = m_buffer_model->buffer_from_row(0);
        replace_buffers_in_frames(original_buffer, replacement_buffer);
    });

    buffer_picker->open();
}

void Window::externally_modified_buffer(QString const& file_path)
{
    Buffer* buffer = nullptr;
    for (int row = 0; row < m_buffer_model->rowCount(); ++row) {
        if (m_buffer_model->index(row, 1).data() == file_path) {
            buffer = m_buffer_model->buffer_from_row(row);
            break;
        }
    }
    if (buffer == nullptr) {
        m_watcher->removePath(file_path);
        return;
    }

    if (QFile::exists(file_path)) {
        auto result = QMessageBox::question(this, "Warning", "This file was externally modified. Reload?\n" + file_path);
        if (result == QMessageBox::Yes) {
            auto read_error = buffer->read();
            if (!read_error.isEmpty()) {
                QMessageBox::warning(this, "Warning", "File could not be opened: " + read_error);
                return;
            }
        }
    }
}

void Window::closeEvent(QCloseEvent* event)
{
    bool has_unsaved_buffers = false;

    for (int row = 0; row < m_buffer_model->rowCount(); ++row) {
        if (m_buffer_model->index(row, 3).data() == true) {
            has_unsaved_buffers = true;
            break;
        }
    }

    if (!has_unsaved_buffers) {
        event->accept();
        return;
    }

    auto result = QMessageBox::question(this, "Warning", "You have unsaved files. Save them?", QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (result == QMessageBox::Cancel) {
        event->ignore();
    } else {
        if (result == QMessageBox::Yes)
            save_all_buffers();
        event->accept();
    }
}

QString Window::create_untitled_name() const
{
    int untitled_number = 1;
    while (true) {
        QString untitled_name = QStringLiteral("*untitled-%1*").arg(untitled_number);

        bool used = false;
        for (int row = 0; row < m_buffer_model->rowCount(); ++row) {
            if (m_buffer_model->index(row, 0).data() == untitled_name) {
                used = true;
                break;
            }
        }
        if (!used)
            return untitled_name;

        ++untitled_number;
    }
}

void Window::enable_frame_actions(bool enabled)
{
    m_remove_other_frames_action->setEnabled(enabled);
    m_remove_this_frame_action->setEnabled(enabled);
    m_move_to_next_frame_action->setEnabled(enabled);
    m_move_to_previous_frame_action->setEnabled(enabled);
}

void Window::update_window_title()
{
    setWindowTitle(m_active_buffer->objectName() + " - Qmacs");
}

void Window::connect_buffer(Buffer* buffer)
{
    m_buffer_model->add_buffer(buffer);

    if (!buffer->file_path().isEmpty())
        m_watcher->addPath(buffer->file_path());
}

Frame* Window::create_frame()
{
    auto new_frame = new Frame;
    new_frame->set_buffer(m_active_buffer);
    m_frames.push_back(new_frame);
    connect(new_frame, &Frame::gained_focus, this, [=]{ set_active_frame(new_frame); });
    return new_frame;
}

void Window::replace_buffers_in_frames(Buffer* original, Buffer* replacement)
{
    if (replacement == nullptr) {
        replacement = new Buffer(create_untitled_name(), this);
        connect_buffer(replacement);
    }

    for (auto frame : m_frames) {
        auto frame_buffer = frame->buffer();
        if (frame_buffer == original)
            frame->set_buffer(replacement);
    }

    set_active_frame(m_active_frame);
}

QString Window::unwatched_buffer_write(Buffer* buffer)
{
    auto remove_succeeded = m_watcher->removePath(buffer->file_path());
    auto write_error = buffer->write();

    if (remove_succeeded)
        m_watcher->addPath(buffer->file_path());

    return write_error;
}

void Window::set_active_frame(Frame* frame)
{
    m_active_frame = frame;
    m_active_buffer = frame->buffer();
    update_window_title();
}

void Window::set_active_buffer(Buffer* buffer)
{
    m_active_buffer = buffer;
    m_active_frame->set_buffer(buffer);
    update_window_title();
}
