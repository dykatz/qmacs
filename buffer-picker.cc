#include <QTableView>
#include <QVBoxLayout>

#include "buffer-picker.hh"

BufferPicker::BufferPicker(BufferModel* model, QWidget* parent)
    : QDialog(parent, Qt::WindowFlags())
    , m_buffer_model(model)
{
    auto buffer_model_view = new QTableView(this);
    buffer_model_view->setModel(m_buffer_model);
    buffer_model_view->setSelectionMode(QAbstractItemView::SingleSelection);
    buffer_model_view->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto layout = new QVBoxLayout;
    layout->addWidget(buffer_model_view);
    setLayout(layout);

    connect(buffer_model_view, &QAbstractItemView::clicked, this, &BufferPicker::select_buffer);
}

void BufferPicker::select_buffer(QModelIndex const& index)
{
    m_result_row = index.row();
    accept();
}
