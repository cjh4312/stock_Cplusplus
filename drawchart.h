
#ifndef DRAWCHART_H
#define DRAWCHART_H


#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class DrawChart : public QWidget
{
    Q_OBJECT
public:
    explicit DrawChart(QWidget *parent = nullptr);
    QWidget *timeShareChart;
    QWidget *candleChart;
    float candleHighLowPoint[5];
    bool isTimeShareChartPaint=false;
    bool isCandleChartPaint=false;

    void drawTimeShareChart();
    void drawCandleChart();
    void calcHighLowPoint(int begin,int end);

signals:

};

#endif // DRAWCHART_H
