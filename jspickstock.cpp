#include "jspickstock.h"

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
    else if (isRunning)
    {
        QMessageBox::information(this,"提示", "不要重复打开", QMessageBox::Ok);
        return;
    }
    isRunning=true;
    QDialog *pickStockWindow=new QDialog(this);
    pickStockWindow->setAttribute(Qt::WA_DeleteOnClose);
    // pickStockWindow->setWindowFlags(pickStockWindow->windowFlags() | Qt::WindowStaysOnTopHint);
    // pickStockWindow->setWindowModality(Qt::ApplicationModal);
    pickStockWindow->setWindowTitle("公式选股");
    pickStockWindow->setGeometry(450, 200, 1000, 650);
    pickStockWindow->show();
    QHBoxLayout *mainLayout =new QHBoxLayout(pickStockWindow);
    pickStockWindow->setLayout(mainLayout);

    QTreeWidget *formulaTree=new QTreeWidget(pickStockWindow);
    formulaTree->setMaximumWidth(250);
    formulaTree->setHeaderLabel(QStringLiteral("选股公式"));
    QFile file(GlobalVar::currentPath+"/list/formula.f");
    GlobalVar::formula.clear();
    if (file.open(QFile::ReadWrite))
    {
        QTextCodec *codec = QTextCodec::codecForName("gbk");
        QStringList data=codec->toUnicode(file.readAll()).split(SPLITBACK,Qt::SkipEmptyParts);
        for (int i=0;i<data.count();++i)
            GlobalVar::formula.append(data[i].split(SPLITMID));
    }
    file.close();
    QTreeWidgetItem *class_ = new QTreeWidgetItem(formulaTree,QStringList("个人"));
    formulaTree->expandAll();
    for (int i=0;i<GlobalVar::formula.count();++i)
    {
        QTreeWidgetItem *f=new QTreeWidgetItem(class_,QStringList(GlobalVar::formula[i][0]));
        if (i==0)
        {
            f->setSelected(true);
            formulaTree->setCurrentItem(f);
        }
        f->setFlags(f->flags() | Qt::ItemIsEditable);
    }
    QGridLayout *layout2=new QGridLayout();
    mainLayout->addWidget(formulaTree);
    mainLayout->addLayout(layout2,Qt::AlignLeft);

    QLabel *formulaName=new QLabel("公式名称:",pickStockWindow);
    QLineEdit *nameLine=new QLineEdit(pickStockWindow);
    QLabel *formulaDes=new QLabel("公式描述:",pickStockWindow);
    QTextEdit *desText=new QTextEdit(pickStockWindow);
    desText->setMaximumHeight(200);
    QLabel *area=new QLabel("选股范围:",pickStockWindow);
    layout2->addWidget(formulaName,0,0,1,1,Qt::AlignCenter);
    layout2->addWidget(nameLine,0,1,1,5);
    layout2->addWidget(formulaDes,1,0,Qt::AlignTop|Qt::AlignCenter);
    layout2->addWidget(desText,1,1,1,5);
    layout2->addWidget(area,4,0,Qt::AlignCenter);
    QButtonGroup *mButtonGroup = new QButtonGroup(pickStockWindow);
    mButtonGroup->setExclusive(false);

    for (int i=0;i<5;++i)
    {
        QCheckBox *nameBox = new QCheckBox(pickStockWindow);
        nameBox->setText(name[i]);
        nameBox->setChecked(GlobalVar::areaFlag[i]);
        layout2->addWidget(nameBox,4,i+1);
        mButtonGroup->addButton(nameBox);
    }
    QPlainTextEdit *editFormula=new QPlainTextEdit(pickStockWindow);
//    editFormula->redoAvailable(true);
    editFormula->setPlainText(GlobalVar::formulaContent);
    editFormula->setStyleSheet("QPlainTextEdit{font:bold 24px;font:bold}");
    editFormula->setMaximumHeight(350);
    layout2->addWidget(editFormula,5,0,11,6);

    // QLabel *progress=new QLabel("选股进度:",pickStockWindow);
    // QLabel *numbers=new QLabel(QString::number(GlobalVar::mTableListCopy.count()),pickStockWindow);
    // QProgressBar *progressBar = new QProgressBar(pickStockWindow);
    // layout2->addWidget(progress,16,0);
    // layout2->addWidget(numbers,16,1);
    // layout2->addWidget(progressBar,16,2);

    QVBoxLayout *layout3=new QVBoxLayout();
    mainLayout->addSpacing(10);
    mainLayout->addLayout(layout3);
    QPushButton *button[5];
    QString buttonName[]={"开始选股","添加分类","添加","删除","保存"};
    for (int i=0;i<5;++i)
    {
        button[i]=new QPushButton(buttonName[i],pickStockWindow);
        layout3->addWidget(button[i]);
    }
    connect(formulaTree,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem */*item*/){
        int row=formulaTree->currentIndex().row();
        nameLine->setText(GlobalVar::formula.at(row)[1]);
        desText->setText(GlobalVar::formula.at(row)[2]);
        editFormula->setPlainText(GlobalVar::formula.at(row)[3]);
    });
    connect(button[2],&QPushButton::clicked,this,[=](){
        formulaTree->blockSignals(true);
        QString s=QString::number(class_->childCount());
        QString name="公式"+s;
        QTreeWidgetItem *c=new QTreeWidgetItem(class_,QStringList(name));
        c->setFlags(c->flags() | Qt::ItemIsEditable);
        formulaTree->setCurrentItem(c);
        QFile file(GlobalVar::currentPath+"/list/formula.f");
        QStringList t;
        if (file.open(QFile::Append))
        {
            t<<name<<""<<""<<"";
            GlobalVar::formula.append(t);
            file.write(t.join(SPLITMID).toLocal8Bit()+SPLITBACK);
        }
        file.close();
        formulaTree->blockSignals(false);
    });
    connect(button[3],&QPushButton::clicked,this,[=](){
        int row=formulaTree->currentIndex().row();
        int r=QMessageBox::information(this,"提示", "确定删除:"+GlobalVar::formula[row][0], QMessageBox::Yes | QMessageBox::No);
        if (r==QMessageBox::Yes)
        {
            delete formulaTree->currentItem();
            QFile file(GlobalVar::currentPath+"/list/formula.f");
            if (file.open(QFile::WriteOnly))
                for (int i=0;i<GlobalVar::formula.count();++i)
                {
                    if (i==row)
                        continue;
                    file.write(GlobalVar::formula.at(i).join(SPLITMID).toLocal8Bit()+SPLITBACK);
                }
            GlobalVar::formula.removeOne(GlobalVar::formula.at(row));
            file.close();
        }
    });
    connect(button[4],&QPushButton::clicked,this,[=](){
        int row=formulaTree->currentIndex().row();
        QStringList t;
        t<<GlobalVar::formula.at(row)[0]<<nameLine->text()<<desText->toPlainText()<<editFormula->toPlainText();
        GlobalVar::formula.replace(row,t);
        QFile file(GlobalVar::currentPath+"/list/formula.f");
        if (file.open(QFile::WriteOnly))
            for (int i=0;i<GlobalVar::formula.count();++i)
                file.write(GlobalVar::formula.at(i).join(SPLITMID).toLocal8Bit()+SPLITBACK);
        file.close();
    });
//    connect(formulaTree,&QTreeWidget::itemDoubleClicked,this,[=](QTreeWidgetItem *item){
//        GlobalVar::formulaContent=item->text(0);
//    });
    connect(formulaTree,&QTreeWidget::itemChanged,this,[=](QTreeWidgetItem *item){
        int row=formulaTree->currentIndex().row();
        QStringList t;
        t<<item->text(0)<<GlobalVar::formula.at(row)[1]<<GlobalVar::formula.at(row)[2]<<GlobalVar::formula.at(row)[3];
        GlobalVar::formula.replace(row,t);
        QFile file(GlobalVar::currentPath+"/list/formula.f");
        if (file.open(QFile::WriteOnly))
            for (int i=0;i<GlobalVar::formula.count();++i)
                file.write(GlobalVar::formula.at(i).join(SPLITMID).toLocal8Bit()+SPLITBACK);
        file.close();
    });
    connect(editFormula,&QPlainTextEdit::textChanged,editFormula,[=](){
        editFormula->blockSignals(true);
        GlobalVar::formulaContent=editFormula->toPlainText();
        int post=0;
        QTextCursor cursor = editFormula->textCursor();
        QTextCharFormat fmt;
        fmt.setForeground(QColor("black"));
        cursor.setPosition(0,QTextCursor::MoveAnchor);
        cursor.setPosition(GlobalVar::formulaContent.size(),QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(fmt);
        while((post=GlobalVar::formulaContent.indexOf("(",post))!=-1)
        {
            QString t=GlobalVar::formulaContent.mid(post-1,1);
            if (t>='A' && t<='Z')
            {
                int p=GlobalVar::formulaContent.indexOf(")",post)+1;
                if (p==0)
                    break;
                fmt.setForeground(QColor("red"));
                QString s=GlobalVar::formulaContent.mid(post-1,p-post+1);
                bool isCon=true;
                if (t=="D")
                {
                    for (int i=2;i<s.length()-1;++i)
                    {
                        isCon=false;
                        if ((s.mid(i,1)>="A" && s.mid(i,1)<="Z") ||
                            (s.mid(i,1)>="a" && s.mid(i,1)<="z") ||
                            (s.mid(i,1)>="0" && s.mid(i,1)<="9"))
                        {
                            isCon=true;
                            continue;
                        }
                        break;
                    }
                    if (not isCon)
                    {
                        post+=1;
                        continue;
                    }
                }
                else
                {
                    for (int i=2;i<s.length()-1;++i)
                    {
                        isCon=false;
                        if ((s.mid(i,1)==",") || (s.mid(i,1)>="0" && s.mid(i,1)<="9"))
                        {
                            isCon=true;
                            continue;
                        }
                        break;
                    }
                    if (not isCon)
                    {
                        post+=1;
                        continue;
                    }
                }
                cursor.setPosition(post-1,QTextCursor::MoveAnchor);
                cursor.setPosition(p,QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(fmt);
                fmt.setForeground(QColor("black"));
                editFormula->mergeCurrentCharFormat(fmt);
            }
            post+=1;
        }
        editFormula->blockSignals(false);
    });
    connect(mButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    connect(button[0],&QPushButton::clicked,this,[=](){
        if (GlobalVar::settings->value("isDownloadK").toString()!=
                    GlobalVar::curRecentWorkDay(0).toString("yyyy-MM-dd"))
            QMessageBox::information(this,"提示", "确保数据是最新的", QMessageBox::Ok);
        JSPickStock* object = new JSPickStock(this);
        QJSEngine myEngine;
        QJSValue jsObject = myEngine.newQObject(object);
        myEngine.globalObject().setProperty("JS", jsObject);
        GlobalVar::mTableList.clear();
        for (int i=0;i<GlobalVar::mTableListCopy.count();++i)
        {
            if (GlobalVar::areaFlag[4] and (GlobalVar::mTableListCopy.at(i).name.contains("ST") or
                                            GlobalVar::mTableListCopy.at(i).name.contains("退")))
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
            if (value.isError())
            {
                QMessageBox::information(this,"提示", value.property("name").toString()+"\n"+
                        value.property("message").toString()+"\n"+
                        QString::number(value.property("lineNumber").toInt()), QMessageBox::Ok);
                return;
            }
            if (value.toNumber())
            {
                GlobalVar::mTableList.append(GlobalVar::mTableListCopy.at(i));
            }
        }
        emit updateTableList();
    });
    connect(pickStockWindow,&QDialog::destroyed,this,[=](){
        isRunning=false;
    });
}

void JSPickStock::onButtonClicked(QAbstractButton *button)
{
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

float JSPickStock::Y()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).preClose;
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
        for (int i=startDay;i<startDay+endDay;++i)
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

float JSPickStock::A(int startDay, int endDay)
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
        for (int i=startDay;i<startDay+endDay;++i)
        {
            int t=data.count()-1-i;
            if (t<0 or i<0)
            {
                file.close();
                return -100;
            }
            temp+=data.at(t).split(",",Qt::SkipEmptyParts).toList()[2].toFloat();
        }
        file.close();
//        qDebug()<<GlobalVar::mCandleListCode<<temp/endDay;
        return temp/endDay;
    }
    file.close();
    return -100;
}

float JSPickStock::M()
{
    return GlobalVar::mTableListCopy.at(GlobalVar::mTableListNum).volume;
}

float JSPickStock::M(int day)
{
    return getData(day,5);
}

float JSPickStock::M(int startDay, int endDay)
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
        for (int i=startDay;i<startDay+endDay;++i)
        {
            int t=data.count()-1-i;
            if (t<0 or i<0)
            {
                file.close();
                return -100;
            }
            temp+=data.at(t).split(",",Qt::SkipEmptyParts).toList()[5].toFloat();
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

float JSPickStock::D(float f)
{
    return int(f*100+0.5)/100.0;
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
