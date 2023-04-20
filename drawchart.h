
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

    void drawTimeShareChart();
    void drawCandleChart();
//    void flashOldCandleInfo(QMouseEvent *mouseEvent);

signals:

};

#endif // DRAWCHART_H
