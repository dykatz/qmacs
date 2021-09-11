#pragma once

#include <QDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <QSortFilterProxyModel>

class PickerLineEdit final : public QLineEdit
{
    Q_OBJECT

public:
    PickerLineEdit(QWidget* parent)
        : QLineEdit(parent)
    {}

    virtual void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Up)
            emit up_key_pressed();
        else if (event->key() == Qt::Key_Down)
            emit down_key_pressed();
        else
            QLineEdit::keyPressEvent(event);
    }

signals:
    void up_key_pressed();
    void down_key_pressed();
};

class Picker : public QDialog
{
    Q_OBJECT

public:
    Picker(QAbstractItemModel*, QWidget*);
    ~Picker() = default;

public slots:
    void select_buffer(QModelIndex const&);
    int result_row() const { return m_result_row; }

private:
    QSortFilterProxyModel* m_filter_model;
    int m_result_row { -1 };
};
