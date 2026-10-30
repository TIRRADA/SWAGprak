#pragma once

#include "warehousetypes.h"

#include <QMainWindow>

class OrdersTab;
class QLabel;
class QPushButton;
class QTabWidget;
class ShipmentsTab;
class StockTab;
class SupplierRequestsTab;
class WarehouseService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    //здесь бэк подключай
    void setWarehouseService(WarehouseService *warehouseService);

private slots:
    void openSettingsDialog();
    void goToNextDay();
    void runSimulationToEnd();
    void refreshView();

private:
    void createInterface();
    void updateControlsState();
    void showDemoData(); //удали потом, это я чисто для себя смотрел, пока нет бэка

    WarehouseService *m_warehouseService = nullptr;
    SimulationParameters m_parameters;
    bool m_isSimulationStarted = false;

    QLabel *m_dayLabel = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_nextDayButton = nullptr;
    QPushButton *m_runToEndButton = nullptr;
    QTabWidget *m_tabWidget = nullptr;
    StockTab *m_stockTab = nullptr;
    OrdersTab *m_ordersTab = nullptr;
    ShipmentsTab *m_shipmentsTab = nullptr;
    SupplierRequestsTab *m_supplierRequestsTab = nullptr;
};
