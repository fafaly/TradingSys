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
int Login(HSHLPHANDLE);
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
int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs);
//����ί��
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
int MarketPriceEntrust(char *s_code, char *ammount, char *bs);
//�˻������Ϣ��ѯ
int AccQry(char *);
//ǰ̨���ü���
float EntrustFare(char *tk,int amt,float tpx,char* bs);
//���ӷ�����
HSHLPHANDLE Connect();


//const char fund_account[32] = "600511005";//�����ʽ��˺�
//const char password[32] = "606869";//�����ʽ�����
extern char fund_account[32];//�ʽ��˺�
extern char password[32];//�ʽ�����

extern char OUT_TP_DIR[100];


