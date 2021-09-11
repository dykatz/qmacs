#include "buffer.hh"

Buffer::Buffer(QString const& name, QWidget* parent)
    : QTextDocument(parent)
{
    setObjectName(name);
}

void Buffer::set_file_path(QString file_path)
{
    if (file_path != m_file_path) {
        m_file_path = file_path;

        auto file_name = QUrl(file_path).fileName();
        setObjectName(file_name);

        emit file_path_changed(file_path);
    }
}

void Buffer::set_edit_mode(QString edit_mode)
{
    if (edit_mode != m_edit_mode) {
        m_edit_mode = edit_mode;
        emit edit_mode_changed(edit_mode);
    }
}
