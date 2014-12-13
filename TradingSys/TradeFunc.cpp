#include "stdafx.h"
#include "TradeFunc.h"

const char op_entrust_way[4] = "5";//�ⲿ����ͻ�������3��Զ��ί�У�����������Ҫ��Ӫҵ����ʹ�õ��ʽ��˺ſ�ͨ3Ȩ��
const char ClientName[16] = "ZXZQ"; //�ͻ���ƣ�һ����ƴ������ĸ,������޸ģ�

char client_id[32];
char user_token[64];
char branch_no[8];
char asset_prop[4];
char sysnode_id[4];
char entrust_no[16];
char entrust_status[8];
char op_station[128];
char IP[16];
char MAC[16];
char HD[16];

HSHLPHANDLE Run()
{
	int iRet;
	char szMsg[512];

	HSHLPCFGHANDLE hConfig1;
	//	HSHLPCFGHANDLE hConfig2;
	HSHLPHANDLE HlpHandle = NULL;
	if (!GetLocalIP(IP))
	{
		printf("��ȡ����IPCʧ�ܣ����ֶ���������IP����Ӧ�ַ�����\n");
	}
	if (!GetFirstMac(MAC))
	{
		printf("��ȡ����MACʧ�ܣ����ֶ����뱾��MAC����Ӧ�ַ�����\n");
	}
	//op_station��Ҫ�ϸ������й�����д
	strcat(op_station, "tyjr-");//Ĭ��ǰ׺���벻Ҫ�޸�
	strcat(op_station, ClientName);
	strcat(op_station, " IP:");
	strcat(op_station, IP);
	strcat(op_station, " MAC:");
	strcat(op_station, MAC);
	//strcat(op_station," HD:");
	//strcat(op_station,HD);

	iRet = CITICs_HsHlp_LoadConfig(&hConfig1, "Hsconfig.ini");
	if (iRet)
	{
		printf("���������ļ�ʧ��[Hsconfig.ini] ErrorCdoe=(%d)....\n", iRet);
		return HlpHandle;
	}

	/// ����ʹ�ò�ͬ�����þ����ʼ�����Ӿ���������������ӵ���ͬ��ͨ���м��
	iRet = CITICs_HsHlp_Init(&HlpHandle, hConfig1);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, NULL, szMsg);
		printf("��ʼ�����Ӿ��ʧ�� ErrorCdoe=(%d)....\n", iRet);
		return HlpHandle;
	}

	/// ���ӷ�����
	iRet = CITICs_HsHlp_ConnectServer(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, NULL, szMsg);
		printf("����Serverʧ��: ErrorCdoe=(%d) %s....", iRet, szMsg);
		return HlpHandle;
	}
	else
	{
		printf("Connect HSAR OK......\n");
	}

	//�˻���¼.
	Login(HlpHandle);
	//֤ȯ�ֲֲ�ѯ
	GetShare(HlpHandle);
	//֤ȯ���ײ�ѯ
	GetTrade(HlpHandle);
	//NormalEntrust(HlpHandle,"600008","100", "1");
	//CancelEntrust(HlpHandle,"350");
	//EntrustBuyOrSell(HlpHandle, "600009", "100", "20", "1", "2","1");
	//EntrustBuyOrSell(HlpHandle, "600009", "100", "10", "1", "2", "2");
	//CancelEntrustQry(HlpHandle);
	//EntrustQry(HlpHandle);
	//BatchEntrustQry(HlpHandle);
	return HlpHandle;
}

int Login(HSHLPHANDLE HlpHandle)
{
	printf("\n\n---===�ͻ���¼Login===---");
	//��ע���������ʹ��ᶳ���˻���ʮ���Ӻ����Լ��ɡ�
	SetNecessaryParam(HlpHandle);
	CITICs_HsHlp_SetValue(HlpHandle, "input_content", "1");
	CITICs_HsHlp_SetValue(HlpHandle, "account_content", fund_account);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 331100, NULL);
	if (iRet)
	{
		ShowErrMsg(HlpHandle, 331100);
		_getch();
		exit(0);
	}

	//����½�ķ���ֵ�����������Ժ����еı��Ķ���Ҫ
	CITICs_HsHlp_GetValue(HlpHandle, "client_id", client_id);
	CITICs_HsHlp_GetValue(HlpHandle, "user_token", user_token);
	CITICs_HsHlp_GetValue(HlpHandle, "branch_no", branch_no);
	CITICs_HsHlp_GetValue(HlpHandle, "asset_prop", asset_prop);
	CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	ShowAnsData(HlpHandle);
}

void ShowErrMsg(HSHLPHANDLE HlpHandle, int iFunc)
{
	int nErr = 0;
	char szMsg[512] = { 0 };

	CITICs_HsHlp_GetErrorMsg(HlpHandle, &nErr, szMsg);
	printf("����ҵ��ʧ��[%d]: Error=(%d) %s\n", iFunc, nErr, szMsg);
}

//��ӡ����ֵ������������ĵ���һ�µ�������Բ��Խ��Ϊ׼��
void ShowAnsData(HSHLPHANDLE HlpHandle)
{
	int iRow, iCol;
	char szKey[64], szValue[512];
	iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	iCol = CITICs_HsHlp_GetColCount(HlpHandle);
	for (int i = 0; i < iRow; i++)
	{
		if (0 == CITICs_HsHlp_GetNextRow(HlpHandle))
		{
			printf("\n[%03d]:------------------------------------\n", i + 1);
			for (int j = 0; j < iCol; j++)
			{
				CITICs_HsHlp_GetColName(HlpHandle, j, szKey);
				CITICs_HsHlp_GetValueByIndex(HlpHandle, j, szValue);
				//CITICs_HsHlp_GetValue(HlpHandle, szKey, szValue);
				printf("%s:%s, ", szKey, szValue);
			}
		}
	}
	printf("\n");
}

//������������˳���½�����б��Ķ���Ҫ�͵Ĳ�����
void SetNecessaryParam(HSHLPHANDLE HlpHandle)
{
	CITICs_HsHlp_BeginParam(HlpHandle);
	CITICs_HsHlp_SetValue(HlpHandle, "client_id", client_id);
	CITICs_HsHlp_SetValue(HlpHandle, "fund_account", fund_account);
	CITICs_HsHlp_SetValue(HlpHandle, "sysnode_id", sysnode_id);
	CITICs_HsHlp_SetValue(HlpHandle, "identity_type", "2");
	CITICs_HsHlp_SetValue(HlpHandle, "op_branch_no", branch_no);
	CITICs_HsHlp_SetValue(HlpHandle, "branch_no", branch_no);
	CITICs_HsHlp_SetValue(HlpHandle, "op_station", op_station);
	CITICs_HsHlp_SetValue(HlpHandle, "op_entrust_way", op_entrust_way);
	CITICs_HsHlp_SetValue(HlpHandle, "password_type", "2");
	CITICs_HsHlp_SetValue(HlpHandle, "password", password);
	CITICs_HsHlp_SetValue(HlpHandle, "asset_prop", asset_prop);
	CITICs_HsHlp_SetValue(HlpHandle, "user_token", user_token);
	CITICs_HsHlp_SetValue(HlpHandle, "request_num", "500");//���ֵ���������ķ���������������ͣ���Ĭ��Ϊ50
}

//��ȡ����IP��
bool GetLocalIP(char* ip)
{
	//1.��ʼ��wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.��ȡ������
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	//3.��ȡ����ip
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		return false;
	}

	strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	return true;
}

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
}ASTAT, *PASTAT;

ASTAT Adapter;

//�ú���ֻ��ȡ��һ��������mac��ַ�����ж��������������Ӧ�޸ġ�
bool GetFirstMac(char * mac)
{
	NCB ncb;
	UCHAR uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = 0;

	// ���ȶ�ѡ������������һ��NCBRESET����Ա���г�ʼ��
	uRetCode = Netbios(&ncb);
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = 0; // ָ��������

	strcpy((char *)ncb.ncb_callname, "* ");
	ncb.ncb_buffer = (unsigned char *)&Adapter;

	// ָ�����ص���Ϣ��ŵı���
	ncb.ncb_length = sizeof(Adapter);

	// ���ţ����Է���NCBASTAT�����Ի�ȡ��������Ϣ
	uRetCode = Netbios(&ncb);
	//printf( "The NCBASTAT return code is: 0x%x \n", uRetCode );
	if (uRetCode == 0)
	{
		// ������MAC��ַ��ʽ���ɳ��õ�16������ʽ����0010-A4E4-5802
		sprintf(mac, "%02X%02X%02X%02X%02X%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]);
	}

	return uRetCode == 0;
}

/*�ֲ�����ѯ��ɽ���ˮ��ѯ*/
int GetShare(HSHLPHANDLE HlpHandle)
{
	struct tm *newtime;
	char tmpbuf[128];
	time_t lllt1;
	lllt1 = time(NULL);
	newtime = localtime(&lllt1);
	strftime(tmpbuf, 30, "%Y%m%d.PositionSystem.csv", newtime);
	char ntmpbuf[128];
	sprintf(ntmpbuf, "%s%s", OUT_TP_DIR, tmpbuf);
	FILE *fp = fopen(ntmpbuf, "w");
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	CITICs_HsHlp_SetValue(HlpHandle, "query_mode", "1");
	CITICs_HsHlp_SetValue(HlpHandle, "request_num", "1000");
	CITICs_HsHlp_SetValue(HlpHandle, "query_type", "1");
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333104, NULL);
	int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("request shares failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	char szCode[16], szName[32], szValue[32], szAmmount[16];
	char *tmp = "#tk,shares\n";
	fwrite(tmp, 1, strlen(tmp), fp);
	fflush(fp);
	fclose(fp);
	for (int i = 0; i < iRow; i++)
	{
		CITICs_HsHlp_GetNextRow(HlpHandle);
		CITICs_HsHlp_GetValue(HlpHandle, "stock_code", szCode);
		CITICs_HsHlp_GetValue(HlpHandle, "current_amount", szAmmount);

		put_file_str(ntmpbuf, "%s,%d\n", szCode, atoi(szAmmount));
	}
}

int GetTrade(HSHLPHANDLE HlpHandle)
{
	char reqnum[] = "500";
	struct tm *newtime;
	char tmpbuf[128];
	time_t lllt1;
	lllt1 = time(NULL);
	newtime = localtime(&lllt1);
	strftime(tmpbuf, 30, "%Y%m%d.TradingSystem.csv", newtime);
	char ntmpbuf[128];
	sprintf(ntmpbuf, "%s%s", OUT_TP_DIR, tmpbuf);
	FILE *fp = fopen(ntmpbuf, "w");
	char szMsg[512];

	int iRow = atoi(reqnum);
	char *cpage = "2";
	char szPosition_str[32];
	memset(szPosition_str, 0x00, sizeof(szPosition_str));
	int totalRowNum = 0;
	while (iRow == atoi(reqnum))
	{
		SetNecessaryParam(HlpHandle);
		CITICs_HsHlp_SetValue(HlpHandle, "query_mode", "1");
		//CITICs_HsHlp_SetValue(HlpHandle, "request_num", reqnum);
		CITICs_HsHlp_SetValue(HlpHandle, "query_type", "1");
		CITICs_HsHlp_SetValue(HlpHandle, "position_str", szPosition_str);
		//CITICs_HsHlp_SetValue(HlpHandle, "position_str", cpage);
		int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333102, NULL);
		if (iRet)
		{
			CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
			printf("request shares failed, error cdoe=(%d) %s...\n", iRet, szMsg);
			return iRet;
		}
		iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
		char szCode[16], business_amount[32], business_price[32], business_time[16], entrust_bs[16];
		int amount = 0;
		double price = 0.00;
		fprintf(fp, "#ticker,shr,tpx,ttime\n");
		fflush(fp);
		fclose(fp);
		for (int i = 0; i < iRow; i++)
		{
			CITICs_HsHlp_GetNextRow(HlpHandle);
			CITICs_HsHlp_GetValue(HlpHandle, "stock_code", szCode);
			CITICs_HsHlp_GetValue(HlpHandle, "business_amount", business_amount);
			CITICs_HsHlp_GetValue(HlpHandle, "business_price", business_price);
			CITICs_HsHlp_GetValue(HlpHandle, "business_time", business_time);
			CITICs_HsHlp_GetValue(HlpHandle, "entrust_bs", entrust_bs);
			price = atof(business_price);
			amount = atoi(business_amount);
			if (entrust_bs[0] == '2')
			{
				amount = -amount;
			}
			put_file_str(ntmpbuf, "%s,%d,%f,%s\n", szCode, amount, price, business_time);

		}

		CITICs_HsHlp_GetValue(HlpHandle, "position_str", szPosition_str);
		totalRowNum += iRow;
		printf("\nposition_str[%03d]:%s\n", totalRowNum, szPosition_str);
	}

}

/*��ͨί��*/
int NormalEntrust(HSHLPHANDLE HlpHandle, char *s_code, char *ammount, char *tpx)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_amount", ammount);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_price", tpx);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", "1");//1�� 2��
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_prop", "0");//ί�����ͣ�����
	CITICs_HsHlp_SetValue(HlpHandle, "batch_no", "0");//0��ʾ���ʶ���

	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333002, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("NormalEntrust failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}

/*ί�г���*/
int CancelEntrust(HSHLPHANDLE HlpHandle, char *eno)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_no", eno);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333017, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("CancelEntrust failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}

/*
* ���ܣ�����ί�����������
* ����˵����s_code ��Ʊ����
*			ammont ���ʽ�����
*			tpx ���׼۸�
*			entrust_bs 1Ϊ����2Ϊ����
*			market �г�������£�
"0"	ǰ̨δ֪������
"1"	�Ϻ�
"2"	����
"9"	��תA
"A"	��תB
"D"	�Ϻ�B
"H"	����B
"F1"	֣�ݽ�����(�ڻ�)
"F2"	����������(�ڻ�)
"F3"	�Ϻ�������(�ڻ�)
"F4"	���ڽ�����(�ڻ�)
*		en_count ί����
*/
int EntrustBuyOrSell(HSHLPHANDLE HlpHandle, char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);

	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", entrust_bs);//1�� 2��
	CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", market);//������𣬱�������ȷ�����г�����֧��0��ո�
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_style", "2");
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_amount", ammount);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_price", tpx);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_count", en_count);
	int iRet = 0;
	if (strcmp(entrust_bs, "1") == 0)
		CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333019, NULL);
	else
		CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333020, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("Request batch buy or sell failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}

/*�ɳ���ί�в�ѯ*/
int CancelEntrustQry(HSHLPHANDLE HlpHandle)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333100, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("request shares failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}

/*ί�в�ѯ*/
int EntrustQry(HSHLPHANDLE HlpHandle)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333101, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}

/*����ί�в�ѯ*/
int BatchEntrustQry(HSHLPHANDLE HlpHandle)
{
	char szMsg[512];
	SetNecessaryParam(HlpHandle);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333107, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("BatchEntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		return iRet;
	}
	ShowAnsData(HlpHandle);
}


void put_file_str(const char* fname, char* format, ...)
{
	char buff[1024 * 4] = { 0 };
	va_list arg_ptr;
	try
	{
		va_start(arg_ptr, format);
		vsprintf(buff, format, arg_ptr);
		va_end(arg_ptr);
	}
	catch (...) { return; }

	FILE* fo = fopen(fname, "a+b");
	if (fo)
	{
		fwrite(buff, strlen(buff), 1, fo);
		fflush(fo);
		fclose(fo);
	}
}