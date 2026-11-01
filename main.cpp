#include "mainwindow.h"
#include "warehouse_simulation.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
#endif

    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    WarehouseSimulator simulator;
    MainWindow mainWindow;
    mainWindow.setWarehouseService(&simulator);
    mainWindow.show();
    return a.exec();
}
