#include "discounttab.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
constexpr int kDefaultDaysThreshold = 5;
constexpr int kMaxDiscountPercent = 90;
}

DiscountTab::DiscountTab(QWidget *parent) : QWidget(parent){
    m_daysThresholdSpinBox = new QSpinBox(this);
    m_daysThresholdSpinBox->setRange(1, 30);
    m_daysThresholdSpinBox->setValue(kDefaultDaysThreshold);

    auto *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Показывать товары, у которых осталось не больше (дней):", this));
    filterLayout->addWidget(m_daysThresholdSpinBox);
    filterLayout->addStretch();

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"Товар", "Упаковок на складе", "Дней до конца срока", "Цена упаковки", "Уценка", "Цена с уценкой"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_discountSpinBox = new QSpinBox(this);
    m_discountSpinBox->setRange(0, kMaxDiscountPercent);
    m_discountSpinBox->setSuffix(" %");
    m_applyButton = new QPushButton("Применить уценку", this);

    auto *editorLayout = new QHBoxLayout;
    editorLayout->addWidget(new QLabel("Процент уценки для выбранного товара:", this));
    editorLayout->addWidget(m_discountSpinBox);
    editorLayout->addWidget(m_applyButton);
    editorLayout->addStretch();

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(filterLayout);
    layout->addWidget(m_table);
    layout->addLayout(editorLayout);

    connect(m_daysThresholdSpinBox, &QSpinBox::valueChanged, this, [this](int days) {
        rebuildTable();
        emit daysThresholdChanged(days);
    });
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() { updateEditor(); });
    connect(m_applyButton, &QPushButton::clicked, this, [this]() {
        const int index = selectedRowIndex();
        if (index >= 0){
            emit discountRequested(m_shownProducts.at(index).id, m_discountSpinBox->value());
        }
    });
    updateEditor();
}

void DiscountTab::showProducts(const QVector<Product> &products){
    m_allProducts = products;
    rebuildTable();
}

void DiscountTab::rebuildTable(){
    int previouslySelectedId = -1;
    const int previousIndex = selectedRowIndex();
    if (previousIndex >= 0 && previousIndex < m_shownProducts.size())
        previouslySelectedId = m_shownProducts.at(previousIndex).id;

    m_shownProducts.clear();
    for (const Product &product : m_allProducts) {
        if (product.daysUntilExpiration <= m_daysThresholdSpinBox->value()){
            m_shownProducts.append(product);
        }
    }

    {
        QSignalBlocker blocker(m_table);
        m_table->setRowCount(m_shownProducts.size());

        for (int row = 0; row < m_shownProducts.size(); ++row) {
            const Product &product = m_shownProducts.at(row);
            const double discountedPrice = product.pricePerPackage * (100 - product.discountPercent) / 100.0;

            m_table->setItem(row, 0, new QTableWidgetItem(product.name));
            m_table->setItem(row, 1, new QTableWidgetItem(QString::number(product.packagesInStock)));
            m_table->setItem(row, 2, new QTableWidgetItem(QString::number(product.daysUntilExpiration)));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(product.pricePerPackage, 'f', 2)));
            m_table->setItem(row, 4, new QTableWidgetItem(QString("%1%").arg(product.discountPercent)));
            m_table->setItem(row, 5, new QTableWidgetItem(QString::number(discountedPrice, 'f', 2)));

            if (product.id == previouslySelectedId){
                m_table->selectRow(row);
            }
        }
    }

    updateEditor();
}

int DiscountTab::selectedRowIndex() const{
    const QModelIndexList selectedRows = m_table->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()){
        return -1;
    }
    return selectedRows.first().row();
}

void DiscountTab::updateEditor(){
    const int index = selectedRowIndex();
    const bool hasSelection = index >= 0 && index < m_shownProducts.size();

    m_discountSpinBox->setEnabled(hasSelection);
    m_applyButton->setEnabled(hasSelection);

    if (hasSelection){
        m_discountSpinBox->setValue(m_shownProducts.at(index).discountPercent);
    }
}

int DiscountTab::daysThreshold() const
{
    return m_daysThresholdSpinBox->value();
}
