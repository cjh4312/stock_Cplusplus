
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
//#include <QWebEngineView>
//#include <QPdfView>

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
    QLabel vHisLine=QLabel(hisTimeShareChartView);
    QLabel hHisLine=QLabel(hisTimeShareChartView);
    QLabel *annLabel[50];
    QDialog *annoucementWindow=new QDialog(candleChart);
    QListView *annTitle=new QListView(annoucementWindow);
    QStringListModel *model=new QStringListModel(annoucementWindow);
    QTextBrowser *annText=new QTextBrowser(annoucementWindow);

    void drawTimeShareChart();
    void drawHisTimeShare();
    void drawCandleChart();
    void calcHighLowPoint(int begin,int end);
    void calcTSHighLowPoint(int begin,int end);
    void appendAnnoucement(int b,int e,int aveWidth);
    QString autoWordWrap(QString str,int width);

public slots:
    void annClicked(const QModelIndex index);

};

#endif // DRAWCHART_H
