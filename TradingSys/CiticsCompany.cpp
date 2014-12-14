#include "stdafx.h"
#include "CiticsCompany.h"
#include <string.h>

CiticsCompany::CiticsCompany()
{
	memset(orderdir,0,sizeof(orderdir));
	strcpy(orderdir,"20141215.test.csv");
}

float CiticsCompany::GetTax(char*tk,int shr, float tpx, int bs)
{
	float happencash = shr*tpx;
	const float comrate = 2.5 / 10000;
	const float stamprate = 3 / 1000;
	float transfer = 0;
	if (tk[0] == '6')
		transfer = shr * 6 / 10000;
	if (bs == 1)
		return happencash*(1 + comrate) + transfer;
	else if (bs == -1)
		return happencash*(1 + comrate + stamprate) + transfer;
	else
		return 0;
}

CiticsCompany::~CiticsCompany()
{
}
