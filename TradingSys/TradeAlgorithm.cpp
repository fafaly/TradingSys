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
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno, int type)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs, "1");
	else
		strcpy(strbs, "2");
	////////获取买一价API
	char *str_price1 = "20";//暂时所有都按20元交易
	float tpx = 20;////暂时所有都按20元交易
	//printf("tk:%s brokeshr:%d tpx:%f\n", tk, brokeshr, tpx);
	float tradefare = 0;
	if (tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1") < 0)
	{
		//税费计算失败怎么办？--》 按自己的算法去算（两者应该进行一个比较）
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
		if (type == 2)//超过累计数量进行市价委托
		{
			///市价委托API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno) >= 0)
			{
				//委托成功
				printf("tk:%s市价委托已成功! \n", tk);
			}
			else
			{

				printf("tk:%s市价委托发送失败! \n", tk);
				cash = tmp;
				ret = 2;//委托失败
			}
		}
		else
		{
			//printf("tk:%s shrtmp:%s tpx:%s\n", tk, shrtmp, str_price1);
			/////普通委托API
			if (cp->NormalEntrust(tk, shrtmp, str_price1, strbs, eno) >= 0)
			{
				//委托成功
				printf("tk:%s普通委托已成功! \n", tk);
			}
			else
			{
				cash = tmp;
				printf("tk:%s普通委托发送失败! \n", tk);
				ret = 2;//委托失败
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
		if (strcmp(status, "8") == 0)
		{
			printf("%s已经成交\n", tk);
			return 0;
		}
		else if (strcmp(status, "7") == 0)
		{
			printf("%s部分成交\n", tk);
			return atoi(amt);
		}
		else
		{
			printf("%s未成交 状态号%s\n", tk, status);
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
	int direction = 0;//买卖方向
	struct tm *tminfo;
	int failtimes = 0;//委托失败的次数
	int entrusttype = 0;//委托类型，1为普通 ，2为市价
	//检查持仓量
	if (strcmp(od->bs, "S") == 0 && acthold < shr)
	{
		printf("tk: %s 持仓量不足.结束此交易\n", tk);
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
			else if (shr < 100)//不足100股
			{
				brokeshr = shr;
			}
			else
			{
				//足够数量去划分
				brokeshr *= 100;
			}
			//决定委托方式
			if (failtimes >= 3)
			{
				entrusttype = 2;
				failtimes = 0;
			}
			else
			{
				entrusttype = 1;
			}
			//决定买卖方向
			if (strcmp(od->bs, "B") == 0)
			{
				direction = 1;
			}
			else
			{
				direction = 2;
			}
			//买卖股票的
			int ret = BuySellTk(tk, brokeshr, direction, eno, entrusttype);
			if (ret == 1)//没钱买
			{
				printf("金额不足：%f",cash);
				Sleep(INTERVAL_TIME);
				continue;
			}
			else if (ret == 2)
			{
				//委托失败
				//printf("tk：%s 委托失败\n", tk);
				failtimes++;
			}
			else
			{
				//委托成功
			}

			//	printf("tk:%s\t买卖方向:%s\t剩余时间:%d\t剩余交易量:%d\t拆单量:%d\t持有金额:%f\n", tk, od->bs,restime, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
			cp->GetTrade(tk, realstatus);//查询交易状态.

			if (strcmp(realstatus, "0") != 0)//废单或者确认状态
			{
				printf("%s tk: 交易未成功  ", tk);
				if (cp->CancelEntrust(eno) >= 0)//撤单
				{
					printf("tk:%s 撤单成功 \n", tk);
				}
				else
				{
					//如果撤单失败
					printf("tk:%s 撤单失败\n", tk);
				}
			}
			else
			{
				//已经完全成交.
				printf(" tk:%s 交易成功 %d ", tk, brokeshr);
				shr -= brokeshr;
			}
			printf("tk:%s\t剩余时间:%d\t剩余交易量:%d\t拆单量:%d\t持有金额:%f\n", tk, restime, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
		}
		else
		{
			//股票数为0 或者时间为0
			break;
		}
	}
	printf("%s 交易结束，共交易%d,未交易%d\n", tk, od->shr - shr, shr);
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