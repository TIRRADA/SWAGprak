#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QTableWidget;

//вкладка со всеми заказами
class OrdersTab : public QWidget
{
    Q_OBJECT

public:
    explicit OrdersTab(QWidget *parent = nullptr);
    void showOrders(const QVector<Order> &orders);

private:
    QTableWidget *m_table;
};
