#pragma once

#include <QAbstractTableModel>

#include "buffer.hh"

struct BufferModelItem
{
    BufferModelItem(Buffer*, QString, QString);

    Buffer* buffer;
    QString buffer_name;
    QString file_path;
    QString edit_mode;
    bool is_unsaved;
};

class BufferModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    BufferModel(QObject* parent);
    virtual ~BufferModel() = default;

    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override;
    virtual int columnCount(QModelIndex const& parent = QModelIndex()) const override;
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void add_buffer(Buffer* buffer, QString file_path, QString edit_mode);
    void set_file_path_for_buffer(Buffer* buffer, QString file_path);
    QString file_path_for_buffer(Buffer* buffer) const;
    Buffer* buffer_from_row(int row) const;

private:
    QList<BufferModelItem> m_model_items;
};
