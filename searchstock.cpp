
#include "searchstock.h"
#include "globalvar.h"

SearchStock::SearchStock()
{
//    naManager = new QNetworkAccessManager(this);
    searchCodeLine=new QLineEdit(this);
    searchCodeLine->setStyleSheet("background-color: rgb(211, 211, 211);");
    matchCodeText=new QTextEdit(this);
    connect(searchCodeLine,&QLineEdit::textChanged,this,&SearchStock::matchInput);
    connect(searchCodeLine,&QLineEdit::returnPressed,this,&SearchStock::searchFinished);
}

void SearchStock::matchInput()
{
    matchCodeText->clear();
    GlobalVar::curBlock=0;
    QString str=searchCodeLine->text();
    if (not str.isEmpty())
    {
        QFile digitFile(GlobalVar::currentPath+"/list/digit_list.csv");
        QTextCharFormat charFmt=QTextCharFormat();
        charFmt.setForeground(QColor("black"));
        matchCodeText->mergeCurrentCharFormat(charFmt);
        if (digitFile.open(QIODevice::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            QStringList Data=codec->toUnicode(digitFile.readAll()).split("\n",Qt::SkipEmptyParts);
            compare(Data,str,0);
        }

        QString strUpper=str.toUpper();
        QFile file(GlobalVar::currentPath+"/list/abbreviation_list.csv");

        if (file.open(QIODevice::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            QStringList Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
            compare(Data,strUpper,2);
        }

        matchCodeText->moveCursor(QTextCursor::Start);
        QTextBlockFormat blockFmt=QTextBlockFormat();
        blockFmt.setBackground(QColor("white"));
        matchCodeText->textCursor().setBlockFormat(blockFmt);
        QTextBlock block=matchCodeText->document()->findBlockByNumber(0);
        matchCodeText->setTextCursor(QTextCursor(block));
        blockFmt=matchCodeText->textCursor().blockFormat();
        blockFmt.setBackground(QColor(0, 199, 255));
        matchCodeText->textCursor().setBlockFormat(blockFmt);
    }
}

void SearchStock::searchFinished()
{
    QStringList cn=matchCodeText->document()->findBlockByNumber(GlobalVar::curBlock).text().split("   ");
    GlobalVar::isBoard=false;
    QString code=cn.at(0).right(6);
    if (cn.at(2)=="指数" and code.left(1)=="0")
        GlobalVar::curCode="1."+code;
    else if (cn.at(2)=="板块")
    {
        GlobalVar::curBoard=code;
        GlobalVar::isBoard=true;
        getBoardData();
    }
    else
        GlobalVar::curCode=code;
    emit showSearch();
    matchCodeText->clear();
}

void SearchStock::compare(QStringList Data,QString strUpper,int col)
{
    int n=Data.count();
    QStringList strLine;
    int matchNum=0;
    int l = 0;
    int r = n - 1;
    int m=0;
    QString blank="   ";
    while (l <= r)
    {
        int mid = (l + r) / 2;
        strLine = Data.at(mid).split(",");

        if(strUpper==strLine.at(col).left(searchCodeLine->text().length()))
        {
            matchNum=mid;
            QString class_;
            QString s=strLine.at(0);
            if (s.right(1)=="G" or s.left(3)=="399")
                class_="指数";
            else if(s.left(1)=="B")
                class_="板块";
            else if(s.left(1)=="6")
                class_="上海A股";
            else if(s.left(1)=="0")
                class_="深圳A股";
            else if(s.left(1)=="8" or s.left(1)=="4")
                class_="北京A股";
            else if(s.left(1)=="3")
                class_="创业板";
            matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            break;
        }
        else if (strUpper<strLine.at(col).left(searchCodeLine->text().length()))
            r = mid - 1;
        else
            l = mid + 1;
    }
    for (int i=matchNum-1;i>=0;--i)
    {
        QStringList strLine = Data.at(i).split(",");
        QString class_;
        QString s=strLine.at(0);
        if (s.right(1)=="G" or s.left(3)=="399")
            class_="指数";
        else if(s.left(1)=="B")
            class_="板块";
        else if(s.left(1)=="6")
            class_="上海A股";
        else if(s.left(1)=="0")
            class_="深圳A股";
        else if(s.left(1)=="8" or s.left(1)=="4")
            class_="北京A股";
        else if(s.left(1)=="3")
            class_="创业板";
        if (strUpper==strLine.at(col).left(searchCodeLine->text().length()))
        {
            matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            m+=1;
            if (m>100)
                break;
        }
        else
            break;
    }
    for (int i=matchNum+1;i<Data.count();++i)
    {
        QStringList strLine = Data.at(i).split(",");
        QString class_;
        QString s=strLine.at(0);
        if (s.right(1)=="G" or s.left(3)=="399")
            class_="指数";
        else if(s.left(1)=="B")
            class_="板块";
        else if(s.left(1)=="6")
            class_="上海A股";
        else if(s.left(1)=="0")
            class_="深圳A股";
        else if(s.left(1)=="8" or s.left(1)=="4")
            class_="北京A股";
        else if(s.left(1)=="3")
            class_="创业板";
        if (strUpper==strLine.at(col).left(searchCodeLine->text().length()))
        {
            matchCodeText->append(s.mid(0,6)+blank+strLine.at(1)+"("+strLine.at(2)+")"+blank+class_);
            m+=1;
            if (m>100)
                break;
        }
        else
            break;
    }
}

void SearchStock::getBoardData()
{
    QByteArray allData;
    GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f3&fs=b:"+GlobalVar::curBoard+"+f:!50&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297"));
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
            if (GlobalVar::WhichInterface==5)
                info.code = ceilMap.value("f13").toString()+"."+ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toFloat();
            info.pctChg=ceilMap.value("f3").toFloat();
            if (info.pctChg>0)
                GlobalVar::upNums+=1;
            else if (info.pctChg<0)
                GlobalVar::downNums+=1;
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

