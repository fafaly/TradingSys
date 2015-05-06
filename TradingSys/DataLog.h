#pragma once
class DataLog
{
private:
	char COMP_PRO_DIR[100] ;
	char fdate[20];
	char signal[20];
	char m_trade_detail_dir[150];//交易细节文件目录
	char m_acttrade_dir[150];//成交汇总文件目录
	char m_actholding_dir[150];//持有文件目录
public:
	/*
	* comp_no:公司代号
	* pro_no:产品代号
	* fdate:当日日期
	* sign_no:1 INFO 2 WARNING 3 ERROR
	*/
	DataLog(int comp_no, int pro_no, char *fdate, int sign_no);
	~DataLog();
	void WriteLog(char *);
	void Display(char *);
};

