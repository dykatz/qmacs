#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>

#include "buffer-model.hh"

class BufferPicker : public QDialog
{
    Q_OBJECT

public:
    BufferPicker(BufferModel*, QWidget*);
    ~BufferPicker() = default;

public slots:
    void select_buffer(QModelIndex const&);
    int result_row() const { return m_result_row; }

private:
    QSortFilterProxyModel* m_buffer_model_filter_model;
    int m_result_row { -1 };
};
