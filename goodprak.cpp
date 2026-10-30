#include "goodprak.h"
#include "ui_goodprak.h"

GoodPrak::GoodPrak(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GoodPrak)
{
    ui->setupUi(this);
}

GoodPrak::~GoodPrak()
{
    delete ui;
}
