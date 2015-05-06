#pragma once
#include "Company.h"
//#include "DataBaseFunc.h"
#include <map>
using namespace std;

struct Order
{
	char tk[10];//��Ʊ����
	int shr;//������
	char bs[2];//���׷���
	char stime[15];//��ʼʱ��
	char etime[15];//�ر�ʱ��
};

class TradeAlgorithm
{
private:
	//���ݿ��������
	//DataBaseFunc dbf;
	//Ĭ�ϴ����ȯ�̽ӿڵĹ���.
	Company *cp;
	//�ֽ�ֵ.
	float cash = 0;
	//���ڼ򵥽����㷨����Ʊ�����ۼƳ�����ֵ���м۽���.
	int m_limit_shr = 0;
	//��ѯ�ɽ����ļ��ʱ��,5s.
	const int m_chk_trade_time=5000;
	//�ȴ���ѯ�ɹ���ʱ��.
	const int m_re_entrust_time = 20000;
	//��ѯί��״̬��ʱ��2s.
	const int m_chk_entrust_time = 2000;
	//���׼��ʱ��
	const int m_trade_interval = 30000;
	//��־���ļ����.
	FILE *m_logfd;
	//��ȡ�ֽ�.
	int GetCash();
	//��õ�ǰʱ��
	char *GetDate();
	//bs1�� 2��
	//type1��ͨί�У�2�м�ί��
	int BuySellTk(char *tk, int brokeshr, int bs, char *eno,int type);
	//���ί���Ƿ����
	//status:8 �Ѿ��ɽ� 9 ���ֳɽ� ���඼��δ�ɽ��ĸ���״̬
	int CheckEntrust(char *tk, char *eno);
	//������гֲ��Ƿ�
	int CheckHoldShr(string tk);
	//ί��״̬.
	int EntrustStatus(int status);

public:
	//holding shares.
	map<string, int> m_holdmap;
	//���캯������Ҫ�����Ӧ�Ĺ�˾�ӿڵ�����.
	TradeAlgorithm(Company *comp);
	//�㷨��������.
	~TradeAlgorithm();

	//��ʼ���㷨�汾.
	//��ȡʱ����ھ��ֲ��ԣ����ۼƵ�һ������ʱ�м�����.
	int EasyAlgorithm(Order *od);

	//��׼VWAP�㷨.
	//�������ڼ�Ȩƽ���㷨.
	//od�洢���������.
	//days�����������ƽ��ֵ.
	//ipxdir���ipx�Ĵ�ŵ�ַ.
	int NormalVWAP(Order *od,int days,char *ipxdir);
};