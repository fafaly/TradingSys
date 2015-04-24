// TradingSys.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TradeAlgorithm.h"
#include "CiticsCompany.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include "GlobalFunc.h"
using namespace std;
using namespace GlobalFunc;

//��ȡ�����ļ��������Ʒ�����ļ�
int ReadConfig();
//��ȡ��������Ϣ
int ReadOrder();
//���߳�
static void SubThread(Order *od, TradeAlgorithm *ta);

char * GetDate()
{
	char fdate[10];
	struct tm *newtime;
	char tmpbuf[30];
	time_t lllt1 = time(NULL);
	newtime = localtime(&lllt1);
	strftime(fdate, 30, "%Y%m%d", newtime);
	return fdate;
}

//ÿ�����ѯһ���Ƿ��˸÷��͸ý��׵�ʱ����
//У���ʱ�䰴���Ӻ�Сʱ�㣨�ȶ�����ʱ�䣩
void SubThread(Order *od, TradeAlgorithm *ta)
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
			if (tminfo->tm_hour >= shour && tminfo->tm_min >= smin)
			{
				if (ta->EasyAlgorithm(od) > 0)
					break;
			}
			Sleep(2000);
		} while (1);
	}
	catch (...)
	{
	}
}

//�����߳�
int Run(Company *cp,TradeAlgorithm *ta)
{
	try
	{
		cp->Connect();
		cp->Login();
		char tmpp[10], amt[10], tpx[10];
		//PriceQry("600008",tmpp,1);
		//cp->FundAry(tmpp);
		//StockQry("510300");
		//EntrustBuyAmount("510300", "4", amt);
		//cp->MarketPriceEntrust("000011", "100", "1", tmpp);
		//EntrustFare("510300", 1000, 4, "1");
		//HistDeliverQry("20141201", "20141224");
		//HistBussinessQry("20141201", "20141224");
		//HistFundStockQry("20141201", "20141224");
		//HistTradeTotQry("20141201", "20141224");
		//StockFastQry();
		//EntrustQry("600008", amt, tpx, tmpp);
		//Subscribe();
		//cout << tmpp << endl;
		//NormalEntrust("510300", "100", "1", "1", tmpp);
		//CancelEntrustQry();
		//CancelEntrust("1");

		char *fdate = GetDate();
		//ta->GetCash();
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
			thread *td = new thread(SubThread, od,ta);
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


int _tmain(int argc, _TCHAR* argv[])
{
	//InitialAccount("600511005", "606869", "");
	//InitialAccount("1000000033", "606869", "");
	Company *cp = new CiticsCompany("600511005", "606869", "20141216.order.csv");
	TradeAlgorithm *ta = new TradeAlgorithm(cp);
	Run(cp,ta);
	return 0;
}

