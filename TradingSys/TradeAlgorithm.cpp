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

mutex cashmtx;//��ס�ֽ�

TradeAlgorithm::TradeAlgorithm(Company *comp)
{
	cp = comp;
}

TradeAlgorithm::~TradeAlgorithm(){}

//bs1�� 2��
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs,"1");
	else
		strcpy(strbs, "2");
	////////��ȡ��һ��API
	char *str_price1 = "20";//��ʱ���ж���20Ԫ����
	float tpx = 20;////��ʱ���ж���20Ԫ����
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
			///�м�ί��API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno) >= 0)
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
			if (cp->NormalEntrust(tk, shrtmp, str_price1, strbs, eno) >= 0)
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
int TradeAlgorithm::CheckDeal(char *tk)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (cp->EntrustQry(tk, amt, tpx, status) >= 0)
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
			printf("tk:%s\tdirection:%s\tresttime:%d\trestshr:%d\tbrokeshr:%d\tcash:%f\n", tk, od->bs,restime, shr, brokeshr, cash);
			Sleep(INTERVAL_TIME);
			int realamt = CheckDeal(tk);
			if (realamt > 0)//������ֳɽ��Ļ�
			{
				shr += brokeshr - realamt;
			}
			else if (realamt < 0)//��ȫû�ɽ�
			{
				if (cp->CancelEntrust(eno) >= 0)//����
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
	printf("%s finish.\n",tk);
	delete od;
	return 0;
}

int NormalVWAP(Order *od, int days, char *ipxdir)
{
	int ret = 0;
	//�ӽ��쿪ʼ��ǰ��ѯdays��
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