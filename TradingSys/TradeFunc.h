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
int Login(HSHLPHANDLE);
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
int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs);
//撤销委托
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
int MarketPriceEntrust(char *s_code, char *ammount, char *bs);
//账户相关信息查询
int AccQry(char *);
//前台费用计算
float EntrustFare(char *tk,int amt,float tpx,char* bs);
//连接服务器
HSHLPHANDLE Connect();


//const char fund_account[32] = "600511005";//测试资金账号
//const char password[32] = "606869";//测试资金密码
extern char fund_account[32];//资金账号
extern char password[32];//资金密码

extern char OUT_TP_DIR[100];


