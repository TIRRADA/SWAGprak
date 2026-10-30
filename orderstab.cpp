#include "orderstab.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

OrdersTab::OrdersTab(QWidget *parent) : QWidget(parent){
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"Заказ №", "Торговая точка", "Товар", "Заказано (ед.)", "Выдано упаковок", "Выдано (ед.)", "Стоимость"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_table);
}

void OrdersTab::showOrders(const QVector<Order> &orders){
    int totalRowCount = 0;
    for (const Order &order : orders)
        totalRowCount += order.items.size();
    m_table->setRowCount(totalRowCount);

    int row = 0;
    for (const Order &order : orders) {
        for (const OrderItem &item : order.items) {
            m_table->setItem(row, 0, new QTableWidgetItem(QString::number(order.id)));
            m_table->setItem(row, 1, new QTableWidgetItem(QString("Точка %1").arg(order.shopNumber)));
            m_table->setItem(row, 2, new QTableWidgetItem(item.productName));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(item.requestedUnits)));
            m_table->setItem(row, 4, new QTableWidgetItem(QString::number(item.issuedPackages)));
            m_table->setItem(row, 5, new QTableWidgetItem(QString::number(item.issuedUnits)));
            m_table->setItem(row, 6, new QTableWidgetItem(QString::number(item.cost, 'f', 2)));
            ++row;
        }
    }
}
