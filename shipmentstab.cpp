#include "shipmentstab.h"

#include <QColor>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QVBoxLayout>

ShipmentsTab::ShipmentsTab(QWidget *parent) : QWidget(parent){
    m_shipmentsTable = new QTableWidget(this);
    m_shipmentsTable->setColumnCount(5);
    m_shipmentsTable->setHorizontalHeaderLabels({ "Перевозка №", "Торговая точка", "Позиций", "Сумма", "Статус"});
    m_shipmentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_shipmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_shipmentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_shipmentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_itemsTable = new QTableWidget(this);
    m_itemsTable->setColumnCount(5);
    m_itemsTable->setHorizontalHeaderLabels({"Товар", "Заказано (ед.)", "Выдано упаковок", "Выдано (ед.)", "Стоимость"});
    m_itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_approveButton = new QPushButton("Утвердить выбранную перевозку", this);
    m_approveButton->setEnabled(false);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_approveButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Перевозки на следующий рабочий день:", this));
    layout->addWidget(m_shipmentsTable);
    layout->addWidget(new QLabel("Состав выбранной перевозки:", this));
    layout->addWidget(m_itemsTable);
    layout->addLayout(buttonLayout);

    connect(m_shipmentsTable, &QTableWidget::itemSelectionChanged, this, [this]() {showItemsOfSelectedShipment();updateApproveButton();});
    connect(m_approveButton, &QPushButton::clicked, this, [this]() {const int index = selectedShipmentIndex();if (index >= 0)emit approveRequested(m_shipments.at(index).id);});
}

void ShipmentsTab::showShipments(const QVector<Shipment> &shipments){
    int previouslySelectedId = -1;
    const int previousIndex = selectedShipmentIndex();
    if (previousIndex >= 0){
        previouslySelectedId = m_shipments.at(previousIndex).id;
    }
    m_shipments = shipments;

    {
        QSignalBlocker blocker(m_shipmentsTable);
        m_shipmentsTable->setRowCount(m_shipments.size());
        for (int row = 0; row < m_shipments.size(); ++row) {
            const Shipment &shipment = m_shipments.at(row);
            const QString statusText = shipment.isApproved ? "Утверждена" : "Ожидает утверждения";

            QVector<QTableWidgetItem *> rowItems = {
                new QTableWidgetItem(QString::number(shipment.id)),
                new QTableWidgetItem(QString("Точка %1").arg(shipment.shopNumber)),
                new QTableWidgetItem(QString::number(shipment.items.size())),
                new QTableWidgetItem(QString::number(shipment.totalCost, 'f', 2)),
                new QTableWidgetItem(statusText)
            };

            for (int column = 0; column < rowItems.size(); ++column) {
                if (shipment.isApproved) {
                    rowItems[column]->setBackground(QColor(200, 230, 201));
                    rowItems[column]->setForeground(QColor(0, 0, 0));
                }
                m_shipmentsTable->setItem(row, column, rowItems.at(column));
            }
            if (shipment.id == previouslySelectedId){
                m_shipmentsTable->selectRow(row);
            }
        }
    }
    showItemsOfSelectedShipment();
    updateApproveButton();
}

int ShipmentsTab::selectedShipmentIndex() const{
    const QModelIndexList selectedRows = m_shipmentsTable->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()){
        return -1;
    }
    return selectedRows.first().row();
}

void ShipmentsTab::showItemsOfSelectedShipment(){
    const int index = selectedShipmentIndex();
    if (index < 0 || index >= m_shipments.size()) {
        m_itemsTable->setRowCount(0);
        return;
    }

    const QVector<OrderItem> &items = m_shipments.at(index).items;
    m_itemsTable->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const OrderItem &item = items.at(row);
        m_itemsTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        m_itemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.requestedUnits)));
        m_itemsTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.issuedPackages)));
        m_itemsTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.issuedUnits)));
        m_itemsTable->setItem(row, 4, new QTableWidgetItem(QString::number(item.cost, 'f', 2)));
    }
}

void ShipmentsTab::updateApproveButton(){
    const int index = selectedShipmentIndex();
    const bool canApprove = index >= 0 && index < m_shipments.size() && !m_shipments.at(index).isApproved;
    m_approveButton->setEnabled(canApprove);
}
