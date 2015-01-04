#pragma once
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
//#pragma once
#include <time.h>

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")



void ShowAnsData();
void ShowErrMsg(int iFunc);
//HSHLPHANDLE Run();
//用户登录
int Login();
//获取必要参数
void SetNecessaryParam();
bool GetLocalIP(char* ip);
bool GetFirstMac(char * mac);
void put_file_str(const char* fname, char* format, ...);
//获取持仓
int GetShare();
//获取当日交易明细
int GetTrade();
//普通委托
//bs:1买 2卖 eno:委托编号
int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs,OUT char *eno);
//撤销委托
//eno：委托编号entrust_no
int CancelEntrust( char *eno);
//批量委托
int EntrustBuyOrSell( char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
//可撤销委托查询
int CancelEntrustQry();
//批量委托查询
int BatchEntrustQry();
//查询行情
int PriceQry(char *s_code, char *price1, int bs);
//委托查询
//status:0未报; 1待报; 2已报 ;6已撤; 7部分成交; 8全部成交;
int EntrustQry(char *s_code, char *amt, char *tpx, char *status);
//市价交易
//bs:1买 2卖 eno:委托编号
int MarketPriceEntrust(char *s_code, char *ammount, char *bs, OUT char *eno);
//资金明细查询.
int FundAry(char *out_cash);
//前台费用计算
float EntrustFare(char *tk,int amt,float tpx,char* bs);
//股票信息查询
int StockQry(char *s_code);
//最大买入量
int EntrustBuyAmount(char *s_code, char *tpx,OUT char *amount);
//历史交割信息查询
int HistDeliverQry(char *sdate, char *edate);
//历史成交查询.
int HistBussinessQry(char *sdate, char *edate);
//历史资金证券流水查询.
int HistFundStockQry(char *sdate, char *edate);
//历史证券成交统计查询
int HistTradeTotQry(char *sdate, char *edate);
//快速持仓查询.
int StockFastQry();
//订阅证券成交回报
int Subscribe();
//取消证券成交回报
int SubCancel();
//连接服务器
HSHLPHANDLE Connect();




