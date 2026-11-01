#pragma once

#include "warehouseservice.h"
#include <QMap>


class WarehouseSimulator : public WarehouseService {
    Q_OBJECT

public:
    explicit WarehouseSimulator(QObject *parent = nullptr);

    void startSimulation(const SimulationParameters &parameters) override;
    void nextDay() override;
    int currentDay() const override;
    bool isFinished() const override;

    QVector <Product> stock() const override;
    QVector <Order> todayOrders() const override;
    QVector <Shipment> tomorrowShipments() const override;
    QVector <SupplierRequest> supplierRequests() const override;
    QVector <WrittenOffItem> writtenOffItems() const override;
    Statistics statistics() const override;

    void setDiscount(int productId, int discountPercent) override;
    void approveShipment(int shipmentId) override;
    void approveSupplierRequest(int requestId) override;

private:
    void receiveDeliveries();
    void checkExpiration();
    void generateOrders();
    void createSupplierRequests();
    Product *findProduct(int productId);
    int pendingPackages(int productId) const;

    SimulationParameters m_parameters;
    int m_day = 0;
    bool m_started = false;

    QVector <Product> m_products;
    QMap <int, int> m_shelfLife;
    QVector <Order> m_orders;
    QVector <Shipment> m_shipments;
    QVector <SupplierRequest> m_requests;
    QMap <int, int> m_arrivalDay;
    QVector <WrittenOffItem> m_writtenOff;
    Statistics m_statistics;

    int m_nextOrderId = 1;
    int m_nextShipmentId = 1;
    int m_nextRequestId = 1;
};
