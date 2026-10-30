#include "stocktab.h"

#include <QColor>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
constexpr int kExpirationWarningDays = 3;
}

StockTab::StockTab(QWidget *parent) : QWidget(parent){
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"Товар", "Упаковок (есть / макс.)", "В упаковке", "Цена упаковки", "Дней до конца срока", "Уценка"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_table);
}

void StockTab::showProducts(const QVector<Product> &products){
    m_table->setRowCount(products.size());

    for (int row = 0; row < products.size(); ++row) {
        const Product &product = products.at(row);
        const bool isExpiringSoon = product.daysUntilExpiration <= kExpirationWarningDays;
        setCellText(row, 0, product.name, isExpiringSoon);
        setCellText(row, 1, QString("%1 / %2").arg(product.packagesInStock).arg(product.maxPackages), isExpiringSoon);
        setCellText(row, 2, QString("%1 %2").arg(product.unitsPerPackage).arg(product.unitName), isExpiringSoon);
        setCellText(row, 3, QString::number(product.pricePerPackage, 'f', 2), isExpiringSoon);
        setCellText(row, 4, QString::number(product.daysUntilExpiration), isExpiringSoon);
        setCellText(row, 5, QString("%1%").arg(product.discountPercent), isExpiringSoon);
    }
}

void StockTab::setCellText(int row, int column, const QString &text, bool isHighlighted){
    auto *item = new QTableWidgetItem(text);
    if (isHighlighted) {
        item->setBackground(QColor(255, 224, 178));
        item->setForeground(QColor(0, 0, 0));
    }
    m_table->setItem(row, column, item);
}
