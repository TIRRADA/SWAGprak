#pragma once

#include "warehousetypes.h"

#include <QMainWindow>

class QLabel;
class QPushButton;
class QTabWidget;
class StockTab;
class WarehouseService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Подключает бэкенд склада (его пишет напарник).
    void setWarehouseService(WarehouseService *warehouseService);

private slots:
    void openSettingsDialog();
    void goToNextDay();
    void runSimulationToEnd();
    void refreshView();

private:
    void createInterface();
    void updateControlsState();
    void showDemoData(); // ВРЕМЕННО: пока нет бэка, чтобы увидеть таблицу

    WarehouseService *m_warehouseService = nullptr;
    SimulationParameters m_parameters;
    bool m_isSimulationStarted = false;

    QLabel *m_dayLabel = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_nextDayButton = nullptr;
    QPushButton *m_runToEndButton = nullptr;
    QTabWidget *m_tabWidget = nullptr;
    StockTab *m_stockTab = nullptr;
};
