#include <QKeyEvent>
#include <QLineEdit>
#include <QTableView>
#include <QVBoxLayout>

#include "picker.hh"

Picker::Picker(QAbstractItemModel* model, int initial_row, QWidget* parent)
    : QDialog(parent, Qt::WindowFlags())
{
    auto filter_line_edit = new PickerLineEdit(this);
    m_filter_model = new QSortFilterProxyModel(this);
    m_filter_model->setSourceModel(model);
    connect(filter_line_edit, &QLineEdit::textChanged, m_filter_model, &QSortFilterProxyModel::setFilterWildcard);

    auto model_view = new QTableView(this);
    model_view->setModel(m_filter_model);
    model_view->setSelectionMode(QAbstractItemView::SingleSelection);
    model_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    model_view->selectRow(initial_row < m_filter_model->rowCount() ? initial_row : 0);
    connect(filter_line_edit, &QLineEdit::textChanged, model_view, [=] { model_view->selectRow(0); });
    connect(filter_line_edit, &PickerLineEdit::up_key_pressed, model_view, [=] {
        auto current_row = model_view->currentIndex().row() - 1;
        if (current_row == -1)
            current_row = m_filter_model->rowCount() - 1;
        model_view->selectRow(current_row);
    });
    connect(filter_line_edit, &PickerLineEdit::down_key_pressed, model_view, [=] {
        auto current_row = model_view->currentIndex().row() + 1;
        if (current_row == m_filter_model->rowCount())
            current_row = 0;
        model_view->selectRow(current_row);
    });

    auto layout = new QVBoxLayout;
    layout->addWidget(filter_line_edit);
    layout->addWidget(model_view);
    setLayout(layout);

    connect(model_view, &QAbstractItemView::clicked, this, &Picker::select_buffer);
    connect(filter_line_edit, &QLineEdit::returnPressed, this, [=] { select_buffer(model_view->currentIndex()); });
}

void Picker::select_buffer(QModelIndex const& index)
{
    m_result_row = m_filter_model->mapToSource(index).row();
    if (m_result_row == -1)
        reject();
    else
        accept();
}
