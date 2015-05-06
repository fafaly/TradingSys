#pragma once
#include "Company.h"
//#include "DataBaseFunc.h"
#include <map>
using namespace std;

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
	//数据库操作对象
	//DataBaseFunc dbf;
	//默认处理的券商接口的功能.
	Company *cp;
	//现金值.
	float cash = 0;
	//用于简单交易算法，股票数量累计超过此值就市价交易.
	int m_limit_shr = 0;
	//查询成交与否的间隔时间,5s.
	const int m_chk_trade_time=5000;
	//等待查询成功的时间.
	const int m_re_entrust_time = 20000;
	//查询委托状态的时间2s.
	const int m_chk_entrust_time = 2000;
	//交易间隔时间
	const int m_trade_interval = 30000;
	//日志的文件句柄.
	FILE *m_logfd;
	//获取现金.
	int GetCash();
	//获得当前时间
	char *GetDate();
	//bs1买 2卖
	//type1普通委托，2市价委托
	int BuySellTk(char *tk, int brokeshr, int bs, char *eno,int type);
	//检查委托是否完成
	//status:8 已经成交 9 部分成交 其余都是未成交的各种状态
	int CheckEntrust(char *tk, char *eno);
	//检查已有持仓是否够
	int CheckHoldShr(string tk);
	//委托状态.
	int EntrustStatus(int status);

public:
	//holding shares.
	map<string, int> m_holdmap;
	//构造函数，需要传入对应的公司接口的引用.
	TradeAlgorithm(Company *comp);
	//算法析构函数.
	~TradeAlgorithm();

	//初始的算法版本.
	//采取时间段内均分策略，在累计到一定股数时市价买入.
	int EasyAlgorithm(Order *od);

	//标准VWAP算法.
	//基于日内加权平均算法.
	//od存储单相关内容.
	//days多少天的日内平均值.
	//ipxdir存放ipx的存放地址.
	int NormalVWAP(Order *od,int days,char *ipxdir);
};