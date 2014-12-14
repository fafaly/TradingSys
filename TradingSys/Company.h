#pragma once
class Company
{
public:
	char orderdir[100];//order文件目录
	Company();
	//shr股数，tpx交易价格,bs交易方向：1为买，-1为卖
	virtual float GetTax(char* tk,int shr, float tpx, int bs) = 0;
	~Company();
};

