#pragma once
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <Wincon.h>
#include <stdio.h>
#include "CITICs_HsT2Hlp.h"
#include <nb30.h >
//#pragma once
#include <time.h>

#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib, "CITICs_HsT2Hlp.lib")
#pragma comment(lib,"wsock32.lib")

void ShowAnsData();
void ShowErrMsg(int iFunc);
//HSHLPHANDLE Run();
int Login(HSHLPHANDLE);
void SetNecessaryParam();
bool GetLocalIP(char* ip);
bool GetFirstMac(char * mac);
void put_file_str(const char* fname, char* format, ...);
int GetShare();
int GetTrade();
int NormalEntrust( char *s_code, char *ammount, char *tpx);
int CancelEntrust( char *eno);
int EntrustBuyOrSell( char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs);
int CancelEntrustQry();
int EntrustQry();
int BatchEntrustQry();
HSHLPHANDLE Connect();


//const char fund_account[32] = "600511005";//测试资金账号
//const char password[32] = "606869";//测试资金密码
extern char fund_account[32];//资金账号
extern char password[32];//资金密码

extern char OUT_TP_DIR[100];


