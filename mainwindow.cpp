#include "mainwindow.h"

#include "orderstab.h"
#include "shipmentstab.h"
#include "simulationsettingsdialog.h"
#include "stocktab.h"
#include "supplierrequeststab.h"
#include "warehouseservice.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle("Система управления оптовым складом");
    resize(1000, 650);

    createInterface();
    updateControlsState();
    showDemoData();
}

void MainWindow::setWarehouseService(WarehouseService *warehouseService){
    m_warehouseService = warehouseService;
    if (m_warehouseService) {
        connect(m_warehouseService, &WarehouseService::stateChanged, this, &MainWindow::refreshView);

        //утверждаем по кнопке
        connect(m_shipmentsTab, &ShipmentsTab::approveRequested, m_warehouseService, &WarehouseService::approveShipment);
        connect(m_supplierRequestsTab, &SupplierRequestsTab::approveRequested, m_warehouseService, &WarehouseService::approveSupplierRequest);
    }
    updateControlsState();
    refreshView();
}

void MainWindow::createInterface(){
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);

    m_dayLabel = new QLabel("День: 0", centralWidget);
    m_settingsButton = new QPushButton("Параметры…", centralWidget);
    m_nextDayButton = new QPushButton("Следующий день", centralWidget);
    m_runToEndButton = new QPushButton("Запустить до конца", centralWidget);

    auto *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(m_dayLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(m_settingsButton);
    controlLayout->addWidget(m_nextDayButton);
    controlLayout->addWidget(m_runToEndButton);

    m_tabWidget = new QTabWidget(centralWidget);
    m_stockTab = new StockTab(m_tabWidget);
    m_ordersTab = new OrdersTab(m_tabWidget);
    m_shipmentsTab = new ShipmentsTab(m_tabWidget);
    m_supplierRequestsTab = new SupplierRequestsTab(m_tabWidget);

    m_tabWidget->addTab(m_stockTab, "Склад");
    m_tabWidget->addTab(m_ordersTab, "Заказы за день");
    m_tabWidget->addTab(m_shipmentsTab, "Перевозки на завтра");
    m_tabWidget->addTab(m_supplierRequestsTab, "Заявки поставщикам");

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_tabWidget);
    setCentralWidget(centralWidget);

    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::openSettingsDialog);
    connect(m_nextDayButton, &QPushButton::clicked, this, &MainWindow::goToNextDay);
    connect(m_runToEndButton, &QPushButton::clicked, this, &MainWindow::runSimulationToEnd);
}

void MainWindow::updateControlsState(){
    const bool canAdvance = m_warehouseService != nullptr && m_isSimulationStarted && !m_warehouseService->isFinished();
    m_nextDayButton->setEnabled(canAdvance);
    m_runToEndButton->setEnabled(canAdvance);
}

void MainWindow::openSettingsDialog(){
    SimulationSettingsDialog dialog(m_parameters, this);
    if (dialog.exec() != QDialog::Accepted){
        return;
    }

    m_parameters = dialog.parameters();

    if (m_warehouseService) {
        m_warehouseService->startSimulation(m_parameters);
        m_isSimulationStarted = true;
    }
    updateControlsState();
}

void MainWindow::goToNextDay(){
    m_warehouseService->nextDay();
}

void MainWindow::runSimulationToEnd(){
    while (!m_warehouseService->isFinished()){
        m_warehouseService->nextDay();
    }
}

void MainWindow::refreshView(){
    if (!m_warehouseService){
        return;
    }

    m_dayLabel->setText(QString("День: %1 / %2").arg(m_warehouseService->currentDay()).arg(m_parameters.dayCount));

    m_stockTab->showProducts(m_warehouseService->stock());
    m_ordersTab->showOrders(m_warehouseService->todayOrders());
    m_shipmentsTab->showShipments(m_warehouseService->tomorrowShipments());
    m_supplierRequestsTab->showRequests(m_warehouseService->supplierRequests());

    updateControlsState();
}

void MainWindow::showDemoData(){
    //временные данные, потом удалишь
    Product rice;
    rice.id = 1;
    rice.name = "Рис";
    rice.packagesInStock = 30;
    rice.maxPackages = 60;
    rice.unitsPerPackage = 20;
    rice.unitName = "пачек";
    rice.pricePerPackage = 1500.0;
    rice.daysUntilExpiration = 2;
    rice.discountPercent = 10;

    Product buckwheat;
    buckwheat.id = 2;
    buckwheat.name = "Гречка";
    buckwheat.packagesInStock = 45;
    buckwheat.maxPackages = 60;
    buckwheat.unitsPerPackage = 12;
    buckwheat.unitName = "пачек";
    buckwheat.pricePerPackage = 1100.0;
    buckwheat.daysUntilExpiration = 14;

    m_stockTab->showProducts({rice, buckwheat});

    OrderItem riceItem;
    riceItem.productId = 1;
    riceItem.productName = "Рис";
    riceItem.requestedUnits = 100;
    riceItem.issuedPackages = 5;
    riceItem.issuedUnits = 100;
    riceItem.cost = 6750.0;

    OrderItem buckwheatItem;
    buckwheatItem.productId = 2;
    buckwheatItem.productName = "Гречка";
    buckwheatItem.requestedUnits = 50;
    buckwheatItem.issuedPackages = 4;
    buckwheatItem.issuedUnits = 48;
    buckwheatItem.cost = 4400.0;

    Order order;
    order.id = 1;
    order.shopNumber = 2;
    order.items = {riceItem, buckwheatItem};
    m_ordersTab->showOrders({order});

    Shipment shipment;
    shipment.id = 1;
    shipment.shopNumber = 2;
    shipment.items = order.items;
    shipment.totalCost = 11150.0;
    m_shipmentsTab->showShipments({shipment});

    SupplierRequest request;
    request.id = 2;
    request.productId = 1;
    request.productName = "Рис";
    request.packagesToOrder = 30;
    request.deliveryDays = 3;
    m_supplierRequestsTab->showRequests({request});
}
