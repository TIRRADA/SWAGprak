#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QPushButton;
class QSpinBox;
class QTableWidget;

//вкладка для уценки товара
class DiscountTab : public QWidget
{
    Q_OBJECT

public:
    explicit DiscountTab(QWidget *parent = nullptr);
    void showProducts(const QVector<Product> &products);
    int daysThreshold() const;

signals:
    void discountRequested(int productId, int discountPercent);
    void daysThresholdChanged(int days);

private:
    void rebuildTable();
    int selectedRowIndex() const;
    void updateEditor();

    QVector<Product> m_allProducts;    //все товары склада
    QVector<Product> m_shownProducts;  //товары, которые поподают под порог

    QSpinBox *m_daysThresholdSpinBox;
    QTableWidget *m_table;
    QSpinBox *m_discountSpinBox;
    QPushButton *m_applyButton;
};
