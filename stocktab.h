#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QTableWidget;

// Вкладка со списком товаров на складе.
class StockTab : public QWidget
{
    Q_OBJECT

public:
    explicit StockTab(QWidget *parent = nullptr);

    void showProducts(const QVector<Product> &products);

private:
    void setCellText(int row, int column, const QString &text, bool isHighlighted);

    QTableWidget *m_table;
};
