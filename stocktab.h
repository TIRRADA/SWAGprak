#pragma once

#include "warehousetypes.h"

#include <QWidget>
//не трогай
class QTableWidget;

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
