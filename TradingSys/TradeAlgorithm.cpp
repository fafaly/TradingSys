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
//type1��ͨί�У�2�м�ί��
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno,int type)
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
	float tradefare = 0;
	if (tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1") < 0)
	{
		//˰�Ѽ���ʧ����ô�죿
	}
	cashmtx.lock();
	if (GetCash() < 0)
	{
		printf("��ȡ�����ʽ�������\n");
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
		if (brokeshr >= m_limit_shr)//�����ۼ����������м�ί��
		{
			///�м�ί��API
			char tmpbs[10];
			//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
			if (cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno) >= 0)
			{
				//ί�гɹ�
				printf("�м�ί���ѳɹ�!tk:%s \n",tk);
			}
			else
			{

				printf("�м�ί�з���ʧ��!tk:%s \n", tk);
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
				printf("��ͨί���ѷ���!tk:%s \n", tk);
			}
			else
			{
				cash = tmp;
				printf("��ͨί�з���ʧ��!tk:%s \n", tk);
			}
		}
	}
	cashmtx.unlock();
	return ret;
}

////���ί�����.
int TradeAlgorithm::CheckEntrust(char *tk)
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
	else
	{
		printf("��ѯ��Ʊ�ɽ�״̬ʧ��\n");
		return -1;
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
	int acthold = m_holdmap[tk];
	int shr = od->shr;
	int ehour = atoi(strtok(od->etime, ":"));
	int emin = atoi(strtok(NULL, ":"));
	char realstatus[16] = { 0 };
	time_t rawtime;
	struct tm *tminfo;
	//���ֲ���
	if (strcmp(od->bs,"S")==0 && acthold < shr)
	{
		printf("The actholding of %s in not enough\n", tk);
		return -1;
	}
	while (1)
	{
		//��.
		time(&rawtime);
		tminfo = localtime(&rawtime);
		int restime = (ehour - tminfo->tm_hour) * 60 - tminfo->tm_min + emin;
		if (restime != 0 && shr != 0)//�������ʣ��ʱ��͹�Ʊû����
		{
			int brokeshr = shr / 100 / restime;//ÿ�β𵥵Ĺ�Ʊ����������ƽ̯��ÿ����
			char eno[20];//���ڱ���ί�б�ţ��ڳ���ʱ���Դ���
			if (brokeshr == 0)//����ƽ̯��ʣ�µ�ÿ������������ֱ�Ӳ��100��
			{
				brokeshr = 100;
				//brokeshr = shr / 100 % restime;
			}
			else if (shr < 100 )//����100��
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
			cp->GetTrade(tk, realstatus);//��ѯ����״̬.
			
			if (strcmp(realstatus,"0")!=0)//�ϵ�����ȷ��
			{
				if (cp->CancelEntrust(eno) >= 0)//����
				{
					printf("Cancel the entrust tk:%s\n",tk);
					shr += brokeshr;//��ȥ���ļӻ���
				}
				else
				{
					//�������ʧ��

				}
			}
			else
			{
				//�Ѿ���ȫ�ɽ�.
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
	printf("%s ���׽�����������%d,δ����%d\n", tk, od->shr - shr, shr);
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