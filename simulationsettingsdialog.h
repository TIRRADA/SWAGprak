#pragma once

#include "warehousetypes.h"

#include <QDialog>

class QSpinBox;
class QTableWidget;

// Окно, в котором пользователь задаёт параметры моделирования.
class SimulationSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationSettingsDialog(const SimulationParameters &initialParameters, QWidget *parent = nullptr);
    SimulationParameters parameters() const;
    void accept() override;

private:
    void resizeProductsTable(int rowCount);
    bool validateProducts();

    QSpinBox *m_dayCountSpinBox;
    QSpinBox *m_shopCountSpinBox;
    QSpinBox *m_productKindCountSpinBox;
    QSpinBox *m_minItemsPerOrderSpinBox;
    QSpinBox *m_maxItemsPerOrderSpinBox;
    QSpinBox *m_minUnitsPerItemSpinBox;
    QSpinBox *m_maxUnitsPerItemSpinBox;
    QSpinBox *m_minDeliveryDaysSpinBox;
    QSpinBox *m_maxDeliveryDaysSpinBox;

    QTableWidget *m_productsTable;
};
