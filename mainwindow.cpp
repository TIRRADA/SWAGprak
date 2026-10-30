#include "mainwindow.h"

#include "simulationsettingsdialog.h"
#include "stocktab.h"
#include "warehouseservice.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent){
    setWindowTitle("Система управления оптовым складом");
    resize(1000, 650);

    createInterface();
    updateControlsState();
    showDemoData();
}

void MainWindow::setWarehouseService(WarehouseService *warehouseService){
    m_warehouseService = warehouseService;
    if (m_warehouseService) {
        connect(m_warehouseService, &WarehouseService::stateChanged,
                this, &MainWindow::refreshView);
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
    m_tabWidget->addTab(m_stockTab, "Склад");

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
    updateControlsState();
}

void MainWindow::showDemoData(){
    //временные данные
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
}
