#include "writeofftab.h"

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

WriteOffTab::WriteOffTab(QWidget *parent) : QWidget(parent){
    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Товар", "Списано упаковок", "Потери от списания"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_writtenOffPackagesLabel = new QLabel(this);
    m_writeOffLossLabel = new QLabel(this);
    m_discountLossLabel = new QLabel(this);
    m_totalLossLabel = new QLabel(this);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Просроченные товары, вывезенные со склада:", this));
    layout->addWidget(m_table);
    layout->addWidget(m_writtenOffPackagesLabel);
    layout->addWidget(m_writeOffLossLabel);
    layout->addWidget(m_discountLossLabel);
    layout->addWidget(m_totalLossLabel);
}

void WriteOffTab::showWriteOffs(const QVector<WrittenOffItem> &items, const Statistics &statistics){
    m_table->setRowCount(items.size());
    for (int row = 0; row < items.size(); ++row) {
        const WrittenOffItem &item = items.at(row);
        m_table->setItem(row, 0, new QTableWidgetItem(item.productName));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(item.packages)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::number(item.loss, 'f', 2)));
    }

    const double totalLoss = statistics.writeOffLoss + statistics.discountLoss;
    m_writtenOffPackagesLabel->setText(QString("Всего списано упаковок: %1").arg(statistics.writtenOffPackages));
    m_writeOffLossLabel->setText(QString("Потери от списания: %1").arg(statistics.writeOffLoss, 0, 'f', 2));
    m_discountLossLabel->setText(QString("Потери от уценки: %1").arg(statistics.discountLoss, 0, 'f', 2));
    m_totalLossLabel->setText(QString("Общие потери: %1").arg(totalLoss, 0, 'f', 2));
}
