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

mutex cashmtx;//��ס�ֽ�



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
		printf("δ�ǼǸý������ʺ�\n");
		ret = -1;
		break;
	case -52:
		printf("�˹�Ʊ������\n");
		ret = -1;
		break;
	case -55:
		printf("���������Ͻ������涨\n");
		ret = -1;
		break;
	case -56:
		printf("ί�м۸񲻷��Ͻ������涨\n");
		ret = -1;
		break;
	case -57:
		printf("���ý���\n");
		ret = -2;
		break;
	case -58:
		printf("��Ʊ����\n");
		ret = -1;
		break;
	case -59:
		printf("����ʱ�䲻��ί��\n");
		ret = -1;
		break;
	case -64:
		printf("�˹ɲ��ܽ��д���ί��\n");
		ret = -1;
		break;
	case -65:
		printf("�˹�Ʊͣ�ƣ�����ί��\n");
		ret = -1;
		break;
	case -66:
		printf("�ʻ�״̬������\n");
		ret = -1;
		break;
	case -67:
		printf("��δ��ָ������\n");
		ret = -1;
		break;
	case -61:
		printf("ί��ʧ��\n");
		ret = -1;
		break;
	}
	return ret;
}

//bs1�� 2��
//type1��ͨί�У�2�м�ί��
int TradeAlgorithm::BuySellTk(char *tk, int brokeshr, int bs, char *eno, int type)
{
	int ret = 0;
	char strbs[2];
	if (bs == 1)
		strcpy(strbs, "1");
	else
		strcpy(strbs, "2");
	////////��ȡ��һ��API
	//char *str_price1 = "20";//��ʱ���ж���20Ԫ����
	//float tpx = 20;////��ʱ���ж���20Ԫ����
	float tpx=float(lv2(tk, bs, 1))/10000;//��ȡʵʱ�����level2����ǰ����
	char  str_price1[20] = {0};
	sprintf(str_price1,"%f",tpx);
	float tradefare = 0;
	if (tradefare = cp->EntrustFare(tk, brokeshr, tpx, "1") < 0)
	{
		//˰�Ѽ���ʧ����ô�죿--�� ���Լ����㷨ȥ�㣨����Ӧ�ý���һ���Ƚϣ�
	}
	cashmtx.lock();
	if (GetCash() < 0)
	{
		printf("��ȡ�����ʽ�������\n");
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
			if (type == 2)//�����ۼ����������м�ί��
			{
				///�м�ί��API
				char tmpbs[10];
				//printf("tk:%s shrtmp:%s bs:%s\n", tk, shrtmp,"1");
				entrustret = cp->MarketPriceEntrust(tk, shrtmp, _itoa(bs, tmpbs, 10), eno);
				if (entrustret >= 0)
				{
					//ί�гɹ�
					printf("tk:%s�м�ί�з��ͳɹ�! \n", tk);
				}
				else
				{

					printf("tk:%s�м�ί�з���ʧ��! \n", tk);
					cash = tmp;
					ret = 2;//ί��ʧ��
				}
			}
			else
			{
				//printf("tk:%s shrtmp:%s tpx:%s\n", tk, shrtmp, str_price1);
				/////��ͨί��API
				entrustret = cp->NormalEntrust(tk, shrtmp, str_price1, strbs, eno);
				if (entrustret >= 0)
				{
					//ί�гɹ�
					printf("tk:%s��ͨί�з��ͳɹ�! \n", tk);
				}
				else
				{
					cash = tmp;
					printf("tk:%s��ͨί�з���ʧ��! \n", tk);
					ret = 2;//ί��ʧ��
				}
			}
			if (EntrustStatus(entrustret) == -1)
			{
				printf("�ù�Ʊ�˳�");
			}
		}
	}
	cashmtx.unlock();
	return ret;
}

////���ί�����.
int TradeAlgorithm::CheckEntrust(char *tk,char *eno)
{
	char amt[20];
	char tpx[10];
	char status[3];
	if (cp->EntrustQry(tk, amt, tpx,eno, status) >= 0)
	{
		if (strcmp(status, "8") == 0)
		{
			printf("%s�Ѿ��ɽ�\n", tk);
			return 0;
		}
		else if (strcmp(status, "7") == 0)
		{
			printf("%s���ֳɽ�\n", tk);
			return atoi(amt);
		}
		else
		{
			printf("%sδ�ɽ� ״̬��%s\n", tk, status);
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
	int direction = 0;//��������
	struct tm *tminfo;
	int failtimes = 0;//ί��ʧ�ܵĴ���
	int entrusttype = 0;//ί�����ͣ�1Ϊ��ͨ ��2Ϊ�м�
	//���ֲ���
	if (strcmp(od->bs, "S") == 0 && acthold < shr)
	{
		printf("tk: %s �ֲ�������.�����˽���\n", tk);
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
			else if (shr < 100)//����100��
			{
				brokeshr = shr;
			}
			else
			{
				//�㹻����ȥ����
				brokeshr *= 100;
			}
			//����ί�з�ʽ
			if (failtimes >= 3)
			{
				entrusttype = 2;
				failtimes = 0;
			}
			else
			{
				entrusttype = 1;
			}
			//������������
			if (strcmp(od->bs, "B") == 0)
			{
				direction = 1;
			}
			else
			{
				direction = 2;
			}
			//ί��
			int ret = BuySellTk(tk, brokeshr, direction, eno, entrusttype);
			if (ret == 1)//����Ǯί��
			{
				printf("���㣺%f",cash);
				Sleep(m_re_entrust_time);
				continue;
			}
			else if (ret == 2)
			{
				//ί��ʧ��
				//printf("tk��%s ί��ʧ��\n", tk);
				failtimes++;
			}
			else if (ret == -1)
			{ 
				//ȡ�������
				printf("ί��ʱȡ�ֽ�ʧ���Ժ�����\n");
				Sleep(m_re_entrust_time);
				continue;
			}
			else
			{
				//ί�з��ͳɹ���cash�Ѿ��������ĺ����м���
				//���ί��״̬
				Sleep(m_chk_entrust_time);
				brokeshr = CheckEntrust(tk,eno);
			}

			//	printf("tk:%s\t��������:%s\tʣ��ʱ��:%d\tʣ�ཻ����:%d\t����:%d\t���н��:%f\n", tk, od->bs,restime, shr, brokeshr, cash);
			Sleep(m_chk_trade_time);//�ȴ���Ʊ�Ƿ��׳ɹ�
			cp->GetTrade(tk, realstatus);//��ѯ����״̬.

			if (strcmp(realstatus, "0") != 0)//�ϵ�����ȷ��״̬
			{
				printf("%s tk: ����δ�ɹ�  ", tk);
				if (cp->CancelEntrust(eno) >= 0)//����
				{
					printf("tk:%s �����ɹ� \n", tk);
					continue;
				}
				else
				{
					//�������ʧ��
					printf("tk:%s ����ʧ��\n", tk);
				}
			}
			else
			{
				//�Ѿ���ȫ�ɽ�.
				printf(" tk:%s ���׳ɹ� %d ", tk, brokeshr);
				m_holdmap[tk] += brokeshr;
				shr -= brokeshr;
			}
			printf("tk:%s\tʣ��ʱ��:%d\tʣ�ཻ����:%d\t����:%d\t���н��:%f\n", tk, restime, shr, brokeshr, cash);
			Sleep(m_chk_trade_time);
		}
		else
		{
			//��Ʊ��Ϊ0 ����ʱ��Ϊ0
			break;
		}
	}
	printf("%s ���׽�����������%d,δ����%d\n", tk, od->shr - shr, shr);
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