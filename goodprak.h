#ifndef GOODPRAK_H
#define GOODPRAK_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class GoodPrak;
}
QT_END_NAMESPACE

class GoodPrak : public QMainWindow
{
    Q_OBJECT

public:
    GoodPrak(QWidget *parent = nullptr);
    ~GoodPrak();

private:
    Ui::GoodPrak *ui;
};
#endif // GOODPRAK_H
