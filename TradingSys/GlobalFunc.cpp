#include "stdafx.h"
#include "DataLog.h"
#include "GlobalFunc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace GlobalFunc
{

	HINSTANCE hdll;

	FILE *F_OPEN(char *fname, char *mode, int log_no, char *fdate)
	{
		FILE *fp = fopen(fname, mode);
		char logstr[100];
		if (!fp)
		{
			DataLog dl(1, log_no, fdate, 3);
			sprintf(logstr, "%s is not existed.Check it please!", fname);
			dl.WriteLog(logstr);
			system("pause");
			exit(1);
		}
		else
		{
			return fp;
		}
	}

	void LoadDll(char *dllname)
	{
		hdll = LoadLibrary(_T("kmdsHQdll.dll"));
		if (hdll == NULL)
		{
			FreeLibrary(hdll);
		}
		
		//typedef void(*SubscribeData)();
		//SubscribeData sd;
		//sd = (SubscribeData)GetProcAddress(hdll, "SubscribeData");
		//if (sd == NULL)
		//{
		//	FreeLibrary(hdll);
		//}
		//sd();
	}


	void LoadSubscribeData()
	{
		typedef void(*SubscribeData)();
		SubscribeData sd;
		sd = (SubscribeData)GetProcAddress(hdll, "SubscribeData");
		if (sd == NULL)
		{
			FreeLibrary(hdll);
		}
		sd();
	}
	
	float  LoadGetLevel2PxDll(char *tk,int direction,int index)
	{
		typedef float(*GetLevel2Px)(char * , int , int );
		GetLevel2Px lv2;
		lv2 = (GetLevel2Px)GetProcAddress(hdll,"GetLevel2Px" );
		if (lv2 == NULL)
		{
			FreeLibrary(hdll);
		}
		return lv2(tk, direction, index);
	}

	void DetachDll(HINSTANCE hdll)
	{
		FreeLibrary(hdll);
	}
}