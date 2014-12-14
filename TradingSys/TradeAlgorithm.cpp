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

#define INTERVAL_TIME 60000

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

int BuyTk(char *tk, int brokeshr, Company *cp)
{
	int ret = 0;
	////////获取买一价API
	int tpx = 0;
	cashmtx.lock();
	float tmp = cash;
	cash -= brokeshr*tpx - cp->GetTax(tk,brokeshr,tpx,1);//扣去金额和税收
	if (cash < 0)
	{
		cash = tmp;
		ret = 1;
	}
	else
	{
		if (brokeshr >= 1500)
		{
			///市价买入API
		}
		else
		{
			/////买API
		}
	}
	cashmtx.unlock();
	return ret;
}

int SellTk(char *tk, int brokeshr, Company *cp)
{
	int ret = 1;
	///////获取卖一价API
	int tpx = 0;
	float happencash = brokeshr * tpx;
	cashmtx.lock();
	cash -= happencash - cp->GetTax(tk, brokeshr, tpx, -1);//扣去金额和税收
	if (brokeshr >= 1500)
	{
		///市价卖API
	}
	else
	{
		/////市价买API
	}
	cash += brokeshr*tpx;
	cashmtx.unlock();
	return ret;
}

//得到现在的现金值
int GetCash()
{
	cashmtx.lock();
	cash = 200000;//获取现金API
	cashmtx.unlock();
	return 0;
}


//bs: buy:1  sell:-1
int RunAlgorithm(Order *od, Company *cp)
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
		if (restime != 0 && shr != 0)
		{
			int brokeshr = shr / 100 / restime;
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
			shr -= brokeshr;
			if (strcmp(od->bs, "B") == 0)
			{
				BuyTk(tk,brokeshr,cp);
			}
			else
			{
				SellTk(tk,brokeshr,cp);
			}
			printf("tk:%s\trestshr:%d\tbrokeshr:%d\n", tk, shr, brokeshr);
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
		Connect();
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
			thread *td = new thread(SubThread, od,cp);
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
void SubThread(Order *od,Company *cp)
{
	try
	{
		time_t rawtime;
		struct tm *tminfo;
		//cout << this_thread::get_id() << endl;
		int shour = atoi(strtok(od->stime, ":"));
		int smin = atoi(strtok(NULL, ":"));
		//int ehour = atoi(strtok(etime, ":"));
		//int emin = atoi(strtok(NULL, ":"));
		//int direction = 0;
		//if (strcmp(bs,"B")==0)
		//{
		//	direction = 1;
		//}
		//else
		//{
		//	direction = -1;
		//}
		do
		{
			time(&rawtime);
			tminfo = localtime(&rawtime);
			if (tminfo->tm_hour == shour && tminfo->tm_min == smin)
			{
				if (RunAlgorithm(od,cp) > 0)
					break;
			}
			Sleep(2000);
		} while (1);
	}
	catch (...)
	{
	}
}