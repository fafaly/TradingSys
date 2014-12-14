#pragma once
#include "Company.h"
class CiticsCompany :public Company
{
private:
	
public:
	//char orderdir[100];//orderÎÄ¼şÄ¿Â¼
	CiticsCompany();
	~CiticsCompany();

	float GetTax(char*tk, int shr, float tpx, int bs);

};

