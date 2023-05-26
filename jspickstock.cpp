#include "jspickstock.h"
#include "globalvar.h"
#include "qbuttongroup.h"
#include "qlineedit.h"
#include "qmessagebox.h"
#include <QFile>

JSPickStock::JSPickStock(QDialog *parent)
    : QDialog{parent}
{

}

JSPickStock::~JSPickStock()
{

}

void JSPickStock::PickStockInterface()
{
    if (GlobalVar::WhichInterface!=1)
    {
        QMessageBox::information(this,"提示", "只能选A股或者在主界面", QMessageBox::Ok);
        return;
    }
    QDialog *pickStockWindow=new QDialog(this);
    pickStockWindow->setWindowFlags(pickStockWindow->windowFlags() | Qt::WindowStaysOnTopHint);
    pickStockWindow->setWindowTitle("公式选股");
    pickStockWindow->setGeometry(450, 200, 1000, 650);
    pickStockWindow->show();
    QHBoxLayout *mainLayout =new QHBoxLayout();
    pickStockWindow->setLayout(mainLayout);

    QTreeWidget *formulaTree=new QTreeWidget();
    formulaTree->setMaximumWidth(250);
    formulaTree->setHeaderLabel(QStringLiteral("公式"));
    QStringList formula=GlobalVar::settings->value("formula").toStringList();

    QTreeWidgetItem *class_ = new QTreeWidgetItem(formulaTree,QStringList("个人"));
    formulaTree->expandAll();
    for (int i=0;i<formula.count();++i)
    {
        QTreeWidgetItem *f=new QTreeWidgetItem(class_,QStringList(formula[i]));
        f->setFlags(f->flags() | Qt::ItemIsEditable);
    }
    QGridLayout *layout2=new QGridLayout();
    mainLayout->addWidget(formulaTree);
    mainLayout->addLayout(layout2,Qt::AlignLeft);

    QLabel *formulaName=new QLabel("公式名称:");
    QLineEdit *nameLine=new QLineEdit();
    QLabel *formulaDes=new QLabel("公式描述:");
    QTextEdit *desText=new QTextEdit();
    desText->setMaximumHeight(200);
    QLabel *area=new QLabel("选股范围:");
    layout2->addWidget(formulaName,0,0,1,1,Qt::AlignCenter);
    layout2->addWidget(nameLine,0,1,1,5);
    layout2->addWidget(formulaDes,1,0,Qt::AlignTop|Qt::AlignCenter);
    layout2->addWidget(desText,1,1,1,5);
    layout2->addWidget(area,4,0,Qt::AlignCenter);
    QButtonGroup *mButtonGroup = new QButtonGroup();
    mButtonGroup->setExclusive(false);
    QString name[]={"上证A股","深圳A股","科创版","北交所","剔除ST"};
    for (int i=0;i<5;++i)
    {
        QCheckBox *nameBox = new QCheckBox();
        nameBox->setText(name[i]);
        nameBox->setChecked(GlobalVar::areaFlag[i]);
        layout2->addWidget(nameBox,4,i+1);
        mButtonGroup->addButton(nameBox);
    }
    QPlainTextEdit *editFormula=new QPlainTextEdit();
    editFormula->setPlainText(GlobalVar::formulaContent);
    editFormula->setStyleSheet("QPlainTextEdit{font:bold 24px;font:bold}");
    editFormula->setMaximumHeight(350);
    layout2->addWidget(editFormula,5,0,12,6);

    QVBoxLayout *layout3=new QVBoxLayout();
    mainLayout->addSpacing(10);
    mainLayout->addLayout(layout3);
    QPushButton *button[5];
    QString buttonName[]={"开始选股","添加根节点","添加","删除","保存"};
    for (int i=0;i<5;++i)
    {
        button[i]=new QPushButton(buttonName[i]);
        layout3->addWidget(button[i]);
    }
    connect(formulaTree,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item){
        if (item->text(0)==formula[0])
        {
            QStringList temp=GlobalVar::settings->value("formula1").toStringList();
            nameLine->setText(temp[0]);
            desText->setText(temp[1]);
            editFormula->setPlainText(temp[2]);
        }
        else if (item->text(0)==formula[1])
        {
            QStringList temp=GlobalVar::settings->value("formula2").toStringList();
            nameLine->setText(temp[0]);
            desText->setText(temp[1]);
            editFormula->setPlainText(temp[2]);
        }
    });
    connect(button[4],&QPushButton::clicked,this,[=](){
        if (formulaTree->currentItem()->text(0)==formula[0])
        {
            QStringList t;
            t<<nameLine->text()<<desText->toPlainText()<<editFormula->toPlainText();
            GlobalVar::settings->setValue("formula1",t);
        }
        else if (formulaTree->currentItem()->text(0)==formula[1])
        {
            QStringList t;
            t<<nameLine->text()<<desText->toPlainText()<<editFormula->toPlainText();
            GlobalVar::settings->setValue("formula2",t);
        }
    });
    connect(formulaTree,&QTreeWidget::itemDoubleClicked,this,[=](QTreeWidgetItem *item){
        GlobalVar::formulaContent=item->text(0);
    });
    connect(formulaTree,&QTreeWidget::itemChanged,this,[=](QTreeWidgetItem *item){

        QStringList t;
        if (GlobalVar::formulaContent==formula[0])
        {
            t<<item->text(0)<<formula[1];
        }
        else if (GlobalVar::formulaContent==formula[1])
            t<<formula[0]<<item->text(0);

        GlobalVar::settings->setValue("formula",t);
    });
    connect(editFormula,&QPlainTextEdit::textChanged,editFormula,[=](){
        GlobalVar::formulaContent=editFormula->toPlainText();
//        qDebug()<<GlobalVar::formulaContent;
    });
    connect(mButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    connect(button[0],&QPushButton::clicked,this,[=](){
        JSPickStock* object = new JSPickStock();
        QJSEngine myEngine;
        QJSValue jsObject = myEngine.newQObject(object);
        myEngine.globalObject().setProperty("JS", jsObject);
        GlobalVar::mTableList.clear();
        for (int i=0;i<GlobalVar::mTableListCopy.count();++i)
        {
            if (GlobalVar::areaFlag[4] and GlobalVar::mTableListCopy.at(i).name.contains("ST"))
                continue;
            else if (not GlobalVar::areaFlag[0] and GlobalVar::mTableListCopy.at(i).code.left(1)=="6" and
                    GlobalVar::mTableListCopy.at(i).code.left(3)!="688" )
                continue;
            else if (not GlobalVar::areaFlag[1] and (GlobalVar::mTableListCopy.at(i).code.left(1)=="0" or
                                        GlobalVar::mTableListCopy.at(i).code.left(1)=="3"))
                continue;
            else if (not GlobalVar::areaFlag[2] and GlobalVar::mTableListCopy.at(i).code.left(3)=="688")
                continue;
            else if (not GlobalVar::areaFlag[3] and (GlobalVar::mTableListCopy.at(i).code.left(1)=="4" or
                                           GlobalVar::mTableListCopy.at(i).code.left(1)=="8"))
                continue;
            GlobalVar::mTableListNum=i;
            GlobalVar::mCandleListCode=GlobalVar::mTableListCopy.at(i).code;
            QJSValue value = myEngine.evaluate(replaceFormula(editFormula->toPlainText()));
            if (value.toNumber())
            {
                GlobalVar::mTableList.append(GlobalVar::mTableListCopy.at(i));
            }
        }
        emit updateTableList();
    });
}

void JSPickStock::onButtonClicked(QAbstractButton *button)
{
    QString name[]={"上证A股","深圳A股","科创版","北交所","剔除ST"};
    for (int i=0;i<5;++i)
    {
        if (button->text()==name[i])
        {
            GlobalVar::areaFlag[i]=not GlobalVar::areaFlag[i];
            break;
        }
    }
}

float JSPickStock::H()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).high;
}

float JSPickStock::H(int day)
{
    return getData(day,3);
}

float JSPickStock::H(int startDay, int endDay)
{
    QString path;
    if (GlobalVar::mCandleListCode.left(1)=="6")
        path="/list/data/sh/"+GlobalVar::mCandleListCode+".csv";
    else if (GlobalVar::mCandleListCode.left(1)=="8" or GlobalVar::mCandleListCode.left(1)=="4")
        path="/list/data/bj/"+GlobalVar::mCandleListCode+".csv";
    else
        path="/list/data/sz/"+GlobalVar::mCandleListCode+".csv";
    QFile file(GlobalVar::currentPath+path);
    if (file.open(QFile::ReadOnly) and file.size()>0)
    {
        QTextStream in(&file);
        QString s = in.readAll();
        QList<QString> data=s.split("\n",Qt::SkipEmptyParts).toList();
        float h=0.0;
        for (int i=startDay;i<startDay+endDay;++i)
        {
            int t=data.count()-1-i;
            if (t<0 or i<0)
            {
                file.close();
                return -100;
            }
            float temp=data.at(t).split(",",Qt::SkipEmptyParts).toList()[3].toFloat();
            if (temp>h)
                h=temp;
        }
        file.close();
        return h;

    }
    file.close();
    return -100;
}

float JSPickStock::L()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).low;
}

float JSPickStock::L(int day)
{
    return getData(day,4);
}

float JSPickStock::L(int startDay, int endDay)
{
    QString path;
    if (GlobalVar::mCandleListCode.left(1)=="6")
        path="/list/data/sh/"+GlobalVar::mCandleListCode+".csv";
    else if (GlobalVar::mCandleListCode.left(1)=="8" or GlobalVar::mCandleListCode.left(1)=="4")
        path="/list/data/bj/"+GlobalVar::mCandleListCode+".csv";
    else
        path="/list/data/sz/"+GlobalVar::mCandleListCode+".csv";
    QFile file(GlobalVar::currentPath+path);
    if (file.open(QFile::ReadOnly) and file.size()>0)
    {
        QTextStream in(&file);
        QString s = in.readAll();
        QList<QString> data=s.split("\n",Qt::SkipEmptyParts).toList();
        float l=100000.0;
        for (int i=startDay;i<startDay+endDay;++i)
        {
            int t=data.count()-1-i;
            if (t<0 or i<0)
            {
                file.close();
                return -100;
            }
            float temp=data.at(t).split(",",Qt::SkipEmptyParts).toList()[4].toFloat();
            if (temp<l)
                l=temp;
        }
        file.close();
        return l;

    }
    file.close();
    return -100;
}

float JSPickStock::C()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).close;
}

float JSPickStock::C(int day)
{
    return getData(day,2);
}

float JSPickStock::T()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).turn;
}

float JSPickStock::T(int day)
{
    return getData(day,10);
}

float JSPickStock::T(int startDay, int endDay)
{
    QString path;
    if (GlobalVar::mCandleListCode.left(1)=="6")
        path="/list/data/sh/"+GlobalVar::mCandleListCode+".csv";
    else if (GlobalVar::mCandleListCode.left(1)=="8" or GlobalVar::mCandleListCode.left(1)=="4")
        path="/list/data/bj/"+GlobalVar::mCandleListCode+".csv";
    else
        path="/list/data/sz/"+GlobalVar::mCandleListCode+".csv";
    QFile file(GlobalVar::currentPath+path);
    if (file.open(QFile::ReadOnly) and file.size()>0)
    {
        QTextStream in(&file);
        QString s = in.readAll();
        QList<QString> data=s.split("\n",Qt::SkipEmptyParts).toList();
        float temp=0;
        for (int i=startDay;i<=startDay+endDay;++i)
        {
            int t=data.count()-1-i;
            if (t<0 or i<0)
            {
                file.close();
                return -100;
            }
            temp+=data.at(t).split(",",Qt::SkipEmptyParts).toList()[10].toFloat();
        }
        file.close();
        return temp;
    }
    file.close();
    return -100;
}

float JSPickStock::V()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).totalValue;
}

float JSPickStock::P()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).pctChg;
}

float JSPickStock::P(int day)
{
    return getData(day,8);
}

float JSPickStock::O()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).open;
}

float JSPickStock::O(int day)
{
    return getData(day,1);
}

float JSPickStock::getData(int day, int column)
{
    QString path;
    if (GlobalVar::mCandleListCode.left(1)=="6")
        path="/list/data/sh/"+GlobalVar::mCandleListCode+".csv";
    else if (GlobalVar::mCandleListCode.left(1)=="8" or GlobalVar::mCandleListCode.left(1)=="4")
        path="/list/data/bj/"+GlobalVar::mCandleListCode+".csv";
    else
        path="/list/data/sz/"+GlobalVar::mCandleListCode+".csv";
    QFile file(GlobalVar::currentPath+path);
    if (file.open(QFile::ReadOnly) and file.size()>0)
    {
        QTextStream in(&file);
        QString s = in.readAll();
        QList<QString> data=s.split("\n",Qt::SkipEmptyParts).toList();
        int t=data.count()-1-day;
        if (t<0 or day<0)
        {
            file.close();
            return -100;
        }
        float temp=data.at(t).split(",",Qt::SkipEmptyParts).toList()[column].toFloat();
        //        qDebug()<<p;
        file.close();
        return temp;
    }
    file.close();
    return -100;
}

QString JSPickStock::replaceFormula(QString s)
{
    int post=0;
    while((post=s.indexOf("(",post))!=-1)
    {
        QString t=s.mid(post-1,1);
        if (t>='A' && t<='Z')
        {
            s.insert(post-1,"JS.");
            post+=4;
        }
        else
            post+=1;
    }
    return s;
}
