#include "supplierrequeststab.h"

#include <QColor>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QVBoxLayout>

SupplierRequestsTab::SupplierRequestsTab(QWidget *parent) : QWidget(parent){
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Заявка №", "Товар", "Заказать упаковок", "Срок поставки (дней)", "Статус"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_approveButton = new QPushButton("Утвердить выбранную заявку", this);
    m_approveButton->setEnabled(false);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_approveButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Заявки фирмам-поставщикам:", this));
    layout->addWidget(m_table);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {updateApproveButton();});

    connect(m_approveButton, &QPushButton::clicked, this, [this]() {const int index = selectedRequestIndex();if (index >= 0)emit approveRequested(m_requests.at(index).id);});
}

void SupplierRequestsTab::showRequests(const QVector<SupplierRequest> &requests){
    int previouslySelectedId = -1;
    const int previousIndex = selectedRequestIndex();
    if (previousIndex >= 0){
        previouslySelectedId = m_requests.at(previousIndex).id;
    }

    m_requests = requests;

    {
        QSignalBlocker blocker(m_table);
        m_table->setRowCount(m_requests.size());

        for (int row = 0; row < m_requests.size(); ++row) {
            const SupplierRequest &request = m_requests.at(row);
            const QString statusText = request.isApproved ? "Утверждена" : "Ожидает утверждения";

            QVector<QTableWidgetItem *> rowItems = {
                new QTableWidgetItem(QString::number(request.id)),
                new QTableWidgetItem(request.productName),
                new QTableWidgetItem(QString::number(request.packagesToOrder)),
                new QTableWidgetItem(QString::number(request.deliveryDays)),
                new QTableWidgetItem(statusText)
            };

            for (int column = 0; column < rowItems.size(); ++column) {
                if (request.isApproved) {
                    rowItems[column]->setBackground(QColor(200, 230, 201));
                    rowItems[column]->setForeground(QColor(0, 0, 0));
                }
                m_table->setItem(row, column, rowItems.at(column));
            }
            if (request.id == previouslySelectedId){
                m_table->selectRow(row);
            }
        }
    }
    updateApproveButton();
}

int SupplierRequestsTab::selectedRequestIndex() const{
    const QModelIndexList selectedRows = m_table->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()){
        return -1;
    }
    return selectedRows.first().row();
}

void SupplierRequestsTab::updateApproveButton(){
    const int index = selectedRequestIndex();
    const bool canApprove = index >= 0 && index < m_requests.size() && !m_requests.at(index).isApproved;
    m_approveButton->setEnabled(canApprove);
}
