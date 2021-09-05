#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>

#include "window.hh"

Window::Window()
    : QMainWindow(nullptr)
{
    auto file_menu = menuBar()->addMenu("&File");

    auto new_buffer_action = new QAction("&New Buffer", this);
    new_buffer_action->setShortcut(QKeySequence::New);
    connect(new_buffer_action, &QAction::triggered, this, &Window::new_buffer);
    file_menu->addAction(new_buffer_action);

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
    m_active_buffer = create_buffer();

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
    setCentralWidget(m_active_frame);
    delete central_widget;
    m_active_frame->setFocus();
}

void Window::remove_this_frame()
{
    auto splitter = qobject_cast<QSplitter*>(m_active_frame->parent());
    if (!splitter)
        return;

    auto index_of_active_frame = splitter->indexOf(m_active_frame);
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
    set_active_buffer(create_buffer());
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

Buffer* Window::create_buffer()
{
    auto new_buffer = new Buffer(create_untitled_name(), this);
    m_buffer_model->add_buffer(new_buffer, "", "text");
    return new_buffer;
}

Frame* Window::create_frame()
{
    auto new_frame = new Frame;
    new_frame->setDocument(m_active_buffer);
    connect(new_frame, &Frame::gained_focus, this, [=]{ set_active_frame(new_frame); });
    return new_frame;
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
