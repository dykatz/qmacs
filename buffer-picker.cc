#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

#include "buffer-picker.hh"

BufferPicker::BufferPicker(BufferModel* model, QWidget* parent)
    : QDialog(parent, Qt::WindowFlags())
    , m_buffer_model(model)
{
    auto filter_line_edit = new QLineEdit(this);
    auto buffer_model_filter_model = new QSortFilterProxyModel(this);
    buffer_model_filter_model->setSourceModel(model);
    connect(filter_line_edit, &QLineEdit::textChanged, buffer_model_filter_model, &QSortFilterProxyModel::setFilterWildcard);

    auto buffer_model_view = new QTableView(this);
    buffer_model_view->setModel(buffer_model_filter_model);
    buffer_model_view->setSelectionMode(QAbstractItemView::SingleSelection);
    buffer_model_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    buffer_model_view->selectRow(0);

    auto layout = new QVBoxLayout;
    layout->addWidget(filter_line_edit);
    layout->addWidget(buffer_model_view);
    setLayout(layout);

    connect(buffer_model_view, &QAbstractItemView::clicked, this, &BufferPicker::select_buffer);
}

void BufferPicker::select_buffer(QModelIndex const& index)
{
    m_result_row = index.row();
    accept();
}
