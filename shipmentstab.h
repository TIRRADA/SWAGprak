#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QPushButton;
class QTableWidget;

//вкадка с перевозками на след день
class ShipmentsTab : public QWidget{
    Q_OBJECT

public:
    explicit ShipmentsTab(QWidget *parent = nullptr);
    void showShipments(const QVector<Shipment> &shipments);

signals:
    void approveRequested(int shipmentId);

private:
    int selectedShipmentIndex() const;
    void showItemsOfSelectedShipment();
    void updateApproveButton();

    QVector<Shipment> m_shipments;
    QTableWidget *m_shipmentsTable;
    QTableWidget *m_itemsTable;
    QPushButton *m_approveButton;
};
