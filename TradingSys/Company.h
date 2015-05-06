#pragma once
#include<map>
using namespace std;
class Company
{
public:
	char fund_account[32];//�ʽ��˺�
	char password[32];//�ʽ�����
	char orderdir[100];//order�ļ�Ŀ¼
	Company();
	//fund���˺�
	//pwd:����
	//o_dir:order�ļ���λ��
	Company(char *fund, char *pwd, char *o_dir);
	//HSHLPHANDLE Run();
	//�û���¼
	virtual int Login();
	//��ȡ�ֲ�
	virtual int GetShare(map<string, int>&);
	//��ȡ���ս�����ϸ
	virtual int GetTrade(char *tk, char *realstatus);
	//��ͨί��
	//���룺bs:1�� 2�� 
	//�����eno ί�б��
	virtual int NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, char *eno);
	//����ί��
	//eno��ί�б��entrust_no
	virtual int CancelEntrust(char *eno);
	//����ί��
	virtual int EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
	//�ɳ���ί�в�ѯ
	virtual int CancelEntrustQry();
	//����ί�в�ѯ
	virtual int BatchEntrustQry();
	//��ѯ����
	virtual int PriceQry(char *s_code, char *price1, int bs);
	//ί�в�ѯ
	//status:0δ��; 1����; 2�ѱ� ;6�ѳ�; 7���ֳɽ�; 8ȫ���ɽ�;
	virtual int EntrustQry(char *s_code, char *amt, char *tpx,char *eno, char *status);
	//�м۽���
	//bs:1�� 2�� eno:ί�б��
	virtual int MarketPriceEntrust(char *s_code, char *ammount, char *bs, char *eno);
	//�ʽ���ϸ��ѯ.
	virtual int FundAry(char *out_cash);
	//ǰ̨���ü���
	virtual float EntrustFare(char *tk, int amt, float tpx, char* bs);
	//��Ʊ��Ϣ��ѯ
	virtual int StockQry(char *s_code);
	//���������
	//amount����
	virtual int EntrustBuyAmount(char *s_code, char *tpx, char *amount);
	//��ʷ������Ϣ��ѯ
	virtual int HistDeliverQry(char *sdate, char *edate);
	//��ʷ�ɽ���ѯ.
	virtual int HistBussinessQry(char *sdate, char *edate);
	//��ʷ�ʽ�֤ȯ��ˮ��ѯ.
	virtual int HistFundStockQry(char *sdate, char *edate);
	//��ʷ֤ȯ�ɽ�ͳ�Ʋ�ѯ
	virtual int HistTradeTotQry(char *sdate, char *edate);
	//���ٳֲֲ�ѯ.
	virtual int StockFastQry();
	//���ӷ�����
	virtual int Connect();
	//��������.
	~Company();
};

