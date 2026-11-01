#include "warehouse_simulation.h"

#include <QRandomGenerator>
#include <QtGlobal>
#include <algorithm>
#include <cmath>


const int kDiscountDays = 5;
const int kAutoDiscountPercent = 20;
const double kShopOrderChance = 0.8;
const double kLowStockPart = 0.3;


static int randomBetween(int a, int b) {
    if (b < a) std::swap(a, b);
    return QRandomGenerator::global()->bounded(a, b + 1);
}

WarehouseSimulator::WarehouseSimulator(QObject *parent) : WarehouseService(parent) {

}

void WarehouseSimulator::startSimulation(const SimulationParameters &parameters) {
    m_parameters = parameters;
    m_day = 0;
    m_started = true;
    m_products = parameters.initialProducts;
    m_shelfLife.clear();
    m_orders.clear();
    m_shipments.clear();
    m_requests.clear();
    m_arrivalDay.clear();
    m_writtenOff.clear();
    m_statistics = Statistics();
    m_nextOrderId = 1;
    m_nextShipmentId = 1;
    m_nextRequestId = 1;

    for (Product& product : m_products) {
        product.discountPercent = 0;
        m_shelfLife[product.id] = product.daysUntilExpiration;
    }
    emit stateChanged();
}

void WarehouseSimulator::nextDay() {
    if (!m_started || isFinished()) {
        return;
    }
    m_day++;
    m_orders.clear();
    m_shipments.clear();

    receiveDeliveries();
    checkExpiration();
    generateOrders();
    createSupplierRequests();

    emit stateChanged();
}

int WarehouseSimulator::currentDay() const {
    return m_day;
}

bool WarehouseSimulator::isFinished() const {
    return m_started && m_day >= m_parameters.dayCount;
}

QVector <Product> WarehouseSimulator::stock() const {
    return m_products;
}

QVector <Order> WarehouseSimulator::todayOrders() const {
    return m_orders;
}

QVector <Shipment> WarehouseSimulator::tomorrowShipments() const {
    return m_shipments;
}

QVector <SupplierRequest> WarehouseSimulator::supplierRequests() const {
    return m_requests;
}

QVector <WrittenOffItem> WarehouseSimulator::writtenOffItems() const {
    return m_writtenOff;
}

Statistics WarehouseSimulator::statistics() const {
    return m_statistics;
}

void WarehouseSimulator::setDiscount(int productId, int discountPercent) {
    Product *product = findProduct(productId);
    if (!product) return;
    product->discountPercent = qBound(0, discountPercent, 90);
    emit stateChanged();
}

void WarehouseSimulator::approveShipment(int shipmentId) {
    for (Shipment& shipment : m_shipments) {
        if (shipment.id == shipmentId) {
            shipment.isApproved = true;
        }
    }
    emit stateChanged();
}

void WarehouseSimulator::approveSupplierRequest(int requestId) {
    for (SupplierRequest& request : m_requests) {
        if (request.id == requestId) {
            request.isApproved = true;
        }
    }
    emit stateChanged();
}

Product *WarehouseSimulator::findProduct(int productId) {
    for (Product& product : m_products) {
        if (product.id == productId) {
            return &product;
        }
    }
    return nullptr;
}

int WarehouseSimulator::pendingPackages(int productId) const {
    int sum = 0;
    for (const SupplierRequest& request : m_requests) {
        if (request.productId == productId && m_arrivalDay.value(request.id) > m_day) {
            sum += request.packagesToOrder;
        }
    }
    return sum;
}

void WarehouseSimulator::receiveDeliveries() {
    for (const SupplierRequest& request : m_requests) {
        if (m_arrivalDay.value(request.id) != m_day) {
            continue;
        }
        Product *product = findProduct(request.productId);
        if (!product) {
            continue;
        }
        const int freePlace = product->maxPackages - product->packagesInStock;
        const int added = qMin(request.packagesToOrder, freePlace);
        if (added <= 0) {
            continue;
        }

        const int fresh = m_shelfLife.value(product->id);
        if (product->packagesInStock == 0) {
            product->daysUntilExpiration = fresh;
        } else {
            const double mixed = (double(product->daysUntilExpiration) * product->packagesInStock + double(fresh) * added)
                                 / (product->packagesInStock + added);
            product->daysUntilExpiration = int(mixed);
        }
        product->packagesInStock += added;
        if (product->daysUntilExpiration > kDiscountDays) {
            product->discountPercent = 0;
        }
    }
}

void WarehouseSimulator::checkExpiration() {
    for (Product& product : m_products) {
        product.daysUntilExpiration--;

        if (product.daysUntilExpiration <= 0) {
            if (product.packagesInStock > 0) {
                WrittenOffItem item;
                item.productName = product.name;
                item.packages = product.packagesInStock;
                item.loss = product.packagesInStock * product.pricePerPackage * (100 - product.discountPercent) / 100.0;
                m_writtenOff.append(item);

                m_statistics.writtenOffPackages += item.packages;
                m_statistics.writeOffLoss += item.loss;
                product.packagesInStock = 0;
            }
            product.daysUntilExpiration = 0;
            product.discountPercent = 0;
        } else if (product.daysUntilExpiration <= kDiscountDays && product.discountPercent == 0 && product.packagesInStock > 0) {
            product.discountPercent = kAutoDiscountPercent;
        }
    }
}

void WarehouseSimulator::generateOrders() {
    for (int shop = 1; shop <= m_parameters.shopCount; ++shop) {
        if (QRandomGenerator::global()->generateDouble() > kShopOrderChance) {
            continue;
        }

        Order order;
        order.id = m_nextOrderId++;
        order.shopNumber = shop;

        const int itemCount = randomBetween(m_parameters.minItemsPerOrder, m_parameters.maxItemsPerOrder);
        QVector <int> usedProducts;

        for (int i = 0; i < itemCount && usedProducts.size() < m_products.size(); ++i) {
            double totalWeight = 0;
            for (const Product& product : m_products) {
                if (!usedProducts.contains(product.id)) {
                    totalWeight += 1.0 + product.discountPercent / 10.0;
                }
            }
            double point = QRandomGenerator::global()->generateDouble() * totalWeight;
            Product *chosen = nullptr;
            for (Product& product : m_products) {
                if (usedProducts.contains(product.id)) {
                    continue;
                }
                point -= 1.0 + product.discountPercent / 10.0;
                chosen = &product;
                if (point <= 0) {
                    break;
                }
            }
            if (!chosen) {
                break;
            }
            usedProducts.append(chosen->id);

            OrderItem item;
            item.productId = chosen->id;
            item.productName = chosen->name;
            item.requestedUnits = randomBetween(m_parameters.minUnitsPerItem, m_parameters.maxUnitsPerItem);

            int packages = int(std::round(double(item.requestedUnits) / chosen->unitsPerPackage));
            packages = qMax(packages, 1);
            packages = qMin(packages, chosen->packagesInStock);

            item.issuedPackages = packages;
            item.issuedUnits = packages * chosen->unitsPerPackage;
            const double fullPrice = packages * chosen->pricePerPackage;
            item.cost = fullPrice * (100 - chosen->discountPercent) / 100.0;

            chosen->packagesInStock -= packages;
            m_statistics.soldUnits += item.issuedUnits;
            m_statistics.soldRevenue += item.cost;
            m_statistics.discountLoss += fullPrice - item.cost;

            order.items.append(item);
        }

        Shipment shipment;
        shipment.id = m_nextShipmentId++;
        shipment.shopNumber = shop;
        shipment.items = order.items;
        for (const OrderItem& item : order.items) {
            shipment.totalCost += item.cost;
        }

        m_orders.append(order);
        m_shipments.append(shipment);
    }
}

void WarehouseSimulator::createSupplierRequests() {
    for (const Product& product : m_products) {
        const int pending = pendingPackages(product.id);
        const int total = product.packagesInStock + pending;
        if (total >= product.maxPackages * kLowStockPart) {
            continue;
        }

        SupplierRequest request;
        request.id = m_nextRequestId++;
        request.productId = product.id;
        request.productName = product.name;
        request.packagesToOrder = product.maxPackages - total;
        request.deliveryDays = randomBetween(m_parameters.minDeliveryDays, m_parameters.maxDeliveryDays);

        m_arrivalDay[request.id] = m_day + request.deliveryDays;
        m_requests.append(request);
    }
}
