#include "stdafx.h"
#include "TradeAlgorithm.h"
#include "GlobalFunc.h"
#include "Company.h"
#include "CiticsCompany.h"
#include <Windows.h>
#include <time.h>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
using namespace std;
using namespace GlobalFunc;

mutex cashmtx;//锁住现金

TradeAlgorithm::TradeAlgorithm(Company *comp)
{
	cp = comp;
}

TradeAlgorithm::~TradeAlgorithm(){}

//bs1买 2卖
//type1普通委托，2市价委托
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno,int type)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs,"1");
	else
		strcpy(strbs, "2");
	////////获取买一价API
	char *str_price1 = "20";//暂时所有都按20元交易
	float tpx = 20;////暂时所有都按20元交易
	//printf("tk:%s brokeshr:%d tpx:%f\n", tk, brokeshr, tpx);
	float tradefare = 0;
	if (tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1") < 0)
	{
		//税费计算失败怎么办？
	}
	cashmtx.lock();
	if (GetCash() < 0)
	{
		printf("获取现有资金量错误\n");
		exit(1);
	}
	//printf("cash:%f\n", cash);
	float tmp = cash;
	//cash -= brokeshr*tpx - cp->GetTax(tk, brokeshr, tpx, bs);
	cash -= tradefare - tpx * brokeshr;
	if (cash < 0)
	{
		cash = tmp;
		ret = 1;
	}
	else
	{
		char shrtmp[20];
		sprintf(shrtmp, "%d\0", brokeshr);
		if (brokeshr >= m_limit_shr)//超过累计数量进行市价委托
		{
			///市价委托API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno) >= 0)
			{
				//委托成功
				printf("市价委托已成功!tk:%s \n",tk);
			}
			else
			{

				printf("市价委托发送失败!tk:%s \n", tk);
				cash = tmp;
			}
		}
		else
		{
			//printf("tk:%s shrtmp:%s tpx:%s\n", tk, shrtmp, str_price1);
			/////普通委托API
			if (cp->NormalEntrust(tk, shrtmp, str_price1, strbs, eno) >= 0)
			{
				//委托成功
				printf("普通委托已发送!tk:%s \n", tk);
			}
			else
			{
				cash = tmp;
				printf("普通委托发送失败!tk:%s \n", tk);
			}
		}
	}
	cashmtx.unlock();
	return ret;
}

////检查委托情况.
int TradeAlgorithm::CheckEntrust(char *tk)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (cp->EntrustQry(tk, amt, tpx, status) >= 0)
	{
		if (strcmp(status,"8")==0)
		{
			printf("%s已经成交\n",tk);
			return 0;
		}
		else if (strcmp(status, "7") == 0)
		{
			printf("%s部分成交\n",tk);
			return atoi(amt);
		}
		else
		{
			printf("%s未成交 状态号%s\n",tk,status);
			return -1;
		}
	}
	else
	{
		printf("查询股票成交状态失败\n");
		return -1;
	}
}

//得到现在的现金值
int TradeAlgorithm::GetCash()
{
	char strcash[20];
	int ret = 0;
	if (cp->FundAry(strcash))
	{
		//cash = 1000000;
		cash = atof(strcash);
	}
	else
	{
		ret = -1;
	}
	return ret;
}

int TradeAlgorithm::EasyAlgorithm(Order *od)
{
	char *tk = od->tk;
	int acthold = m_holdmap[tk];
	int shr = od->shr;
	int ehour = atoi(strtok(od->etime, ":"));
	int emin = atoi(strtok(NULL, ":"));
	char realstatus[16] = { 0 };
	time_t rawtime;
	struct tm *tminfo;
	//检查持仓量
	if (strcmp(od->bs,"S")==0 && acthold < shr)
	{
		printf("The actholding of %s in not enough\n", tk);
		return -1;
	}
	while (1)
	{
		//拆单.
		time(&rawtime);
		tminfo = localtime(&rawtime);
		int restime = (ehour - tminfo->tm_hour) * 60 - tminfo->tm_min + emin;
		if (restime != 0 && shr != 0)//如果还有剩余时间和股票没交易
		{
			int brokeshr = shr / 100 / restime;//每次拆单的股票数量，尽量平摊在每分钟
			char eno[20];//用于保存委托编号，在撤单时可以处理
			if (brokeshr == 0)//不能平摊在剩下的每分钟里的情况，直接拆成100股
			{
				brokeshr = 100;
				//brokeshr = shr / 100 % restime;
			}
			else if (shr < 100 )//不足100股
			{
				brokeshr = shr;
			}
			else
			{
				//足够数量去划分
				brokeshr *= 100;
			}

			if (strcmp(od->bs, "B") == 0)
			{
				if (BuySellTk(tk, brokeshr, 1,eno) == 1)//没钱买
				{
					Sleep(INTERVAL_TIME);
					continue;
				}
				else
				{
					shr -= brokeshr;
				}
			}
			else
			{
				BuySellTk(tk, brokeshr,2,eno);
				shr -= brokeshr;
			}
			printf("tk:%s\tdirection:%s\tresttime:%d\trestshr:%d\tbrokeshr:%d\tcash:%f\n", tk, od->bs,restime, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
			cp->GetTrade(tk, realstatus);//查询交易状态.
			
			if (strcmp(realstatus,"0")!=0)//废单或者确认
			{
				if (cp->CancelEntrust(eno) >= 0)//撤单
				{
					printf("Cancel the entrust tk:%s\n",tk);
					shr += brokeshr;//把去掉的加回来
				}
				else
				{
					//如果撤单失败

				}
			}
			else
			{
				//已经完全成交.
				printf("%s tk: complete %d ",tk,brokeshr);
			}
			printf("resttime:%d\ttk:%s\trestshr:%d\tbrokeshr:%d\tcash:%f\n", restime, tk, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
		}
		else
		{
			
			break;
		}
	}
	printf("%s 交易结束，共交易%d,未交易%d\n", tk, od->shr - shr, shr);
	delete od;
	return 0;
}

int NormalVWAP(Order *od, int days, char *ipxdir)
{
	int ret = 0;
	//从今天开始向前轮询days天
	time_t timev;
	time(&timev);
	struct tm * now = localtime(&timev);
	for (int i = 0; i < days; i++)
	{
		timev = timev - (24 * 60 * 60);
		now = localtime(&timev);
	}
	
	return ret;
}