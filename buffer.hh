#pragma once

#include <QTextDocument>
#include <QWidget>

class Buffer : public QTextDocument
{
    Q_OBJECT

public:
    Buffer(QString const&, QWidget*);
    virtual ~Buffer() = default;

    QString read();
    QString write();
    QString file_path() const { return m_file_path; }
    QString edit_mode() const { return m_edit_mode; }

public slots:
    void set_file_path(QString file_path);
    void set_edit_mode(QString edit_mode);
    void set_font_point_size(int);

signals:
    void file_path_changed(QString file_path);
    void edit_mode_changed(QString edit_mode);

private:
    QString m_file_path { "" };
    QString m_edit_mode { "text" };
};
