#pragma once
class DataLog
{
private:
	char COMP_PRO_DIR[100] ;
	char fdate[20];
	char signal[20];
public:
	/*
	* comp_no:��˾����
	* pro_no:��Ʒ����
	* fdate:��������
	* sign_no:1 INFO 2 WARNING 3 ERROR
	*/
	DataLog(int comp_no, int pro_no, char *fdate, int sign_no);
	~DataLog();
	void WriteLog(char *);
	void Display(char *);
};

