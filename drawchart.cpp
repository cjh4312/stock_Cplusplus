
#include "drawchart.h"

DrawChart::DrawChart(QWidget *parent)
    : QWidget{parent}
{
//    hisTimeShareChart->setWindowFlags(hisTimeShareChart->windowFlags() | Qt::WindowStaysOnTopHint);
//    hisTimeShareChart->setWindowFlag(Qt::Popup);

    QHBoxLayout *titleLayout=new QHBoxLayout(hisTimeShareChartTitle);
    hisTimeShareChartTitle->setLayout(titleLayout);
    hisTimeShareChartTitle->setMaximumHeight(TITLEHEIGHT);
    hisTimeShareChartTitle->setStyleSheet("background-color:rgb(153, 204, 255);");
    title->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(255, 0, 127)}");
    QVBoxLayout *main=new QVBoxLayout();
    main->setSpacing(0);
    main->setContentsMargins(0,0,0,0);
    hisTimeShareChart->setLayout(main);
    main->addWidget(hisTimeShareChartTitle);
    main->addWidget(hisTimeShareChartView);
    QPushButton *close=new QPushButton(hisTimeShareChartTitle);
    QIcon myicon;
    myicon.addFile(tr(":/new/png/png/close.png"));
    close->setIcon(myicon);
    close->setIconSize(QSize(20,20));
    close->setMaximumSize(QSize(30,30));
    titleLayout->addWidget(title);
    titleLayout->addWidget(close);
    hisTimeShareChart->setGeometry(200, 100, 600, 450);
//    hisTimeShareChartView->setMouseTracking(true);
//    hisTimeShareChartTitle->setMouseTracking(true);
    hisTimeShareChart->hide();

    colPrice=new QLabel(candleChart);
    rowTime=new QLabel(candleChart);
    colPrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    rowTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //    colPrice->resize(72,15);
    colPrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
    colPrice->setAlignment(Qt::AlignCenter);
    rowTime->resize(95,15);
    rowTime->setStyleSheet("color:yellow;font:bold;font-size:18px");
    rowTime->setAlignment(Qt::AlignCenter);
    hKLine->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vKLine->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timeSharePrice=new QLabel(timeShareChart);
    timeShareVol=new QLabel(timeShareChart);
    timeShareTime=new QLabel(timeShareChart);
    timeSharePrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timeShareVol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timeShareTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timeShareChart->setStyleSheet("color:white;font:bold;font-size:14px");
    timeSharePrice->setAlignment(Qt::AlignCenter);
    timeShareVol->setStyleSheet("color:white;font:bold;font-size:18px");
    timeShareVol->setAlignment(Qt::AlignCenter);
    timeShareTime->setStyleSheet("color:white;font:bold;font-size:18px");
    timeShareTime->setAlignment(Qt::AlignCenter);
    timeShareTime->resize(50,18);
    hisTimeSharePrice=new QLabel(hisTimeShareChartView);
    hisTimeShareVol=new QLabel(hisTimeShareChartView);
    hisTimeShareTime=new QLabel(hisTimeShareChartView);
    hisTimeSharePrice->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hisTimeShareTime->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hisTimeShareVol->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vHisLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hHisLine.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    hisTimeShareChart->setStyleSheet("color:white;font:bold;font-size:14px");
    hisTimeSharePrice->setAlignment(Qt::AlignCenter);
    hisTimeShareVol->setStyleSheet("color:white;font:bold;font-size:18px");
    hisTimeShareVol->setAlignment(Qt::AlignCenter);
    hisTimeShareTime->setStyleSheet("color:white;font:bold;font-size:18px");
    hisTimeShareTime->setAlignment(Qt::AlignCenter);
    hisTimeShareTime->resize(50,18);

    annoucementWindow->setWindowFlag(Qt::Popup);
    annoucementWindow->resize(1000,600);
    annTitle->setMaximumWidth(300);
    annTitle->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *annLayout=new QHBoxLayout(annoucementWindow);
    annoucementWindow->setLayout(annLayout);
//    annTitle->setStyleSheet("QListView::item{height:60px}");
    QFont list_font;
    list_font.setPointSize(14);
    annTitle->setFont(list_font);
    annTitle->setStyleSheet("QListView::item::selected{border:1px solid blue;color:red}");
    annTitle->setWordWrap(true);
    annTitle->setAlternatingRowColors(true);
    annTitle->setSpacing(6);
    annText->setOpenExternalLinks(true);
    annText->setStyleSheet("QTextBrowser{font:20px}");
    annLayout->addWidget(annTitle);
    annLayout->addWidget(annText);

//    annoucementWindow
    QPixmap *pixmap = new QPixmap(":/new/png/png/annoucement.png");
    for (int i=0;i<50;++i)
    {
        annLabel[i]=new QLabel(candleChart);
        annLabel[i]->resize(TIPWIDTH,TIPWIDTH);
        pixmap->scaled(annLabel[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        annLabel[i]->setScaledContents(true);
        annLabel[i]->setPixmap(*pixmap);
        annLabel[i]->setStyleSheet("QToolTip{border:2px solid darkkhaki;padding:5px;border-radius:3px;opacity:200;}");
        annLabel[i]->hide();
    }
    connect(close,&QPushButton::clicked,this,[=](){hisTimeShareChart->close();});
    connect(annTitle, SIGNAL(clicked(QModelIndex)),this, SLOT(annClicked(QModelIndex)));
}

void DrawChart::drawTimeShareChart(QPainter *painter)
{
    // QPainter *painter=new QPainter(timeShareChart);
    int trendsTotal=GlobalVar::trendsTotal;

    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int timeShareChartWidth=timeShareChart->width();
    int timeShareChartHeight=timeShareChart->height();
    int priceH=timeShareChartHeight*12/15;
    painter->drawRect(0,0,timeShareChartWidth-1,timeShareChartHeight-1);
    painter->drawLine(0,priceH,timeShareChartWidth,priceH);
    painter->drawLine(0,0,timeShareChartWidth,0);
    painter->drawLine(0,timeShareChartHeight-BOTTOMHEIGHTEDGE,timeShareChartWidth,timeShareChartHeight-BOTTOMHEIGHTEDGE);
    float high=GlobalVar::timeShareHighLowPoint[0];
    float low=GlobalVar::timeShareHighLowPoint[1];
    float stopH=GlobalVar::timeShareHighLowPoint[3];
    float stopL=GlobalVar::timeShareHighLowPoint[4];
    float interval=high-low;
    float lineNums=3;

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
        painter->drawLine(QPointF(i*d*aveWidth/2+WIDTHEDGE,0),QPointF(i*d*aveWidth/2+WIDTHEDGE,timeShareChartHeight-BOTTOMHEIGHTEDGE));

    painter->setPen(QColor(79,79,79));
    for (int i=1;i<lineNums+1;++i)
        painter->drawLine(QPointF(WIDTHEDGE, interval*i/(lineNums+1)*aveHeight+TOPHEIGHTEDGE),
            QPointF(timeShareChartWidth-WIDTHEDGE, interval*i/(lineNums+1)*aveHeight+TOPHEIGHTEDGE));
    // qDebug()<<GlobalVar::mTimeShareChartList.size();
    if (GlobalVar::mTimeShareChartList.isEmpty())
        return;
    //绘制时间
    QRect rect;
    painter->setPen(Qt::white);
    for (int i=0;i<GlobalVar::mTimeShareChartList.count();i=i+d)
    {
        int offset=WIDTHEDGE;
        if (i>0)
            offset=WIDTHEDGE-15;
        //        qDebug()<<offset+aveWidth*(i-1)<<timeShareChartWidth-d/3-1;
        if (offset+aveWidth*(i-1)>timeShareChartWidth-d/3-1)
            offset-=15;
        rect=QRect(offset+aveWidth*(i-1),timeShareChartHeight-BOTTOMHEIGHTEDGE,36,20);
        painter->drawText(rect,Qt::AlignLeft,GlobalVar::mTimeShareChartList.at(i).time.right(5));
    }
    int v=high*aveHeight+TOPHEIGHTEDGE;
    if (v>=20)
        v=v-20;
    rect=QRect(timeShareChartWidth-210,v,200,20);
    painter->drawText(rect,Qt::AlignRight,QString::number(GlobalVar::preClose));
    rect=QRect(timeShareChartWidth-100,timeShareChartHeight*12/15,100,20);
    painter->drawText(rect,Qt::AlignLeft,QString::number(GlobalVar::timeShareHighLowPoint[2],'f',0));
    painter->setPen(Qt::blue);
    painter->drawLine(QPointF(0, high*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth, high*aveHeight+TOPHEIGHTEDGE));
    if (high>=5)
    {
        painter->setPen(Qt::red);
        painter->drawLine(QPointF(WIDTHEDGE, (high-5)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-5)*aveHeight+TOPHEIGHTEDGE));
    }
    if (high==stopH)
    {
        painter->setPen(QColor(255,0,255));
        painter->drawLine(QPointF(WIDTHEDGE, (high-stopH)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-stopH)*aveHeight+TOPHEIGHTEDGE));
        if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(3)=="688" or GlobalVar::curCode.left(1)=="4")
        {
            painter->setPen(QColor(255,182,193));
            painter->drawLine(QPointF(WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE));
        }
    }
    if (low==stopL)
    {
        painter->setPen(QColor(60,179,113));
        painter->drawLine(QPointF(WIDTHEDGE, (high-stopL)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-stopL)*aveHeight+TOPHEIGHTEDGE));
        painter->setPen(QColor(0, 255, 0));
        painter->drawLine(QPointF(WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-10)*aveHeight+TOPHEIGHTEDGE));
    }
    if (low<=-5)
    {
        painter->setPen(QColor(0, 255, 0));
        painter->drawLine(QPointF(WIDTHEDGE, (high+5)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high+5)*aveHeight+TOPHEIGHTEDGE));
    }
    for (int i=0;i<GlobalVar::mTimeShareChartList.count();++i)
    {
        if (i>=1)
        {
            float price1=GlobalVar::mTimeShareChartList.at(i-1).price;
            float price2=GlobalVar::mTimeShareChartList.at(i).price;
            float avePrice1=GlobalVar::mTimeShareChartList.at(i-1).avePrice;
            float avePrice2=GlobalVar::mTimeShareChartList.at(i).avePrice;
            painter->setPen(Qt::white);
            if (high!=low)
                painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-price1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-price2)*aveHeight+TOPHEIGHTEDGE));
            else
            {
                if (high>0)
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, TOPHEIGHTEDGE));
                else
                {
                    painter->setPen(QColor(0, 255, 0));
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), priceH-TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, priceH-TOPHEIGHTEDGE));
                }
            }
            painter->setPen(Qt::yellow);
            if (high!=low)
                painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-avePrice1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-avePrice2)*aveHeight+TOPHEIGHTEDGE));
        }

        float vol=GlobalVar::mTimeShareChartList.at(i).vol;
//        qDebug()<<vol;
        if (GlobalVar::mTimeShareChartList.at(i).direct==1)
            painter->setPen(QColor(0, 255, 0));
        else if (GlobalVar::mTimeShareChartList.at(i).direct==2)
            painter->setPen(Qt::red);
        else
            painter->setPen(Qt::white);
        painter->drawLine(QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE));
//        qDebug()<<vol<<timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight;
    }
    // painter->end();
}

void DrawChart::drawHisTimeShare(QPainter *painter)
{
    int trendsTotal=GlobalVar::mHisTimeShareChartList.count();
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int timeShareChartWidth=hisTimeShareChartView->width();
    int timeShareChartHeight=hisTimeShareChartView->height();
    int priceH=timeShareChartHeight*12/15;
    painter->drawRect(0,0,timeShareChartWidth-1,timeShareChartHeight-1);
    painter->drawLine(0,priceH,timeShareChartWidth,priceH);
    painter->drawLine(0,0,timeShareChartWidth,0);
    painter->drawLine(0,timeShareChartHeight-BOTTOMHEIGHTEDGE,timeShareChartWidth,timeShareChartHeight-BOTTOMHEIGHTEDGE);

    calcTSHighLowPoint(0,trendsTotal);

    float high=hisTimeShareHighLowPoint[0];
    float low=hisTimeShareHighLowPoint[1];
    float lineNums=3;
    float interval=timeShareChartHeight/(lineNums+1);

    float aveWidth=0;
    float aveHeight=0;
    if (high!=low)
        aveHeight=(priceH-2*TOPHEIGHTEDGE)/(high-low);
    if (trendsTotal!=0)
        aveWidth=(timeShareChartWidth-2*WIDTHEDGE)/trendsTotal;

    painter->setPen(QColor(79,79,79));
    for (int i=1;i<lineNums+1;++i)
        painter->drawLine(QPointF(WIDTHEDGE, interval*i/(lineNums+1)*aveHeight+TOPHEIGHTEDGE),
            QPointF(timeShareChartWidth-WIDTHEDGE, interval*i/(lineNums+1)*aveHeight+TOPHEIGHTEDGE));

    float volAveHeight=0;
    if (hisTimeShareHighLowPoint[2]!=0)
        volAveHeight=(3*timeShareChartHeight/15-TOPHEIGHTEDGE-BOTTOMHEIGHTEDGE)/hisTimeShareHighLowPoint[2];

    int d=60;
    for (int i=1;i<trendsTotal/30;++i)
        painter->drawLine(QPointF(i*d*aveWidth/2+WIDTHEDGE+2,0),QPointF(i*d*aveWidth/2+WIDTHEDGE+2,timeShareChartHeight-BOTTOMHEIGHTEDGE));
    QRect rect;
    painter->setPen(Qt::white);
    for (int i=1;i<trendsTotal;i=i+d)
    {
        int offset=WIDTHEDGE;
        if (i>1)
            offset=WIDTHEDGE-15;
        //        qDebug()<<offset+aveWidth*(i-1)<<timeShareChartWidth-d/3-1;
        if (i+d>trendsTotal)
            offset-=15;
        rect=QRect(offset+aveWidth*(i-1),timeShareChartHeight-BOTTOMHEIGHTEDGE,36,20);
        painter->drawText(rect,Qt::AlignLeft,GlobalVar::mHisTimeShareChartList.at(i).time.right(5));
    }
    int v=(high-GlobalVar::hisPreClose)*aveHeight+TOPHEIGHTEDGE;
    if (v>=20)
        v=v-20;
    rect=QRect(timeShareChartWidth-210,v,200,20);
    painter->drawText(rect,Qt::AlignRight,QString::number(GlobalVar::hisPreClose));
    rect=QRect(timeShareChartWidth-100,timeShareChartHeight*12/15,100,20);
    painter->drawText(rect,Qt::AlignLeft,QString::number(hisTimeShareHighLowPoint[2],'f',0));
    painter->setPen(Qt::blue);
    painter->drawLine(QPointF(0, (high-GlobalVar::hisPreClose)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth, (high-GlobalVar::hisPreClose)*aveHeight+TOPHEIGHTEDGE));
    float c=1.05*GlobalVar::hisPreClose;
    if (high>=c)
    {
        painter->setPen(Qt::red);
        painter->drawLine(QPointF(WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE));
    }
    c=1.1*GlobalVar::hisPreClose;
    if (high>=1.1*GlobalVar::hisPreClose)
    {
        painter->setPen(QColor(255,182,193));
        painter->drawLine(QPointF(WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE));
    }
    c=0.9*GlobalVar::hisPreClose;
    if (low<=c)
    {
        painter->setPen(QColor(60,179,113));
        painter->drawLine(QPointF(WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE));
    }
    c=0.95*GlobalVar::hisPreClose;
    if (low<=c)
    {
        painter->setPen(QColor(0, 255, 0));
        painter->drawLine(QPointF(WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE), QPointF(timeShareChartWidth-WIDTHEDGE, (high-c)*aveHeight+TOPHEIGHTEDGE));
    }
    for (int i=0;i<trendsTotal;++i)
    {
        float vol=GlobalVar::mHisTimeShareChartList.at(i).vol;
        if (i>=1)
        {
            float price1=GlobalVar::mHisTimeShareChartList.at(i-1).price;
            float price2=GlobalVar::mHisTimeShareChartList.at(i).price;

            float avePrice1=GlobalVar::mHisTimeShareChartList.at(i-1).avePrice;
            float avePrice2=GlobalVar::mHisTimeShareChartList.at(i).avePrice;
            painter->setPen(Qt::white);
            if (high!=low)
                painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-price1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-price2)*aveHeight+TOPHEIGHTEDGE));
            else
            {
                if (high>0)
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, TOPHEIGHTEDGE));
                else
                {
                    painter->setPen(QColor(0, 255, 0));
                    painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), priceH-TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, priceH-TOPHEIGHTEDGE));
                }
            }
            painter->setPen(Qt::yellow);
            if (high!=low)
                painter->drawLine(QPointF(WIDTHEDGE+aveWidth*(i-1), (high-avePrice1)*aveHeight+TOPHEIGHTEDGE), QPointF(WIDTHEDGE+aveWidth*i, (high-avePrice2)*aveHeight+TOPHEIGHTEDGE));

            if (price2<price1)
                painter->setPen(QColor(0, 255, 0));
            else if (price2>price1)
                painter->setPen(Qt::red);
            else
                painter->setPen(Qt::white);
            painter->drawLine(QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE));

        }
        else {
            if (GlobalVar::mHisTimeShareChartList.at(1).price>GlobalVar::hisPreClose)
                painter->setPen(Qt::red);
            else if (GlobalVar::mHisTimeShareChartList.at(1).price<GlobalVar::hisPreClose)
                painter->setPen(QColor(0, 255, 0));
            else
                painter->setPen(Qt::white);
            painter->drawLine(QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE-vol*volAveHeight), QPointF(WIDTHEDGE+aveWidth*i, timeShareChartHeight-BOTTOMHEIGHTEDGE));
        }
    }
}

void DrawChart::drawCandleChart(QPainter *painter)
{
    int total=GlobalVar::mCandleChartList.count();
    int begin=total-GlobalVar::offsetLocal;
    if (begin<0)
        begin=0;
    int end=total-GlobalVar::offsetLocal+GlobalVar::KRange-GlobalVar::offsetEnd;
    if (end<50)
        end=total;
//    qDebug()<<total<<GlobalVar::KRange<<GlobalVar::offsetLocal<<begin<<end;
//    qDebug()<<total<<GlobalVar::KBegin<<end;
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::black);
    int candleChartWidth=candleChart->width();
    int canldeChartHeight=candleChart->height();
    int priceH=canldeChartHeight*12/15;
    painter->drawRect(0,0,candleChartWidth,canldeChartHeight);
    painter->drawLine(0,priceH,candleChartWidth-KRIGHTWIDTHEDGE,priceH);

    if (GlobalVar::mCandleChartList.isEmpty())
        return;

    calcHighLowPoint(begin,end);
    float highPoint=candleHighLowPoint[0];
    float lowPoint=candleHighLowPoint[1];
    float interval=highPoint-lowPoint;
    int lineNums=20;
    float rate=0.2f;
    if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or GlobalVar::curCode.left(1)=="1" or
        GlobalVar::curCode.left(1)=="9" or GlobalVar::curCode.left(3)=="399" or GlobalVar::curCode.left(3)=="899")
        rate=0.05f;
    while (lineNums>2 and interval/(lineNums-1)/lowPoint<rate)
        lineNums-=1;

    float aveWidth=(candleChartWidth-2*KWIDTHEDGE-KRIGHTWIDTHEDGE)/GlobalVar::KRange;
//        qDebug()<<aveWidth;
    int w=aveWidth/4;
    float aveHeight=0;
    QRect rect;
    if (highPoint!=lowPoint)
        aveHeight=(priceH-2*KTOPHEIGHTEDGE)/(highPoint-lowPoint);
    painter->setPen(QColor(79,79,79));
    painter->drawLine(QPointF(candleChartWidth-KRIGHTWIDTHEDGE, 0),
                      QPointF(candleChartWidth-KRIGHTWIDTHEDGE, canldeChartHeight));
    for (int i=0;i<lineNums;++i)
    {
        painter->setPen(QColor(79,79,79));
        painter->drawLine(QPointF(KWIDTHEDGE, interval*i/(lineNums-1)*aveHeight+KTOPHEIGHTEDGE),
            QPointF(candleChartWidth-KRIGHTWIDTHEDGE+5, interval*i/(lineNums-1)*aveHeight+KTOPHEIGHTEDGE));
        rect=QRect(candleChartWidth-KRIGHTWIDTHEDGE,interval*i/(lineNums-1)*aveHeight+KTOPHEIGHTEDGE-10,60,20);
        painter->setPen(QColor(255,165,0));
        painter->drawText(rect,Qt::AlignCenter,QString::number(highPoint-interval*i/(lineNums-1),'f',2));
    }

    float aveHeightVol=0;
    if (candleHighLowPoint[2]!=0)
        aveHeightVol=(canldeChartHeight*3/15-2*KBOTTOMHEIGHTEDGE)/candleHighLowPoint[2];

    interval=candleHighLowPoint[2];
    lineNums=5;
    for (int i=1;i<lineNums;++i)
    {
        painter->setPen(QColor(79,79,79));
        painter->drawLine(QPointF(KWIDTHEDGE, canldeChartHeight-KBOTTOMHEIGHTEDGE-interval*i/(lineNums-1)*aveHeightVol),
                          QPointF(candleChartWidth-KRIGHTWIDTHEDGE+5, canldeChartHeight-KBOTTOMHEIGHTEDGE-interval*i/(lineNums-1)*aveHeightVol));
        rect=QRect(candleChartWidth-KRIGHTWIDTHEDGE,canldeChartHeight-KBOTTOMHEIGHTEDGE-interval*i/(lineNums-1)*aveHeightVol-10,60,20);
        painter->setPen(QColor(255,165,0));
        painter->drawText(rect,Qt::AlignCenter,GlobalVar::format_conversion(candleHighLowPoint[2]-interval*(lineNums-1-i)/(lineNums-1)));
    }
    float curXPos=KWIDTHEDGE+aveWidth/2;
    float preXPos=KWIDTHEDGE-aveWidth/2;
    for (int n=begin;n<end;++n)
    {
        float open=GlobalVar::mCandleChartList.at(n).open;
        float close=GlobalVar::mCandleChartList.at(n).close;
        float high=GlobalVar::mCandleChartList.at(n).high;
        float low=GlobalVar::mCandleChartList.at(n).low;
        float vol=GlobalVar::mCandleChartList.at(n).vol;
        float Y=(highPoint-close)*aveHeight;
        float height=(close-open)*aveHeight;
        painter->setPen(Qt::white);
        painter->setBrush(Qt::white);
        if (open>close)
        {
            Y=(highPoint-open)*aveHeight;
            height=(open-close)*aveHeight;
            painter->setPen(QColor(2,148,255));
            painter->setBrush(QColor(2,148,255));
        }
//        qDebug()<<(highPoint-high)*aveHeight+KTOPHEIGHTEDGE<<(highPoint-low)*aveHeight+KTOPHEIGHTEDGE<<Y+KTOPHEIGHTEDGE;
//        if (high!=low)
        painter->drawLine(QPointF(curXPos,(highPoint-high)*aveHeight+KTOPHEIGHTEDGE),
                QPointF(curXPos,(highPoint-low)*aveHeight+KTOPHEIGHTEDGE));
        painter->drawRect(curXPos-w,Y+KTOPHEIGHTEDGE,2*w,height);
//        qDebug()<<aveHeightVol<<candleHighLowPoint[2];
        painter->drawRect(curXPos-w,canldeChartHeight-vol*aveHeightVol-KBOTTOMHEIGHTEDGE,
                         2*w,vol*aveHeightVol);
        if (n>0)
        {
            float preMA=GlobalVar::mCandleChartList.at(n-1).MA5;
            float MA=GlobalVar::mCandleChartList.at(n).MA5;
            painter->setPen(QColor(255, 255, 255));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                                 QPointF(curXPos,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
            preMA=GlobalVar::mCandleChartList.at(n-1).MA10;
            MA=GlobalVar::mCandleChartList.at(n).MA10;
            painter->setPen(QColor(255, 255, 0));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                                 QPointF(curXPos,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
            preMA=GlobalVar::mCandleChartList.at(n-1).MA20;
            MA=GlobalVar::mCandleChartList.at(n).MA20;
            painter->setPen(QColor(0, 255, 0));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                                 QPointF(curXPos,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));
            preMA=GlobalVar::mCandleChartList.at(n-1).MA60;
            MA=GlobalVar::mCandleChartList.at(n).MA60;
            painter->setPen(QColor(205,0,0));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,(highPoint-preMA)*aveHeight+KTOPHEIGHTEDGE),
                                 QPointF(curXPos,(highPoint-MA)*aveHeight+KTOPHEIGHTEDGE));

            preMA=GlobalVar::mCandleChartList.at(n-1).VMA5;
            MA=GlobalVar::mCandleChartList.at(n).VMA5;
            painter->setPen(QColor(255, 255, 0));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,canldeChartHeight-preMA*aveHeightVol-KBOTTOMHEIGHTEDGE),
                                 QPointF(curXPos,canldeChartHeight-MA*aveHeightVol-KBOTTOMHEIGHTEDGE));
            preMA=GlobalVar::mCandleChartList.at(n-1).VMA10;
            MA=GlobalVar::mCandleChartList.at(n).VMA10;
            painter->setPen(QColor(0, 255, 0));
            if (preMA>0)
                painter->drawLine(QPointF(preXPos,canldeChartHeight-preMA*aveHeightVol-KBOTTOMHEIGHTEDGE),
                                 QPointF(curXPos,canldeChartHeight-MA*aveHeightVol-KBOTTOMHEIGHTEDGE));
        }
        curXPos+=aveWidth;
        preXPos+=aveWidth;
    }
//    qDebug()<<candleHighLowPoint[3]<<candleHighLowPoint[4];
    painter->setPen(Qt::red);
    painter->setFont(QFont("微软雅黑",15,700));
//    qDebug()<<KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]<<GlobalVar::format_conversion(highPoint);

    if (not GlobalVar::offsetEnd)
    {
        if (highPoint!=0)
        {
            if (candleHighLowPoint[3]>=GlobalVar::KRange/2)
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3]-100,30,100,30);
                painter->drawText(rect,Qt::AlignRight,QString::number(highPoint));
            }
            else
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[3],30,100,30);
                painter->drawText(rect,Qt::AlignLeft,QString::number(highPoint));
            }
        }
        painter->setPen(QColor(0, 255, 0));
        if (lowPoint!=0)
        {
            if (candleHighLowPoint[4]>=GlobalVar::KRange/2)
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4]-100,canldeChartHeight*12/15-35,100,30);
                painter->drawText(rect,Qt::AlignRight,QString::number(lowPoint));
            }
            else
            {
                rect=QRect(KWIDTHEDGE+aveWidth/2+aveWidth*candleHighLowPoint[4],canldeChartHeight*12/15-35,100,30);
                painter->drawText(rect,Qt::AlignLeft,QString::number(lowPoint));
            }
        }
    }
    appendAnnNews(end);
}

void DrawChart::calcHighLowPoint(int begin,int end)
{
    candleHighLowPoint[0]={0.0};
    candleHighLowPoint[1]={1000000.0};
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

void DrawChart::calcTSHighLowPoint(int begin, int end)
{
    hisTimeShareHighLowPoint[0]={0.0};
    hisTimeShareHighLowPoint[1]={1000000.0};
    hisTimeShareHighLowPoint[2]={0.0};
    float temp;
    for (int i=begin;i<end;++i)
    {
        temp=GlobalVar::mHisTimeShareChartList.at(i).price;
        if (temp>hisTimeShareHighLowPoint[0])
        {
            hisTimeShareHighLowPoint[0]=temp;
        }
        temp=GlobalVar::mHisTimeShareChartList.at(i).price;
        if (temp<hisTimeShareHighLowPoint[1])
        {
            hisTimeShareHighLowPoint[1]=temp;
        }
        temp=GlobalVar::mHisTimeShareChartList.at(i).vol;
        if (temp>hisTimeShareHighLowPoint[2])
            hisTimeShareHighLowPoint[2]=temp;
    }
    if (hisTimeShareHighLowPoint[0]<GlobalVar::hisPreClose)
        hisTimeShareHighLowPoint[0]=GlobalVar::hisPreClose;
    if (hisTimeShareHighLowPoint[1]>GlobalVar::hisPreClose)
        hisTimeShareHighLowPoint[1]=GlobalVar::hisPreClose;
}

void DrawChart::appendAnnNews(int end)
{
    int m=0;
    QString content="";
    int pos=0;
    int n=GlobalVar::KRange-1;
    if (GlobalVar::mCandleChartList.count()-1<n)
        n=GlobalVar::mCandleChartList.count()-1;
    int b=end-1-KRANGE;
    if (b<0)
        b=0;
    for (int j=end-1;j>=b;--j)
    {
        for (int i=pos;i<GlobalVar::annoucementList.count();++i)
        {
            QString c=GlobalVar::annoucementList.at(i)[0];
            QString l=GlobalVar::annoucementList.at(i)[1];
            QString t=GlobalVar::annoucementList.at(i)[2];
            QString time=t.mid(1,10);
            int result=QString::compare(GlobalVar::mCandleChartList.at(j).time,time);
            if (result>0)
            {
                if (not content.isEmpty())
                {
                    annLabel[m]->show();
                    annLabel[m]->setToolTip(content);
                    int posX=(2*n+1)*(candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE)/(2*GlobalVar::KRange);
                    annLabel[m]->move(posX+KWIDTHEDGE-TIPWIDTH/2,10);
                    ++m;
                    pos=i;
                    content="";
                    if (m>49)
                        return;
                }
                break;
            }
            else
            {
                if (content.isEmpty())
                    content=t+l+"\n"+autoWordWrap(c,20);
                else
                    content=content+"\n"+t+l+"\n"+autoWordWrap(c,20);
            }
        }
        --n;
    }
    for (int i=m;i<50;++i)
        annLabel[i]->hide();
}

QString DrawChart::autoWordWrap(QString str, int width)
{
    int curPost=0;
    QString s="";
    while(1)
    {
        QString temp =str.mid(curPost,width);
        s=s+temp;
        curPost=curPost+width;
        if (curPost > str.size())
        {
            s=s+str.mid(curPost,width);
            break;
        }
        else
            s=s+"\n";
    }
    return s;
}

QString retain(QString s,QString b,QString e)
{
    QString Hlabel="<"+e;
    QString Elabel="</"+e+">";
    int headPos=s.indexOf(b);
    int curPos=headPos;
//    QString head=s.mid(0,headPos);
    int endPos=s.indexOf(Elabel,headPos);
    while(1)
    {
        if ((curPos=s.indexOf(Hlabel,curPos+1))!=-1)
        {
            if (curPos>endPos)
                break;
            endPos=s.indexOf(Elabel,endPos+1);
        }
        else
            break;
    }
    return s.mid(headPos,endPos+Elabel.length()-headPos);
}

QString peel(QString s,QString b,QString e)
{
    QString Hlabel="<"+e;
    QString Elabel="</"+e+">";
    int headPos=s.indexOf(b);
    int curPos=headPos;
    QString head=s.mid(0,headPos);
    int endPos=s.indexOf(Elabel,headPos);
    while(1)
    {
        if ((curPos=s.indexOf(Hlabel,curPos+1))!=-1)
        {
            if (curPos>endPos)
                break;
            endPos=s.indexOf(Elabel,endPos+1);
        }
        else
            break;
    }
    return head+s.mid(endPos+Elabel.length(),-1);
}

void DrawChart::annClicked(const QModelIndex index)
{

    if (GlobalVar::annoucementList.at(index.row())[1]=="[新闻]")
    {
        QString s="<font size=\"4\" color=blue>"+GlobalVar::annoucementList.at(index.row())[4]
                +/*GlobalVar::annoucementList.at(index.row())[2]+*/"\r\n"+"</font>"
                +"<font size=\"3\">"+GlobalVar::annoucementList.at(index.row())[5]+"\r\n"+"</font>"
                +"<span> <a href="+GlobalVar::annoucementList.at(index.row())[3]+">"+
                +"原文"+"</a></span>";
        annText->setText(s);
    }
    else if(GlobalVar::annoucementList.at(index.row())[1]=="[大事提醒]")
    {
        QString s="<font size=\"4\" color=blue>"+GlobalVar::annoucementList.at(index.row())[0]
                    +"\r\n"+"</font>"
                    +"<font size=\"3\">"+GlobalVar::annoucementList.at(index.row())[3]+"\r\n"+"</font>";
        annText->setText(s);
    }
    else
    {
       QString html;
       QByteArray allData;
       QNetworkRequest request;
       QString url=GlobalVar::annoucementList.at(index.row())[3];
       request.setUrl(QUrl(url));
       request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
       GlobalVar::getData(allData,2,request);

       if (GlobalVar::annoucementList.at(index.row())[1]=="[证券时报]")
       {
           QTextCodec *codec = QTextCodec::codecForName("utf-8");
           html=codec->toUnicode(allData);
           html=retain(html,"<div class=\"detail-info\"","div")+retain(html,"<div class=\"detail-content\"","div");
           html=peel(html,"<div class=\"fenlei1\"","div");
           html=peel(html,"<div class=\"fenlei2\"","div");

   //        html=peel(html,"<div class=\"social-bar\"","div");
   //        html=retain(html,"<div class=\"main-content text-large\"","div");
   //        html=peel(html,"<div class=\"bullet\"","div");
   //        html=peel(html,"<div class=\"bullet\"","div");
           annText->setText(html);
       }
       else if (GlobalVar::annoucementList.at(index.row())[1]=="[公告]")
       {
           QTextCodec *codec = QTextCodec::codecForName("gbk");
           html=codec->toUnicode(allData);
           html=peel(html,"<div class=\"head\"","div");
           html=peel(html,"<div class=\"search\"","div");
           html=peel(html,"<div class=\"w1200 center\"","div");
           html=peel(html,"<div class=\"footer mg0\"","div");
           annText->setText(html);
       }
       else if (GlobalVar::annoucementList.at(index.row())[1]=="[东方公告]")
            annText->setText("<span> <a href="+GlobalVar::annoucementList.at(index.row())[3]+">"+
                         "<font size=\"4\">"+GlobalVar::annoucementList.at(index.row())[0]+"</font>"+"</a> </span>");
    }
}

