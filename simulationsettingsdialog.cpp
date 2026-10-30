#include "simulationsettingsdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {

QSpinBox *createSpinBox(int minValue, int maxValue, int value, QWidget *parent){
    auto *spinBox = new QSpinBox(parent);
    spinBox->setRange(minValue, maxValue);
    spinBox->setValue(value);
    return spinBox;
}

}

SimulationSettingsDialog::SimulationSettingsDialog(const SimulationParameters &initialParameters, QWidget *parent) : QDialog(parent){
    setWindowTitle("Параметры моделирования");

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

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SimulationSettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SimulationSettingsDialog::reject);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
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
    return parameters;
}

void SimulationSettingsDialog::accept(){
    const bool areRangesValid = m_minItemsPerOrderSpinBox->value() <= m_maxItemsPerOrderSpinBox->value() && m_minUnitsPerItemSpinBox->value() <= m_maxUnitsPerItemSpinBox->value() && m_minDeliveryDaysSpinBox->value() <= m_maxDeliveryDaysSpinBox->value();

    if (!areRangesValid) {
        QMessageBox::warning(this, "Неверные параметры", "В диапазонах значение «от» не может быть больше значения «до».");
        return;
    }

    QDialog::accept();
}
