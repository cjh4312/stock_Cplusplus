
#include "drawchart.h"
#include "globalvar.h"
DrawChart::DrawChart(QWidget *parent)
    : QWidget{parent}
{
    timeShareChart=new QWidget(this);
    candleChart=new QWidget(this);
}

void DrawChart::drawTimeShareChart()
{
    int trendsTotal=0;
    if (GlobalVar::mTimeShareChartList.count()!=0)
        trendsTotal=GlobalVar::trendsTotal;
    QPainter painter(timeShareChart);
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::black);
    int timeShareChartWidth=timeShareChart->width();
    int timeShareChartHeight=timeShareChart->height();
    int priceH=timeShareChartHeight*12/15;
    painter.drawRect(0,0,timeShareChartWidth,timeShareChartHeight);
    painter.drawLine(0,priceH,timeShareChartWidth,priceH);
    painter.drawLine(0,0,timeShareChartWidth,0);
    painter.drawLine(0,timeShareChartHeight-BOTTOMHEIGHTEDGE,timeShareChartWidth,timeShareChartHeight-BOTTOMHEIGHTEDGE);
    float high=GlobalVar::timeShareHighLowPoint[0];
    float low=GlobalVar::timeShareHighLowPoint[1];
    float stopH=GlobalVar::timeShareHighLowPoint[3];
    float stopL=GlobalVar::timeShareHighLowPoint[4];
    float aveWidth=0;
    float aveHeight=0;
    if (high!=low)
        aveHeight=(priceH-2*TOPHEIGHTEDGE)/(high-low);
    if (trendsTotal!=0)
        aveWidth=(timeShareChartWidth-2*WIDTHEDGE)/trendsTotal;

    float volAveHeight=0;
    if (GlobalVar::timeShareHighLowPoint[2]!=0)
        volAveHeight=(3*timeShareChartHeight/15-TOPHEIGHTEDGE-BOTTOMHEIGHTEDGE)/GlobalVar::timeShareHighLowPoint[2];
//    qDebug()<<volAveHeight;
    int d=60;
    if (trendsTotal>391)
        d=180;
    else if(trendsTotal>241)
        d=90;
    for (int i=1;i<=trendsTotal/30;++i)
        painter.drawLine(QPointF(i*d*aveWidth/2+WIDTHEDGE,0),QPointF(i*d*aveWidth/2+WIDTHEDGE,timeShareChartHeight-BOTTOMHEIGHTEDGE));
    //绘制时间
    QRect rect;
    painter.setPen(Qt::white);
    for (int i=0;i<trendsTotal;i=i+d)
    {
        if (i>GlobalVar::mTimeShareChartList.count())
            break;

        int offset=WIDTHEDGE;
        if (i>0)
            offset=WIDTHEDGE-15;
        //        qDebug()<<offset+aveWidth*(i-1)<<timeShareChartWidth-d/3-1;
        if (offset+aveWidth*(i-1)>timeShareChartWidth-d/3-1)
            offset-=15;
        rect=QRect(offset+aveWidth*(i-1),timeShareChartHeight-BOTTOMHEIGHTEDGE,36,20);
        painter.drawText(rect,Qt::AlignLeft,GlobalVar::mTimeShareChartList.at(i).time.right(5));
    }

    if (high>=0 and low<=0)
    {
        painter.setPen(Qt::blue);
        painter.drawLine(QPointF(0, high*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth, high*aveHeight+TOPHEIGHTEDGE));
    }
    if (high>=5 and low<=5)
    {
        painter.setPen(Qt::red);
        painter.drawLine(QPointF(WIDTHEDGE, (high-5)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-5)*aveHeight+TOPHEIGHTEDGE));
    }
    if (low>5 or high==stopH)
    {
        painter.setPen(QColor(255,0,255));
        painter.drawLine(QPointF(WIDTHEDGE, (high-stopH)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-stopH)*aveHeight+TOPHEIGHTEDGE));
        if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(3)=="688")
        {
            painter.setPen(QColor(255,182,193));
            painter.drawLine(QPointF(WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE));
        }
    }
    if (high<5 or low==stopL)
    {
        painter.setPen(QColor(60,179,113));
        painter.drawLine(QPointF(WIDTHEDGE, (high-stopL)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-stopL)*aveHeight+TOPHEIGHTEDGE));
        painter.setPen(QColor(0, 255, 0));
        painter.drawLine(QPointF(WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE));
    }
    if (low<=-5 and high>=-5)
    {
        painter.setPen(QColor(0, 255, 0));
        painter.drawLine(QPointF(WIDTHEDGE, (high+5)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high+5)*aveHeight+TOPHEIGHTEDGE));
    }
    for (int i=0;i<GlobalVar::mTimeShareChartList.count();++i)
    {
        if (i>=1)
        {
            float price1=GlobalVar::mTimeShareChartList.at(i-1).price;
            float price2=GlobalVar::mTimeShareChartList.at(i).price;
            float avePrice1=GlobalVar::mTimeShareChartList.at(i-1).avePrice;
            float avePrice2=GlobalVar::mTimeShareChartList.at(i).avePrice;
            painter.setPen(Qt::yellow);
            if (high!=low)
                painter.drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-price1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-price2)*aveHeight+TOPHEIGHTEDGE));
            else
            {
                if (high>0)
                    painter.drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, TOPHEIGHTEDGE));
                else
                {
                    painter.setPen(QColor(0, 255, 0));
                    painter.drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), priceH-TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, priceH-TOPHEIGHTEDGE));
                }
            }
            painter.setPen(Qt::white);
            if (high!=low)
                painter.drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-avePrice1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-avePrice2)*aveHeight+TOPHEIGHTEDGE));
        }

        float vol=GlobalVar::mTimeShareChartList.at(i).vol;
//        qDebug()<<vol;
        if (GlobalVar::mTimeShareChartList.at(i).direct==1)
            painter.setPen(QColor(0, 255, 0));
        else if (GlobalVar::mTimeShareChartList.at(i).direct==2)
            painter.setPen(Qt::red);
        else
            painter.setPen(Qt::white);
        painter.drawLine(QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE));
//        qDebug()<<vol<<timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight;
    }
}

void DrawChart::drawCandleChart()
{
    int total=GlobalVar::mCandleChartList.count();
    if (total==0)
        return;
    int begin=total-GlobalVar::offsetLocal;
    if (begin<0)
        begin=0;
    int end=total-GlobalVar::offsetLocal+GlobalVar::KRange-GlobalVar::offsetEnd;
    if (end<50)
        end=total;
//    qDebug()<<total<<GlobalVar::KRange<<GlobalVar::offsetLocal<<begin<<end;
//    qDebug()<<total<<GlobalVar::KBegin<<end;
    QPainter painter(candleChart);
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::black);
    int candleChartWidth=candleChart->width();
    int canldeChartHeight=candleChart->height();
    int priceH=canldeChartHeight*12/15;
    painter.drawRect(0,0,candleChartWidth,canldeChartHeight);
    painter.drawLine(0,priceH,candleChartWidth,priceH);
    calcHighLowPoint(begin,end);
    float highPoint=candleHighLowPoint[0];
    float lowPoint=candleHighLowPoint[1];
    float aveWidth=(candleChartWidth-2*KWIDTHEDGE)/GlobalVar::KRange;
//        qDebug()<<aveWidth;
    float w=aveWidth/4;
    float aveHeight=0;
    if (highPoint!=lowPoint)
        aveHeight=(priceH-2*KTOPHEIGHTEDGE)/(highPoint-lowPoint);
    float aveHeightVol=0;
    if (candleHighLowPoint[2]!=0)
        aveHeightVol=(canldeChartHeight*3/15-2*KBOTTOMHEIGHTEDGE)/candleHighLowPoint[2];
    int j=0;
    for (int n=begin;n<end;++n)
    {
//        int n=GlobalVar::mCandleChartList.count()-KRange+i;
        float open=GlobalVar::mCandleChartList.at(n).open;
        float close=GlobalVar::mCandleChartList.at(n).close;
        float high=GlobalVar::mCandleChartList.at(n).high;
        float low=GlobalVar::mCandleChartList.at(n).low;
        float vol=GlobalVar::mCandleChartList.at(n).vol;
        float Y=(highPoint-close)*aveHeight;
        float height=(close-open)*aveHeight;
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
        if (open>close)
        {
            Y=(highPoint-open)*aveHeight;
            height=(open-close)*aveHeight;
            painter.setPen(QColor(2,148,255));
            painter.setBrush(QColor(2,148,255));
        }
//        qDebug()<<(highPoint-high)*aveHeight+KTOPHEIGHTEDGE<<(highPoint-low)*aveHeight+KTOPHEIGHTEDGE<<Y+KTOPHEIGHTEDGE;
//        if (high!=low)
        painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-high)*aveHeight+KTOPHEIGHTEDGE),
                QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-low)*aveHeight+KTOPHEIGHTEDGE));
        painter.drawRect(KWIDTHEDGE+aveWidth/2+aveWidth*j-w,Y+KTOPHEIGHTEDGE,2*w,height);
//        qDebug()<<aveHeightVol<<candleHighLowPoint[2];
        painter.drawRect(KWIDTHEDGE+aveWidth/2+aveWidth*j-w,canldeChartHeight-vol*aveHeightVol-KBOTTOMHEIGHTEDGE,
                         2*w,vol*aveHeightVol);

        float preMA=GlobalVar::mCandleChartList.at(n-1).MA5;
        float MA=GlobalVar::mCandleChartList.at(n).MA5;
        painter.setPen(QColor(255, 255, 255));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
        preMA=GlobalVar::mCandleChartList.at(n-1).MA10;
        MA=GlobalVar::mCandleChartList.at(n).MA10;
        painter.setPen(QColor(255, 255, 0));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
        preMA=GlobalVar::mCandleChartList.at(n-1).MA20;
        MA=GlobalVar::mCandleChartList.at(n).MA20;
        painter.setPen(QColor(0, 255, 0));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
        preMA=GlobalVar::mCandleChartList.at(n-1).MA60;
        MA=GlobalVar::mCandleChartList.at(n).MA60;
        painter.setPen(QColor(205,0,0));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));

        preMA=GlobalVar::mCandleChartList.at(n-1).VMA5;
        MA=GlobalVar::mCandleChartList.at(n).VMA5;
        painter.setPen(QColor(255, 255, 0));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),canldeChartHeight-preMA*aveHeightVol-KBOTTOMHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,canldeChartHeight-MA*aveHeightVol-KBOTTOMHEIGHTEDGE));
        preMA=GlobalVar::mCandleChartList.at(n-1).VMA10;
        MA=GlobalVar::mCandleChartList.at(n).VMA10;
        painter.setPen(QColor(0, 255, 0));
        if (preMA>0)
            painter.drawLine(QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*(j-1),canldeChartHeight-preMA*aveHeightVol-KBOTTOMHEIGHTEDGE),
                             QPointF(KWIDTHEDGE+aveWidth/2+aveWidth*j,canldeChartHeight-MA*aveHeightVol-KBOTTOMHEIGHTEDGE));
        j+=1;
    }
//    qDebug()<<candleHighLowPoint[3]<<candleHighLowPoint[4];
    painter.setPen(Qt::red);
    painter.setFont(QFont("微软雅黑",15,700));
//    qDebug()<<KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]<<GlobalVar::format_conversion(highPoint);
    QRect rect;
    if (not GlobalVar::offsetEnd)
    {
        if (highPoint!=0)
        {
            if (candleHighLowPoint[3]>=GlobalVar::KRange/2)
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]-95,10,100,30);
                painter.drawText(rect,Qt::AlignRight,QString::number(highPoint));
            }
            else
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]-5,10,100,30);
                painter.drawText(rect,Qt::AlignLeft,QString::number(highPoint));
            }
        }
        painter.setPen(QColor(0, 255, 0));
        if (lowPoint!=0)
        {
            if (candleHighLowPoint[4]>=GlobalVar::KRange/2)
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4]-95,canldeChartHeight*12/15-35,100,30);
                painter.drawText(rect,Qt::AlignRight,QString::number(lowPoint));
            }
            else
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4]-5,canldeChartHeight*12/15-35,100,30);
                painter.drawText(rect,Qt::AlignLeft,QString::number(lowPoint));
            }
        }
    }
}

void DrawChart::calcHighLowPoint(int begin,int end)
{
    candleHighLowPoint[0]={0.0};
    candleHighLowPoint[1]={100000.0};
    candleHighLowPoint[2]={0.0};
    float temp;
    for (int i=begin;i<end;++i)
    {
        temp=GlobalVar::mCandleChartList.at(i).high;
        if (temp>candleHighLowPoint[0])
        {
            candleHighLowPoint[0]=temp;
            candleHighLowPoint[3]=i-begin;
        }
        temp=GlobalVar::mCandleChartList.at(i).low;
        if (temp<candleHighLowPoint[1])
        {
            candleHighLowPoint[1]=temp;
            candleHighLowPoint[4]=i-begin;
        }
        temp=GlobalVar::mCandleChartList.at(i).vol;
        if (temp>candleHighLowPoint[2])
            candleHighLowPoint[2]=temp;
    }
}

