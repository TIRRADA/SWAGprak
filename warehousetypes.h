#pragma once

#include <QString>
#include <QVector>

//товары
struct Product{
    int id = 0;
    QString name;
    int packagesInStock = 0;
    int maxPackages = 0;
    int unitsPerPackage = 1;
    QString unitName;
    double pricePerPackage = 0.0;
    int daysUntilExpiration = 0;
    int discountPercent = 0;
};

//инфа о заказе
struct OrderItem{
    int productId = 0;
    QString productName;
    int requestedUnits = 0;
    int issuedPackages = 0;
    int issuedUnits = 0;
    double cost = 0.0;
};

//заказ
struct Order{
    int id = 0;
    int shopNumber = 0;
    QVector<OrderItem> items;
};

//первозка
struct Shipment{
    int id = 0;
    int shopNumber = 0;
    QVector<OrderItem> items;
    double totalCost = 0.0;
    bool isApproved = false;
};

//заявка фирме
struct SupplierRequest{
    int id = 0;
    int productId = 0;
    QString productName;
    int packagesToOrder = 0;
    int deliveryDays = 0;
    bool isApproved = false;
};

//списанный товар
struct WrittenOffItem{
    QString productName;
    int packages = 0;
    double loss = 0.0;
};

//итоги
struct Statistics{
    int soldUnits = 0;
    double soldRevenue = 0.0;
    double discountLoss = 0.0;
    double writeOffLoss = 0.0;
    int writtenOffPackages = 0;
};

//параметры моделирования
struct SimulationParameters{
    int dayCount = 10;
    int shopCount = 3;
    int productKindCount = 12;

    int minItemsPerOrder = 1;
    int maxItemsPerOrder = 4;
    int minUnitsPerItem = 20;
    int maxUnitsPerItem = 200;
    int minDeliveryDays = 1;
    int maxDeliveryDays = 5;
};
