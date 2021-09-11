#include <QLineEdit>
#include <QTableView>
#include <QVBoxLayout>

#include "buffer-picker.hh"

BufferPicker::BufferPicker(BufferModel* model, QWidget* parent)
    : QDialog(parent, Qt::WindowFlags())
{
    auto filter_line_edit = new QLineEdit(this);
    m_buffer_model_filter_model = new QSortFilterProxyModel(this);
    m_buffer_model_filter_model->setSourceModel(model);
    connect(filter_line_edit, &QLineEdit::textChanged, m_buffer_model_filter_model, &QSortFilterProxyModel::setFilterWildcard);

    auto buffer_model_view = new QTableView(this);
    buffer_model_view->setModel(m_buffer_model_filter_model);
    buffer_model_view->setSelectionMode(QAbstractItemView::SingleSelection);
    buffer_model_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    buffer_model_view->selectRow(0);
    connect(filter_line_edit, &QLineEdit::textChanged, buffer_model_view, [=] { buffer_model_view->selectRow(0); });

    auto layout = new QVBoxLayout;
    layout->addWidget(filter_line_edit);
    layout->addWidget(buffer_model_view);
    setLayout(layout);

    connect(buffer_model_view, &QAbstractItemView::clicked, this, &BufferPicker::select_buffer);
    connect(filter_line_edit, &QLineEdit::returnPressed, this, [=] { select_buffer(buffer_model_view->currentIndex()); });
}

void BufferPicker::select_buffer(QModelIndex const& index)
{
    m_result_row = m_buffer_model_filter_model->mapToSource(index).row();
    if (m_result_row == -1)
        reject();
    else
        accept();
}
