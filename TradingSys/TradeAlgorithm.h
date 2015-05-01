#pragma once
#include "Company.h"
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
	//Ĭ�ϴ����ȯ�̽ӿڵĹ���
	Company *cp;
	//�ֽ�ֵ
	float cash = 0;
	//���ڼ򵥽����㷨����Ʊ�����ۼƳ�����ֵ���м۽���
	int m_limit_shr = 0;
	//��ѯ�ɽ����ļ��ʱ��
	int INTERVAL_TIME=30000;

	//��ȡ�ֽ�.
	int GetCash();
	//��õ�ǰʱ��
	char *GetDate();
	//bs1�� 2��
	//type1��ͨί�У�2�м�ί��
	int BuySellTk(char *tk, int brokeshr, int bs, char *eno,int type);
	//���ί���Ƿ����
	//status:8 �Ѿ��ɽ� 9 ���ֳɽ� ���඼��δ�ɽ��ĸ���״̬
	int CheckEntrust(char *tk);
	//������гֲ��Ƿ�
	int CheckHoldShr(string tk);

public:
	//holding shares
	map<string, int> m_holdmap;
	//���캯������Ҫ�����Ӧ�Ĺ�˾�ӿڵ�����
	TradeAlgorithm(Company *comp);
	//�㷨��������
	~TradeAlgorithm();

	//��ʼ���㷨�汾
	//��ȡʱ����ھ��ֲ��ԣ����ۼƵ�һ������ʱ�м�����
	int EasyAlgorithm(Order *od);

	//��׼VWAP�㷨
	//�������ڼ�Ȩƽ���㷨
	//od�洢���������
	//days�����������ƽ��ֵ
	//ipxdir���ipx�Ĵ�ŵ�ַ
	int NormalVWAP(Order *od,int days,char *ipxdir);
};