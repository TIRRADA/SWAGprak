#pragma once

#include <QString>
#include <QVector>

//товар на складе
struct Product{
    int id = 0;
    QString name;
    int packagesInStock = 0;      //кол-во упаковок на складе
    int maxPackages = 0;          //сколько склад может вместить товара
    int unitsPerPackage = 1;      //единиц товара
    QString unitName;             //"пачки", "кг"
    double pricePerPackage = 0.0; //цена без уценки
    int daysUntilExpiration = 0;  //до конца срока годности
    int discountPercent = 0;      //процент уценки сейчас
};

//инфа про товар
struct OrderItem{
    int productId = 0;
    QString productName;
    int requestedUnits = 0;   //сколько заказали
    int issuedPackages = 0;   //сколько упакововк выделили
    int issuedUnits = 0;      //сколько единиц выделили
    double cost = 0.0;        //стоимость выданного
};

//инфа про заказ
struct Order{
    int id = 0;
    int shopNumber = 0;
    QVector<OrderItem> items;
};

//инфа про перевозку
struct Shipment{
    int id = 0;
    int shopNumber = 0;
    QVector<OrderItem> items;
    double totalCost = 0.0;
    bool isApproved = false;  //ждем подтверждения зав. складом
};

//заявка на пополнение товара
struct SupplierRequest{
    int id = 0;
    int productId = 0;
    QString productName;
    int packagesToOrder = 0;
    int deliveryDays = 0;     //сколько ждать доставку
    bool isApproved = false;
};

//списание просроченного товара
struct WrittenOffItem{
    QString productName;
    int packages = 0;
    double loss = 0.0;        //потери
};

//итоги
struct Statistics{
    int soldUnits = 0;
    double soldRevenue = 0.0;
    double discountLoss = 0.0;      //потери за уценку
    double writeOffLoss = 0.0;      //потери за списание
    int writtenOffPackages = 0;
};

//тут ниче не трогай, это параметры N, M и K
struct SimulationParameters{
    int dayCount = 10;              //N
    int shopCount = 3;              //M
    int productKindCount = 12;      //K

    QVector<Product> initialProducts; //начальный набор товаров на складе

    int minItemsPerOrder = 1;       //кол-во позиций в заказе
    int maxItemsPerOrder = 4;
    int minUnitsPerItem = 20;       //кол-во товара
    int maxUnitsPerItem = 200;
    int minDeliveryDays = 1;        //срок доставки от и до
    int maxDeliveryDays = 5;
};
