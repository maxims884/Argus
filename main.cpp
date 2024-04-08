#include "mainwindow.h"

#include <QApplication>

#include <logger/logmanager.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogManager().initDefault();
    MainWindow w;
    return a.exec();
}
