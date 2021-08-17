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
    horizontal_split_frame_action->setShortcut(Qt::CTRL | Qt::Key_2);
    connect(horizontal_split_frame_action, &QAction::triggered, this, &Window::horizontal_split_frame);
    frame_menu->addAction(horizontal_split_frame_action);

    auto vertical_split_frame_action = new QAction("Split &Vertically", this);
    vertical_split_frame_action->setShortcut(Qt::CTRL | Qt::Key_3);
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

    m_active_buffer = create_buffer();

    auto first_frame = create_frame();
    m_active_frame = first_frame;
    setCentralWidget(first_frame);
}

void Window::horizontal_split_frame()
{
    auto parent = m_active_frame->parent();
    auto splitter = qobject_cast<QSplitter*>(parent);
    if (!splitter) {
        splitter = new QSplitter(this);
        splitter->addWidget(m_active_frame);
        setCentralWidget(splitter);
    } else if (splitter->orientation() != Qt::Horizontal) {
        auto index_of_current_frame = splitter->indexOf(m_active_frame);
        auto new_splitter = new QSplitter;
        splitter->replaceWidget(index_of_current_frame, new_splitter);
        new_splitter->addWidget(m_active_frame);
        splitter = new_splitter;
    }

    splitter->addWidget(create_frame());
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
        setCentralWidget(splitter);
    } else if (splitter->orientation() != Qt::Vertical) {
        auto index_of_current_frame = splitter->indexOf(m_active_frame);
        auto new_splitter = new QSplitter;
        new_splitter->setOrientation(Qt::Vertical);
        splitter->replaceWidget(index_of_current_frame, new_splitter);
        new_splitter->addWidget(m_active_frame);
        splitter = new_splitter;
    }

    splitter->addWidget(create_frame());
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

    auto frames = findChildren<Frame*>();
    auto active_frame_index = frames.indexOf(m_active_frame);
    delete m_active_frame;

    frames = findChildren<Frame*>();
    if (active_frame_index >= frames.count())
        m_active_frame = frames[frames.count() - 1];
    else
        m_active_frame = frames[active_frame_index];

    if (splitter->count() == 1) {
        auto splitter_child = splitter->findChild<QWidget*>();
        auto splitter_parent = qobject_cast<QSplitter*>(splitter->parent());
        if (!splitter_parent) {
            setCentralWidget(splitter_child);
        } else {
            auto splitter_index = splitter_parent->indexOf(splitter);
            splitter_parent->replaceWidget(splitter_index, splitter_child);
        }
        delete splitter;
    }

    m_active_frame->setFocus();
}

void Window::move_to_next_frame()
{
    auto frames = findChildren<Frame*>();
    auto new_frame_index = frames.indexOf(m_active_frame) - 1;

    if (new_frame_index < 0)
        new_frame_index = frames.count() - 1;

    set_active_frame(frames[new_frame_index]);
    m_active_frame->setFocus();
}

void Window::move_to_previous_frame()
{
    auto frames = findChildren<Frame*>();
    auto new_frame_index = frames.indexOf(m_active_frame) + 1;

    if (new_frame_index >= frames.count())
        new_frame_index = 0;

    set_active_frame(frames[new_frame_index]);
    m_active_frame->setFocus();
}

void Window::new_buffer()
{
    set_active_buffer(create_buffer());
}

Buffer* Window::create_buffer()
{
    auto new_buffer = new Buffer(this);
    m_buffers.append(new_buffer);
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
}

void Window::set_active_buffer(Buffer* buffer)
{
    m_active_buffer = buffer;
    m_active_frame->setDocument(buffer);
}
