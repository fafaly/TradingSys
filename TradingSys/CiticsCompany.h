#pragma once
#include "Company.h"
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
#include <time.h>

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")

#define IN
#define OUT

class CiticsCompany :public Company
{

public:
	//char orderdir[100];//orderÎÄ¼þÄ¿Â¼
	CiticsCompany();
	~CiticsCompany();
};

