// TradingSys.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TradeAlgorithm.h"
#include <string.h>

char fund_account[32];//资金账号
char password[32];//资金密码
char OUT_TP_DIR[100];

void InitialAccount(char *fund, char *pwd, char *out_dir)
{
	memset(fund_account, 0, strlen(fund_account));
	memset(password, 0, strlen(password));
	memset(OUT_TP_DIR, 0, strlen(OUT_TP_DIR));
	strcpy(fund_account, fund);
	strcpy(password, pwd);
	strcpy(OUT_TP_DIR, out_dir);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//InitialAccount("600511005", "606869", "");
	InitialAccount("1000000033", "606869", "");
	Run();
	return 0;
}

