#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QPushButton;
class QTableWidget;

//вкладка с заявками
class SupplierRequestsTab : public QWidget{
    Q_OBJECT

public:
    explicit SupplierRequestsTab(QWidget *parent = nullptr);
    void showRequests(const QVector<SupplierRequest> &requests);

signals:
    void approveRequested(int requestId);

private:
    int selectedRequestIndex() const;
    void updateApproveButton();

    QVector<SupplierRequest> m_requests;
    QTableWidget *m_table;
    QPushButton *m_approveButton;
};
