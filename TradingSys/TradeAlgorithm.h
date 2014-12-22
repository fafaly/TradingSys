#pragma once
#include "Company.h"

struct Order
{
	char tk[10];//股票代码
	int shr;//交易量
	char bs[2];//交易方向
	char stime[15];//起始时间
	char etime[15];//关闭时间
};

//读取配置文件，例如产品几的文件
int ReadConfig();

//读取订单的信息
int ReadOrder();

//主线程
int Run();

//子线程
void SubThread(Order *od);

//获取现金.
int GetCash();