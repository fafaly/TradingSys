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
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno)
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
	float tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1");
	cashmtx.lock();
	GetCash();
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
		if (brokeshr >= 1500)
		{
			///市价委托API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno) >= 0)
			{
				//委托成功
				printf("Market price entrust success!tk:%s \n",tk);
			}
			else
			{
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
				printf("Normal entrust success!tk:%s \n", tk);
			}
			else
			{
				cash = tmp;
			}
		}
	}
	cashmtx.unlock();
	return ret;
}

//检查成交情况.
int TradeAlgorithm::CheckDeal(char *tk)
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
	int shr = od->shr;
	int ehour = atoi(strtok(od->etime, ":"));
	int emin = atoi(strtok(NULL, ":"));
	time_t rawtime;
	struct tm *tminfo;
	while (1)
	{
		time(&rawtime);
		tminfo = localtime(&rawtime);
		int restime = (ehour - tminfo->tm_hour) * 60 - tminfo->tm_min + emin;
		if (restime != 0 && shr != 0)//如果还有剩余时间和股票没交易
		{
			int brokeshr = shr / 100 / restime;//尽量平摊在每分钟
			char eno[20];//用于保存委托编号，在撤单时可以处理
			if (brokeshr == 0)//不能平摊在剩下的每分钟里的情况，直接拆成100股
			{
				brokeshr = 100;
				//brokeshr = shr / 100 % restime;
			}
			else if (shr % 100 != 0)//不足100股
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
			int realamt = CheckDeal(tk);
			if (realamt > 0)//如果部分成交的话
			{
				shr += brokeshr - realamt;
			}
			else if (realamt < 0)//完全没成交
			{
				if (cp->CancelEntrust(eno) >= 0)//撤单
				{
					printf("Cancel the entrust tk:%s\n",tk);
					shr += brokeshr;//把去掉的加回来
				}
				else
				{
					//如果撤销失败怎么办？
				}
			}
			else
			{
				//已经完全成交.
			}
			printf("resttime:%d\ttk:%s\trestshr:%d\tbrokeshr:%d\tcash:%f\n", restime, tk, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
		}
		else
		{
			return 1;
		}
	}
	printf("%s finish.\n",tk);
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