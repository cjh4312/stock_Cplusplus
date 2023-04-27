
#include "searchstock.h"
#include "globalvar.h"

SearchStock::SearchStock()
{
    searchCodeLine=new QLineEdit(this);
    searchCodeLine->setStyleSheet("background-color: rgb(211, 211, 211);");
    matchCodeText=new QTextEdit(this);
    connect(searchCodeLine,&QLineEdit::textChanged,this,&SearchStock::matchInput);
    connect(searchCodeLine,&QLineEdit::returnPressed,this,&SearchStock::searchFinished);
}

void SearchStock::findStockArea()
{
    QFile file(GlobalVar::currentPath+"/list/stock_list.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QStringList Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);

        int l = 0;
        int r = Data.count() - 1;

        while (l <= r)
        {
            int mid = (l + r) / 2;
            QStringList strLine = Data.at(mid).split(",");
            if(strLine[1]==GlobalVar::curCode)
            {
                GlobalVar::curName=strLine[2]+"("+strLine[3]+")"+"--"+strLine[4];
                break;
            }
            else if (strLine[1]>GlobalVar::curCode)
                r = mid - 1;
            else
                l = mid + 1;
        }
    }
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
    GlobalVar::curCode=cn.at(0).right(6);
//    qDebug()<<cn.at(2);
    if (cn.at(2)=="指数" and GlobalVar::curCode.left(1)=="0")
        GlobalVar::curCode="1."+GlobalVar::curCode;
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

