#include "statisticstab.h"

#include <QFont>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {

QLabel *createValueLabel(QWidget *parent){
    auto *label = new QLabel("—", parent);
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);
    return label;
}

}

StatisticsTab::StatisticsTab(QWidget *parent) : QWidget(parent){
    m_statusLabel = new QLabel("Моделирование не запущено", this);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(statusFont.pointSize() + 2);
    m_statusLabel->setFont(statusFont);

    m_soldUnitsLabel = createValueLabel(this);
    m_soldRevenueLabel = createValueLabel(this);
    m_discountLossLabel = createValueLabel(this);
    m_writeOffLossLabel = createValueLabel(this);
    m_totalLossLabel = createValueLabel(this);
    m_writtenOffPackagesLabel = createValueLabel(this);

    auto *formLayout = new QFormLayout;
    formLayout->addRow("Продано (единиц товара):", m_soldUnitsLabel);
    formLayout->addRow("Выручка от продаж:", m_soldRevenueLabel);
    formLayout->addRow("Потери от уценки:", m_discountLossLabel);
    formLayout->addRow("Потери от списания:", m_writeOffLossLabel);
    formLayout->addRow("Общие потери:", m_totalLossLabel);
    formLayout->addRow("Списано упаковок:", m_writtenOffPackagesLabel);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_statusLabel);
    layout->addLayout(formLayout);
    layout->addStretch();
}

void StatisticsTab::showStatistics(const Statistics &statistics, bool isSimulationFinished){
    m_statusLabel->setText(isSimulationFinished ? "Моделирование завершено. Итоги за весь период:" : "Моделирование идёт. Промежуточные итоги:");

    const double totalLoss = statistics.writeOffLoss + statistics.discountLoss;

    m_soldUnitsLabel->setText(QString::number(statistics.soldUnits));
    m_soldRevenueLabel->setText(QString::number(statistics.soldRevenue, 'f', 2));
    m_discountLossLabel->setText(QString::number(statistics.discountLoss, 'f', 2));
    m_writeOffLossLabel->setText(QString::number(statistics.writeOffLoss, 'f', 2));
    m_totalLossLabel->setText(QString::number(totalLoss, 'f', 2));
    m_writtenOffPackagesLabel->setText(QString::number(statistics.writtenOffPackages));
}
