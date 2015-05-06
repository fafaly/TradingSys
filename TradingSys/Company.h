#pragma once
#include<map>
using namespace std;
class Company
{
public:
	char fund_account[32];//资金账号
	char password[32];//资金密码
	char orderdir[100];//order文件目录
	Company();
	//fund：账号
	//pwd:密码
	//o_dir:order文件的位置
	Company(char *fund, char *pwd, char *o_dir);
	//HSHLPHANDLE Run();
	//用户登录
	virtual int Login();
	//获取持仓
	virtual int GetShare(map<string, int>&);
	//获取当日交易明细
	virtual int GetTrade(char *tk, char *realstatus);
	//普通委托
	//输入：bs:1买 2卖 
	//输出：eno 委托编号
	virtual int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, char *eno);
	//撤销委托
	//eno：委托编号entrust_no
	virtual int CancelEntrust(char *eno);
	//批量委托
	virtual int EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
	//可撤销委托查询
	virtual int CancelEntrustQry();
	//批量委托查询
	virtual int BatchEntrustQry();
	//查询行情
	virtual int PriceQry(char *s_code, char *price1, int bs);
	//委托查询
	//status:0未报; 1待报; 2已报 ;6已撤; 7部分成交; 8全部成交;
	virtual int EntrustQry(char *s_code, char *amt, char *tpx,char *eno, char *status);
	//市价交易
	//bs:1买 2卖 eno:委托编号
	virtual int MarketPriceEntrust(char *s_code, char *ammount, char *bs, char *eno);
	//资金明细查询.
	virtual int FundAry(char *out_cash);
	//前台费用计算
	virtual float EntrustFare(char *tk, int amt, float tpx, char* bs);
	//股票信息查询
	virtual int StockQry(char *s_code);
	//最大买入量
	//amount变量
	virtual int EntrustBuyAmount(char *s_code, char *tpx, char *amount);
	//历史交割信息查询
	virtual int HistDeliverQry(char *sdate, char *edate);
	//历史成交查询.
	virtual int HistBussinessQry(char *sdate, char *edate);
	//历史资金证券流水查询.
	virtual int HistFundStockQry(char *sdate, char *edate);
	//历史证券成交统计查询
	virtual int HistTradeTotQry(char *sdate, char *edate);
	//快速持仓查询.
	virtual int StockFastQry();
	//连接服务器
	virtual int Connect();
	//析构函数.
	~Company();
};

