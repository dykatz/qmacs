#include "buffer-model.hh"

BufferModelItem::BufferModelItem(Buffer* buffer, QString file_path, QString edit_mode)
    : buffer(buffer)
    , buffer_name(buffer->objectName())
    , file_path(file_path)
    , edit_mode(edit_mode)
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
        case 3: return model_item.buffer->isModified();
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

    connect(buffer, &QTextDocument::modificationChanged, this, [=](bool changed) {
        int row_of_buffer = row_from_buffer(buffer);
        emit dataChanged(index(row_of_buffer, 3), index(row_of_buffer, 3));
    });
}

void BufferModel::set_file_path_for_buffer(Buffer* buffer, QString file_path)
{
    int row_of_buffer = row_from_buffer(buffer);
    if (row_of_buffer == -1)
        return;

    auto file_name = QUrl(file_path).fileName();
    buffer->setObjectName(file_name);
    m_model_items[row_of_buffer].buffer_name = file_name;
    m_model_items[row_of_buffer].file_path = file_path;

    emit dataChanged(index(row_of_buffer, 0), index(row_of_buffer, 1));
}

QString BufferModel::file_path_for_buffer(Buffer* buffer) const
{
    int row_of_buffer = row_from_buffer(buffer);
    if (row_of_buffer == -1)
        return "";
    return m_model_items[row_of_buffer].file_path;
}

int BufferModel::row_from_buffer(Buffer* buffer) const
{
    for (int row = 0; row < m_model_items.count(); ++row) {
        if (m_model_items[row].buffer == buffer)
            return row;
    }
    return -1;
}

Buffer* BufferModel::buffer_from_row(int row) const
{
    return m_model_items[row].buffer;
}
