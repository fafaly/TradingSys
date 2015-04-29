#include "stdafx.h"
#include "Company.h"
#include <string.h>

Company::Company()
{}


Company::Company(char *fund, char *pwd, char *o_dir)
{
	strcpy(fund_account, fund);
	strcpy(password, pwd);
	strcpy(orderdir, o_dir);
}


Company::~Company()
{
}

int Company::Login(){ return -1; }
//获取持仓
int Company::GetShare(map<string,int>&){ return -1; }
//获取当日交易明细
int Company::GetTrade(){ return -1; }
//普通委托
//bs:1买 2卖 eno:委托编号
int Company::NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, char *eno){ return -1; }
//撤销委托
//eno：委托编号entrust_no
int Company::CancelEntrust(char *eno){ return -1; }
//批量委托
int Company::EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs){ return -1; }
//可撤销委托查询
int Company::CancelEntrustQry(){ return -1; }
//批量委托查询
int Company::BatchEntrustQry(){ return -1; }
//查询行情
int Company::PriceQry(char *s_code, char *price1, int bs){ return -1; }
//委托查询
//status:0未报; 1待报; 2已报 ;6已撤; 7部分成交; 8全部成交;
int Company::EntrustQry(char *s_code, char *amt, char *tpx, char *status){ return -1; }
//市价交易
//bs:1买 2卖 eno:委托编号
int Company::MarketPriceEntrust(char *s_code, char *amount, char *bs, char *eno){ return -1; }
//资金明细查询.
int Company::FundAry(char *out_cash){ return -1; }
//前台费用计算
float Company::EntrustFare(char *tk, int amt, float tpx, char* bs){ return -1; }
//股票信息查询
int Company::StockQry(char *s_code){ return -1; }
//最大买入量
int Company::EntrustBuyAmount(char *s_code, char *tpx, char *amount){ return -1; }
//历史交割信息查询
int Company::HistDeliverQry(char *sdate, char *edate){ return -1; }
//历史成交查询.
int Company::HistBussinessQry(char *sdate, char *edate){ return -1; }
//历史资金证券流水查询.
int Company::HistFundStockQry(char *sdate, char *edate){ return -1; }
//历史证券成交统计查询
int Company::HistTradeTotQry(char *sdate, char *edate){ return -1; }
//快速持仓查询.
int Company::StockFastQry(){ return -1; }
//连接服务器
int Company::Connect(){ return -1; }