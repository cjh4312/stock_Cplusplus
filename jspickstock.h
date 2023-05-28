#ifndef JSPICKSTOCK_H
#define JSPICKSTOCK_H

#include <QDialog>
#include <QObject>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QJSEngine>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>

class JSPickStock : public QDialog
{
    Q_OBJECT
public:
    Q_INVOKABLE JSPickStock(QDialog *parent = nullptr);
    ~JSPickStock();
    Q_INVOKABLE float P();
    Q_INVOKABLE float P(int day);
    Q_INVOKABLE float O();
    Q_INVOKABLE float O(int day);
    Q_INVOKABLE float H();
    Q_INVOKABLE float H(int day);
    Q_INVOKABLE float H(int startDay,int endDay);
    Q_INVOKABLE float L();
    Q_INVOKABLE float L(int day);
    Q_INVOKABLE float L(int startDay,int endDay);
    Q_INVOKABLE float C();
    Q_INVOKABLE float C(int day);
    Q_INVOKABLE float T();
    Q_INVOKABLE float T(int day);
    Q_INVOKABLE float T(int startDay,int endDay);
    Q_INVOKABLE float A(int startDay,int endDay);
    Q_INVOKABLE float M();
    Q_INVOKABLE float M(int day);
    Q_INVOKABLE float M(int startDay,int endDay);
    Q_INVOKABLE float V();

    void initPickStockWindow();
    void PickStockInterface();
    void initWindow();
    float getData(int day,int column);
    QString replaceFormula(QString s);
    bool isRunning=false;

signals:
    void updateTableList();

private slots:
    void onButtonClicked(QAbstractButton *button);
//    void saveFormula();
};

#endif // JSPICKSTOCK_H
