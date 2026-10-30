#pragma once

#include "warehousetypes.h"

#include <QWidget>

class QLabel;
class QTableWidget;

//вкладка с потерями и списанными товарами
class WriteOffTab : public QWidget
{
    Q_OBJECT

public:
    explicit WriteOffTab(QWidget *parent = nullptr);
    void showWriteOffs(const QVector<WrittenOffItem> &items, const Statistics &statistics);

private:
    QTableWidget *m_table;
    QLabel *m_writtenOffPackagesLabel;
    QLabel *m_writeOffLossLabel;
    QLabel *m_discountLossLabel;
    QLabel *m_totalLossLabel;
};
