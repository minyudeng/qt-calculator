#include "mainwindow.h"
#include "myapplication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
