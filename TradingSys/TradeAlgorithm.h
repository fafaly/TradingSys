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

//��ȡ�����ļ��������Ʒ�����ļ�
int ReadConfig();

//��ȡ��������Ϣ
int ReadOrder();

//���߳�
int Run();

//���߳�
void SubThread(Order *od);

//��ȡ�ֽ�.
int GetCash();