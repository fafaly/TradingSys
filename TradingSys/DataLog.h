#pragma once
class DataLog
{
private:
	char COMP_PRO_DIR[100] ;
	char fdate[20];
	char signal[20];
	char m_trade_detail_dir[150];//����ϸ���ļ�Ŀ¼
	char m_acttrade_dir[150];//�ɽ������ļ�Ŀ¼
	char m_actholding_dir[150];//�����ļ�Ŀ¼
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

