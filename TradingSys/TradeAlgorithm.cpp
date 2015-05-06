#include "stdafx.h"
#include "TradeAlgorithm.h"
#include "GlobalFunc.h"
#include "Company.h"
#include "CiticsCompany.h"
//#include "DataBaseFunc.h"
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

int TradeAlgorithm::EntrustStatus(int status)
{
	int ret = 0;
	switch (status)
	{
	case -51:
		printf("未登记该交易所帐号\n");
		ret = -1;
		break;
	case -52:
		printf("此股票不存在\n");
		ret = -1;
		break;
	case -55:
		printf("数量不符合交易所规定\n");
		ret = -1;
		break;
	case -56:
		printf("委托价格不符合交易所规定\n");
		ret = -1;
		break;
	case -57:
		printf("可用金额不足\n");
		ret = -2;
		break;
	case -58:
		printf("股票余额不足\n");
		ret = -1;
		break;
	case -59:
		printf("闭市时间不能委托\n");
		ret = -1;
		break;
	case -64:
		printf("此股不能进行此类委托\n");
		ret = -1;
		break;
	case -65:
		printf("此股票停牌，不能委托\n");
		ret = -1;
		break;
	case -66:
		printf("帐户状态不正常\n");
		ret = -1;
		break;
	case -67:
		printf("您未作指定交易\n");
		ret = -1;
		break;
	case -61:
		printf("委托失败\n");
		ret = -1;
		break;
	}
	return ret;
}

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
	//char *str_price1 = "20";//暂时所有都按20元交易
	//float tpx = 20;////暂时所有都按20元交易
	float tpx=float(lv2(tk, bs, 1))/10000;//获取实时行情的level2的最前数据
	char  str_price1[20] = {0};
	sprintf(str_price1,"%f",tpx);
	float tradefare = 0;
	if (tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1") < 0)
	{
		//税费计算失败怎么办？--》 按自己的算法去算（两者应该进行一个比较）
	}
	cashmtx.lock();
	if (GetCash() < 0)
	{
		printf("获取现有资金量错误\n");
		ret = -1;
	}
	else
	{
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
			int entrustret = 0;
			if (type == 2)//超过累计数量进行市价委托
			{
				///市价委托API
				char tmpbs[10];
				//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
				entrustret = cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno);
				if (entrustret >= 0)
				{
					//委托成功
					printf("tk:%s市价委托发送成功! \n", tk);
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
				entrustret = cp->NormalEntrust(tk, shrtmp, str_price1, strbs, eno);
				if (entrustret >= 0)
				{
					//委托成功
					printf("tk:%s普通委托发送成功! \n", tk);
				}
				else
				{
					cash = tmp;
					printf("tk:%s普通委托发送失败! \n", tk);
					ret = 2;//委托失败
				}
			}
			if (EntrustStatus(entrustret) == -1)
			{
				printf("该股票退出");
			}
		}
	}
	cashmtx.unlock();
	return ret;
}

////检查委托情况.
int TradeAlgorithm::CheckEntrust(char *tk,char *eno)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (cp->EntrustQry(tk, amt, tpx,eno, status) >= 0)
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
			//委托
			int ret = BuySellTk(tk, brokeshr, direction, eno, entrusttype);
			if (ret == 1)//不够钱委托
			{
				printf("金额不足：%f",cash);
				Sleep(m_re_entrust_time);
				continue;
			}
			else if (ret == 2)
			{
				//委托失败
				//printf("tk：%s 委托失败\n", tk);
				failtimes++;
			}
			else if (ret == -1)
			{ 
				//取不到金额
				printf("委托时取现金失败稍后再试\n");
				Sleep(m_re_entrust_time);
				continue;
			}
			else
			{
				//委托发送成功，cash已经在买卖的函数中减掉
				//检查委托状态
				Sleep(m_chk_entrust_time);
				brokeshr = CheckEntrust(tk,eno);
			}

			//	printf("tk:%s\t买卖方向:%s\t剩余时间:%d\t剩余交易量:%d\t拆单量:%d\t持有金额:%f\n", tk, od->bs,restime, shr, brokeshr, cash);
			Sleep(m_chk_trade_time);//等待股票是否交易成功
			cp->GetTrade(tk, realstatus);//查询交易状态.

			if (strcmp(realstatus, "0") != 0)//废单或者确认状态
			{
				printf("%s tk: 交易未成功  ", tk);
				if (cp->CancelEntrust(eno) >= 0)//撤单
				{
					printf("tk:%s 撤单成功 \n", tk);
					continue;
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
				m_holdmap[tk] += brokeshr;
				shr -= brokeshr;
			}
			printf("tk:%s\t剩余时间:%d\t剩余交易量:%d\t拆单量:%d\t持有金额:%f\n", tk, restime, shr, brokeshr, cash);
			Sleep(m_chk_trade_time);
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