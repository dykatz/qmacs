#include <QFile>
#include <QPlainTextDocumentLayout>

#include "buffer.hh"

Buffer::Buffer(QString const& name, QWidget* parent)
    : QTextDocument(parent)
{
    setObjectName(name);
    setDocumentLayout(new QPlainTextDocumentLayout(this));

    QFont font("");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    setDefaultFont(font);
}

QString Buffer::read()
{
    if (m_file_path.isEmpty())
        return "Cannot load untracked file";

    QFile file(m_file_path);

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return file.errorString();

    QTextStream file_stream(&file);
    auto file_contents = file_stream.readAll();
    file.close();

    setPlainText(file_contents);
    setModified(false);
    return "";
}

QString Buffer::write()
{
    if (!isModified())
        return "";
    if (m_file_path.isEmpty())
        return "Cannot save untracked file";

    QFile file(m_file_path);

    if (!file.open(QFile::WriteOnly | QFile::Text))
        return file.errorString();

    QTextStream file_stream(&file);
    file_stream << toPlainText();
    file.close();

    setModified(false);
    return "";
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
