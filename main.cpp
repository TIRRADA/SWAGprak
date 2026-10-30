#include "goodprak.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GoodPrak w;
    w.show();
    return a.exec();
}
