#include "simulationsettingsdialog.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSet>
#include <QSpinBox>
#include <QStringList>
#include <QTableWidget>
#include <QVBoxLayout>

enum ProductColumn {
    NameColumn,
    PackagesInStockColumn,
    MaxPackagesColumn,
    UnitsPerPackageColumn,
    UnitNameColumn,
    PricePerPackageColumn,
    DaysUntilExpirationColumn,
    ProductColumnCount
};

namespace {

QSpinBox *createSpinBox(int minValue, int maxValue, int value, QWidget *parent){
    auto *spinBox = new QSpinBox(parent);
    spinBox->setRange(minValue, maxValue);
    spinBox->setValue(value);
    return spinBox;
}

Product createDefaultProduct(int index){
    static const QStringList defaultNames = {
        "Рис", "Гречка", "Сахар", "Мука", "Макароны", "Соль", "Масло",
        "Чай", "Кофе", "Тушенка", "Горох", "Хлопья", "Сгущённое молоко",
        "Печенье", "Манная крупа", "Шпроты", "Мед", "Джем", "Какао", "Кетчуп"
    };

    Product product;
    product.name = index < defaultNames.size() ? defaultNames.at(index)
                                               : QString("Товар %1").arg(index + 1);
    product.packagesInStock = 30 + (index % 5) * 5;
    product.maxPackages = 60;
    product.unitsPerPackage = 10 + (index % 4) * 5;
    product.unitName = "пачек";
    product.pricePerPackage = 800.0 + index * 150.0;
    product.daysUntilExpiration = 5 + (index * 3) % 16;
    return product;
}

void setProductRow(QTableWidget *table, int row, const Product &product){
    table->setItem(row, NameColumn, new QTableWidgetItem(product.name));
    table->setItem(row, UnitNameColumn, new QTableWidgetItem(product.unitName));

    table->setCellWidget(row, PackagesInStockColumn, createSpinBox(0, 100000, product.packagesInStock, table));
    table->setCellWidget(row, MaxPackagesColumn, createSpinBox(1, 100000, product.maxPackages, table));
    table->setCellWidget(row, UnitsPerPackageColumn, createSpinBox(1, 100000, product.unitsPerPackage, table));
    table->setCellWidget(row, DaysUntilExpirationColumn, createSpinBox(1, 365, product.daysUntilExpiration, table));

    auto *priceSpinBox = new QDoubleSpinBox(table);
    priceSpinBox->setRange(0.01, 10000000.0);
    priceSpinBox->setDecimals(2);
    priceSpinBox->setValue(product.pricePerPackage);
    table->setCellWidget(row, PricePerPackageColumn, priceSpinBox);
}

int intValueAt(const QTableWidget *table, int row, int column){
    return qobject_cast<QSpinBox *>(table->cellWidget(row, column))->value();
}

Product readProductRow(const QTableWidget *table, int row){
    Product product;
    product.id = row + 1;
    product.name = table->item(row, NameColumn)->text().trimmed();
    product.unitName = table->item(row, UnitNameColumn)->text().trimmed();
    product.packagesInStock = intValueAt(table, row, PackagesInStockColumn);
    product.maxPackages = intValueAt(table, row, MaxPackagesColumn);
    product.unitsPerPackage = intValueAt(table, row, UnitsPerPackageColumn);
    product.daysUntilExpiration = intValueAt(table, row, DaysUntilExpirationColumn);
    product.pricePerPackage = qobject_cast<QDoubleSpinBox *>(table->cellWidget(row, PricePerPackageColumn))->value();
    return product;
}

}   //namespace

SimulationSettingsDialog::SimulationSettingsDialog(const SimulationParameters &initialParameters,QWidget *parent) : QDialog(parent){
    setWindowTitle("Параметры моделирования");
    resize(950, 750);

    m_dayCountSpinBox = createSpinBox(10, 30, initialParameters.dayCount, this);
    m_shopCountSpinBox = createSpinBox(3, 9, initialParameters.shopCount, this);
    m_productKindCountSpinBox = createSpinBox(12, 20, initialParameters.productKindCount, this);
    m_minItemsPerOrderSpinBox = createSpinBox(1, 20, initialParameters.minItemsPerOrder, this);
    m_maxItemsPerOrderSpinBox = createSpinBox(1, 20, initialParameters.maxItemsPerOrder, this);
    m_minUnitsPerItemSpinBox = createSpinBox(1, 10000, initialParameters.minUnitsPerItem, this);
    m_maxUnitsPerItemSpinBox = createSpinBox(1, 10000, initialParameters.maxUnitsPerItem, this);
    m_minDeliveryDaysSpinBox = createSpinBox(1, 5, initialParameters.minDeliveryDays, this);
    m_maxDeliveryDaysSpinBox = createSpinBox(1, 5, initialParameters.maxDeliveryDays, this);

    auto *formLayout = new QFormLayout;
    formLayout->addRow("Дней моделирования (N):", m_dayCountSpinBox);
    formLayout->addRow("Торговых точек (M):", m_shopCountSpinBox);
    formLayout->addRow("Видов товаров (K):", m_productKindCountSpinBox);
    formLayout->addRow("Позиций в заказе, от:", m_minItemsPerOrderSpinBox);
    formLayout->addRow("Позиций в заказе, до:", m_maxItemsPerOrderSpinBox);
    formLayout->addRow("Количество в позиции, от:", m_minUnitsPerItemSpinBox);
    formLayout->addRow("Количество в позиции, до:", m_maxUnitsPerItemSpinBox);
    formLayout->addRow("Срок поставки (дней), от:", m_minDeliveryDaysSpinBox);
    formLayout->addRow("Срок поставки (дней), до:", m_maxDeliveryDaysSpinBox);

    auto *settingsGroup = new QGroupBox("Общие параметры", this);
    settingsGroup->setLayout(formLayout);

    m_productsTable = new QTableWidget(this);
    m_productsTable->setColumnCount(ProductColumnCount);
    m_productsTable->setHorizontalHeaderLabels({"Название", "Упаковок на складе", "Макс. упаковок", "Единиц в упаковке", "Единица (кг, пачек…)", "Цена упаковки", "Дней до конца срока"});
    m_productsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    const int productCount = m_productKindCountSpinBox->value();
    m_productsTable->setRowCount(productCount);
    for (int row = 0; row < productCount; ++row) {
        const bool hasSavedProduct = row < initialParameters.initialProducts.size();
        setProductRow(m_productsTable, row, hasSavedProduct ? initialParameters.initialProducts.at(row) : createDefaultProduct(row));
    }

    connect(m_productKindCountSpinBox, &QSpinBox::valueChanged, this, [this](int rowCount) { resizeProductsTable(rowCount); });

    auto *productsLayout = new QVBoxLayout;
    productsLayout->addWidget(m_productsTable);

    auto *productsGroup = new QGroupBox("Начальный набор товаров на складе (строк столько же, сколько K)", this);
    productsGroup->setLayout(productsLayout);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SimulationSettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SimulationSettingsDialog::reject);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(productsGroup, 1);
    mainLayout->addWidget(buttonBox);
}


SimulationParameters SimulationSettingsDialog::parameters() const{
    SimulationParameters parameters;
    parameters.dayCount = m_dayCountSpinBox->value();
    parameters.shopCount = m_shopCountSpinBox->value();
    parameters.productKindCount = m_productKindCountSpinBox->value();
    parameters.minItemsPerOrder = m_minItemsPerOrderSpinBox->value();
    parameters.maxItemsPerOrder = m_maxItemsPerOrderSpinBox->value();
    parameters.minUnitsPerItem = m_minUnitsPerItemSpinBox->value();
    parameters.maxUnitsPerItem = m_maxUnitsPerItemSpinBox->value();
    parameters.minDeliveryDays = m_minDeliveryDaysSpinBox->value();
    parameters.maxDeliveryDays = m_maxDeliveryDaysSpinBox->value();

    for (int row = 0; row < m_productsTable->rowCount(); ++row){
        parameters.initialProducts.append(readProductRow(m_productsTable, row));
    }

    return parameters;
}

bool SimulationSettingsDialog::validateProducts()
{
    QSet<QString> usedNames;

    for (int row = 0; row < m_productsTable->rowCount(); ++row) {
        const Product product = readProductRow(m_productsTable, row);
        QString problem;

        if (product.name.isEmpty()){
            problem = "не заполнено название товара";
        }
        else if (usedNames.contains(product.name)){
            problem = QString("название «%1» уже встречалось выше").arg(product.name);
        }
        else if (product.unitName.isEmpty()){
            problem = "не заполнена единица измерения";
        }
        else if (product.packagesInStock > product.maxPackages){
            problem = "упаковок на складе больше, чем вместимость склада";
        }

        if (!problem.isEmpty()) {
            m_productsTable->selectRow(row);
            QMessageBox::warning(this, "Неверный начальный набор", QString("Строка %1: %2.").arg(row + 1).arg(problem));
            return false;
        }
        usedNames.insert(product.name);
    }
    return true;
}

void SimulationSettingsDialog::accept()
{
    const bool areRangesValid =
        m_minItemsPerOrderSpinBox->value() <= m_maxItemsPerOrderSpinBox->value()
        && m_minUnitsPerItemSpinBox->value() <= m_maxUnitsPerItemSpinBox->value()
        && m_minDeliveryDaysSpinBox->value() <= m_maxDeliveryDaysSpinBox->value();

    if (!areRangesValid) {
        QMessageBox::warning(this, "Неверные параметры", "В диапазонах значение «от» не может быть больше значения «до».");
        return;
    }

    if (!validateProducts()){
        return;
    }

    QDialog::accept();
}

void SimulationSettingsDialog::resizeProductsTable(int rowCount){
    const int oldRowCount = m_productsTable->rowCount();
    m_productsTable->setRowCount(rowCount);

    for (int row = oldRowCount; row < rowCount; ++row){
        setProductRow(m_productsTable, row, createDefaultProduct(row));
    }
}
