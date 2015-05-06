#pragma once
#include "Company.h"
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
#include <time.h>
using namespace std;

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")

#define IN
#define OUT
#define REQ_NUM "500"

class CiticsCompany :public Company
{
private:
	//const char fund_account[32] = "600511005";//测试资金账号
	//const char password[32] = "606869";//测试资金密码
	//char fund_account[32];//资金账号
	//char password[32];//资金密码

	char op_entrust_way[4] ;//外部接入客户必须用3（远程委托），生产后需要让营业部对使用的资金账号开通3权限
	char ClientName[16] ; //客户简称，一般用拼音首字母,请务必修改！

	char client_id[32];
	char user_token[64];
	char branch_no[8];
	char asset_prop[4];
	char sysnode_id[4];
	char entrust_no[16];
	char entrust_status[8];
	char op_station[128];
	char IP[16];
	char MAC[16];
	char HD[16];

	HSHLPHANDLE HlpHandle = NULL;
	bool GetLocalIP(char* ip);
	bool GetFirstMac(char * mac);

public:
	//char orderdir[100];//order文件目录
	CiticsCompany(char *fund, char *pwd, char *o_dir);
	~CiticsCompany();

	void ShowAnsData();
	void ShowErrMsg(int iFunc);
	//HSHLPHANDLE Run();
	//用户登录
	int Login();
	//获取必要参数
	void SetNecessaryParam();
	void put_file_str(const char* fname, char* format, ...);
	//获取持仓
	int GetShare(map<string, int>&);
	//获取当日交易明细
	int GetTrade(char *tk,char *realstatus);
	//普通委托
	//bs:1买 2卖 eno:委托编号
	int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, OUT char *eno);
	//撤销委托
	//eno：委托编号entrust_no
	int CancelEntrust(char *eno);
	//批量委托
	int EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
	//可撤销委托查询
	int CancelEntrustQry();
	//批量委托查询
	int BatchEntrustQry();
	//查询行情
	int PriceQry(char *s_code, char *price1, int bs);
	//委托查询
	//status:0未报; 1待报; 2已报 ;6已撤; 7部分成交; 8全部成交;
	int EntrustQry(IN char *s_code,OUT char *amt,OUT char *tpx,OUT char* eno,OUT char *status);
	//市价交易
	//bs:1买 2卖 eno:委托编号
	int MarketPriceEntrust(char *s_code, char *ammount, char *bs, OUT char *eno);
	//资金明细查询.
	int FundAry(char *out_cash);
	//前台费用计算
	float EntrustFare(char *tk, int amt, float tpx, char* bs);
	//股票信息查询
	int StockQry(char *s_code);
	//最大买入量
	int EntrustBuyAmount(char *s_code, char *tpx, OUT char *amount);
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
	//连接服务器
	int Connect();
};

