#pragma once

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
void SubThread(char* tk,int shr,char* bs,char* stime,char* etime);