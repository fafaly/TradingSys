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

mutex cashmtx;//��ס�ֽ�


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

//bs1�� 2��
int BuySellTk(char *tk, int brokeshr,int bs,char *eno)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs,"1");
	else
		strcpy(strbs, "2");
	////////��ȡ��һ��API
	char *str_price1 = "20";//��ʱ���ж���20Ԫ����
	//PriceQry(tk, str_price1, bs); ���鹦���ڲ��Ի����ﲻ����
	float tpx = 20;////��ʱ���ж���20Ԫ����
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
			///�м�ί��API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (MarketPriceEntrust(tk, shrtmp, _itoa(bs,tmpbs,10),eno)>=0)
			{
				//ί�гɹ�
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
			/////��ͨί��API
			if (NormalEntrust(tk, shrtmp, str_price1, strbs, eno)>=)
			{
				//ί�гɹ�
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

//���ɽ����.
int CheckDeal(char *tk)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (EntrustQry(tk, amt, tpx, status) >= 0)
	{
		if (strcmp(status,"8")==0)
		{
			printf("%s�Ѿ��ɽ�\n",tk);
			return 0;
		}
		else if (strcmp(status, "7") == 0)
		{
			printf("%s���ֳɽ�\n",tk);
			return atoi(amt);
		}
		else
		{
			printf("%sδ�ɽ� ״̬��%s\n",tk,status);
			return -1;
		}
	}
}

//�õ����ڵ��ֽ�ֵ
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
		if (restime != 0 && shr != 0)//�������ʣ��ʱ��͹�Ʊû����
		{
			int brokeshr = shr / 100 / restime;//����ƽ̯��ÿ����
			char eno[20];//���ڱ���ί�б�ţ��ڳ���ʱ���Դ���
			if (brokeshr == 0)//����ƽ̯��ʣ�µ�ÿ������������ֱ�Ӳ��100��
			{
				brokeshr = 100;
				//brokeshr = shr / 100 % restime;
			}
			else if (shr % 100 != 0)//����100��
			{
				brokeshr = shr;
			}
			else
			{
				//�㹻����ȥ����
				brokeshr *= 100;
			}

			if (strcmp(od->bs, "B") == 0)
			{
				if (BuySellTk(tk, brokeshr, 1,eno) == 1)//ûǮ��
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
			if (realamt > 0)//������ֳɽ��Ļ�
			{
				shr += brokeshr - realamt;
			}
			else if (realamt < 0)//��ȫû�ɽ�
			{
				if (CancelEntrust(eno) > 0)//����
				{
					printf("Cancel the entrust tk:%s\n",tk);
					shr += brokeshr;//��ȥ���ļӻ���
				}
				else
				{
					//�������ʧ����ô�죿
				}
			}
			else
			{
				//�Ѿ���ȫ�ɽ�.
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

//�����߳�
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

//ÿ�����ѯһ���Ƿ��˸÷��͸ý��׵�ʱ����
//У���ʱ�䰴���Ӻ�Сʱ�㣨�ȶ�����ʱ�䣩
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