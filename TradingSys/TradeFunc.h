#pragma once
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
//#pragma once
#include <time.h>

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")



void ShowAnsData();
void ShowErrMsg(int iFunc);
//HSHLPHANDLE Run();
//�û���¼
int Login();
//��ȡ��Ҫ����
void SetNecessaryParam();
bool GetLocalIP(char* ip);
bool GetFirstMac(char * mac);
void put_file_str(const char* fname, char* format, ...);
//��ȡ�ֲ�
int GetShare();
//��ȡ���ս�����ϸ
int GetTrade();
//��ͨί��
//bs:1�� 2�� eno:ί�б��
int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs,OUT char *eno);
//����ί��
//eno��ί�б��entrust_no
int CancelEntrust( char *eno);
//����ί��
int EntrustBuyOrSell( char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
//�ɳ���ί�в�ѯ
int CancelEntrustQry();
//����ί�в�ѯ
int BatchEntrustQry();
//��ѯ����
int PriceQry(char *s_code, char *price1, int bs);
//ί�в�ѯ
//status:0δ��; 1����; 2�ѱ� ;6�ѳ�; 7���ֳɽ�; 8ȫ���ɽ�;
int EntrustQry(char *s_code, char *amt, char *tpx, char *status);
//�м۽���
//bs:1�� 2�� eno:ί�б��
int MarketPriceEntrust(char *s_code, char *ammount, char *bs, OUT char *eno);
//�ʽ���ϸ��ѯ.
int FundAry(char *out_cash);
//ǰ̨���ü���
float EntrustFare(char *tk,int amt,float tpx,char* bs);
//��Ʊ��Ϣ��ѯ
int StockQry(char *s_code);
//���������
int EntrustBuyAmount(char *s_code, char *tpx,OUT char *amount);
//��ʷ������Ϣ��ѯ
int HistDeliverQry(char *sdate, char *edate);
//��ʷ�ɽ���ѯ.
int HistBussinessQry(char *sdate, char *edate);
//��ʷ�ʽ�֤ȯ��ˮ��ѯ.
int HistFundStockQry(char *sdate, char *edate);
//��ʷ֤ȯ�ɽ�ͳ�Ʋ�ѯ
int HistTradeTotQry(char *sdate, char *edate);
//���ٳֲֲ�ѯ.
int StockFastQry();
//����֤ȯ�ɽ��ر�
int Subscribe();
//ȡ��֤ȯ�ɽ��ر�
int SubCancel();
//���ӷ�����
HSHLPHANDLE Connect();




