#pragma once
#include "Company.h"
class CiticsCompany :public Company
{
private:
	
public:
	//char orderdir[100];//order�ļ�Ŀ¼
	CiticsCompany();
	~CiticsCompany();

	//bs:1�� 2��
	float GetTax(char*tk, int shr, float tpx, int bs);

};

