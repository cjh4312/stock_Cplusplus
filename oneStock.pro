QT       += core gui network texttospeech qml
RC_FILE += app.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commondelegate.cpp \
    drawchart.cpp \
    f10view.cpp \
    fundflow.cpp \
    getf10info.cpp \
    globalvar.cpp \
    main.cpp \
    mainwindow.cpp \
    modeltablestock.cpp \
    modeltimeshare.cpp \
    requeststocsv.cpp \
    searchstock.cpp \
    tablestock.cpp \
    threadcandlechart.cpp \
    threadindex.cpp \
    threadnewsreport.cpp \
    threadtable.cpp \
    threadtimesharechart.cpp \
    threadtimesharetick.cpp

HEADERS += \
    commondelegate.h \
    drawchart.h \
    f10view.h \
    fundflow.h \
    getf10info.h \
    globalvar.h \
    mainwindow.h \
    modeltablestock.h \
    modeltimeshare.h \
    requeststocsv.h \
    searchstock.h \
    stockinfo.h \
    tablestock.h \
    threadcandlechart.h \
    threadindex.h \
    threadnewsreport.h \
    threadtable.h \
    threadtimesharechart.h \
    threadtimesharetick.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
