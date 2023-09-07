#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    static QSharedMemory *shareMem = new QSharedMemory( "oneStock");
    if ( !shareMem->create( 1 ) )
    {
        qApp->quit();
        return -1;
    }

    MainWindow w;
    a.installEventFilter(&w);
    w.showMaximized();

    return a.exec();
}
