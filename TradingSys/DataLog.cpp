#include "stdafx.h"
#include "DataLog.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <io.h>

/*
* comp_no:公司代号 
* pro_no:产品代号
* fdate:当日日期
* sign_no:1 INFO 2 WARNING 3 ERROR
*/
DataLog::DataLog(int comp_no,int pro_no,char *fdate,int sign_no)
{
	strcpy(this->fdate,fdate);
	switch (pro_no)
	{
	case 1:
		strcpy(COMP_PRO_DIR, "Z:\\data\\WindDB\\production\\position\\log\\");
		break;
	case 2:
		strcpy(COMP_PRO_DIR, "Z:\\data\\WindDB\\production2\\position\\log\\");
		break;
	default:
		printf("[WARNING]WriteLog failed.Please Input the right production/company number\n");
		break;
	}
	switch (sign_no)
	{
	case 1:
		strcpy(signal, "[INFO]");
		break;
	case 2:
		strcpy(signal, "[WARN]");
		break;
	case 3:
		strcpy(signal, "[ERROR]");
		break;
	default:
		break;
	}
}


DataLog::~DataLog()
{
}

void DataLog::WriteLog(char *str)
{
	FILE *fp;
	char fname[100];
	char logstr[200];
	sprintf(fname, "%s%s.log.txt", this->COMP_PRO_DIR,fdate);
	fp = fopen(fname, "a+");
	if (!fp)
	{
		perror("fopen COMP_PRO_DIR");
		return;
	}
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime); 
	memset(logstr,0,sizeof(logstr));
	sprintf(logstr,"%s\t[%d:%d:%d]\t%s\n", signal,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,str);
	fwrite(logstr, strlen(logstr), 1,fp);
	printf(logstr);
	fclose(fp);
}

void DataLog::Display(char *str)
{
	printf("%s",str);
}
