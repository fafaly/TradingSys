#include "stdafx.h"
#include "Company.h"
#include <string.h>

Company::Company()
{}


Company::Company(char *fund, char *pwd, char *o_dir)
{
	strcpy(fund_account, fund);
	strcpy(password, pwd);
	strcpy(orderdir, o_dir);
}


Company::~Company()
{
}

int Company::Login(){ return -1; }
//��ȡ�ֲ�
int Company::GetShare(map<string,int>&){ return -1; }
//��ȡ���ս�����ϸ
int Company::GetTrade(){ return -1; }
//��ͨί��
//bs:1�� 2�� eno:ί�б��
int Company::NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, char *eno){ return -1; }
//����ί��
//eno��ί�б��entrust_no
int Company::CancelEntrust(char *eno){ return -1; }
//����ί��
int Company::EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs){ return -1; }
//�ɳ���ί�в�ѯ
int Company::CancelEntrustQry(){ return -1; }
//����ί�в�ѯ
int Company::BatchEntrustQry(){ return -1; }
//��ѯ����
int Company::PriceQry(char *s_code, char *price1, int bs){ return -1; }
//ί�в�ѯ
//status:0δ��; 1����; 2�ѱ� ;6�ѳ�; 7���ֳɽ�; 8ȫ���ɽ�;
int Company::EntrustQry(char *s_code, char *amt, char *tpx, char *status){ return -1; }
//�м۽���
//bs:1�� 2�� eno:ί�б��
int Company::MarketPriceEntrust(char *s_code, char *amount, char *bs, char *eno){ return -1; }
//�ʽ���ϸ��ѯ.
int Company::FundAry(char *out_cash){ return -1; }
//ǰ̨���ü���
float Company::EntrustFare(char *tk, int amt, float tpx, char* bs){ return -1; }
//��Ʊ��Ϣ��ѯ
int Company::StockQry(char *s_code){ return -1; }
//���������
int Company::EntrustBuyAmount(char *s_code, char *tpx, char *amount){ return -1; }
//��ʷ������Ϣ��ѯ
int Company::HistDeliverQry(char *sdate, char *edate){ return -1; }
//��ʷ�ɽ���ѯ.
int Company::HistBussinessQry(char *sdate, char *edate){ return -1; }
//��ʷ�ʽ�֤ȯ��ˮ��ѯ.
int Company::HistFundStockQry(char *sdate, char *edate){ return -1; }
//��ʷ֤ȯ�ɽ�ͳ�Ʋ�ѯ
int Company::HistTradeTotQry(char *sdate, char *edate){ return -1; }
//���ٳֲֲ�ѯ.
int Company::StockFastQry(){ return -1; }
//���ӷ�����
int Company::Connect(){ return -1; }