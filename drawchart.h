
#ifndef DRAWCHART_H
#define DRAWCHART_H

#include "qdialog.h"
#include "qlabel.h"
#include "qtextbrowser.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QListView>
#include <QStringListModel>
#include "globalvar.h"
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qtextcodec.h"


class DrawChart : public QWidget
{
    Q_OBJECT
public:
    explicit DrawChart(QWidget *parent = nullptr);
    QWidget *timeShareChart=new QWidget(this);
    QWidget *candleChart=new QWidget(this);
    QWidget *hisTimeShareChart=new QWidget(this);
    QWidget *hisTimeShareChartTitle=new QWidget(hisTimeShareChart);
    QWidget *hisTimeShareChartView=new QWidget(hisTimeShareChart);
    QLabel *title=new QLabel("标题栏",hisTimeShareChartTitle);
    float candleHighLowPoint[5];
    float hisTimeShareHighLowPoint[3];
    bool isMoveHisTimeShareChart=false;
    int moveUnit=1;

    QLabel *colPrice;
    QLabel *rowTime;
    QLabel *vKLine=new QLabel(candleChart);
    QLabel *hKLine=new QLabel(candleChart);
    QLabel *timeSharePrice;
    QLabel *timeShareVol;
    QLabel *timeShareTime;
    QLabel vLine=QLabel(timeShareChart);
    QLabel hLine=QLabel(timeShareChart);
    QLabel *hisTimeSharePrice;
    QLabel *hisTimeShareVol;
    QLabel *hisTimeShareTime;
    QLabel vHisLine=QLabel(hisTimeShareChartView);
    QLabel hHisLine=QLabel(hisTimeShareChartView);
    QLabel *annLabel[50];
    QDialog *annoucementWindow=new QDialog(candleChart);
    QListView *annTitle=new QListView(annoucementWindow);
    QStringListModel *model=new QStringListModel(annoucementWindow);
    QTextBrowser *annText=new QTextBrowser(annoucementWindow);

    void drawTimeShareChart(QPainter *painter);
    void drawHisTimeShare(QPainter *painter);
    void drawCandleChart(QPainter *painter);
    void calcHighLowPoint(int begin,int end);
    void calcTSHighLowPoint(int begin,int end);
    void appendAnnNews(int end);
    QString autoWordWrap(QString str,int width);

public slots:
    void annClicked(const QModelIndex index);

};

#endif // DRAWCHART_H
