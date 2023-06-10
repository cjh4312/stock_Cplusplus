#include "mainwindow.h"
#include <DbgHelp.h>
#include <QMessageBox>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.installEventFilter(&w);
    w.showMaximized();

    return a.exec();
}
