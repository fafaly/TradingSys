#pragma once
#include "Company.h"
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
#include <time.h>
using namespace std;

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")

#define IN
#define OUT
#define REQ_NUM "500"

class CiticsCompany :public Company
{
private:
	//const char fund_account[32] = "600511005";//�����ʽ��˺�
	//const char password[32] = "606869";//�����ʽ�����
	//char fund_account[32];//�ʽ��˺�
	//char password[32];//�ʽ�����

	char op_entrust_way[4] ;//�ⲿ����ͻ�������3��Զ��ί�У�����������Ҫ��Ӫҵ����ʹ�õ��ʽ��˺ſ�ͨ3Ȩ��
	char ClientName[16] ; //�ͻ���ƣ�һ����ƴ������ĸ,������޸ģ�

	char client_id[32];
	char user_token[64];
	char branch_no[8];
	char asset_prop[4];
	char sysnode_id[4];
	char entrust_no[16];
	char entrust_status[8];
	char op_station[128];
	char IP[16];
	char MAC[16];
	char HD[16];

	HSHLPHANDLE HlpHandle = NULL;
	bool GetLocalIP(char* ip);
	bool GetFirstMac(char * mac);

public:
	//char orderdir[100];//order�ļ�Ŀ¼
	CiticsCompany(char *fund, char *pwd, char *o_dir);
	~CiticsCompany();

	void ShowAnsData();
	void ShowErrMsg(int iFunc);
	//HSHLPHANDLE Run();
	//�û���¼
	int Login();
	//��ȡ��Ҫ����
	void SetNecessaryParam();
	void put_file_str(const char* fname, char* format, ...);
	//��ȡ�ֲ�
	int GetShare(map<string, int>&);
	//��ȡ���ս�����ϸ
	int GetTrade(char *tk,char *realstatus);
	//��ͨί��
	//bs:1�� 2�� eno:ί�б��
	int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, OUT char *eno);
	//����ί��
	//eno��ί�б��entrust_no
	int CancelEntrust(char *eno);
	//����ί��
	int EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
	//�ɳ���ί�в�ѯ
	int CancelEntrustQry();
	//����ί�в�ѯ
	int BatchEntrustQry();
	//��ѯ����
	int PriceQry(char *s_code, char *price1, int bs);
	//ί�в�ѯ
	//status:0δ��; 1����; 2�ѱ� ;6�ѳ�; 7���ֳɽ�; 8ȫ���ɽ�;
	int EntrustQry(IN char *s_code,OUT char *amt,OUT char *tpx,OUT char* eno,OUT char *status);
	//�м۽���
	//bs:1�� 2�� eno:ί�б��
	int MarketPriceEntrust(char *s_code, char *ammount, char *bs, OUT char *eno);
	//�ʽ���ϸ��ѯ.
	int FundAry(char *out_cash);
	//ǰ̨���ü���
	float EntrustFare(char *tk, int amt, float tpx, char* bs);
	//��Ʊ��Ϣ��ѯ
	int StockQry(char *s_code);
	//���������
	int EntrustBuyAmount(char *s_code, char *tpx, OUT char *amount);
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
	//���ӷ�����
	int Connect();
};

