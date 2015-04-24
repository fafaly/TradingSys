#pragma once
#include "Company.h"

struct Order
{
	char tk[10];//股票代码
	int shr;//交易量
	char bs[2];//交易方向
	char stime[15];//起始时间
	char etime[15];//关闭时间
};

class TradeAlgorithm
{
private:
	//默认处理的券商接口的功能
	Company *cp;
	//现金值
	float cash = 0;
	//查询成交与否的间隔时间
	int INTERVAL_TIME=30000;
	//获取现金.
	int GetCash();
	//获得当前时间
	char *GetDate();
	//买或者卖
	int BuySellTk(char *tk, int brokeshr, int bs, char *eno);
	//检查交易是否完成
	//status:8 已经成交 9 部分成交 其余都是未成交的各种状态
	int CheckDeal(char *tk);

public:
	//构造函数，需要传入对应的公司接口的引用
	TradeAlgorithm(Company *comp);
	//算法析构函数
	~TradeAlgorithm();
	//初始的算法版本
	//采取均分策略，在累计到一定股数时市价买入
	int EasyAlgorithm(Order *od);
	//标准VWAP算法
	//基于日内加权平均算法
	//od存储单相关内容
	//days多少天的日内平均值
	//ipxdir存放ipx的存放地址
	int NormalVWAP(Order *od,int days,char *ipxdir);
};