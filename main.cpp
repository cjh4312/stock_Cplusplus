#include "mainwindow.h"
//#include <Windows.h>
//#include <DbgHelp.h>
//#include <QMessageBox>
#include <QApplication>
#include <QSharedMemory>

//LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
//{
//    HANDLE hDumpFile = CreateFile(L"crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//    if(hDumpFile != INVALID_HANDLE_VALUE)
//    {
//        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
//        dumpInfo.ExceptionPointers = pException;
//        dumpInfo.ThreadId = GetCurrentThreadId();
//        dumpInfo.ClientPointers = TRUE;
//        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
//    }
//    QMessageBox msgBox;
//    msgBox.setText("application crash!");
//    msgBox.exec();

//    return EXCEPTION_EXECUTE_HANDLER;
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    static QSharedMemory *shareMem = new QSharedMemory( "oneStock",&a);
    if ( !shareMem->create( 1 ) )
    {
        qApp->quit();
        return -1;
    }
//    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
//    setvbuf(stdout, NULL, _IONBF, 0);
//    setvbuf(stderr, NULL, _IONBF, 0);

    MainWindow w;
    a.installEventFilter(&w);
    w.showMaximized();
    return a.exec();
}
