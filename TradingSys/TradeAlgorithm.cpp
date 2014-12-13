#include "stdafx.h"
#include "TradeAlgorithm.h"
#include "GlobalFunc.h"
#include <Windows.h>
#include <time.h>
#include <thread>
#include <vector>
#include <iostream>
using namespace std;
using namespace GlobalFunc;

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

void Buy(){};

void Sell(){};

//bs: buy:1  sell:-1
int RunAlgorithm(char *tk,int bs,int shr,int ehour,int emin)
{
	time_t rawtime;
	struct tm *tminfo;
	while (1)
	{
		time(&rawtime);
		tminfo = localtime(&rawtime);
		int restime = (ehour - tminfo->tm_hour)*60-tminfo->tm_min + emin - tminfo->tm_min;
		if (restime != 0 && shr != 0)
		{
			int brokeshr = shr / 100 / restime;
			if (brokeshr == 0)
				brokeshr = shr / 100 % restime;
			shr -= brokeshr;
			cout << "shr:" << shr << endl;
		}
		else
		{
			return 1;
		}
	} 
	return 0;
}

//启动线程
int Run()
{
	try
	{
		char *fname = "Z:\\data\\WindDB\\production\\order\\20141215.test.csv";
		char *fdate = GetDate();
		FILE *fp = F_OPEN(fname,"r",1,fdate);
		char strline[100];
		vector<thread*> threads;
		while (fgets(strline, sizeof(strline), fp) != NULL)
		{
			Order od;
			strcpy(od.tk, strtok(strline, ","));
			od.shr = atoi(strtok(NULL, ","));
			strcpy(od.bs, strtok(NULL, ","));
			strcpy(od.stime, strtok(NULL, ","));
			strcpy(od.etime, strtok(NULL, ","));
			thread *td = new thread(SubThread,od.tk,od.shr,od.bs,od.stime,od.etime);
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
void SubThread(char* tk, int shr, char* bs, char* stime, char* etime)
{
	try
	{
		time_t rawtime;
		struct tm *tminfo;
		//cout << this_thread::get_id() << endl;
		int shour = atoi(strtok(stime, ":"));
		int smin = atoi(strtok(NULL, ":"));
		int ehour = atoi(strtok(etime, ":"));
		int emin = atoi(strtok(NULL, ":"));
		int direction = 0;
		if (strcmp(bs,"B")==0)
		{
			direction = 1;
		}
		else
		{
			direction = -1;
		}
		do
		{
			time(&rawtime);
			tminfo = localtime(&rawtime);
			if (tminfo->tm_hour == shour && tminfo->tm_min == smin)
			{
				if (RunAlgorithm(tk, direction, shr, ehour,emin) > 0)
					break;
			}
		} while (1);
	}
	catch (...)
	{
	}
}