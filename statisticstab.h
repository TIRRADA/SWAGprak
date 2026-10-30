#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QLabel;

//вкладка с итогами
class StatisticsTab : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsTab(QWidget *parent = nullptr);

    void showStatistics(const Statistics &statistics, bool isSimulationFinished);

private:
    QLabel *m_statusLabel;
    QLabel *m_soldUnitsLabel;
    QLabel *m_soldRevenueLabel;
    QLabel *m_discountLossLabel;
    QLabel *m_writeOffLossLabel;
    QLabel *m_totalLossLabel;
    QLabel *m_writtenOffPackagesLabel;
};
