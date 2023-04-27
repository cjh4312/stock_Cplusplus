
#ifndef SEARCHSTOCK_H
#define SEARCHSTOCK_H

#include <QLineEdit>
#include <QTextEdit>
#include <QFile>
#include <QTextCodec>
#include <QKeyEvent>
#include <QTextBlock>

class SearchStock : public QLineEdit
{
    Q_OBJECT
public:
    SearchStock();
    QLineEdit *searchCodeLine;
    QTextEdit *matchCodeText;
    void findStockArea();

signals:
    void showSearch();
private:
    void matchInput();
    void searchFinished();
    void compare(QStringList Data,QString s,int col);

};

#endif // SEARCHSTOCK_H
