#include "buffer-model.hh"

BufferModel::BufferModel(QObject* parent)
    : QAbstractTableModel(parent)
{}

int BufferModel::rowCount(QModelIndex const& parent) const
{
    return m_buffers.count();
}

int BufferModel::columnCount(QModelIndex const& parent) const
{
    return 4;
}

QVariant BufferModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() >= m_buffers.count())
        return QVariant();

    auto buffer = m_buffers[index.row()];
    switch (index.column()) {
        case 0: return buffer->objectName();
        case 1: return buffer->file_path();
        case 2: return buffer->edit_mode();
        case 3: return buffer->isModified();
        default: return QVariant();
    }
}

QVariant BufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
        case 0: return "Buffer Name";
        case 1: return "File Path";
        case 2: return "Edit Mode";
        case 3: return "Unsaved?";
        default: return QVariant();
    }
}

void BufferModel::add_buffer(Buffer* buffer)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_buffers.push_front(buffer);
    endInsertRows();

    connect(buffer, &QTextDocument::modificationChanged, this, [=](bool changed) {
        int row_of_buffer = row_from_buffer(buffer);
        emit dataChanged(index(row_of_buffer, 3), index(row_of_buffer, 3));
    });

    connect(buffer, &Buffer::edit_mode_changed, this, [=](QString edit_mode) {
        int row_of_buffer = row_from_buffer(buffer);
        emit dataChanged(index(row_of_buffer, 2), index(row_of_buffer, 2));
    });

    connect(buffer, &Buffer::file_path_changed, this, [=](QString file_path) {
        int row_of_buffer = row_from_buffer(buffer);
        emit dataChanged(index(row_of_buffer, 0), index(row_of_buffer, 1));
    });
}

void BufferModel::remove_buffer(int buffer_index)
{
    beginRemoveRows(QModelIndex(), buffer_index, buffer_index);
    m_buffers.removeAt(buffer_index);
    endRemoveRows();
}

void BufferModel::move_buffer_to_top(int buffer_index)
{
    beginMoveRows(QModelIndex(), buffer_index, buffer_index, QModelIndex(), 0);
    m_buffers.move(buffer_index, 0);
    endMoveRows();
}

int BufferModel::row_from_buffer(Buffer* buffer) const
{
    for (int row = 0; row < m_buffers.count(); ++row) {
        if (m_buffers[row] == buffer)
            return row;
    }
    return -1;
}

Buffer* BufferModel::buffer_from_row(int row) const
{
    if (row >= 0 && row < m_buffers.count())
        return m_buffers[row];
    return nullptr;
}
