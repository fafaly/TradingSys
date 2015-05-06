#include "stdafx.h"
#include "DataLog.h"
#include "GlobalFunc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace GlobalFunc
{
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
	
	float  LoadGetLevel2PxDll(char *tk,int direction,int index)
	{
		HINSTANCE hdll;
		typedef float(*GetLevel2Px)(char * , int , int );
		void LoadGetLevel2PxDll();
		GetLevel2Px lv2;
		hdll = LoadLibrary(_T("kmdsHQdll.dll"));
		if (hdll == NULL)
		{
			FreeLibrary(hdll);
		}
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