#pragma once
class Company
{
public:
	char orderdir[100];//order�ļ�Ŀ¼
	Company();
	//shr������tpx���׼۸�,bs���׷���1Ϊ��-1Ϊ��
	virtual float GetTax(char* tk,int shr, float tpx, int bs) = 0;
	~Company();
};

