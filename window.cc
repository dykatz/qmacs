#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>

#include "buffer-picker.hh"
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

    auto remove_other_frames_action = new QAction("Remove &Other", this);
    remove_other_frames_action->setShortcut(Qt::CTRL | Qt::Key_1);
    connect(remove_other_frames_action, &QAction::triggered, this, &Window::remove_other_frames);
    frame_menu->addAction(remove_other_frames_action);

    auto remove_this_frame_action = new QAction("Remove &Current", this);
    remove_this_frame_action->setShortcut(Qt::CTRL | Qt::Key_0);
    connect(remove_this_frame_action, &QAction::triggered, this, &Window::remove_this_frame);
    frame_menu->addAction(remove_this_frame_action);

    auto move_to_next_frame_action = new QAction("Move to &Next Frame", this);
    move_to_next_frame_action->setShortcut(Qt::CTRL | Qt::Key_Tab);
    connect(move_to_next_frame_action, &QAction::triggered, this, &Window::move_to_next_frame);
    frame_menu->addAction(move_to_next_frame_action);

    auto move_to_previous_frame_action = new QAction("Move to &Previous Frame", this);
    move_to_previous_frame_action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab);
    connect(move_to_previous_frame_action, &QAction::triggered, this, &Window::move_to_previous_frame);
    frame_menu->addAction(move_to_previous_frame_action);

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

        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            error_messages.append(file.errorString());
            continue;
        }

        auto file_buffer = new Buffer("", this);
        file_buffer->set_file_path(file_path);

        QTextStream file_stream(&file);
        auto file_contents = file_stream.readAll();
        file.close();
        file_buffer->setPlainText(file_contents);

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

    QFile this_file(file_path);
    if (!this_file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Could not save file: " + this_file.errorString());
        return;
    }

    QTextStream file_stream(&this_file);
    file_stream << m_active_buffer->toPlainText();
    this_file.close();

    m_active_buffer->setModified(false);
}

void Window::save_buffer_as()
{
    auto new_file_path = QFileDialog::getSaveFileName(this, "Save file as...");
    if (new_file_path.isEmpty())
        return;

    QFile new_file(new_file_path);
    if (!new_file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Could not save file: " + new_file.errorString());
        return;
    }

    QTextStream file_stream(&new_file);
    file_stream << m_active_buffer->toPlainText();
    new_file.close();

    m_active_buffer->set_file_path(new_file_path);
    m_active_buffer->setModified(false);
    set_active_buffer(m_active_buffer);
}

void Window::switch_buffer()
{
    auto buffer_picker = new BufferPicker(m_buffer_model, this);

    connect(buffer_picker, &QDialog::accepted, this, [=] {
        set_active_buffer(m_buffer_model->buffer_from_row(buffer_picker->result_row()));
    });

    buffer_picker->open();
}

void Window::close_buffer()
{
    auto buffer_picker = new BufferPicker(m_buffer_model, this);

    connect(buffer_picker, &QDialog::accepted, this, [=] {
        auto buffer_row = buffer_picker->result_row();
        auto original_buffer = m_buffer_model->buffer_from_row(buffer_row);
        m_buffer_model->remove_buffer(buffer_row);
        auto replacement_buffer = m_buffer_model->buffer_from_row(0);
        replace_buffers_in_frames(original_buffer, replacement_buffer);
    });

    buffer_picker->open();
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

void Window::update_window_title()
{
    setWindowTitle(m_active_buffer->objectName() + " - Qmacs");
}

void Window::connect_buffer(Buffer* buffer)
{
    m_buffer_model->add_buffer(buffer);
}

Frame* Window::create_frame()
{
    auto new_frame = new Frame;
    new_frame->setDocument(m_active_buffer);
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
        auto frame_buffer = qobject_cast<Buffer*>(frame->document());
        if (frame_buffer == original)
            frame->setDocument(replacement);
    }

    set_active_frame(m_active_frame);
}

void Window::set_active_frame(Frame* frame)
{
    m_active_frame = frame;
    m_active_buffer = qobject_cast<Buffer*>(frame->document());
    update_window_title();
}

void Window::set_active_buffer(Buffer* buffer)
{
    m_active_buffer = buffer;
    m_active_frame->setDocument(buffer);
    update_window_title();
}
