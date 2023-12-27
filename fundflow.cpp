
#include "fundflow.h"
#include "globalvar.h"
//#include "commondelegate.h"
//#include "qheaderview.h"

FundFlow::FundFlow()
{
//    naManager = new QNetworkAccessManager(this);
//    fundFlowChart->setAttribute(Qt::WA_DeleteOnClose);
    fundFlowChart->setWindowFlags(fundFlowChart->windowFlags() | Qt::WindowStaysOnTopHint);
    fundFlowChart->setGeometry(850, 150, 1000, 800);
    fundFlowChart->hide();
    vKLine->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    vKLine->setStyleSheet("QLabel{border:2px dotted blue;}");
    vKLine->resize(1,(fundFlowChart->height()-150)/2);
    vKLine->hide();
    backGround->setStyleSheet("QLabel{Background:rgba(127,193,128,0.3)}");
    backGround->setGeometry(0,375,fundFlowChart->width(),60);
    time->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,0,255)}");
    time->setGeometry(25,380,200,20);
    for (int i=0;i<5;++i)
    {
        textFund[i]=new QLabel(fundFlowChart);
//        textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,0,255)}");
        textFund[i]->setGeometry(180+i*150,410,100,20);
    }
    for (int i=0;i<6;++i)
    {
        checkBox[i]=new QCheckBox(tableChart);
        checkBox[i]->setGeometry(54+100*i,20,20,20);
        checkBox[i]->setChecked(isShow[i]);
    }
}

void FundFlow::getEastPlateFundFlow(int days)
{
    QString url[]={"https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=50&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A1&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13",
                "https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=500&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A3&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13",
                "https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=100&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A2&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13"};
    QStringList FundFlowCol;
    QString h="今日";
    if (days==5)
    {
        h="5日";
        url[0]="https://push2.eastmoney.com/api/qt/clist/get?fid=f109&po=1&pz=50&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A1&fields=f12%2Cf14%2Cf2%2Cf109%2Cf164%2Cf165%2Cf166%2Cf167%2Cf168%2Cf169%2Cf170%2Cf171%2Cf172%2Cf173%2Cf257%2Cf258%2Cf124%2Cf1%2Cf13";
        url[1]="https://push2.eastmoney.com/api/qt/clist/get?fid=f109&po=1&pz=500&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A3&fields=f12%2Cf14%2Cf2%2Cf109%2Cf164%2Cf165%2Cf166%2Cf167%2Cf168%2Cf169%2Cf170%2Cf171%2Cf172%2Cf173%2Cf257%2Cf258%2Cf124%2Cf1%2Cf13";
        url[2]="https://push2.eastmoney.com/api/qt/clist/get?fid=f109&po=1&pz=100&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A2&fields=f12%2Cf14%2Cf2%2Cf109%2Cf164%2Cf165%2Cf166%2Cf167%2Cf168%2Cf169%2Cf170%2Cf171%2Cf172%2Cf173%2Cf257%2Cf258%2Cf124%2Cf1%2Cf13";
    }
    else if (days==10)
    {
        h="10日";
        url[0]="https://push2.eastmoney.com/api/qt/clist/get?fid=f160&po=1&pz=100&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A2&fields=f12%2Cf14%2Cf2%2Cf160%2Cf174%2Cf175%2Cf176%2Cf177%2Cf178%2Cf179%2Cf180%2Cf181%2Cf182%2Cf183%2Cf260%2Cf261%2Cf124%2Cf1%2Cf13";
        url[1]="https://push2.eastmoney.com/api/qt/clist/get?fid=f160&po=1&pz=500&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A3&fields=f12%2Cf14%2Cf2%2Cf160%2Cf174%2Cf175%2Cf176%2Cf177%2Cf178%2Cf179%2Cf180%2Cf181%2Cf182%2Cf183%2Cf260%2Cf261%2Cf124%2Cf1%2Cf13";
        url[2]="https://push2.eastmoney.com/api/qt/clist/get?fid=f160&po=1&pz=50&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A1&fields=f12%2Cf14%2Cf2%2Cf160%2Cf174%2Cf175%2Cf176%2Cf177%2Cf178%2Cf179%2Cf180%2Cf181%2Cf182%2Cf183%2Cf260%2Cf261%2Cf124%2Cf1%2Cf13";
    }
    FundFlowCol<<"名称"<<h+"涨跌幅"<<"主力净额"<<"主力净占比"<<"超大单净额"<<"超大单净占比"<<"大单净额"
                <<"大单净占比"<<"中单净额"<<"中单净占比"<<"小单净额"<<"小单净占比"<<"主力净流入最大股"<<"CODE";
    FundFlowList.clear();
    for (int i=0;i<3;++i)
    {
        QByteArray allData;
        GlobalVar::getData(allData,1,QUrl(url[i]));
        getData(days,allData);
    }
    std::sort(FundFlowList.begin(),FundFlowList.end(),[](QStringList a,QStringList b){
        return a[1].toFloat()>b[1].toFloat();
    });
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    for (int i=0;i<FundFlowList.count();++i)
    {
        for (int j=0;j<FundFlowList.at(i).count();++j)
        {
            if (j==2 or j==4 or j==6 or j==8 or j==10)
            {
                QString s=FundFlowList.at(i)[j];
                model->setItem(i,j,new QStandardItem(GlobalVar::format_conversion(s.toFloat())));
                if (s.left(1)=="-")
                    model->item(i,j)->setForeground(QColor(0, 191, 0));
            }
            else if (j%2==1)
            {
                QString s=FundFlowList.at(i)[j];
                model->setItem(i,j,new QStandardItem(s));
                if (s.toFloat()>0)
                    model->item(i,j)->setForeground(Qt::red);
                if (s.left(1)=="-")
                    model->item(i,j)->setForeground(QColor(0, 191, 0));
                if (j==1)
                    model->item(i,j)->setFont(boldFont);
            }
            else
            {
                model->setItem(i,j,new QStandardItem(FundFlowList.at(i)[j]));
                model->item(i,j)->setFont(QFont("宋体"));
                if (j==0)
                {
                    model->item(i,0)->setFont(boldFont);
                    model->item(i,0)->setForeground(QColor(72,61,139));
                }
            }
        }
    }
    model->setHorizontalHeaderLabels(FundFlowCol);
}

void FundFlow::getFundFlowChartData(QString code)
{
    QString BK="90."+code;
    QString url="https://push2.eastmoney.com/api/qt/stock/fflow/kline/get?&lmt=0&klt=1&fields1=f1%2Cf2%2Cf3%2Cf7&fields2=f51%2Cf52%2Cf53%2Cf54%2Cf55%2Cf56%2Cf57%2Cf58%2Cf59%2Cf60%2Cf61%2Cf62%2Cf63%2Cf64%2Cf65&ut=b2884a393a59ad64002292a3e90d46a5&secid="+BK+"&_=1692319348628";
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl(url));

    fundFlowKList.clear();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();
        maxMinKChart[0]=0.0;
        maxMinKChart[1]=0.0;
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList list=data.at(i).toString().split(",");
            for (int j=1;j<list.count();++j)
            {
                float temp=list[j].toFloat();
                if (temp>maxMinKChart[0])
                    maxMinKChart[0]=temp;
                else if (temp<maxMinKChart[1])
                    maxMinKChart[1]=temp;
            }
            if (maxMinKChart[0]<0)
                maxMinKChart[0]=0;
            if (maxMinKChart[1]>0)
                maxMinKChart[1]=0;
            fundFlowKList.append(list);
        }
    }

    url="https://push2.eastmoney.com/api/qt/ulist.np/get?&fltt=2&secids="+BK+"&fields=f62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf64%2Cf65%2Cf70%2Cf71%2Cf76%2Cf77%2Cf82%2Cf83%2Cf164%2Cf166%2Cf168%2Cf170%2Cf172%2Cf252%2Cf253%2Cf254%2Cf255%2Cf256%2Cf124%2Cf6%2Cf278%2Cf279%2Cf280%2Cf281%2Cf282&ut=b2884a393a59ad64002292a3e90d46a5&_=1692347418545";
    GlobalVar::getData(allData,1,QUrl(url));

    doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();

        QJsonValue value = data.at(0);
        QVariantMap ceilMap = value.toVariant().toMap();
        pieData[0]=ceilMap.value("f64").toFloat();
        pieData[1]=ceilMap.value("f70").toFloat();
        pieData[2]=ceilMap.value("f76").toFloat();
        pieData[3]=ceilMap.value("f82").toFloat();
        pieData[4]=ceilMap.value("f83").toFloat();
        pieData[5]=ceilMap.value("f77").toFloat();
        pieData[6]=ceilMap.value("f71").toFloat();
        pieData[7]=ceilMap.value("f65").toFloat();
    }

    url="https://push2his.eastmoney.com/api/qt/stock/fflow/daykline/get?&lmt=0&klt=101&fields1=f1%2Cf2%2Cf3%2Cf7&fields2=f51%2Cf52%2Cf53%2Cf54%2Cf55%2Cf56%2Cf57%2Cf58%2Cf59%2Cf60%2Cf61%2Cf62%2Cf63%2Cf64%2Cf65&ut=b2884a393a59ad64002292a3e90d46a5&secid="+BK+"&_=1692405309552";
    GlobalVar::getData(allData,1,QUrl(url));

    fundFlowHKList.clear();
    doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();
        int len=data.size();
        maxMinHKChart[0]=0.0;
        maxMinHKChart[1]=0.0;
        int l=44;
        if (len<l)
            l=len;
        for (int i = len-l; i < len; ++i)
        {
            QStringList list=data.at(i).toString().split(",");
            for (int j=1;j<6;++j)
            {
                float temp=list[j].toFloat();
                if (temp>maxMinHKChart[0])
                    maxMinHKChart[0]=temp;
                else if (temp<maxMinHKChart[1])
                    maxMinHKChart[1]=temp;
            }
            if (maxMinHKChart[0]<0)
                maxMinHKChart[0]=0;
            if (maxMinHKChart[1]>0)
                maxMinHKChart[1]=0;
            fundFlowHKList.append(list);
        }

    }
}

void FundFlow::drawFundFlowChart(QPainter *painter)
{
    int maxNums=fundFlowKList.count();
    int maxHNums=fundFlowHKList.count();
    int width=fundFlowChart->width();
    int height=fundFlowChart->height();
    float aveW=0;
    if (maxNums!=0)
        aveW=width/2/maxNums;
    float interval=maxMinKChart[0]-maxMinKChart[1];
    QColor fiveColor[5]={QColor(254,62,225),QColor(148,196,238),QColor(255,184,61),QColor(255,17,23),QColor(101,0,0)};
    QColor color[8]={QColor(111,0,0),QColor(174,0,0),QColor(248,52,52),QColor(255,128,128),
                       QColor(119,233,122),QColor(39,183,41),QColor(10,130,10),QColor(0,72,0)};
    QColor fColor[5]={QColor(254,62,225),QColor(101,0,0),QColor(255,17,23),QColor(255,184,61),QColor(148,196,238)};
    QString name[5]={"主力净流入","超大单流入","大单净流入","中单净流入","小单净流入"};

    float aveHW=0.0;
    if (maxHNums!=0)
        aveHW=width/2/maxHNums;
    int bottom=150;
    int left=width/4-60;
    int right=width*3/4-60;
    int up=height/2-30;
    int down=height-15;
    int leftOffset=10;
    int textHeight=20;
    painter->setFont(QFont("微软雅黑",14,700));
    QPen pen=QPen(Qt::gray,1,Qt::DashLine);
    painter->setPen(pen);
    float zeroLocation=(height-bottom)/2*maxMinKChart[0]/(maxMinKChart[0]-maxMinKChart[1]);
    painter->drawLine(0,zeroLocation,width/2,zeroLocation);
    zeroLocation=height-40+325*maxMinHKChart[1]/(maxMinHKChart[0]-maxMinHKChart[1]);
    painter->drawLine(0,zeroLocation,width/2,zeroLocation);

    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawText(left,up,"实时资金流向图");
    painter->drawText(right,up,"实时成交分布图");
    painter->drawText(left,down,"盘后资金流向趋势");
    painter->drawText(right-60,down,"盘后资金流向统计(5日、20日)");

    painter->drawLine(0,(height-bottom)/2,width/2,(height-bottom)/2);
    painter->drawLine(width/2,0,width/2,(height-bottom)/2);
    painter->drawText(width/2,textHeight,GlobalVar::format_conversion(maxMinKChart[0]));
    painter->drawText(width/2,(height-bottom)/2,GlobalVar::format_conversion(maxMinKChart[1]));
    painter->drawLine(0,height-40,width/2,height-40);
    painter->drawLine(width/2,height-365,width/2,height-40);

    painter->drawText(width/2,height-345,GlobalVar::format_conversion(maxMinHKChart[0]));
    painter->drawText(width/2,height-40,GlobalVar::format_conversion(maxMinHKChart[1]));

    if (maxHNums!=0)
    {
        painter->drawText(0,height-40+textHeight,fundFlowHKList.at(0)[0].mid(5,5));
        painter->drawText((maxHNums-1)*aveHW-15,height-40+textHeight,fundFlowHKList.at(maxHNums-1)[0].mid(5,5));
    }
    for (int i=0;i<5;++i)
    {
        painter->setBrush(fColor[i]);
        painter->drawRect(150+150*i,(height-bottom)/2+80,20,20);
        painter->drawText(180+150*i,(height-bottom)/2+80,name[i]);
    }

    if (maxNums!=0)
    {
        painter->drawText(0,(height-bottom)/2+textHeight,fundFlowKList.at(0)[0].mid(11,5));
        painter->drawText(maxNums/2*aveW-15,(height-bottom)/2+textHeight,fundFlowKList.at(maxNums/2-1)[0].mid(11,5));
        painter->drawText(maxNums/4*aveW-15,(height-bottom)/2+textHeight,fundFlowKList.at(maxNums/4-1)[0].mid(11,5));
        painter->drawText(maxNums*3/4*aveW-15,(height-bottom)/2+textHeight,fundFlowKList.at(maxNums*3/4-1)[0].mid(11,5));
        painter->drawText((maxNums-1)*aveW-15,(height-bottom)/2+textHeight,fundFlowKList.at(maxNums-1)[0].mid(11,5));
    }

    for (int j=1;j<6;++j)
    {
        for (int i=1;i<maxNums;++i)
        {
            float p1=(maxMinKChart[0]-fundFlowKList.at(i-1)[j].toFloat())/interval*(height-bottom)/2;
            float p2=(maxMinKChart[0]-fundFlowKList.at(i)[j].toFloat())/interval*(height-bottom)/2;
            painter->setPen(QPen(fiveColor[j-1],2));
            painter->drawLine(QPointF((i-1)*aveW+leftOffset,p1),QPointF(i*aveW+leftOffset,p2));
        }
    }
    float total=0.0;
    float temp=0.0;
    float angle=0.0;
    float initAngle=90;
    for (int i=0;i<8;++i)
        total+=pieData[i];
    painter->setPen(Qt::white);
    for (int i=0;i<8;++i)
    {
        painter->setBrush(color[i]);
        if (total!=0)
            angle=pieData[i]/total*360;
        pointX[i]=width*5/8+leftOffset+width/8-qSin((temp+angle/2)*PI/180)*width/8;
        pointY[i]=(height-bottom)/13+width/8-qCos((temp+angle/2)*PI/180)*width/8;
        temp+=angle;
        degree[i]=temp;
        if (i==whichPiece-1)
        {
            painter->drawPie(width*5/8+leftOffset-25, (height-bottom)/13-25, width/4+50,width/4+50,initAngle*16,angle*16);
        }
        else
            painter->drawPie(width*5/8+leftOffset, (height-bottom)/13, width/4,width/4,initAngle*16,angle*16);
        initAngle=initAngle+angle;
    }

    float x[8]={640,560,520,605,836,898,892,821},y[8]={40,85,190,310,310,190,85,40};
    int t=85;
    QString na[8]={"超","大","中","小","小","中","大","超"};
    painter->setPen(Qt::red);
    for (int i=0;i<4;++i)
    {
        painter->drawText(x[i],y[i],na[i]+GlobalVar::format_conversion(pieData[i]));
        painter->drawLine(x[i]+t,y[i]-5,pointX[i],pointY[i]);
    }
    painter->setPen(QColor(0,191,0));
    for (int i=4;i<8;++i)
    {
        t=0;
        painter->drawText(x[i],y[i],na[i]+GlobalVar::format_conversion(-pieData[i]));
        painter->drawLine(x[i]+t,y[i]-5,pointX[i],pointY[i]);
    }

    interval=maxMinHKChart[0]-maxMinHKChart[1];
    int offset=110;
    for (int j=1;j<6;++j)
    {
        for (int i=1;i<maxHNums;++i)
        {
            float p1=(2*maxMinHKChart[0]-maxMinHKChart[1]-fundFlowHKList.at(i-1)[j].toFloat())/interval*(height-bottom)/2;
            float p2=(2*maxMinHKChart[0]-maxMinHKChart[1]-fundFlowHKList.at(i)[j].toFloat())/interval*(height-bottom)/2;
            painter->setPen(QPen(fiveColor[j-1],2));
            painter->drawLine(QPointF((i-1)*aveHW+leftOffset,p1+offset),QPointF(i*aveHW+leftOffset,p2+offset));
        }
    }

    for (int i=0;i<5;++i)
    {
        fiveTotal[i]=0.0;
        twentyTotal[i]=0.0;
    }
    float max=0.0;
    float min=0.0;
    int twentyN=20;
    int fiveN=5;
    if (fundFlowHKList.count()<twentyN)
        twentyN=fundFlowHKList.count();
    if (fundFlowHKList.count()<fiveN)
        fiveN=fundFlowHKList.count();
    for (int i=fundFlowHKList.count()-twentyN;i<fundFlowHKList.count();++i)
    {
        twentyTotal[0]+=fundFlowHKList.at(i)[1].toFloat();
        if (i>fundFlowHKList.count()-fiveN-1)
            fiveTotal[0]+=fundFlowHKList.at(i)[1].toFloat();
        for (int j=2;j<6;++j)
        {
            twentyTotal[6-j]+=fundFlowHKList.at(i)[j].toFloat();
            if (i>fundFlowHKList.count()-fiveN-1)
                fiveTotal[6-j]+=fundFlowHKList.at(i)[j].toFloat();
        }
    }
    for (int j=0;j<5;++j)
    {
        if (fiveTotal[j]>max)
            max=fiveTotal[j];
        else if (fiveTotal[j]<min)
            min=fiveTotal[j];
        if (twentyTotal[j]>max)
            max=twentyTotal[j];
        else if (twentyTotal[j]<min)
            min=twentyTotal[j];
    }
    float mid=0.0-min;
    interval=max-min;
    offset=140;
    for (int i=0;i<5;++i)
    {
        float five=fiveTotal[i]-min;
        float twenty=twentyTotal[i]-min;
        painter->setPen(QPen(fColor[i],1));
        painter->setBrush(fColor[i]);
        if (fiveTotal[i]<0)
            painter->drawRect(width/2+five/interval*(width/2-leftOffset)+5,(height-bottom)/2+offset+25*i,
                              (mid-five)/interval*(width/2-leftOffset),textHeight);
        else
            painter->drawRect(width/2+mid/interval*(width/2-leftOffset)+5,(height-bottom)/2+offset+25*i,
                              (five-mid)/interval*(width/2-leftOffset),textHeight);
        if (twentyTotal[i]<0)
            painter->drawRect(width/2+twenty/interval*(width/2-leftOffset)+5,(height+bottom)/2+offset+25*i,
                              (mid-twenty)/interval*(width/2-leftOffset),textHeight);
        else
            painter->drawRect(width/2+mid/interval*(width/2-leftOffset)+5,(height+bottom)/2+offset+25*i,
                              (twenty-mid)/interval*(width/2-leftOffset),textHeight);
    }
    if (isClick)
    {
        time->setText("一周5日资金流入");
        for (int i=0;i<5;++i)
        {
            if (fiveTotal[i]<0)
                textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
            else
                textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
            textFund[i]->setText(GlobalVar::format_conversion(fiveTotal[i]));
        }
        isClick=false;
    }
}

void FundFlow::getData(int days,const QByteArray &allData)
{
    QString v1="f3",v2="f62",v3="f184",v4="f66",v5="f69",v6="f72",v7="f75",
        v8="f78",v9="f81",v10="f84",v11="f87",v12="f204",v13="f12";
    if (days==5)
        v1="f109",v2="f164",v3="f165",v4="f166",v5="f167",v6="f168",v7="f169",
            v8="f170",v9="f171",v10="f172",v11="f173",v12="f257";
    else if (days==10)
        v1="f160",v2="f174",v3="f175",v4="f176",v5="f177",v6="f178",v7="f179",
            v8="f180",v9="f181",v10="f182",v11="f183",v12="f260";
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();

        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QStringList dataList;
            dataList<<ceilMap.value("f14").toString()<<ceilMap.value(v1).toString()
                    <<ceilMap.value(v2).toString()<<ceilMap.value(v3).toString()
                    <<ceilMap.value(v4).toString()<<ceilMap.value(v5).toString()
                    <<ceilMap.value(v6).toString()<<ceilMap.value(v7).toString()
                    <<ceilMap.value(v8).toString()<<ceilMap.value(v9).toString()
                    <<ceilMap.value(v10).toString()<<ceilMap.value(v11).toString()
                    <<ceilMap.value(v12).toString()<<ceilMap.value(v13).toString();
            FundFlowList.append(dataList);
        }
    }
}

void FundFlow::getBoardStock(QString name)
{
    QByteArray allData;
    QFile file(GlobalVar::currentPath+"/list/concept_industry_board.csv");
    if (file.open(QFile::ReadOnly))
    {
        QStringList data=QTextCodec::codecForName("GBK")->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
        for (int i=0;i<data.count();++i)
        {
            QStringList strLine = data.at(i).split(",");
            if (name==strLine[1])
            {
                GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=2000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&wbp2u=|0|0|0|web&fid=f3&fs=b:"+strLine[0]+"+f:!50&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1665552114833"));
                break;
            }
        }
    }
    file.close();
    GlobalVar::mTableList.clear();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();

            StockInfo info;
            info.code = ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toFloat();
            info.pctChg=ceilMap.value("f3").toFloat();
            info.turn=ceilMap.value("f8").toFloat();
            info.amount=ceilMap.value("f6").toFloat();
            info.velocity = ceilMap.value("f22").toFloat();
            info.pe = ceilMap.value("f9").toFloat();
            info.totalValue = ceilMap.value("f20").toFloat();
            info.circulatedValue=ceilMap.value("f21").toFloat();
            info.pctYear=ceilMap.value("f25").toFloat();
            info.pctSixty=ceilMap.value("f24").toFloat();
            info.volume = ceilMap.value("f5").toFloat();
            info.high = ceilMap.value("f15").toFloat();
            info.low = ceilMap.value("f16").toFloat();
            info.open=ceilMap.value("f17").toFloat();
            info.preClose=ceilMap.value("f18").toFloat();
            GlobalVar::mTableList.append(info);
        }
    }
}

void FundFlow::getIntervalHighLow()
{
    QStringList HighLowCol;
    HighLowCol<<"日期"<<"收盘价"<<"20日新高"<<"20日新低"<<"60日新高"<<"60日新低"<<"120日新高"<<"120日新低"
        <<""<<""<<"图"<<""<<""<<"表"<<""<<"";
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl("https://legulegu.com/stockdata/member-ship/get-high-low-statistics/all"));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    model->clear();
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonArray data = doc.array();
        QString name[]={"date","close","high20","low20","high60","low60","high120","low120"};
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            model->setItem(i,0,new QStandardItem(QDateTime::fromSecsSinceEpoch(ceilMap.value("date").toLongLong()/1000).toString("yyyy-MM-dd")));
            for (int j=1;j<8;++j)
                model->setItem(i,j,new QStandardItem(ceilMap.value(name[j]).toString()));
        }
        model->setItem(2,8,new QStandardItem());
        model->setItem(2,15,new QStandardItem());
    }
    model->setHorizontalHeaderLabels(HighLowCol);
}

void FundFlow::drawIntervalHighLowChart(QPainter *painter)
{
    painter->setPen(Qt::black);
    painter->drawRect(0,0,799,599);
    tableChart->setMaximumWidth(800);
    tableChart->setMaximumHeight(600);
    QColor color[6]={QColor(254,62,225),QColor(119,233,122),QColor(255,17,23),QColor(39,183,41),QColor(255,184,61),QColor(10,130,10)};
    QString name[6]={"20日新高","20日新低","60日新高","60日新低","120日新高","120日新低"};
    int maxNums=80;
    float aveW=tableChart->width()/maxNums;
    for (int i=0;i<6;++i)
    {
        painter->setBrush(color[i]);
        painter->drawRect(50+100*i,20,20,20);
        painter->drawText(75+100*i,35,name[i]);
    }
    painter->setPen(Qt::gray);
    painter->drawLine(0,tableChart->height()-30,tableChart->width(),tableChart->height()-30);
    painter->drawText(0,tableChart->height()-15,model->item(maxNums-1,0)->text().mid(5,5));
    painter->drawText(tableChart->width()-40,tableChart->height()-15,model->item(maxNums-80,0)->text().mid(5,5));
    for (int j=0;j<6;++j)
    {
        painter->setPen(QPen(color[j],2));
        if (isShow[j])
        {
            for (int i=1;i<maxNums;++i)
            {
                float p1=(1-model->item(maxNums-i,j+2)->text().toFloat()/4000)*tableChart->height();
                float p2=(1-model->item(maxNums-i-1,j+2)->text().toFloat()/4000)*tableChart->height();
                painter->drawLine(QPointF((i-1)*aveW,p1-50),QPointF(i*aveW,p2-50));
            }
        }
    }
}

void FundFlow::getStockPoolStrong(QString date)
{
    QStringList strongStockCol;
    strongStockCol<<"代码"<<"名称"<<"涨跌幅"<<"最新价"<<"涨停价"<<"成交量"<<"流通市值"<<"总市值"
                  <<"换手"<<"涨速"<<"是否新高"<<"量比"<<"涨停统计"<<"入选理由"<<"所属行业";
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl("https://push2ex.eastmoney.com/getTopicQSPool/get?ut=7eea3edcaed734bea9cbfc24409ed989&dpt=wz.ztzt&Pageindex=0&pagesize=170&sort=zdp:desc&date="+date+"&_=1621590489736"));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("pool").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            auto t=ceilMap.value("zttj").toJsonValue().toVariant().toMap().value("days");
            int n=t.toInt();
            QString s=t.toString()+"/"+ceilMap.value("zttj").toJsonValue().toVariant().toMap().value("ct").toString();
            model->setItem(i,0,new QStandardItem(ceilMap.value("c").toString()));
            model->setItem(i,1,new QStandardItem(ceilMap.value("n").toString()));
            model->setItem(i,2,new QStandardItem(QString::number(ceilMap.value("zdp").toFloat(),'f',2)));
            model->item(i,1)->setFont(boldFont);
            model->item(i,1)->setForeground(QColor(72,61,139));
            model->item(i,2)->setFont(boldFont);
            model->item(i,2)->setForeground(Qt::red);
            model->setItem(i,3,new QStandardItem(QString::number(ceilMap.value("p").toFloat()/1000)));
            model->setItem(i,4,new QStandardItem(QString::number(ceilMap.value("ztp").toFloat()/1000)));
            float a=ceilMap.value("amount").toFloat();
            model->setItem(i,5,new QStandardItem(GlobalVar::format_conversion(a)));
            if (a>= 1000000000)
                model->item(i,5)->setForeground(QColor(153, 0, 153));
            else if (a>=300000000)
                model->item(i,5)->setForeground(QColor(0, 191, 255));
            model->setItem(i,6,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("ltsz").toFloat())));
            model->setItem(i,7,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("tshare").toFloat())));
            a=ceilMap.value("hs").toFloat();
            model->setItem(i,8,new QStandardItem(QString::number(a,'f',2)));
            if (a>=15)
                model->item(i,8)->setForeground(QColor(204, 204, 0));
            model->setItem(i,9,new QStandardItem(QString::number(ceilMap.value("zs").toFloat(),'f',2)));
            model->setItem(i,10,new QStandardItem(ceilMap.value("nh").toString()));
            model->setItem(i,11,new QStandardItem(QString::number(ceilMap.value("lb").toFloat(),'f',2)));
            model->setItem(i,12,new QStandardItem(s));
            if (n>2)
            {
                model->item(i,12)->setForeground(Qt::red);
                model->item(i,12)->setFont(boldFont);
            }
            model->setItem(i,13,new QStandardItem(ceilMap.value("cc").toString()));
            model->setItem(i,14,new QStandardItem(ceilMap.value("hybk").toString()));
        }

    }
    model->setHorizontalHeaderLabels(strongStockCol);
}

void FundFlow::getNorthFundFlow(QString days)
{
    QStringList northFundFlowCol;
    northFundFlowCol<<"代码"<<"名称"<<"收盘价"<<"涨跌幅"<<"持股数"<<"持市值"<<"持占流通股比"<<"持占总股本比"
                   <<"增股数"<<"增市值"<<"增市值增幅"<<"增占流通股比"<<"增占总股本比"<<"所属板块";
    QString curDate=GlobalVar::curRecentWorkDay(2).date().toString("yyyy-MM-dd");
    QByteArray allData;
    GlobalVar::getData(allData,3,QUrl("https://datacenter-web.eastmoney.com/api/data/v1/get?sortColumns=ADD_MARKET_CAP&sortTypes=-1&pageSize=5000&pageNumber=1&reportName=RPT_MUTUAL_STOCK_NORTHSTA&columns=ALL&source=WEB&client=WEB&filter=(TRADE_DATE%3D%27"+curDate+"%27)(INTERVAL_TYPE%3D%22"+days+"%22)"));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            model->setItem(i,0,new QStandardItem(ceilMap.value("SECURITY_CODE").toString()));
            model->setItem(i,1,new QStandardItem(ceilMap.value("SECURITY_NAME").toString()));
            model->setItem(i,2,new QStandardItem(ceilMap.value("CLOSE_PRICE").toString()));
            model->item(i,1)->setFont(boldFont);
            model->item(i,1)->setForeground(QColor(72,61,139));
            QString s=ceilMap.value("CHANGE_RATE").toString();
            model->setItem(i,3,new QStandardItem(s));
            model->item(i,3)->setFont(boldFont);
            if (s.left(1)=="-")
            {
                model->item(i,2)->setForeground(QColor(0,191,0));
                model->item(i,3)->setForeground(QColor(0,191,0));
            }
            else
            {
                model->item(i,2)->setForeground(Qt::red);
                model->item(i,3)->setForeground(Qt::red);
            }

            if (ceilMap.value("HOLD_SHARES").toFloat()>=10000)
                s=QString::number(ceilMap.value("HOLD_SHARES").toFloat()/10000,'f',2)+"亿";
            else
                s=ceilMap.value("HOLD_SHARES").toString()+"万";
            model->setItem(i,4,new QStandardItem(s));
            model->setItem(i,5,new QStandardItem(QString::number(ceilMap.value("HOLD_MARKET_CAP").toFloat()/10000,'f',2)+"亿"));
            model->setItem(i,6,new QStandardItem(ceilMap.value("FREE_SHARES_RATIO").toString()));
            model->setItem(i,7,new QStandardItem(ceilMap.value("TOTAL_SHARES_RATIO").toString()));
            model->setItem(i,8,new QStandardItem(ceilMap.value("ADD_SHARES_REPAIR").toString()+"万"));
            model->setItem(i,9,new QStandardItem(QString::number(ceilMap.value("ADD_MARKET_CAP").toFloat()/10000,'f',2)+"亿"));
            model->setItem(i,10,new QStandardItem(ceilMap.value("ADD_SHARES_AMP").toString()));
            model->setItem(i,11,new QStandardItem(ceilMap.value("FREECAP_RATIO_CHG").toString()));
            model->setItem(i,12,new QStandardItem(ceilMap.value("TOTAL_RATIO_CHG").toString()));
            model->setItem(i,13,new QStandardItem(ceilMap.value("INDUSTRY_NAME").toString()));
        }
    }
    model->setHorizontalHeaderLabels(northFundFlowCol);
}

void FundFlow::getDragonTigerList(int nums,int pages)
{
    QStringList dragonTigerCol;
    dragonTigerCol<<"代码"<<"名称"<<"解读"<<"收盘价"<<"涨跌幅"<<"净买额"<<"买入额"<<"卖出额"
        <<"成交额"<<"总成交额"<<"净买占总比"<<"成交占总比"<<"换手率"<<"流通市值"<<"上榜原因";
    if (nums!=1)
        dragonTigerCol<<"上榜日";
    QString StartDate;
    QString endDate;
    QDateTime curDate=QDateTime::currentDateTime();
    int counts=0;
    for (int i=0;i<60;++i)
    {
        QDateTime t=curDate.addDays(-i);
        if (GlobalVar::isZhWorkDay(t,1))
        {
            counts+=1;
            if (counts==1)
                endDate=t.date().toString("yyyy-MM-dd");
            if (counts==nums)
            {
                StartDate=t.date().toString("yyyy-MM-dd");
                break;
            }
        }
    }
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    for (int j=1;j<pages+1;++j)
    {
        QByteArray allData;
        GlobalVar::getData(allData,2,QUrl("https://datacenter-web.eastmoney.com/api/data/v1/get?sortColumns=BILLBOARD_NET_AMT%2CTRADE_DATE%2CSECURITY_CODE&sortTypes=-1%2C-1%2C1&pageSize=500&pageNumber="+QString::number(j)+"&reportName=RPT_DAILYBILLBOARD_DETAILSNEW&columns=SECURITY_CODE%2CSECUCODE%2CSECURITY_NAME_ABBR%2CTRADE_DATE%2CEXPLAIN%2CCLOSE_PRICE%2CCHANGE_RATE%2CBILLBOARD_NET_AMT%2CBILLBOARD_BUY_AMT%2CBILLBOARD_SELL_AMT%2CBILLBOARD_DEAL_AMT%2CACCUM_AMOUNT%2CDEAL_NET_RATIO%2CDEAL_AMOUNT_RATIO%2CTURNOVERRATE%2CFREE_MARKET_CAP%2CEXPLANATION%2CD1_CLOSE_ADJCHRATE%2CD2_CLOSE_ADJCHRATE%2CD5_CLOSE_ADJCHRATE%2CD10_CLOSE_ADJCHRATE%2CSECURITY_TYPE_CODE&source=WEB&client=WEB&filter=(TRADE_DATE%3C%3D%27"+endDate+"%27)(TRADE_DATE%3E%3D%27"+StartDate+"%27)"));
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
            int n=model->rowCount();
            for (int i = 0; i < data.size(); ++i)
            {
                QJsonValue value = data.at(i);
                QVariantMap ceilMap = value.toVariant().toMap();
                model->setItem(i+n,0,new QStandardItem(ceilMap.value("SECURITY_CODE").toString()));
                model->setItem(i+n,1,new QStandardItem(ceilMap.value("SECURITY_NAME_ABBR").toString()));
                model->setItem(i+n,2,new QStandardItem(ceilMap.value("EXPLAIN").toString()));
                model->setItem(i+n,3,new QStandardItem(ceilMap.value("CLOSE_PRICE").toString()));
                float c=ceilMap.value("CHANGE_RATE").toFloat();
                model->setItem(i+n,4,new QStandardItem(GlobalVar::format_conversion(c)));
                if (c<0)
                {
                    model->item(i+n,3)->setForeground(QColor(0, 191, 0));
                    model->item(i+n,4)->setForeground(QColor(0, 191, 0));
                }
                else if (c>0)
                {
                    model->item(i+n,3)->setForeground(Qt::red);
                    model->item(i+n,4)->setForeground(Qt::red);
                }
                c=ceilMap.value("BILLBOARD_NET_AMT").toFloat();
                model->setItem(i+n,5,new QStandardItem(GlobalVar::format_conversion(c)));
                if (c<0)
                    model->item(i+n,5)->setForeground(QColor(0, 191, 0));
                else if (c>0)
                    model->item(i+n,5)->setForeground(Qt::red);
                model->item(i,1)->setFont(boldFont);
                model->item(i,1)->setForeground(QColor(72,61,139));
                model->item(i,4)->setFont(boldFont);
                model->setItem(i+n,6,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_BUY_AMT").toFloat())));
                model->item(i+n,6)->setForeground(Qt::red);
                model->setItem(i+n,7,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_SELL_AMT").toFloat())));
                model->item(i+n,7)->setForeground(QColor(0, 191, 0));
                model->setItem(i+n,8,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_DEAL_AMT").toFloat())));
                model->setItem(i+n,9,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("ACCUM_AMOUNT").toFloat())));
                model->setItem(i+n,10,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("DEAL_NET_RATIO").toFloat())));
                model->setItem(i+n,11,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("DEAL_AMOUNT_RATIO").toFloat())));
                float a=ceilMap.value("TURNOVERRATE").toFloat();
                model->setItem(i+n,12,new QStandardItem(GlobalVar::format_conversion(a)));
                if (a>=15)
                    model->item(i+n,12)->setForeground(QColor(204, 204, 0));
                model->setItem(i+n,13,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("FREE_MARKET_CAP").toFloat())));
                model->setItem(i+n,14,new QStandardItem(ceilMap.value("EXPLANATION").toString()));
                if (nums!=1)
                    model->setItem(i+n,15,new QStandardItem(ceilMap.value("TRADE_DATE").toString()));
            }
        }
        model->setHorizontalHeaderLabels(dragonTigerCol);
    }
}

void FundFlow::countSingleStockBoard(QString nums,int pages)
{
    QStringList everyStockCol;
    everyStockCol<<"代码"<<"名称"<<"最近上榜日"<<"收盘价"<<"涨跌幅"<<"次数"<<"净买额"<<"买入额"<<"卖出额"
                <<"总成交额"<<"机构买次"<<"机构卖次"<<"买入净额"<<"买入总额"<<"卖出总额"
                  <<"1个月P"<<"3个月P"<<"6个月P"<<"1年P";
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    for (int i=1;i<pages+1;++i)
    {
        QByteArray allData;
        GlobalVar::getData(allData,2,QUrl("https://datacenter-web.eastmoney.com/api/data/v1/get?sortColumns=BILLBOARD_TIMES%2CLATEST_TDATE%2CSECURITY_CODE&sortTypes=-1%2C-1%2C1&pageSize=1000&pageNumber="+QString::number(i)+"&reportName=RPT_BILLBOARD_TRADEALLNEW&columns=ALL&source=WEB&client=WEB&filter=(STATISTICS_CYCLE%3D%22"+nums+"%22)"));
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
            int n=model->rowCount();
            QString name[]={"IPCT1M","IPCT3M","IPCT6M","IPCT1Y"};
            for (int i = 0; i < data.size(); ++i)
            {
                QJsonValue value = data.at(i);
                QVariantMap ceilMap = value.toVariant().toMap();
                model->setItem(i+n,0,new QStandardItem(ceilMap.value("SECURITY_CODE").toString()));
                model->setItem(i+n,1,new QStandardItem(ceilMap.value("SECURITY_NAME_ABBR").toString()));
                model->setItem(i+n,2,new QStandardItem(ceilMap.value("LATEST_TDATE").toString()));
                model->setItem(i+n,3,new QStandardItem(ceilMap.value("CLOSE_PRICE").toString()));
                float c=ceilMap.value("CHANGE_RATE").toFloat();
                model->setItem(i+n,4,new QStandardItem(QString::number(c,'f',2)));
                if (c<0)
                {
                    model->item(i+n,3)->setForeground(QColor(0, 191, 0));
                    model->item(i+n,4)->setForeground(QColor(0, 191, 0));
                }
                else if (c>0)
                {
                    model->item(i+n,3)->setForeground(Qt::red);
                    model->item(i+n,4)->setForeground(Qt::red);
                }

                model->setItem(i+n,5,new QStandardItem(ceilMap.value("BILLBOARD_TIMES").toString()));
                model->item(i,1)->setFont(boldFont);
                model->item(i,1)->setForeground(QColor(72,61,139));
                model->item(i,4)->setFont(boldFont);
                model->item(i,5)->setFont(boldFont);
                float b=ceilMap.value("BILLBOARD_NET_BUY").toFloat();
                model->setItem(i+n,6,new QStandardItem(GlobalVar::format_conversion(b)));
                if (b>0)
                    model->item(i+n,6)->setForeground(Qt::red);
                else if (b<0)
                    model->item(i+n,6)->setForeground(QColor(0, 191, 0));
                model->setItem(i+n,7,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_BUY_AMT").toFloat())));
                model->item(i+n,7)->setForeground(Qt::red);
                model->setItem(i+n,8,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_SELL_AMT").toFloat())));
                model->item(i+n,8)->setForeground(QColor(0, 191, 0));
                model->setItem(i+n,9,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("BILLBOARD_DEAL_AMT").toFloat())));
                model->setItem(i+n,10,new QStandardItem(ceilMap.value("ORG_BUY_TIMES").toString()));
                model->setItem(i+n,11,new QStandardItem(ceilMap.value("ORG_SELL_TIMES").toString()));
                b=ceilMap.value("ORG_NET_BUY").toFloat();
                model->setItem(i+n,12,new QStandardItem(GlobalVar::format_conversion(b)));
                if (b>0)
                    model->item(i+n,12)->setForeground(Qt::red);
                else if (b<0)
                    model->item(i+n,12)->setForeground(QColor(0, 191, 0));
                model->setItem(i+n,13,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("ORG_BUY_AMT").toFloat())));
                model->item(i+n,13)->setForeground(Qt::red);
                model->setItem(i+n,14,new QStandardItem(GlobalVar::format_conversion(ceilMap.value("ORG_SELL_AMT").toFloat())));
                model->item(i+n,14)->setForeground(QColor(0, 191, 0));

                for (int j=0;j<4;++j)
                {
                    b=ceilMap.value(name[j]).toFloat();
                    model->setItem(i+n,j+15,new QStandardItem(GlobalVar::format_conversion(b)));
                    if (b>=100)
                        model->item(i+n,j+15)->setForeground(QColor(153, 0, 153));
                    else if (b>0)
                        model->item(i+n,j+15)->setForeground(Qt::red);
                    else if (b<0)
                        model->item(i+n,j+15)->setForeground(QColor(0, 191, 0));
                }
            }
        }
        model->setHorizontalHeaderLabels(everyStockCol);
    }
}

void FundFlow::getStockHot()
{
    QJsonObject json;
    json.insert("appId", "appId01");
    json.insert("globalId", "786e4c21-70dc-435a-93bb-38");
    json.insert("pageNo", "1");
    json.insert("marketType", "");
    json.insert("pageSize", "100");

    QJsonDocument doc;
    doc.setObject(json);
    QByteArray dataArray = doc.toJson(QJsonDocument::Compact);
    QByteArray allData;
    GlobalVar::postData(dataArray,allData,1,QUrl("https://emappdata.eastmoney.com/stockrank/getAllCurrentList"));
    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(allData, &jsonError);
    QStringList stockHotCol;
    stockHotCol<<"代码"<<"名称"<<"收盘价"<<"涨跌幅"<<"排名";
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toArray();
        int n=GlobalVar::mTableListCopy.count();
        if (n==0)
            return;
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QString code=ceilMap.value("sc").toString().right(6);
            QString rank=ceilMap.value("rk").toString();
            int l = 0;
            int r = n - 1;
            while (l <= r)
            {
                int mid = (l + r) / 2;
                if (GlobalVar::mTableListCopy.at(mid).code == code)
                {
                    model->setItem(i,0,new QStandardItem(GlobalVar::mTableListCopy.at(mid).code));
                    model->setItem(i,1,new QStandardItem(GlobalVar::mTableListCopy.at(mid).name));
                    model->setItem(i,2,new QStandardItem(QString::number(GlobalVar::mTableListCopy.at(mid).close)));
                    float p=GlobalVar::mTableListCopy.at(mid).pctChg;
                    model->setItem(i,3,new QStandardItem(QString::number(p)));
                    if (p<0)
                    {
                        model->item(i,2)->setForeground(QColor(0, 191, 0));
                        model->item(i,3)->setForeground(QColor(0, 191, 0));
                    }
                    else if (p>0)
                    {
                        model->item(i,2)->setForeground(Qt::red);
                        model->item(i,3)->setForeground(Qt::red);
                    }
                    model->setItem(i,4,new QStandardItem(rank));
                    break;
                }
                else if (GlobalVar::mTableListCopy.at(mid).code > code)
                    r = mid - 1;
                else
                    l = mid + 1;
            }
            model->item(i,1)->setFont(boldFont);
            model->item(i,1)->setForeground(QColor(72,61,139));
            model->item(i,3)->setFont(boldFont);
        }
        model->setHorizontalHeaderLabels(stockHotCol);
    }
}

void FundFlow::openFundRank(QString ft,QString sc)
{
    QDateTime curtime=QDateTime::currentDateTime();
    QString sd=curtime.addYears(-1).toString("yyyy-MM-dd");
    QString ed=curtime.toString("yyyy-MM-dd");
    QNetworkRequest request;
    request.setRawHeader("referer", "http://fund.eastmoney.com/fundguzhi.html");
    request.setUrl("https://fund.eastmoney.com/data/rankhandler.aspx?op=ph&dt=kf&ft="+ft+"&rs=&gs=0&sc="+sc+"&st=desc&sd="+sd+"&ed="+ed+"&qdii=&tabSubtype=,,,,,&pi=1&pn=20000&dx=1&v=0.1591891419018292");
    QByteArray allData;
    GlobalVar::getData(allData,8,request);
    QJsonParseError jsonError;
    allData=allData.mid(22,allData.lastIndexOf("]")-21);
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    QStringList openFundCol;
    openFundCol<<"代码"<<"基金名称"<<"日期"<<"单位净值"<<"累计净值"<<"日增长率"<<"近一周"<<"近一月"<<"近3月"
        <<"近6月"<<"近1年"<<"近2年"<<"近3年"<<"今年来"<<"成立来"<<"手续费";
    model->clear();
    QFont boldFont = QFont();
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonArray data = doc.array();
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList str=data[i].toString().split(",");
            model->setItem(i,0,new QStandardItem(str[0]));
            model->setItem(i,1,new QStandardItem(str[1]));
            model->setItem(i,2,new QStandardItem(str[3]));
            model->setItem(i,3,new QStandardItem(str[4]));
            model->setItem(i,4,new QStandardItem(str[5]));
            model->item(i,1)->setFont(boldFont);
            model->item(i,1)->setForeground(QColor(72,61,139));
            model->item(i,4)->setFont(boldFont);
            model->item(i,3)->setFont(boldFont);
            for (int j=5;j<15;++j)
            {
                QString s=str[j+1];
                if (s=="")
                    s="......";
                else
                    s+="%";
                model->setItem(i,j,new QStandardItem(s));
                if (s.left(1)=="-")
                    model->item(i,j)->setForeground(QColor(0, 191, 0));
                else
                    model->item(i,j)->setForeground(Qt::red);
            }
            model->setItem(i,15,new QStandardItem(str[20]));
        }
    }
    model->setHorizontalHeaderLabels(openFundCol);
}

void FundFlow::getRoyalFlushFundFlow()
{
    QByteArray allData;
    QStringList RoyalFlushCol;
    RoyalFlushCol<<"行业"<<"行业指数"<<"涨跌幅"<<"流入资金"<<"流出资金"<<"净额"<<"公司家数"<<"领涨股"<<"涨跌幅"<<"当前价";
    QJSEngine myEngine;
    QString fileName = "/list/ths.js";
    QFile scriptFile(GlobalVar::currentPath+fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
        return;
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    myEngine.evaluate(contents);
    QJSValue func;
    func = myEngine.globalObject().property("v");

    QNetworkRequest request;
//    QNetworkAccessManager naManager =QNetworkAccessManager();
    model->clear();
    model->setHorizontalHeaderLabels(RoyalFlushCol);

    QString url[]={"http://data.10jqka.com.cn/funds/gnzjl/field/tradezdf/order/desc/page/",
                    "http://data.10jqka.com.cn/funds/hyzjl/field/tradezdf/order/desc/page/"};
    int page[]={7,2};
    //    request.setRawHeader("referer","http://data.10jqka.com.cn/funds/gnzjl/");
//    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    int len=sizeof(url)/sizeof(url[0]);
    for (int n=0;n<len;++n)
    {
        for (int i=1;i<=page[n];++i)
        {
            request.setRawHeader("hexin-v", func.call().toString().toLocal8Bit());
            request.setUrl(QUrl(url[n]+QString::number(i)+"/ajax/1/free/1/"));
            GlobalVar::getData(allData,1,request);

            QString html=QTextCodec::codecForName("GBK")->toUnicode(allData);
            QString str=GlobalVar::peelStr(html,"<tbody","-1");

            QPair<QString, QString> pair;
            int n=model->rowCount();
            int m=0;
            while(1)
            {
                if (str.indexOf("<tr")==-1)
                    break;
                pair=GlobalVar::cutStr(str,"<tr","</tr>");
                str=pair.second;
                QStringList l;
                GlobalVar::getAllContent(pair.first,l,"<td");
//                QString p;
                for (int j=0;j<10;++j)
                {
//                    if (j==2)
//                    {
//                        p=l[j+1].split("%")[0];
//                        model->setItem(m+n,j,new QStandardItem(p));
//                    }
//                    else
                        model->setItem(m+n,j,new QStandardItem(l[j+1]));
                }
                m+=1;
            }
        }
    }
    model->sort(2,Qt::DescendingOrder);
}

void FundFlow::getNotNormalStock()
{
//    getHisTimeShareTick();
    QStringList NotNormalStock;
    NotNormalStock<<"代码"<<"名称"<<"时间"<<"类型"<<"相关信息";
    QByteArray allData;
    QString url="https://push2ex.eastmoney.com/getAllStockChanges?type=8201,8202,8193,4,32,64,8207,8209,8211,8213,8215,8204,8203,8194,8,16,128,8208,8210,8212,8214,8216&ut=7eea3edcaed734bea9cbfc24409ed989&pageindex=0&pagesize=10000&dpt=wzchanges&_=1692089577464";
    GlobalVar::getData(allData,2,QUrl(url));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    QString name[22]={"火箭发射","快速反弹","高台跳水","加速下跌","大笔买入","大笔卖出","封涨停板","封涨停板",
        "打开跌停板","打开涨停板","竞价下跌","竞价上涨","有大卖盘","有大买盘","高开5日线","低开5日线",
        "向上缺口","向下缺口","60日新高","60日新低","60日大幅上涨","60日大幅下跌"};
    QString code[22]={"8201","8202","8203","8204","8193","8194","4","8",
        "32","16","8208","8207","128","64","8209","8210",
        "8211","8212","8213","8214","8215","8216"};
    int len=sizeof(code)/sizeof(code[0]);
    model->clear();
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("allstock").toArray();

        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
//            QStringList list;
            model->setItem(i,0,new QStandardItem(ceilMap.value("c").toString()));
            model->setItem(i,1,new QStandardItem(ceilMap.value("n").toString()));
            model->setItem(i,2,new QStandardItem(ceilMap.value("tm").toString()));
//            list<<ceilMap.value("tm").toString()<<ceilMap.value("c").toString()
//                 <<ceilMap.value("n").toString();
            for (int j=0;j<len;++j)
            {
                if (code[j]==ceilMap.value("t").toString())
                {
                    model->setItem(i,3,new QStandardItem(name[j]));
                    model->setItem(i,4,new QStandardItem(ceilMap.value("i").toString().split(",")[0]));
//                    list<<name[i]<<ceilMap.value("i").toString().split(",");
                    break;
                }
            }
//            qDebug()<<list;
        }
    model->setHorizontalHeaderLabels(NotNormalStock);
    }
}

void FundFlow::getHisTimeShareTick()
{
    QNetworkRequest request;
    int page=1;
    QList<QStringList> data;
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    while(1)
    {
        QByteArray allData;
        QString url="https://market.finance.sina.com.cn/transHis.php?symbol=sz301205&date=2022-11-28&page="+QString::number(page);
        request.setUrl(QUrl(url));
        GlobalVar::getData(allData,2,request);
        if (allData.isEmpty())
            break;
        QString html=codec->toUnicode(allData);
        QString str=GlobalVar::peelStr(html,"<tbody>","-1");
        while(1)
        {
            if (str.indexOf("<tr")==-1)
                break;
            QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
            QString s=GlobalVar::peelStr(pair.first,"<tr","-1");
            QStringList l;
            QStringList list;
            GlobalVar::getAllContent(s,l,"<t");
            QString d;
            if (l[5]=="卖盘")
                d="-1";
            else if (l[5]=="买盘")
                d="1";
            else
                d="0";
            list<<l[0]<<l[1]<<l[3]<<d;
            qDebug()<<list;
            data.append(list);
            str=pair.second;
        }
        page+=1;
    }
}

void FundFlow::getTimeShareMin(QString code,QString date)
{
    QJSEngine myEngine;
    QString fileName = "/list/sf_sdk.js";
    QFile scriptFile(GlobalVar::currentPath+fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
        return;
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    myEngine.evaluate(contents);
    QJSValue func;
    func = myEngine.globalObject().property("decode");

    QByteArray allData;
//    QNetworkAccessManager naManager =QNetworkAccessManager();
    QString url="https://finance.sina.com.cn/realstock/company/"+code+"/hisdata/"+date.left(4)+"/"+date.mid(5,2)+".js?d="+date;
    GlobalVar::getData(allData,2,QUrl(url));

    QString s=QString(allData).split(";")[0];
    int pos=s.indexOf("\"")+1;
    QString str=s.mid(pos,s.size()-pos-1);

    QStringList ss=str.split(",");
    QList<timeShartChartInfo> hisTimeShareChartList;
    for (int i=0;i<ss.count();++i)
    {
        QJSValueList args;
        args << ss[i];
        QJSValue funcValue=func.call(args);

        QString date1=funcValue.property(0).property("date").toString();
//        qDebug()<<date1;
        if (date!=QDateTime::fromString(date1).addSecs(28800).toString("yyyy-MM-dd"))
            continue;
        int l=funcValue.property("length").toInt();
        GlobalVar::hisPreClose=funcValue.property(0).property("prevclose").toNumber();
        QDateTime curTime=QDateTime::fromString("2023-06-05 09:25", "yyyy-MM-dd hh:mm");
        QString d=date.left(10);
        for (int j=1;j<l;++j)
        {
            timeShartChartInfo info;
            info.price=funcValue.property(j).property("price").toNumber();
            info.vol=funcValue.property(j).property("volume").toNumber()/100;
            info.avePrice=funcValue.property(j).property("avg_price").toNumber();
            if (j==1)
            {
                info.time=d+" "+curTime.toString("hh:mm");
            }
            else if (j<=122)
                info.time=d+" "+curTime.addSecs(300+(j-2)*60).toString("hh:mm");
            else
                info.time=d+" "+curTime.addSecs(5700+(j-2)*60).toString("hh:mm");
            hisTimeShareChartList.append(info);
        }
        GlobalVar::mHisTimeShareChartList=hisTimeShareChartList;
//        for (int i=0;i<GlobalVar::mHisTimeShareChartList.count();++i)
//            qDebug()<<GlobalVar::mHisTimeShareChartList.at(i).price<<GlobalVar::mHisTimeShareChartList.at(i).vol;
    }
}

void FundFlow::initAllNews()
{
    if (isGetNews)
        return;
    isGetNews=true;
    GlobalVar::annoucementList.clear();
    if (GlobalVar::curCode.left(1)!="1" and GlobalVar::curCode.left(3)!="399" and GlobalVar::curCode.length()!=5)
    {
        getAnnoucement();
        getGreatEvent();
//        getNews();
    }
    getEastNews();
    QStringList list;
    list<<""<<"[结束]"<<"(2000-01-01)";
    GlobalVar::annoucementList.append(list);
    std::sort(GlobalVar::annoucementList.begin(),GlobalVar::annoucementList.end(),[](QStringList a,QStringList b){
        return a[2]>b[2];
    });
    isGetNews=false;
    //    for (int i=0;i<GlobalVar::annoucementList.count();++i)
    //        qDebug()<<GlobalVar::annoucementList.at(i)[2];
}

void FundFlow::getAnnoucement()
{
    QByteArray allData;
    QNetworkRequest request;
    QString url="https://np-anotice-stock.eastmoney.com/api/security/ann?sr=-1&page_size=50&page_index=1&ann_type=A&client_source=web&stock_list="+GlobalVar::curCode+"&f_node=0&s_node=0";
//    QString url="http://ddx.gubit.cn/gonggao/"+GlobalVar::curCode;
    request.setUrl(QUrl(url));
    GlobalVar::getData(allData,2,request);
    if (allData.isEmpty())
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("list").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList l;
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            l<<ceilMap.value("title").toString()<<"[公告]"<<"("+ceilMap.value("notice_date").toString()
              <<"https://data.eastmoney.com/notices/detail/"+GlobalVar::curCode+"/"+ceilMap.value("art_code").toString()+".html";
            GlobalVar::annoucementList.append(l);
        }
    }

//    QTextCodec *codec = QTextCodec::codecForName("GBK");
//    QString html=codec->toUnicode(allData);
//    QString str=GlobalVar::peelStr(html,"<tbody>","-1");
//    while(1)
//    {
//        if (str.indexOf("<tr")==-1)
//            break;
//        QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
//        QString s=GlobalVar::peelStr(pair.first,"<tr","-1");
//        QStringList list;
//        QStringList l;
//        QString href=GlobalVar::getAttributeContent(s,"href","\"");
//        GlobalVar::getAllContent(s,l,"<td");
//        if (l.size()>2)
//        {
//            list<<l[1]<<"[公告]"<<"("+l[2]+")"<<url+href.mid(1,-1);
////            qDebug()<<list;
//            GlobalVar::annoucementList.append(list);
//        }
//        str=pair.second;
//    }
}

void FundFlow::getNews()
{
    QString s=GlobalVar::getCookies("http://www.stcn.com/article/search.html");
    QString name1="acw_tc=";
    QString name2="advanced-stcn_web=";
    int BPos1=s.indexOf(name1)+name1.length();
    int BPos2=s.indexOf(name2)+name2.length();
    QString acw=s.mid(BPos1,s.indexOf(";")-BPos1);
    QString advanced=s.mid(BPos2,s.indexOf(";",BPos2)-BPos2);

    QNetworkRequest request;
    QByteArray allData;
    request.setRawHeader("x-requested-with","XMLHttpRequest");
    QList<QNetworkCookie> Web_cookies;
    Web_cookies.push_back(QNetworkCookie("acw_tc",acw.toLocal8Bit()));
    Web_cookies.push_back(QNetworkCookie("advanced-stcn_web",advanced.toLocal8Bit()));
    QVariant var;
    var.setValue(Web_cookies);
    request.setHeader(QNetworkRequest::CookieHeader,var);

//    QString url1="https://search-api-web.eastmoney.com/search/jsonp?cb=&param=%7B%22uid%22%3A%227111416627128474%22%2C%22keyword%22%3A%22%E4%B8%AD%E9%99%85%E6%97%AD%E5%88%9B%22%2C%22type%22%3A%5B%22cmsArticleWebOld%22%5D%2C%22client%22%3A%22web%22%2C%22clientType%22%3A%22web%22%2C%22clientVersion%22%3A%22curr%22%2C%22param%22%3A%7B%22cmsArticleWebOld%22%3A%7B%22searchScope%22%3A%22default%22%2C%22sort%22%3A%22default%22%2C%22pageIndex%22%3A1%2C%22pageSize%22%3A50%2C%22preTag%22%3A%22%3Cem%3E%22%2C%22postTag%22%3A%22%3C%2Fem%3E%22%7D%7D%7D&_=1686979606619";
    for (int i=1;i<2;++i)
    {
//        QString url="https://search.cnstock.com/search/result/"+QString::number(i)+"?t=0&k="+GlobalVar::curName.left(GlobalVar::curName.indexOf("("));
        QString url="http://www.stcn.com/article/search.html?search_type=news&keyword="+GlobalVar::curCode+"&page_time="+QString::number(i);
        request.setUrl(QUrl(url));
        GlobalVar::getData(allData,3,request);
        if (allData.isEmpty())
            return;
        QString html=QString(allData);
        QString str=GlobalVar::peelStr(html,"<ul class=\"list infinite-list\"","-1");
        while(1)
        {
            if (str.indexOf("<li class=")==-1)
                break;
            QPair<QString, QString> pair=GlobalVar::cutStr(str,"<li","</li");
            QString s=GlobalVar::peelStr(pair.first,"<li","-1");
            QStringList list;
            QStringList l;
            QString href=GlobalVar::getAttributeContent(s,"href","\\\"");
            GlobalVar::getAllContent(s,l,"<a");
            GlobalVar::getAllContent(s,l,"<span");
            QString t=l[l.count()-1];
            if (t.contains("\\n"))
                t=GlobalVar::annoucementList.back()[2];
            else if (t.length()<13)
                t="("+QDateTime::currentDateTime().toString("yyyy")+"-"+t+")";
            else
                t="("+t+")";
            list<<l[0].replace("\\n","").replace(" ","")<<"[证券时报]"<<t<<href.replace("\"","https://www.stcn.com");
            GlobalVar::annoucementList.append(list);
            str=pair.second;
        }

//        QString str=GlobalVar::peelStr(html,"<div class=\"result-left\"","-1");
//        while(1)
//        {
//            if (str.indexOf("<div class=\"result-article\"")==-1)
//                break;
//            QPair<QString, QString> pair=GlobalVar::cutStr(str,"<div","</div");
//            QString s=GlobalVar::peelStr(pair.first,"<div","-1");
//            QStringList list;
//            QStringList l;
//            GlobalVar::getAllContent(s,l,"<a");
//            GlobalVar::getAllContent(s,l,"<span");
//            list<<l[0]<<l[1];
//            QStringList s1=l[2].split(";");
//            list<<"("+s1[1]+")";
//            list<<s1[0].split("&")[0];
//            GlobalVar::annoucementList.append(list);
//            str=pair.second;
//        }
    }
}

void FundFlow::getEastNews()
{
    QString name=GlobalVar::curName.left(GlobalVar::curName.indexOf("(")).left(GlobalVar::curName.indexOf("-"));
    QString url="https://search-api-web.eastmoney.com/search/jsonp?cb=&param=%7B%22uid%22%3A%227111416627128474%22%2C%22keyword%22%3A%22"+name+"%22%2C%22type%22%3A%5B%22cmsArticleWebOld%22%5D%2C%22client%22%3A%22web%22%2C%22clientType%22%3A%22web%22%2C%22clientVersion%22%3A%22curr%22%2C%22param%22%3A%7B%22cmsArticleWebOld%22%3A%7B%22searchScope%22%3A%22default%22%2C%22sort%22%3A%22default%22%2C%22pageIndex%22%3A1%2C%22pageSize%22%3A10%2C%22preTag%22%3A%22%3Cem%3E%22%2C%22postTag%22%3A%22%3C%2Fem%3E%22%7D%7D%7D&_=1687393368973";
    QByteArray allData;
    GlobalVar::getData(allData,2,QUrl(url));
    if (allData.isEmpty())
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData.mid(1,allData.size()-2), &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("result").toObject().value("cmsArticleWebOld").toArray();
        for (int i=0;i<data.count();++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QStringList l;
            l<<ceilMap.value("title").toString().replace("<em>","").replace("</em>","")<<"[新闻]"
                <<"("+ceilMap.value("date").toString()+")"<<ceilMap.value("url").toString()
                <<ceilMap.value("mediaName").toString()
                <<ceilMap.value("content").toString().replace("<em>","").replace("</em>","");

            GlobalVar::annoucementList.append(l);
        }
    }
}

void FundFlow::getGreatEvent()
{
    QString url="http://datacenter-web.eastmoney.com/api/data/v1/get?reportName=RPT_STOCKCALENDAR&columns=SECUCODE%2CSECURITY_CODE%2CSECURITY_INNER_CODE%2CORG_CODE%2CNOTICE_DATE%2CINFO_CODE%2CEVENT_TYPE%2CEVENT_TYPE_CODE%2CLEVEL1_CONTENT%2CCHANGE_RATE%2CCLOSE_PRICE&quoteColumns=&filter=(SECURITY_CODE%3D%22"+GlobalVar::curCode+"%22)(EVENT_TYPE_CODE%20in%20(%22023%22%2C%22012%22%2C%22002%22%2C%22019%22%2C%22020%22%2C%22005%22%2C%22016%22%2C%22017%22%2C%22026%22%2C%22011%22%2C%22006%22%2C%22025%22%2C%22010%22%2C%22007%22%2C%22003%22%2C%22024%22%2C%22004%22%2C%22021%22%2C%22009%22%2C%22013%22%2C%22008%22%2C%22001%22%2C%22014%22%2C%22015%22%2C%22022%22%2C%22018%22))(NOTICE_DATE%3C%3D%272023-10-04%27)&pageNumber=&pageSize=10&sortTypes=-1&sortColumns=NOTICE_DATE%20&source=QuoteWeb&client=WEB&_=1688597948026";
    QByteArray allData;
    GlobalVar::getData(allData,2,QUrl(url));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
        for (int i=0;i<data.count();++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QStringList l;
            l<<ceilMap.value("EVENT_TYPE").toString()<<"[大事提醒]"
              <<"("+ceilMap.value("NOTICE_DATE").toString()+")"
              <<ceilMap.value("LEVEL1_CONTENT").toString();
//            qDebug()<<l;
            GlobalVar::annoucementList.append(l);
        }
    }
}
