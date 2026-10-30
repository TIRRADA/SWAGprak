#pragma once

#include "warehousetypes.h"
#include <QObject>

class WarehouseService : public QObject
{
    Q_OBJECT

public:
    explicit WarehouseService(QObject *parent = nullptr) : QObject(parent) {}
    ~WarehouseService() override = default;

    //управление моделированием
    virtual void startSimulation(const SimulationParameters &parameters) = 0;
    virtual void nextDay() = 0;
    virtual int currentDay() const = 0;
    virtual bool isFinished() const = 0;

    //отображение
    virtual QVector<Product> stock() const = 0;
    virtual QVector<Order> todayOrders() const = 0;
    virtual QVector<Shipment> tomorrowShipments() const = 0;
    virtual QVector<SupplierRequest> supplierRequests() const = 0;
    virtual QVector<WrittenOffItem> writtenOffItems() const = 0;
    virtual Statistics statistics() const = 0;

    //че делает зав. складом
    virtual void setDiscount(int productId, int discountPercent) = 0;
    virtual void approveShipment(int shipmentId) = 0;
    virtual void approveSupplierRequest(int requestId) = 0;

signals:
    void stateChanged();
};
