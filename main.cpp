#include "mainwindow.h"
#include "serial.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    serial::get().initialize();
    MainWindow w;
    serial::get().getMainWindow(&w);
    w.show();
    return a.exec();
}
