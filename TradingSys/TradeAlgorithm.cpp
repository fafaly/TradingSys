#include "stdafx.h"
#include "TradeAlgorithm.h"
#include "GlobalFunc.h"
#include "Company.h"
#include "CiticsCompany.h"
#include "TradeFunc.h"
#include <Windows.h>
#include <time.h>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
using namespace std;
using namespace GlobalFunc;

#define INTERVAL_TIME 30000

mutex cashmtx;//锁住现金


float cash = 0;

char *GetDate()
{
	char fdate[10];
	struct tm *newtime;
	char tmpbuf[30];
	time_t lllt1 = time(NULL);
	newtime = localtime(&lllt1);
	strftime(fdate, 30, "%Y%m%d", newtime);
	return fdate;
}

//bs1买 2卖
int BuySellTk(char *tk, int brokeshr,int bs,char *eno)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs,"1");
	else
		strcpy(strbs, "2");
	////////获取买一价API
	char *str_price1 = "20";//暂时所有都按20元交易
	//PriceQry(tk, str_price1, bs); 行情功能在测试环境里不能用
	float tpx = 20;////暂时所有都按20元交易
	//printf("tk:%s brokeshr:%d tpx:%f\n", tk, brokeshr, tpx);
	float tradefare = EntrustFare(tk, brokeshr, tpx, "1");
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
			if (MarketPriceEntrust(tk, shrtmp, _itoa(bs,tmpbs,10),eno)>=0)
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
			if (NormalEntrust(tk, shrtmp, str_price1, strbs, eno)>=)
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
int CheckDeal(char *tk)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (EntrustQry(tk, amt, tpx, status) >= 0)
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
int GetCash()
{
	char strcash[20];
	int ret = 0;
	if (FundAry(strcash))
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


 
int RunAlgorithm(Order *od)
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
			printf("resttime:%d\ttk:%s\trestshr:%d\tbrokeshr:%d\tcash:%f\n",restime, tk, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
			int realamt = CheckDeal(tk);
			if (realamt > 0)//如果部分成交的话
			{
				shr += brokeshr - realamt;
			}
			else if (realamt < 0)//完全没成交
			{
				if (CancelEntrust(eno) > 0)//撤单
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
	delete od;
	return 0;
}

//启动线程
int Run()
{
	try
	{
		HSHLPHANDLE HlpHandle=Connect();
		Login();
		//CheckDeal("600008");
		//char tmpp[10],amt[10],tpx[10];
		//FundAry(tmpp);
		//PriceQry("600008",tmpp,1);
		//EntrustQry("600008", amt, tpx, tmpp);
		//cout << tmpp << endl;
		//NormalEntrust("600009", "100", "1","1");
		//CancelEntrustQry();
		//CancelEntrust("1");
		//exit(1);
		Company *cp = new CiticsCompany();
		char *fdate = GetDate();
		GetCash();
		FILE *fp = F_OPEN(cp->orderdir, "r", 1, fdate);
		char strline[100];
		vector<thread*> threads;
		while (fgets(strline, sizeof(strline), fp) != NULL)
		{
			Order *od = new Order();
			strcpy(od->tk, strtok(strline, ","));
			od->shr = atoi(strtok(NULL, ","));
			strcpy(od->bs, strtok(NULL, ","));
			strcpy(od->stime, strtok(NULL, ","));
			strcpy(od->etime, strtok(NULL, ","));
			thread *td = new thread(SubThread, od);
			threads.push_back(td);
		}
		fclose(fp);
		for (auto& thread : threads){
			thread->join();
		}
		cout << "Main Thread" << "\t" << this_thread::get_id() << endl;
	}
	catch (...)
	{
	}
	return 0;
}

//每五秒查询一下是否到了该发送该交易的时间了
//校验的时间按分钟和小时算（比对两个时间）
void SubThread(Order *od)
{
	try
	{
		time_t rawtime;
		struct tm *tminfo;
		//cout << this_thread::get_id() << endl;
		int shour = atoi(strtok(od->stime, ":"));
		int smin = atoi(strtok(NULL, ":"));
		do
		{
			time(&rawtime);
			tminfo = localtime(&rawtime);
			if (tminfo->tm_hour == shour && tminfo->tm_min == smin)
			{
				if (RunAlgorithm(od) > 0)
					break;
			}
			Sleep(2000);
		} while (1);
	}
	catch (...)
	{
	}
}