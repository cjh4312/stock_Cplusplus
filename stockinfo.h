#ifndef STOCKINFO_H
#define STOCKINFO_H
#include <QString>

// #define COLNUMS 17
//#define OFFSETEND 0
#define KRANGE 150
#define WIDTHEDGE 2.0
#define BOTTOMHEIGHTEDGE 20
#define TOPHEIGHTEDGE 4.0
#define KWIDTHEDGE 4.0
#define KRIGHTWIDTHEDGE 60.0
#define KTOPHEIGHTEDGE 60.0
#define KBOTTOMHEIGHTEDGE 5
#define per(c) ((c)>=(GlobalVar::preClose))?(int((((c)-(GlobalVar::preClose))/(GlobalVar::preClose))*10000+0.5)/100.0):(int((((c)-(GlobalVar::preClose))/(GlobalVar::preClose))*10000-0.5)/100.0)
//#define per3(c) ((c)>=(GlobalVar::preClose))?(int((((c)-(GlobalVar::preClose))/(GlobalVar::preClose))*100000+0.5)/1000.0):(int((((c)-(GlobalVar::preClose))/(GlobalVar::preClose))*100000-0.5)/1000.0)
#define SPLITMID "/R"
#define SPLITBACK "////"
#define TITLEHEIGHT 40
#define TIPWIDTH 15
#define PI 3.1415926
#define MAX_RETRIES 3
#define POSXCHART 640
#define POSYCHART 120

struct StockInfo
{
    QString code;
    QString name;
    float close;
    float pctChg;
    float turn;
    float amount;
    float velocity;
    float pe;
    float totalValue;
    float circulatedValue;
    float amplitude;
    float qrr;
    float pbr;
    float pctYear;
    float pctSixty;
    float volume;
    float high;
    float low;
    float open;
    float preClose;
};

struct IndexInfo
{
    QString name="name";
    QString close="1";
    QString pctChg="0.01";
};

struct timeShareTickInfo
{
    QString time;
    float price;
    int nums;
    int direct;
    int tick;
};

struct timeShareChartInfo
{
    QString time;
    float price;
    float vol;
    int direct;
    float avePrice;
};

struct candleChartInfo
{
    QString time;
    float open;
    float close;
    float pctChg;
    float high;
    float low;
    float vol;
    float turn;
    float amount;
    float MA5;
    float MA10;
    float MA20;
    float MA60;
    float VMA5;
    float VMA10;
};

#endif // STOCKINFO_H
