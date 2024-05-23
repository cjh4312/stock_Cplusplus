/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *ZHMarket;
    QAction *USMarket;
    QAction *HKMarket;
    QAction *USzMarket;
    QAction *newsReport;
    QAction *exit;
    QAction *fundFlow;
    QAction *actionF10;
    QAction *actionF3;
    QAction *DLStockInfo;
    QAction *DLAllStockK;
    QAction *pickStock;
    QAction *formula;
    QAction *setVacation;
    QAction *login;
    QAction *commonTrade;
    QAction *UKMarket;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QMenu *menu_5;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        ZHMarket = new QAction(MainWindow);
        ZHMarket->setObjectName("ZHMarket");
        ZHMarket->setCheckable(true);
        ZHMarket->setChecked(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/png/png/Zh.png"), QSize(), QIcon::Normal, QIcon::Off);
        ZHMarket->setIcon(icon);
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        ZHMarket->setFont(font);
        USMarket = new QAction(MainWindow);
        USMarket->setObjectName("USMarket");
        USMarket->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/png/png/Us.png"), QSize(), QIcon::Normal, QIcon::Off);
        USMarket->setIcon(icon1);
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font1.setPointSize(12);
        USMarket->setFont(font1);
        HKMarket = new QAction(MainWindow);
        HKMarket->setObjectName("HKMarket");
        HKMarket->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/png/png/Hk.png"), QSize(), QIcon::Normal, QIcon::Off);
        HKMarket->setIcon(icon2);
        HKMarket->setFont(font1);
        USzMarket = new QAction(MainWindow);
        USzMarket->setObjectName("USzMarket");
        USzMarket->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/png/png/Us_Zh.png"), QSize(), QIcon::Normal, QIcon::Off);
        USzMarket->setIcon(icon3);
        USzMarket->setFont(font1);
        newsReport = new QAction(MainWindow);
        newsReport->setObjectName("newsReport");
        newsReport->setCheckable(true);
        exit = new QAction(MainWindow);
        exit->setObjectName("exit");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/new/png/png/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        exit->setIcon(icon4);
        fundFlow = new QAction(MainWindow);
        fundFlow->setObjectName("fundFlow");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/new/png/png/fund.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        fundFlow->setIcon(icon5);
        actionF10 = new QAction(MainWindow);
        actionF10->setObjectName("actionF10");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/new/png/png/finance.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        actionF10->setIcon(icon6);
        actionF3 = new QAction(MainWindow);
        actionF3->setObjectName("actionF3");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/new/png/png/hots.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        actionF3->setIcon(icon7);
        DLStockInfo = new QAction(MainWindow);
        DLStockInfo->setObjectName("DLStockInfo");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/new/png/png/download_2.png"), QSize(), QIcon::Normal, QIcon::Off);
        DLStockInfo->setIcon(icon8);
        DLAllStockK = new QAction(MainWindow);
        DLAllStockK->setObjectName("DLAllStockK");
        DLAllStockK->setEnabled(true);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/new/png/png/Save.png"), QSize(), QIcon::Normal, QIcon::Off);
        DLAllStockK->setIcon(icon9);
        pickStock = new QAction(MainWindow);
        pickStock->setObjectName("pickStock");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/new/png/png/select.png"), QSize(), QIcon::Normal, QIcon::Off);
        pickStock->setIcon(icon10);
        formula = new QAction(MainWindow);
        formula->setObjectName("formula");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/new/png/png/help.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        formula->setIcon(icon11);
        setVacation = new QAction(MainWindow);
        setVacation->setObjectName("setVacation");
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/new/png/png/date.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        setVacation->setIcon(icon12);
        login = new QAction(MainWindow);
        login->setObjectName("login");
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/new/png/png/login.png"), QSize(), QIcon::Normal, QIcon::Off);
        login->setIcon(icon13);
        commonTrade = new QAction(MainWindow);
        commonTrade->setObjectName("commonTrade");
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/new/png/png/bs.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        commonTrade->setIcon(icon14);
        UKMarket = new QAction(MainWindow);
        UKMarket->setObjectName("UKMarket");
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/new/png/png/UK.png"), QSize(), QIcon::Normal, QIcon::Off);
        UKMarket->setIcon(icon15);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(2, 0, 2, 0);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu->setFont(font1);
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName("menu_2");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(12);
        menu_2->setFont(font2);
        menu_3 = new QMenu(menubar);
        menu_3->setObjectName("menu_3");
        menu_3->setFont(font1);
        menu_4 = new QMenu(menubar);
        menu_4->setObjectName("menu_4");
        menu_4->setFont(font1);
        menu_5 = new QMenu(menubar);
        menu_5->setObjectName("menu_5");
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        statusBar->setSizeGripEnabled(true);
        MainWindow->setStatusBar(statusBar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu_3->menuAction());
        menubar->addAction(menu_5->menuAction());
        menubar->addAction(menu_4->menuAction());
        menu->addAction(DLAllStockK);
        menu->addAction(DLStockInfo);
        menu->addAction(newsReport);
        menu->addAction(exit);
        menu_2->addAction(ZHMarket);
        menu_2->addAction(USMarket);
        menu_2->addAction(HKMarket);
        menu_2->addAction(USzMarket);
        menu_2->addAction(UKMarket);
        menu_3->addAction(fundFlow);
        menu_3->addAction(pickStock);
        menu_4->addAction(actionF10);
        menu_4->addAction(actionF3);
        menu_4->addAction(formula);
        menu_4->addAction(setVacation);
        menu_5->addAction(login);
        menu_5->addAction(commonTrade);

        retranslateUi(MainWindow);
        QObject::connect(exit, &QAction::triggered, MainWindow, qOverload<>(&QMainWindow::close));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ZHMarket->setText(QCoreApplication::translate("MainWindow", "A\350\202\241", nullptr));
        USMarket->setText(QCoreApplication::translate("MainWindow", "\347\276\216\350\202\241", nullptr));
        HKMarket->setText(QCoreApplication::translate("MainWindow", "\346\270\257\350\202\241", nullptr));
        USzMarket->setText(QCoreApplication::translate("MainWindow", "\347\276\216\344\270\255\346\246\202\350\202\241", nullptr));
        newsReport->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255\350\257\255\351\237\263\346\222\255\346\212\245", nullptr));
        exit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        fundFlow->setText(QCoreApplication::translate("MainWindow", "\350\265\204\351\207\221\346\265\201", nullptr));
        actionF10->setText(QCoreApplication::translate("MainWindow", "F10--\350\264\242\345\212\241\347\273\217\350\220\245\345\210\206\346\236\220", nullptr));
        actionF3->setText(QCoreApplication::translate("MainWindow", "F3--\347\203\255\345\272\246\345\205\263\351\224\256\350\257\215", nullptr));
        DLStockInfo->setText(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275\346\214\207\346\225\260\346\235\277\345\235\227\344\270\252\350\202\241\344\277\241\346\201\257", nullptr));
        DLAllStockK->setText(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275\346\211\200\346\234\211\350\202\241\347\245\250k\347\272\277", nullptr));
        pickStock->setText(QCoreApplication::translate("MainWindow", "\345\205\254\345\274\217\351\200\211\350\202\241", nullptr));
        formula->setText(QCoreApplication::translate("MainWindow", "\345\205\254\345\274\217\347\274\226\345\206\231\350\257\264\346\230\216", nullptr));
        setVacation->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\350\202\241\345\270\202\345\201\207\346\234\237", nullptr));
        login->setText(QCoreApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        commonTrade->setText(QCoreApplication::translate("MainWindow", "\346\231\256\351\200\232\344\272\244\346\230\223", nullptr));
        UKMarket->setText(QCoreApplication::translate("MainWindow", "\350\213\261\350\202\241", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\345\205\266\345\256\203\345\270\202\345\234\272", nullptr));
        menu_3->setTitle(QCoreApplication::translate("MainWindow", "\345\210\206\346\236\220", nullptr));
        menu_4->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
        menu_5->setTitle(QCoreApplication::translate("MainWindow", "\344\272\244\346\230\223", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
