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
    void setExpirationWarningDays(int days);

private:
    void rebuildTable();
    void setCellText(int row, int column, const QString &text, bool isHighlighted);

    QVector<Product> m_products;
    int m_expirationWarningDays = 5;
    QTableWidget *m_table;
};
