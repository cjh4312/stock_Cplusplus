
#ifndef DRAWCHART_H
#define DRAWCHART_H

#include "qlabel.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class DrawChart : public QWidget
{
    Q_OBJECT
public:
    explicit DrawChart(QWidget *parent = nullptr);
    QWidget *timeShareChart=new QWidget(this);
    QWidget *candleChart=new QWidget(this);
    QWidget *hisTimeShareChart=new QWidget();
    QWidget *hisTimeShareChartTitle=new QWidget(hisTimeShareChart);
    QWidget *hisTimeShareChartView=new QWidget(hisTimeShareChart);
    QLabel *title=new QLabel("标题栏");
    float candleHighLowPoint[5];
    float hisTimeShareHighLowPoint[3];
    bool isTimeShareChartPaint=false;
    bool isCandleChartPaint=false;
    bool isHisTimeShareChartPaint=false;
    bool isMoveHisTimeShareChart=false;

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
    QLabel vHisLine=QLabel(hisTimeShareChart);
    QLabel hHisLine=QLabel(hisTimeShareChart);

    void drawTimeShareChart();
    void drawHisTimeShare();
    void drawCandleChart();
    void calcHighLowPoint(int begin,int end);
    void calcTSHighLowPoint(int begin,int end);

signals:

};

#endif // DRAWCHART_H
