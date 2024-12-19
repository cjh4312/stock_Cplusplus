
#include "getf10info.h"

GetF10Info::GetF10Info(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
//    calcPeriod();
}

void GetF10Info::getStockHotRank()
{
    f10QList.clear();
    QJsonObject json;
    json.insert("appId", "appId01");
    json.insert("globalId", "786e4c21-70dc-435a-93bb-38");
    json.insert("srcSecurityCode", GlobalVar::getStockSymbol());

    QJsonDocument doc;
    doc.setObject(json);
    QByteArray dataArray = doc.toJson(QJsonDocument::Compact);
    QByteArray allData;
    GlobalVar::postData(dataArray,allData,1,QUrl("https://emappdata.eastmoney.com/stockrank/getHotStockRankList"));
    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QStringList dataList;
            dataList<<ceilMap.value("calcTime").toString()<<ceilMap.value("srcSecurityCode").toString()
                     <<ceilMap.value("conceptName").toString()<<ceilMap.value("conceptId").toString()
                    <<ceilMap.value("hitCount").toString();
//            qDebug()<<dataList;
            f10QList.append(dataList);
        }
    }
    QString url="https://datacenter-web.eastmoney.com/api/data/v1/get?reportName=RPT_F10_CORETHEME_BOARDTYPE&columns=SECUCODE%2CSECURITY_CODE%2CSECURITY_NAME_ABBR%2CBOARD_CODE%2CBOARD_NAME%2CIS_PRECISE%2CBOARD_RANK&source=WEB&client=WEB&filter=(SECURITY_CODE%3D%22"+GlobalVar::curCode+"%22)&sortColumns=BOARD_RANK&sortTypes=1&_=1701002504740";
    GlobalVar::getData(allData,1,QUrl(url));
    doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
        int size=f10QList.size();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            QStringList dataList;
            int code=ceilMap.value("BOARD_CODE").toInt();
            QString s=QString::number(code);
            if (code<1000)
                s="0"+s;
            s="BK"+s;
            for (int j=0;j<size;++j)
            {
                if (f10QList.at(j)[3]==s)
                    break;
                else
                    if (j==size-1)
                    {
                        dataList<<""<<""<<ceilMap.value("BOARD_NAME").toString()<<s;
                        f10QList.append(dataList);
                    }
            }
            if (size==0)
            {
                dataList<<""<<""<<ceilMap.value("BOARD_NAME").toString()<<s;
                f10QList.append(dataList);
            }
        }
    }
}

void GetF10Info::getMainIndex()
{
    f10QList.clear();
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl("https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/ZYZBAjaxNew?type=0&code="+GlobalVar::getStockSymbol()));
    QStringList lableList={"EPSJB", "EPSKCJB", "EPSXS","BPS", "MGZBGJ", "MGWFPLR","MGJYXJJE",
                           "TOTALOPERATEREVE","MLR" ,"PARENTNETPROFIT","KCFJCXSYJLR","TOTALOPERATEREVETZ",
                           "PARENTNETPROFITTZ","TOTALDEPOSITS","KCFJCXSYJLRTZ", "YYZSRGDHBZC",
                           "GROSSLOANS","NETPROFITRPHBZC", "KFJLRGDHBZC","LTDRR","ROEJQ", "ROEKCJQ",
                           "ZZCJLL","XSJLL", "XSMLL", "LD", "SD","XJLLB", "NEWCAPITALADER","ZCFZL",
                           "QYCS", "CQBL","ZZCZZTS", "HXYJBCZL","CHZZTS", "YSZKZZTS","TOAZZL",
                           "NONPERLOAN","CHZZL", "YSZKZZL","BLDKBBL","NZBJE"};
    QStringList nameList={"基本每股收益(元)","扣非每股收益(元)","稀释每股收益(元)",
                            "每股净资产(元)","每股公积金(元)","每股未分配利润(元)",
                            "每股经营现金流(元)","营业总收入(元)","毛利润","归属净利润(元)",
                            "扣非净利润(元)","营业总收入同比增长(%)","归属净利润同比增长(%)",
                            "存款总额(元)","扣非净利润同比增长(%)","营业总收入滚动环比增长(%)",
                            "贷款总额(元)","归属净利润滚动环比增长(%)","扣非净利润滚动环比增长(%)",
                            "存贷款比例","净资产收益率(加权)(%)","净资产收益率(扣非/加权)(%)",
                            "净资产收益率(扣非/加权)(%)","净利率(%)","毛利率(%)","流动比率","速动比率",
                            "现金流量比率","资本充足率(%)","资产负债率(%)","权益乘数","产权比率",
                            "总资产周转天数(天)","核心资本充足率(%)","存货周转天数(天)","应收账款周转天数(天)",
                            "总资产周转率(次)","不良贷款率(%)","存货周转率(次)","应收账款周转率(次)",
                            "不良贷款拨备覆盖率(%)","资本净额(元)"};
    getData(allData,lableList,nameList,colList,f10QList);
}

void GetF10Info::mainBusinessComposition()
{
    f10QList.clear();
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl("http://f10.emoney.cn/f10/zygc/"+GlobalVar::curCode));
    QString html=QTextCodec::codecForName("GBK")->toUnicode(allData);
    QPair<QString, QString> pair;
    QString remainder=GlobalVar::peelStr(html,"class=\"swlab_t\"","-1");
    pair=GlobalVar::cutStr(remainder,"<ul","</ul>");
    QString content=pair.first;
    remainder=pair.second;
    QString row;
    QStringList colName;
    GlobalVar::getAllContent(content,colName,"<text");
    int m=0;
    while(1)
    {
        pair=GlobalVar::cutStr(remainder,"<table","</table>");
        content=pair.first;
        remainder=pair.second;
        while(1)
        {
            if (content.indexOf("<tr")==-1)
                break;
            QString s;
            pair=GlobalVar::cutStr(content,"<tr","</tr>");
            QStringList rowList;
            if (pair.first.contains("rowspan"))
            {
                s=GlobalVar::peelStr(pair.first,"<th","-1");
                row=GlobalVar::getContent(s);
            }
            else if(pair.first.contains("colspan"))
            {
                colList={"报告期","分类方向"};
                pair=GlobalVar::cutStr(content,"<tr","</tr>");
                s=pair.first;
                GlobalVar::getAllContent(s,colList,"<th");
                colList[2]="分类";
//                qDebug()<<bussinessColList;
                content=pair.second;
                continue;
            }
            rowList<<colName[m]<<row;
            s=pair.first;
            GlobalVar::getAllContent(s,rowList,"<td");
            content=pair.second;

            f10QList.append(rowList);
            // qDebug()<<rowList;
        }
        if (remainder.indexOf("swlab_c")==-1)
            break;
        m+=1;
    }
}

void GetF10Info::eastMainBusinessComposition()
{
    f10QList.clear();
    QString code;
    if (GlobalVar::curCode.left(1)=="6" or GlobalVar::curCode.left(3)=="688")
        code=GlobalVar::curCode+".SH";
    else if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="0")
        code=GlobalVar::curCode+".SZ";
    else
        code=GlobalVar::curCode+".BJ";

    QString url="https://datacenter.eastmoney.com/securities/api/data/v1/get?reportName=RPT_F10_FN_MAINOP&columns=SECUCODE%2CREPORT_DATE&quoteColumns=&distinct=REPORT_DATE&filter=(SECUCODE%3D%22"+code+"%22)&pageNumber=1&pageSize=&sortTypes=-1&sortColumns=REPORT_DATE&source=HSF10&client=PC&v=026536694635958624";
    QByteArray allData;
    GlobalVar::getData(allData,1,QUrl(url));
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    QString className[4]={"","按行业分类","按产品分类","按地区分类"};
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        for (int i = 0; i < data.size(); ++i)
        {
            if (i>5)
                break;
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            QString date=ceilMap.value("REPORT_DATE").toString().left(10);
            url="https://datacenter.eastmoney.com/securities/api/data/v1/get?reportName=RPT_F10_FN_MAINOP&columns=SECUCODE%2CSECURITY_CODE%2CREPORT_DATE%2CMAINOP_TYPE%2CITEM_NAME%2CMAIN_BUSINESS_INCOME%2CMBI_RATIO%2CMAIN_BUSINESS_COST%2CMBC_RATIO%2CMAIN_BUSINESS_RPOFIT%2CMBR_RATIO%2CGROSS_RPOFIT_RATIO%2CRANK&quoteColumns=&filter=(SECUCODE%3D%22"+code+"%22)(REPORT_DATE%3D%27"+date+"%27)&pageNumber=1&pageSize=200&sortTypes=1%2C1&sortColumns=MAINOP_TYPE%2CRANK&source=HSF10&client=PC&v=06359431030769309";
            GlobalVar::getData(allData,1,QUrl(url));
            doc = QJsonDocument::fromJson(allData, &jsonError);
            if (jsonError.error == QJsonParseError::NoError)
            {
                jsonObject = doc.object();
                QJsonArray data1=jsonObject.value("result").toObject().value("data").toArray();
                for (int i = 0; i < data1.size(); ++i)
                {
                    QStringList strl;
                    value = data1.at(i);
                    ceilMap = value.toVariant().toMap();
                    strl<<ceilMap.value("REPORT_DATE").toString().left(10)<<className[ceilMap.value("MAINOP_TYPE").toInt()]<<ceilMap.value("ITEM_NAME").toString()
                         <<GlobalVar::format_conversion(ceilMap.value("MAIN_BUSINESS_INCOME").toFloat())<<GlobalVar::format_conversion(ceilMap.value("MBI_RATIO").toFloat())
                         <<GlobalVar::format_conversion(ceilMap.value("MAIN_BUSINESS_COST").toFloat())<<GlobalVar::format_conversion(ceilMap.value("MBC_RATIO").toFloat())
                         <<GlobalVar::format_conversion(ceilMap.value("MAIN_BUSINESS_RPOFIT").toFloat())<<GlobalVar::format_conversion(ceilMap.value("MBR_RATIO").toFloat())
                         <<GlobalVar::format_conversion(ceilMap.value("GROSS_RPOFIT_RATIO").toFloat());
                    f10QList.append(strl);
                }
            }

        }
    }
}

void GetF10Info::assetLiability()
{
    QStringList lableList={"MONETARYFUNDS","LEND_FUND", "NOTE_ACCOUNTS_RECE","ACCOUNTS_RECE",
                        "NOTE_RECE", "PREPAYMENT", "TOTAL_OTHER_RECE",
                        "CURRENT_ASSET_BALANCE", "NONCURRENT_ASSET_BALANCE",
                        "INVENTORY", "OTHER_CURRENT_ASSET", "TOTAL_CURRENT_ASSETS",
                        "DIVIDEND_RECE", "BUY_RESALE_FINASSET","LOAN_ADVANCE", "CREDITOR_INVEST",
                        "OTHER_NONCURRENT_FINASSET","INVEST_REALESTATE", "FIXED_ASSET", "CIP",
                        "TRADE_FINASSET_NOTFVTPL","USERIGHT_ASSET", "INTANGIBLE_ASSET", "DEVELOP_EXPENSE",
                        "DERIVE_FINASSET","LONG_PREPAID_EXPENSE", "DEFER_TAX_ASSET",
                        "OTHER_NONCURRENT_ASSET","TOTAL_NONCURRENT_ASSETS", "TOTAL_ASSETS",
                        "ACCOUNTS_PAYABLE", "ADVANCE_RECEIVABLES","NOTE_ACCOUNTS_PAYABLE", "CONTRACT_LIAB",
                        "STAFF_SALARY_PAYABLE","TAX_PAYABLE", "TOTAL_OTHER_PAYABLE",
                        "DIVIDEND_PAYABLE", "GOODWILL","NONCURRENT_LIAB_1YEAR", "OTHER_CURRENT_LIAB",
                        "DEFER_INCOME","TOTAL_CURRENT_LIAB", "TOTAL_NONCURRENT_LIAB",
                        "LEASE_LIAB", "LONG_EQUITY_INVEST","DEFER_TAX_LIAB", "TOTAL_LIABILITIES",
                        "SHARE_CAPITAL", "SHORT_LOAN","CAPITAL_RESERVE", "OTHER_COMPRE_INCOME", "SURPLUS_RESERVE",
                        "LONG_STAFFSALARY_PAYABLE","GENERAL_RISK_RESERVE", "UNASSIGN_RPOFIT",
                        "TOTAL_PARENT_EQUITY", "MINORITY_EQUITY","TOTAL_EQUITY", "TOTAL_LIAB_EQUITY"};
    QStringList nameList={"货币资金","拆出资金", "应收票据及应收账款","应收账款",
            "其中:应收票据","预付款项", "其他应收款合计","流动资产平衡项目", "非流动资产平衡项目",
            "存货","其他流动资产", "流动资产合计","应收股利", "买入返售金融资产",
            "发放贷款及垫款", "债权投资","其他非流动金融资产","投资性房地产","固定资产","在建工程",
            "交易性金融资产","使用权资产", "无形资产","开发支出","衍生金融资产",
            "长期待摊费用","递延所得税资产","其他非流动资产","非流动资产合计","资产总计",
            "其中:应付账款","预收款项","应付票据及应付账款","合同负债","应付职工薪酬",
            "应交税费","其他应付款合计","其中:应付股利","商誉","一年内到期的非流动负债","其他流动负债",
            "递延收益","流动负债合计","非流动负债合计","租赁负债", "长期股权投资",
            "递延所得税负债", "负债合计","实收资本（或股本）","短期借款","资本公积","其他综合收益","盈余公积",
            "长期应付职工薪酬","一般风险准备", "未分配利润","归属于母公司股东权益总计","少数股东权益",
            "股东权益合计",  "负债和股东权益总计"};
    QString url="https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/zcfzbAjaxNew?companyType="+companyType+"&reportDateType=0&reportType=1&dates=";
    getAllData(lableList,nameList,url);
}

void GetF10Info::getIncome()
{
    QStringList lableList={"TOTAL_OPERATE_INCOME", "OPERATE_INCOME","FEE_COMMISSION_INCOME",
            "INTEREST_INCOME", "TOTAL_OPERATE_COST","OPERATE_COST", "ASSET_IMPAIRMENT_INCOME",
            "INTEREST_EXPENSE", "FEE_COMMISSION_EXPENSE","OPERATE_TAX_ADD", "SALE_EXPENSE", "MANAGE_EXPENSE",
            "OTHER_BUSINESS_COST", "ASSET_IMPAIRMENT_LOSS","RESEARCH_EXPENSE", "FINANCE_EXPENSE",
            "FE_INTEREST_EXPENSE", "OTHER_COMPRE_INCOME","FE_INTEREST_INCOME", "FAIRVALUE_CHANGE_INCOME",
            "EXCHANGE_INCOME", "OTHER_BUSINESS_INCOME","INVEST_INCOME","INVEST_JOINT_INCOME",
            "ASSET_DISPOSAL_INCOME","CREDIT_IMPAIRMENT_INCOME", "OTHER_INCOME",
            "OPERATE_PROFIT", "MINORITY_OCI","NONBUSINESS_INCOME", "NONBUSINESS_EXPENSE",
            "TOTAL_PROFIT", "OPERATE_PROFIT_BALANCE",
            "INCOME_TAX", "NETPROFIT", "CONTINUED_NETPROFIT","TOTAL_PROFIT_BALANCE",
            "PARENT_NETPROFIT", "MINORITY_INTEREST","DEDUCT_PARENT_NETPROFIT", "BASIC_EPS",
            "DILUTED_EPS", "PARENT_OCI","TOTAL_COMPRE_INCOME", "MINORITY_TCI",
            "PARENT_TCI","TOTAL_OPERATE_INCOME_YOY", "OPERATE_INCOME_YOY",
            "FEE_COMMISSION_INCOME_YOY","INTEREST_INCOME_YOY", "TOTAL_OPERATE_COST_YOY",
            "OPERATE_COST_YOY", "ASSET_IMPAIRMENT_IMCOME_YOY","INTEREST_EXPENSE_YOY", "FEE_COMMISSION_EXPENSE_YOY",
            "ASSET_IMPAIRMENT_INCOME_YOY","OPERATE_TAX_ADD_YOY", "SALE_EXPENSE_YOY",
            "MANAGE_EXPENSE_YOY", "OTHER_BUSINESS_COST_YOY",
            "RESEARCH_EXPENSE_YOY", "FINANCE_EXPENSE_YOY","FE_INTEREST_EXPENSE_YOY",
            "OTHER_COMPRE_INCOME_YOY","FE_INTEREST_INCOME_YOY","FAIRVALUE_CHANGE_INCOME_YOY",
            "EXCHANGE_INCOME_YOY", "OTHER_BUSINESS_INCOME_YOY","INVEST_INCOME_YOY",
            "INVEST_JOINT_INCOME_YOY","ASSET_DISPOSAL_INCOME_YOY",
            "CREDIT_IMPAIRMENT_INCOME_YOY", "OTHER_INCOME_YOY","OPERATE_PROFIT_YOY", "MINORITY_OCI_YOY",
            "NONBUSINESS_INCOME_YOY","NONBUSINESS_EXPENSE_YOY", "TOTAL_PROFIT_YOY",
            "OPERATE_PROFIT_BALANCE_YOY","INCOME_TAX_YOY", "NETPROFIT_YOY",
            "CONTINUED_NETPROFIT_YOY","TOTAL_PROFIT_BALANCE_YOY",
            "PARENT_NETPROFIT_YOY","MINORITY_INTEREST_YOY","DEDUCT_PARENT_NETPROFIT_YOY",
            "BASIC_EPS_YOY","DILUTED_EPS_YOY", "PARENT_OCI_YOY",
            "TOTAL_COMPRE_INCOME_YOY","MINORITY_TCI_YOY","PARENT_TCI_YOY"};
    QStringList nameList={"营业总收入", "营业收入","其中:手续费及佣金收入","利息收入", "营业总成本",
            "营业成本", "资产减值损失(新)","利息支出", "手续费及佣金支出",
            "税金及附加", "销售费用", "管理费用","其他业务成本", "资产减值损失",
            "研发费用", "财务费用","其中:利息费用", "其他综合收益","利息收入", "加:公允价值变动收益",
            "汇兑损失", "其他业务收入","投资收益","其中:对联营企业和合营企业的投资收益/（损失）",
            "资产处置收益","信用减值损失(新)", "其他收益","营业利润", "归属于少数股东的其他综合收益",
            "加:营业外收入", "减:营业外支出","利润总额", "营业利润平衡项目",
            "减:所得税", "净利润", "持续经营净利润","利润总额平衡项目","归属于母公司股东的净利润", "少数股东损益",
            "扣除非经常性损益后的净利润", "基本每股收益","稀释每股收益", "归属于母公司股东的其他综合收益",
            "综合收益总额", "归属于少数股东的综合收益总额","归属于母公司股东的综合收益总额",
            "同比营业总收入", "同比营业收入","同比其中:手续费及佣金收入","同比利息收入", "同比营业总成本",
            "同比营业成本", "同比资产减值损失(新)","同比利息支出", "同比手续费及佣金支出",
            "同比资产减值损失(新)","同比税金及附加", "同比销售费用","同比管理费用", "同比其他业务成本",
            "同比研发费用", "同比财务费用","同比其中:利息费用","同比其他综合收益","同比利息收入",
            "同比加:公允价值变动收益","同比汇兑损失", "同比其他业务收入","同比投资收益",
            "同比其中:对联营企业和合营企业的投资收益/（损失）","同比资产处置收益",
            "同比信用减值损失(新)", "同比其他收益","同比营业利润", "同比归属于少数股东的其他综合收益",
            "同比加:营业外收入","同比减:营业外支出", "同比利润总额","同比营业利润平衡项目",
            "同比减:所得税", "同比净利润","同比持续经营净利润","同比利润总额平衡项目",
            "同比归属于母公司股东的净利润","同比少数股东损益","同比扣除非经常性损益后的净利润",
            "同比基本每股收益","同比稀释每股收益","同比归属于母公司股东的其他综合收益",
            "同比综合收益总额","同比归属于少数股东的综合收益总额","同比归属于母公司股东的综合收益总额"};

    QString url="https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/lrbAjaxNew?companyType="+companyType+"&reportDateType=0&reportType=1&dates=";
    getAllData(lableList,nameList,url);
}

void GetF10Info::getCashFlow()
{
    QStringList lableList={"SALES_SERVICES","DEPOSIT_INTERBANK_ADD","RECEIVE_INTEREST_COMMISSION",
                "RECEIVE_OTHER_OPERATE","TOTAL_OPERATE_INFLOW","BUY_SERVICES",
                "OBTAIN_SUBSIDIARY_OTHER","ACCEPT_INVEST_CASH","SUBSIDIARY_ACCEPT_INVEST",
                "RECEIVE_OTHER_FINANCE","LOAN_ADVANCE_ADD","PBC_INTERBANK_ADD",
                "PAY_INTEREST_COMMISSION","PAY_STAFF_CASH","PAY_ALL_TAX", "PAY_OTHER_OPERATE",
                "OPERATE_OUTFLOW_OTHER","TOTAL_OPERATE_OUTFLOW","NETCASH_OPERATE",
                "WITHDRAW_INVEST", "RECEIVE_INVEST_INCOME","TOTAL_FINANCE_INFLOW",
                "DISPOSAL_LONG_ASSET","RECEIVE_TAX_REFUND","RECEIVE_OTHER_INVEST",
                "TOTAL_INVEST_INFLOW","CONSTRUCT_LONG_ASSET","INVEST_PAY_CASH",
                "PAY_OTHER_INVEST","TOTAL_INVEST_OUTFLOW","NETCASH_INVEST",
                "ASSIGN_DIVIDEND_PORFIT","SUBSIDIARY_PAY_DIVIDEND","PAY_OTHER_FINANCE",
                "TOTAL_FINANCE_OUTFLOW","NETCASH_FINANCE","RATE_CHANGE_EFFECT",
                "CCE_ADD", "ASSET_IMPAIRMENT","BEGIN_CCE", "END_CCE","NETPROFIT",
                "FA_IR_DEPR","OILGAS_BIOLOGY_DEPR","IA_AMORTIZE", "LPE_AMORTIZE",
                "DISPOSAL_LONGASSET_LOSS","FA_SCRAP_LOSS", "FAIRVALUE_CHANGE_LOSS",
                "FINANCE_EXPENSE","INVEST_LOSS", "DEFER_TAX","DT_ASSET_REDUCE",
                "DT_LIAB_ADD", "INVENTORY_REDUCE","OPERATE_RECE_REDUCE","OPERATE_PAYABLE_ADD",
                "NETCASH_OPERATENOTE","END_CASH", "BEGIN_CASH","CCE_ADDNOTE"};
    QStringList nameList={"销售商品、提供劳务收到的现金","客户存款和同业存放款项净增加额","收取利息、手续费及佣金的现金",
            "收到其他与经营活动有关的现金","经营活动现金流入小计","购买商品、接受劳务支付的现金",
            "取得子公司及其他营业单位支付的现金净额","吸收投资收到的现金","其中:子公司吸收少数股东投资收到的现金",
            "收到的其他与筹资活动有关的现金","客户贷款及垫款净增加额","存放中央银行和同业款项净增加额",
            "支付利息、手续费及佣金的现金","支付给职工以及为职工支付的现金","支付的各项税费",
            "支付其他与经营活动有关的现金","经营活动现金流出的其他项目","经营活动现金流出小计",
            "经营活动产生的现金流量净额","收回投资收到的现金", "取得投资收益收到的现金",
            "筹资活动现金流入小计","处置固定资产、无形资产和其他长期资产收回的现金净额","收到的税收返还",
            "收到的其他与投资活动有关的现金","投资活动现金流入小计","购建固定资产、无形资产和其他长期资产支付的现金",
            "投资支付的现金","支付其他与投资活动有关的现金","投资活动现金流出小计","投资活动产生的现金流量净额",
            "分配股利、利润或偿付利息支付的现金","其中:子公司支付给少数股东的股利、利润","支付的其他与筹资活动有关的现金",
            "筹资活动现金流出小计","筹资活动产生的现金流量净额","汇率变动对现金及现金等价物的影响",
            "现金及现金等价物净增加额", "资产减值准备","加:期初现金及现金等价物余额", "期末现金及现金等价物余额",
            "净利润","固定资产和投资性房地产折旧","其中:固定资产折旧、油气资产折耗、生产性生物资产折旧",
            "无形资产摊销", "长期待摊费用摊销","处置固定资产、无形资产和其他长期资产的损失",
            "固定资产报废损失", "公允价值变动损失","财务费用","投资损失", "递延所得税",
            "其中:递延所得税资产减少","递延所得税负债增加", "存货的减少","经营性应收项目的减少",
            "经营性应付项目的增加","经营活动产生的现金流量净额","现金的期末余额", "减:现金的期初余额",
            "现金及现金等价物的净增加额"};

    QString url="https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/xjllbAjaxNew?companyType="+companyType+"&reportDateType=0&reportType=1&dates=";
    getAllData(lableList,nameList,url);
}

void GetF10Info::getData(const QByteArray &allData,QStringList key,QStringList value,QStringList &col,QList<QStringList> &l)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toArray();
        int len=key.count();
        QStringList cowList[100];
        col={"基本指标"};
        for (int i=0;i<len;++i)
            cowList[i]<<value[i];
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();
            col<<ceilMap.value("REPORT_DATE").toString().left(10);
            for (int j=0;j<len;++j)
            {
                QString s="";
                if (not ceilMap.value(key[j]).isNull())
                    s=GlobalVar::format_conversion(ceilMap.value(key[j]).toFloat());
                cowList[j]<<s;
            }
        }
        for (int i=0;i<len;++i)
            l.append(cowList[i]);
    }
}

void GetF10Info::getAllData(QStringList key, QStringList value,QString url)
{
    f10QList.clear();
    QStringList t[100];
    for (int i=0;i<value.count();++i)
        t[i]<<value[i];
    QStringList c;
    colList={"基本指标"};
    calcPeriod();
    for (int k=0;k<period.count();++k)
    {
        QList<QStringList> temp;
        QByteArray allData;
        GlobalVar::getData(allData,1,QUrl(url+period[k]+"&code="+GlobalVar::getStockSymbol()));
        getData(allData,key,value,c,temp);
        c.remove(0);
        colList<<c;
        for (int i=0;i<temp.count();++i)
        {
            for (int j=1;j<temp.at(i).count();++j)
                t[i]<<temp.at(i)[j];
        }
    }
    for (int i=0;i<value.count();++i)
        f10QList.append(t[i]);
}

void GetF10Info::calcPeriod()
{
    period.clear();
    QByteArray allData;
    if (GlobalVar::curName.contains("证券"))
        companyType="1";
    QString url="https://emweb.securities.eastmoney.com/PC_HSF10/NewFinanceAnalysis/zcfzbDateAjaxNew?companyType="+companyType+"&reportDateType=0&code="+GlobalVar::getStockSymbol();
    GlobalVar::getData(allData,1,QUrl(url));
    QString html=QString(allData);
    QString s="REPORT_DATE\" : \"";
    QString date=html.mid(html.indexOf(s)+s.length(),10);
    QDateTime initTime=QDateTime::fromString(date, "yyyy-MM-dd");

//    QStringList time={"03-31","06-30","09-30","12-31"};
//    QDateTime curTime=QDateTime::currentDateTime();
//    QString year=curTime.toString("yyyy");
//    QString md=curTime.toString("MMdd");
//    QDateTime initTime;
//    if (md<="0415")
//    {
//        QString setTime=curTime.addYears(-1).toString()+"-"+time[3];
//        initTime=QDateTime::fromString(setTime, "yyyy-MM-dd");
//    }
//    else if (md<="0715")
//    {
//        QString setTime=year+"-"+time[0];
//        initTime=QDateTime::fromString(setTime, "yyyy-MM-dd");
//    }
//    else if (md<="1015")
//    {
//        QString setTime=year+"-"+time[1];
//        initTime=QDateTime::fromString(setTime, "yyyy-MM-dd");
//    }
//    else
//    {
//        QString setTime=year+"-"+time[2];
//        initTime=QDateTime::fromString(setTime, "yyyy-MM-dd");
//    }
    for (int j=0;j<2;++j)
    {
        QString s="";
        for (int i=0;i<5;++i)
        {
            QString d=initTime.addMonths(-i*3-j*15).toString("yyyy-MM-dd");
            QString dd=d.split("-")[1];
            if (dd=="03" or dd=="12")
                d.replace("-30","-31");
            s=s+d;
            if (i!=4)
                s=s+"%2C";
        }
        period.append(s);
    }
}
