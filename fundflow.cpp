
#include "fundflow.h"
#include "globalvar.h"
//#include "commondelegate.h"
//#include "qheaderview.h"

FundFlow::FundFlow()
{
//    naManager = new QNetworkAccessManager(this);
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
                <<"大单净占比"<<"中单净额"<<"中单净占比"<<"小单净额"<<"小单净占比"<<"主力净流入最大股";
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
                    model->item(i,0)->setFont(boldFont);
            }
        }
    }
    model->setHorizontalHeaderLabels(FundFlowCol);
}

void FundFlow::getData(int days,const QByteArray &allData)
{
    QString v1="f3",v2="f62",v3="f184",v4="f66",v5="f69",v6="f72",v7="f75",
        v8="f78",v9="f81",v10="f84",v11="f87",v12="f204";
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
                    <<ceilMap.value(v12).toString();
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
    HighLowCol<<"日期"<<"收盘价"<<"20日新高"<<"20日新低"<<"60日新高"<<"60日新低"<<"120日新高"<<"120日新低";
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

            model->setItem(i,0,new QStandardItem(QDateTime::fromSecsSinceEpoch(ceilMap.value("date").toString().left(10).toInt()).toString("yyyy-MM-dd")));
            for (int j=1;j<8;++j)
                model->setItem(i,j,new QStandardItem(ceilMap.value(name[j]).toString()));
        }
    }
    model->setHorizontalHeaderLabels(HighLowCol);
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
    QNetworkAccessManager naManager =QNetworkAccessManager();
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
    //            qDebug()<<l;
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
    QByteArray allData;
    QNetworkRequest request;
    QString url="https://market.finance.sina.com.cn/transHis.php?symbol=sz301205&date=2022-11-28&page=1";
    request.setUrl(QUrl(url));
    GlobalVar::getData(allData,2,request);
    if (allData.isEmpty())
        return;
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString html=codec->toUnicode(allData);
    QString str=GlobalVar::peelStr(html,"<tbody>","-1");
    QList<QStringList> data;
    while(1)
    {
        if (str.indexOf("<tr")==-1)
            break;
        QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
        QString s=GlobalVar::peelStr(pair.first,"<tr","-1");

    //    qDebug()<<str;
        QStringList l;
        GlobalVar::getAllContent(s,l,"<t");
        qDebug()<<l;
        data.append(l);
        str=pair.second;
    }

//    QString str=GlobalVar::peelStr(html,"<tbody","-1");
//    QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
//    QString s=GlobalVar::peelStr(pair.first,"<tr","-1");
//    QStringList l;

//    GlobalVar::getAllContent(s,l,"<a");
//    GlobalVar::getAllContent(s,l,"<p");
//    GlobalVar::getAllContent(s,l,"<td");
//    qDebug()<<l;

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
    QNetworkAccessManager naManager =QNetworkAccessManager();
    QString url="https://finance.sina.com.cn/realstock/company/"+code+"/hisdata/"+date.left(4)+"/"+date.mid(5,2)+".js?d="+date;
    GlobalVar::getData(allData,2,QUrl(url));

    QString s=QString(allData).split(";")[0];
    int pos=s.indexOf("\"")+1;
    QString str=s.mid(pos,s.size()-pos-1);

    QStringList ss=str.split(",");
    GlobalVar::mHisTimeShareChartList.clear();
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
            GlobalVar::mHisTimeShareChartList.append(info);
        }
//        for (int i=0;i<GlobalVar::mHisTimeShareChartList.count();++i)
//            qDebug()<<GlobalVar::mHisTimeShareChartList.at(i).price<<GlobalVar::mHisTimeShareChartList.at(i).vol;
    }
}

void FundFlow::getVacation()
{
    QByteArray allData;
    QNetworkRequest request;
//    QString url="http://www.sse.com.cn/disclosure/dealinstruc/closed/";
    QString area[3]={"zh","us","hk"};
    QString url1="http://ddx.gubit.cn/holiday/";
//    request.setUrl(QUrl(url));
//    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
//    GlobalVar::getData(allData,2,request);
//    if (allData.isEmpty())
//        return;
//    QString html=QString(allData);
//    QString str=GlobalVar::peelStr(html,"<tbody>","-1");
//    QList<QStringList> data;
//    while(1)
//    {
//        if (str.indexOf("<tr")==-1)
//            break;
//        QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
//        QString s=GlobalVar::peelStr(pair.first,"<tr","-1");

//    //    qDebug()<<str;
//        QStringList l;
//        GlobalVar::getAllContent(s,l,"<td");
////        qDebug()<<l;
//        data.append(l);
//        str=pair.second;
//    }
//    QStringList date;
//    for (int i=0;i<data.count();++i)
//    {
//        QString s=data.at(i)[1].split("休市")[0];
//        QStringList t=s.split("至");
//        QDateTime t1[2];
//        for (int j=0;j<t.count();++j)
//        {
//            QString d=t[j].split("日")[0];
//            int n=d.indexOf("年");
//            int m=d.indexOf("月");
//            if (d.length()-m<3)
//                d.insert(m+1,"0");
//            if (m-n<3)
//                d.insert(n+1,"0");
//            if (i>0)
//                d=QDateTime::currentDateTime().toString("yyyy")+"-"+d;
//            t1[j]=QDateTime::fromString(d.replace("年","-").replace("月","-"),"yyyy-MM-dd");
//            if (t.count()==1)
//                t1[1]=t1[0].addDays(1);
//            else
//                t1[1]=t1[1].addDays(1);
//        }
//        while(1)
//        {
//            if (t1[0]==t1[1])
//                break;
//            if (t1[0].toString("yyyy")==QDateTime::currentDateTime().toString("yyyy"))
//                date.append(t1[0].toString("MMdd"));
//            t1[0]=t1[0].addDays(1);
//        }
//    }
//    GlobalVar::settings->setValue("Vacation_ZH",date);
    for (int i=0;i<3;++i)
    {
        request.setUrl(QUrl(url1+area[i]));
        GlobalVar::getData(allData,2,request);
        if (allData.isEmpty())
            return;
        QString html=QString(allData);
        QString str=GlobalVar::peelStr(html,"<tbody>","-1");
        QStringList l;
        while(1)
        {
            if (str.indexOf("<tr")==-1)
                break;
            QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
            QString s=GlobalVar::peelStr(pair.first,"<tr","-1");
            GlobalVar::getAllContent(s,l,"<td class=\"tw1\"");
            str=pair.second;
        }
        QStringList s;
        for (int j=0;j<l.size();++j)
            s<<l[j].mid(5,2)+l[j].right(2);
        GlobalVar::settings->setValue("Vacation_"+area[i].toUpper(),s);
    }
    GlobalVar::settings->setValue("isSetVacation",QDateTime::currentDateTime().toString("yyyy"));
}

void FundFlow::getAnnoucement()
{
    QByteArray allData;
    QNetworkRequest request;
    if (GlobalVar::curCode.left(1)=="1" or GlobalVar::curCode.left(3)=="399" or GlobalVar::curCode.length()==5)
    {
        GlobalVar::annoucementList.clear();
        return;
    }
//    QString url="https://search-api-web.eastmoney.com/search/jsonp?cb=&param=%7B%22uid%22%3A%227111416627128474%22%2C%22keyword%22%3A%22"+GlobalVar::curName.left(GlobalVar::curName.indexOf("("))+"%22%2C%22type%22%3A%5B%22noticeWeb%22%5D%2C%22client%22%3A%22web%22%2C%22clientVersion%22%3A%22curr%22%2C%22clientType%22%3A%22web%22%2C%22param%22%3A%7B%22noticeWeb%22%3A%7B%22preTag%22%3A%22%3Cem%20class%3D%5C%22red%5C%22%3E%22%2C%22postTag%22%3A%22%3C%2Fem%3E%22%2C%22pageSize%22%3A30%2C%22pageIndex%22%3A1%7D%7D%7D&_=1686978800179";
    QString url="http://ddx.gubit.cn/gonggao/"+GlobalVar::curCode;
    request.setUrl(QUrl(url));
    GlobalVar::getData(allData,2,request);
//    QJsonParseError jsonError;
//    QJsonDocument doc = QJsonDocument::fromJson(allData.mid(1,allData.size()-2), &jsonError);
//    if (jsonError.error == QJsonParseError::NoError)
//    {
//        QJsonObject jsonObject = doc.object();
//        QJsonArray data=jsonObject.value("result").toObject().value("noticeWeb").toArray();
//        for (int i = 0; i < data.size(); ++i)
//        {
//            QStringList l;
//            QJsonValue value = data.at(i);
//            QVariantMap ceilMap = value.toVariant().toMap();
//            l<<ceilMap.value("title").toString().split(":")[1]<<"[公告]"
//            <<"("+ceilMap.value("date").toString().left(10)+")"<<ceilMap.value("url").toString();
//            GlobalVar::annoucementList.append(l);
//        }
//    }

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString html=codec->toUnicode(allData);
    QString str=GlobalVar::peelStr(html,"<tbody>","-1");
    GlobalVar::annoucementList.clear();
    while(1)
    {
        if (str.indexOf("<tr")==-1)
            break;
        QPair<QString, QString> pair=GlobalVar::cutStr(str,"<tr","</tr");
        QString s=GlobalVar::peelStr(pair.first,"<tr","-1");
        QStringList list;
        QStringList l;
        QString href=GlobalVar::getAttributeContent(s,"href","\"");
        GlobalVar::getAllContent(s,l,"<td");
        if (l.size()>2)
        {
            list<<l[1]<<"[公告]"<<"("+l[2]+")"<<url+href.mid(1,-1);
//            qDebug()<<list;
            GlobalVar::annoucementList.append(list);
        }
        str=pair.second;
    }
    getNews();
}

void FundFlow::getNews()
{
    QNetworkRequest request;

    QNetworkAccessManager naManager =QNetworkAccessManager();
    request.setUrl(QUrl("http://www.stcn.com/article/search.html"));
    QNetworkReply *reply = naManager.get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    QTimer timer;
//    timer.singleShot(1000, &loop, SLOT(quit()));
//    timer.start();
    loop.exec();
    QString s=QString(QString(reply->rawHeader("Set-Cookie")));
    QString name1="acw_tc=";
    QString name2="advanced-stcn_web=";
    int BPos1=s.indexOf(name1)+name1.length();
    int BPos2=s.indexOf(name2)+name2.length();
    QString acw=s.mid(BPos1,s.indexOf(";")-BPos1);
    QString advanced=s.mid(BPos2,s.indexOf(";",BPos2)-BPos2);
    QByteArray allData;
    request.setRawHeader("x-requested-with","XMLHttpRequest");
    QList<QNetworkCookie> Web_cookies;
    Web_cookies.push_back(QNetworkCookie("acw_tc",acw.toLocal8Bit()));
    Web_cookies.push_back(QNetworkCookie("advanced-stcn_web",advanced.toLocal8Bit()));
    QVariant var;
    var.setValue(Web_cookies);
    request.setHeader(QNetworkRequest::CookieHeader,var);

//    QString url1="https://search-api-web.eastmoney.com/search/jsonp?cb=&param=%7B%22uid%22%3A%227111416627128474%22%2C%22keyword%22%3A%22%E4%B8%AD%E9%99%85%E6%97%AD%E5%88%9B%22%2C%22type%22%3A%5B%22cmsArticleWebOld%22%5D%2C%22client%22%3A%22web%22%2C%22clientType%22%3A%22web%22%2C%22clientVersion%22%3A%22curr%22%2C%22param%22%3A%7B%22cmsArticleWebOld%22%3A%7B%22searchScope%22%3A%22default%22%2C%22sort%22%3A%22default%22%2C%22pageIndex%22%3A1%2C%22pageSize%22%3A50%2C%22preTag%22%3A%22%3Cem%3E%22%2C%22postTag%22%3A%22%3C%2Fem%3E%22%7D%7D%7D&_=1686979606619";
//    for (int i=1;i<2;++i)
//    {
//        QString url="https://search.cnstock.com/search/result/"+QString::number(i)+"?t=0&k="+GlobalVar::curName.left(GlobalVar::curName.indexOf("("));
        QString url="http://www.stcn.com/article/search.html?search_type=news&keyword="+GlobalVar::curCode+"&page_time=1";
        request.setUrl(QUrl(url));
        GlobalVar::getData(allData,3,request);
        QString html=QString(allData);

//        qDebug()<<html;
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
//            qDebug()<<l;
            QString t;
            if (l[l.count()-1].contains("\\n"))
                t=GlobalVar::annoucementList.back()[2];
            else
                t="(2023-"+l[l.count()-1]+")";
            list<<l[0].replace("\\n","").replace(" ","")<<"[新闻]"<<t<<href.replace("\"","https://www.stcn.com");
//            qDebug()<<list;
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
//    }
    std::sort(GlobalVar::annoucementList.begin(),GlobalVar::annoucementList.end(),[](QStringList a,QStringList b){
        return a[2]>b[2];
    });
//    for (int i=0;i<GlobalVar::annoucementList.count();++i)
//        qDebug()<<GlobalVar::annoucementList.at(i)[0]<<GlobalVar::annoucementList.at(i)[1]
//            <<GlobalVar::annoucementList.at(i)[2]<<GlobalVar::annoucementList.at(i)[3];
}
