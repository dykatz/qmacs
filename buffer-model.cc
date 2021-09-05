#include "buffer-model.hh"

BufferModelItem::BufferModelItem(Buffer* buffer, QString file_path, QString edit_mode)
    : buffer(buffer)
    , buffer_name(buffer->objectName())
    , file_path(file_path)
    , edit_mode(edit_mode)
    , is_unsaved(false)
{}

BufferModel::BufferModel(QObject* parent)
    : QAbstractTableModel(parent)
{}

int BufferModel::rowCount(QModelIndex const& parent) const
{
    return m_model_items.count();
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

    if (index.row() >= m_model_items.count())
        return QVariant();

    auto& model_item = m_model_items[index.row()];
    switch (index.column()) {
        case 0: return model_item.buffer_name;
        case 1: return model_item.file_path;
        case 2: return model_item.edit_mode;
        case 3: return model_item.is_unsaved;
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

void BufferModel::add_buffer(Buffer* buffer, QString file_path, QString edit_mode)
{
    beginInsertRows(QModelIndex(), 0, 1);
    m_model_items.emplaceFront(buffer, file_path, edit_mode);
    endInsertRows();
}
