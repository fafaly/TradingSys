#pragma once
#include "Company.h"

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
	//��ѯ�ɽ����ļ��ʱ��
	int INTERVAL_TIME=30000;
	//��ȡ�ֽ�.
	int GetCash();
	//��õ�ǰʱ��
	char *GetDate();
	//�������
	int BuySellTk(char *tk, int brokeshr, int bs, char *eno);
	//��齻���Ƿ����
	int CheckDeal(char *tk);

public:
	TradeAlgorithm(Company *comp);
	~TradeAlgorithm();
	//�����㷨
	int RunAlgorithm(Order *od);
};