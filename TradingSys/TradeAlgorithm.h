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
	int CheckDeal(char *tk);

public:
	TradeAlgorithm(Company *comp);
	~TradeAlgorithm();
	//运行算法
	int RunAlgorithm(Order *od);
};