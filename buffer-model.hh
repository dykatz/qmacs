#pragma once

#include <QAbstractTableModel>

#include "buffer.hh"

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

    void add_buffer(Buffer* buffer);
    void remove_buffer(int buffer_index);
    void move_buffer_to_top(int buffer_index);
    Buffer* buffer_from_row(int row) const;
    int row_from_buffer(Buffer* buffer) const;

private:
    QList<Buffer*> m_buffers;
};
