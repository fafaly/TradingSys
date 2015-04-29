#include "stdafx.h"
#include "CiticsCompany.h"
#include <string.h>
#include <io.h>
#include <mutex>

CiticsCompany::CiticsCompany(char *fund, char *pwd, char *o_dir) 
{
	strcpy(fund_account, fund);
	strcpy(password, pwd);
	strcpy(orderdir, o_dir);
	strcpy(op_entrust_way ,"5");
	strcpy(ClientName, "SHXT");
}


CiticsCompany::~CiticsCompany()
{
}

std::mutex apimtx;//锁住API的操作，以免传参错乱

int CiticsCompany::Connect()
{
	int iRet;
	char szMsg[512];

	HSHLPCFGHANDLE hConfig1;
	//	HSHLPCFGHANDLE hConfig2;

	if (!GetLocalIP(IP))
	{
		printf("获取本机IPC失败，请手动填入内网IP至对应字符串。\n");
	}
	if (!GetFirstMac(MAC))
	{
		//printf("获取本机MAC失败，请手动填入本机MAC至对应字符串。\n");
	}
	//op_station需要严格按照下列规则填写
	strcat(op_station, "tyjr-");//默认前缀，请不要修改
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
		printf("加载配置文件失败[Hsconfig.ini] ErrorCdoe=(%d)....\n", iRet);
		return -1;
	}

	/// 可以使用不同的配置句柄初始化连接句柄，这样可以连接到不同的通信中间件
	apimtx.lock();
	iRet = CITICs_HsHlp_Init(&HlpHandle, hConfig1);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, NULL, szMsg);
		printf("初始化连接句柄失败 ErrorCdoe=(%d)....\n", iRet);
		return -1;
	}

	/// 连接服务器
	iRet = CITICs_HsHlp_ConnectServer(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, NULL, szMsg);
		printf("连接Server失败: ErrorCdoe=(%d) %s....", iRet, szMsg);
		return -1;
	}
	else
	{
		printf("Connect HSAR OK......\n");
	}
	apimtx.unlock();

	return 0;
}

int CiticsCompany::Login()
{
	printf("\n\n---===客户登录Login===---");
	//备注：密码多次送错会冻结账户，十分钟后再试即可。
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "input_content", "1");
	CITICs_HsHlp_SetValue(HlpHandle, "account_content", fund_account);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 331100, NULL);
	if (iRet)
	{
		ShowErrMsg(331100);
		apimtx.unlock();
		_getch();
		exit(0);
	}

	//将登陆的返回值保存下来，以后所有的报文都需要
	CITICs_HsHlp_GetValue(HlpHandle, "client_id", client_id);
	CITICs_HsHlp_GetValue(HlpHandle, "user_token", user_token);
	CITICs_HsHlp_GetValue(HlpHandle, "branch_no", branch_no);
	CITICs_HsHlp_GetValue(HlpHandle, "asset_prop", asset_prop);
	CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);
	ShowAnsData();
	apimtx.unlock();
	printf("----->登录成功\n");
}

void CiticsCompany::ShowErrMsg(int iFunc)
{
	int nErr = 0;
	char szMsg[512] = { 0 };

	CITICs_HsHlp_GetErrorMsg(HlpHandle, &nErr, szMsg);
	printf("请求业务失败[%d]: Error=(%d) %s\n", iFunc, nErr, szMsg);
}

//打印返回值。如果遇到和文档不一致的情况，以测试结果为准。
void CiticsCompany::ShowAnsData()
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

//这个函数设置了除登陆外所有报文都需要送的参数。
void CiticsCompany::SetNecessaryParam()
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
	CITICs_HsHlp_SetValue(HlpHandle, "request_num", REQ_NUM);//这个值决定了最大的返回条数，如果不送，则默认为50
}

//获取本机IP，
bool CiticsCompany::GetLocalIP(char* ip)
{
	//1.初始化wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.获取主机名
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	//3.获取主机ip
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

//该函数只获取第一个网卡的mac地址，如有多个网卡，请做相应修改。
bool CiticsCompany::GetFirstMac(char * mac)
{
	NCB ncb;
	UCHAR uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = 0;

	// 首先对选定的网卡发送一个NCBRESET命令，以便进行初始化
	uRetCode = Netbios(&ncb);
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = 0; // 指定网卡号

	strcpy((char *)ncb.ncb_callname, "* ");
	ncb.ncb_buffer = (unsigned char *)&Adapter;

	// 指定返回的信息存放的变量
	ncb.ncb_length = sizeof(Adapter);

	// 接着，可以发送NCBASTAT命令以获取网卡的信息
	uRetCode = Netbios(&ncb);
	//printf( "The NCBASTAT return code is: 0x%x \n", uRetCode );
	if (uRetCode == 0)
	{
		// 把网卡MAC地址格式化成常用的16进制形式，如0010-A4E4-5802
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

/*持仓量查询或成交流水查询*/
int CiticsCompany::GetShare(map<string,int> &holdmap)
{
	char szMsg[512];
	int totalRowNum = 0;
	char szPosition_str[32];
	memset(szPosition_str, 0x00, sizeof(szPosition_str));
	apimtx.lock();
	int iRow = 500;
	while (iRow == 500)
	{
		SetNecessaryParam();
		CITICs_HsHlp_SetValue(HlpHandle, "query_mode", "1");
		//CITICs_HsHlp_SetValue(HlpHandle, "request_num", "1000");
		CITICs_HsHlp_SetValue(HlpHandle, "query_type", "1");
		CITICs_HsHlp_SetValue(HlpHandle, "position_str", szPosition_str);
		int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333104, NULL);
		iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
		if (iRet)
		{
			CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
			printf("request shares failed, error cdoe=(%d) %s...\n", iRet, szMsg);
			return iRet;
		}
		char szCode[16], szName[32], szValue[32], szAmmount[16];
		for (int i = 0; i < iRow; i++)
		{
			CITICs_HsHlp_GetNextRow(HlpHandle);
			CITICs_HsHlp_GetValue(HlpHandle, "stock_code", szCode);
			CITICs_HsHlp_GetValue(HlpHandle, "current_amount", szAmmount);
			holdmap[szCode] = atoi(szAmmount);
		}
		CITICs_HsHlp_GetValue(HlpHandle, "position_str", szPosition_str);
		totalRowNum += iRow;
		//printf("\nposition_str[%03d]:%s\n", totalRowNum, szPosition_str);
	}
	printf("----->Hold stocks size:%d", holdmap.size());
	apimtx.unlock();
}

int CiticsCompany::GetTrade(char *tk,char *realstatus)
{
	char szMsg[512];
	char *cpage = "2";
	char szPosition_str[32];
	memset(szPosition_str, 0x00, sizeof(szPosition_str));
	int totalRowNum = 0;
	apimtx.lock();
	int iRow = 500;
	while (iRow == 500)
	{
		SetNecessaryParam();
		CITICs_HsHlp_SetValue(HlpHandle, "query_mode", "1");
		//CITICs_HsHlp_SetValue(HlpHandle, "request_num", reqnum);
		CITICs_HsHlp_SetValue(HlpHandle, "query_type", "1");
		CITICs_HsHlp_SetValue(HlpHandle, "position_str", szPosition_str);
		//CITICs_HsHlp_SetValue(HlpHandle, "position_str", cpage);
		if (tk != NULL)
		{
			CITICs_HsHlp_SetValue(HlpHandle, "real_status", realstatus);
		}
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
		}
		CITICs_HsHlp_GetValue(HlpHandle, "position_str", szPosition_str);
		totalRowNum += iRow;
		printf("\nposition_str[%03d]:%s\n", totalRowNum, szPosition_str);
	}
	apimtx.unlock();
}

/*普通委托*/
int CiticsCompany::NormalEntrust(char *s_code, char *ammount, char *tpx, char *bs, OUT char *eno)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_amount", ammount);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_price", tpx);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", bs);//1买 2卖
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_prop", "0");//委托类型：买卖
	CITICs_HsHlp_SetValue(HlpHandle, "batch_no", "0");//0表示单笔订单

	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333002, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("NormalEntrust failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "entrust_no", eno);
	}
	//ShowAnsData();
	apimtx.unlock();
	return iRet;
}


/*市价委托*/
int CiticsCompany::MarketPriceEntrust(char *s_code, char *amount, char *bs, char *eno)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_amount", amount);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", bs);//1买 2卖
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_prop", "0");//委托类型：买卖
	CITICs_HsHlp_SetValue(HlpHandle, "batch_no", "0");//0表示单笔订单
	CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", "2");//深圳
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333003, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("MarketPriceEntrust failed, error cdoe=(%d) %s...\n", iRet, szMsg);

	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "entrust_no", eno);
	}
	//ShowAnsData();
	apimtx.unlock();
	return iRet;
}

/*委托撤销*/
int CiticsCompany::CancelEntrust(char *entrust_no)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	if (strcmp(entrust_no, "") != 0)
	{
		CITICs_HsHlp_SetValue(HlpHandle, "entrust_no", entrust_no);
	}
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333017, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("CancelEntrust failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	//ShowAnsData();
	apimtx.unlock();
	return iRet;
}

/*
* 功能：批量委托买入或卖出
* 参数说明：s_code 股票代码
*			ammont 单笔交易量
*			tpx 交易价格
*			entrust_bs 1为买入2为卖出
*			market 市场编号如下：
"0"	前台未知交易所
"1"	上海
"2"	深圳
"9"	特转A
"A"	特转B
"D"	上海B
"H"	深圳B
"F1"	郑州交易所(期货)
"F2"	大连交易所(期货)
"F3"	上海交易所(期货)
"F4"	金融交易所(期货)
*		en_count 委托数
*/
int CiticsCompany::EntrustBuyOrSell(char *s_code, char *ammount, char *tpx, char *market, char *en_count, char *entrust_bs)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();

	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", entrust_bs);//1买 2卖
	CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", market);//交易类别，必须输入确定的市场，不支持0或空格。
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
	//ShowAnsData();
	apimtx.unlock();
}

/*可撤单委托查询*/
int CiticsCompany::CancelEntrustQry()
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();

	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333100, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("request shares failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;
}

/*委托查询*/
int CiticsCompany::EntrustQry(char *s_code, char *amt, char *tpx, char *status)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333101, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "business_amount", amt);
		CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	//ShowAnsData();
	apimtx.unlock();
	return iRet;
}

/*行情查询*/
int CiticsCompany::PriceQry(char *s_code, char *price1, int bs)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", " ");
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 400, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		if (bs == 1)
			CITICs_HsHlp_GetValue(HlpHandle, "buy_price1", price1);
		else
			CITICs_HsHlp_GetValue(HlpHandle, "sell_price1", price1);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	//ShowAnsData();
	apimtx.unlock();
	return iRet;
}

//账户资金相关查询
int CiticsCompany::FundAry(char *mycash)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 332255, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "enable_balance", mycash);
		//ShowAnsData();
	}
	apimtx.unlock();
	return iRet;
}

/*批量委托查询*/
int CiticsCompany::BatchEntrustQry()
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333107, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("BatchEntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;
}

/*委托费用预算*/
float CiticsCompany::EntrustFare(char *tk, int amt, float tpx, char* bs)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	char stramt[20];
	char strtpx[10];
	memset(stramt, 0, sizeof(stramt));
	memset(strtpx, 0, sizeof(strtpx));
	sprintf(stramt, "%d", amt);
	sprintf(strtpx, "%f", tpx);
	char ffare[10];
	char bfare[10];
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_bs", bs);//1买 2卖
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_prop", "0");//交易类别，必须输入确定的市场，不支持0或空格。
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_style", "2");
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", tk);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_amount", stramt);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_price", strtpx);
	//CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", "0");
	int iRet = 0;
	iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333023, NULL);
	int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("Request batch buy or sell failed, error cdoe=(%d) %s...\n", iRet, szMsg);
		apimtx.unlock();
		return iRet;
	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "ffare_balance", ffare);
		CITICs_HsHlp_GetValue(HlpHandle, "bfare_balance", bfare);
	}
	ShowAnsData();
	apimtx.unlock();
	return atof(ffare) + atof(bfare);
}

int CiticsCompany::StockQry(char *s_code)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 330300, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "business_amount", amt);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;
}

int CiticsCompany::EntrustBuyAmount(char *s_code, char *tpx, OUT char* amount)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "stock_code", s_code);
	CITICs_HsHlp_SetValue(HlpHandle, "exchange_type", "0");
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_price", tpx);
	CITICs_HsHlp_SetValue(HlpHandle, "entrust_prop", "0");
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333001, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;
}

int CiticsCompany::HistDeliverQry(char *sdate, char *edate)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "start_date", sdate);
	CITICs_HsHlp_SetValue(HlpHandle, "end_date", sdate);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 339300, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;

}

int CiticsCompany::HistBussinessQry(char *sdate, char *edate)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "start_date", sdate);
	CITICs_HsHlp_SetValue(HlpHandle, "end_date", sdate);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 339304, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;

}


int CiticsCompany::HistFundStockQry(char *sdate, char *edate)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "start_date", sdate);
	CITICs_HsHlp_SetValue(HlpHandle, "end_date", sdate);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 339305, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;

}

int CiticsCompany::HistTradeTotQry(char *sdate, char *edate)
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	CITICs_HsHlp_SetValue(HlpHandle, "start_date", sdate);
	CITICs_HsHlp_SetValue(HlpHandle, "end_date", sdate);
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 339307, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;

}

int CiticsCompany::StockFastQry()
{
	char szMsg[512];
	apimtx.lock();
	SetNecessaryParam();
	int iRet = CITICs_HsHlp_BizCallAndCommit(HlpHandle, 333103, NULL);
	//int iRow = CITICs_HsHlp_GetRowCount(HlpHandle);
	if (iRet)
	{
		CITICs_HsHlp_GetErrorMsg(HlpHandle, &iRet, szMsg);
		printf("EntrustQry failed, error cdoe=(%d) %s...\n", iRet, szMsg);
	}
	else
	{
		//CITICs_HsHlp_GetValue(HlpHandle, "enable_buy_amount", amount);
		//CITICs_HsHlp_GetValue(HlpHandle, "business_price", tpx);
		//CITICs_HsHlp_GetValue(HlpHandle, "entrust_status", status);
		//CITICs_HsHlp_GetValue(HlpHandle, "sysnode_id", sysnode_id);

	}
	ShowAnsData();
	apimtx.unlock();
	return iRet;

}

void CiticsCompany::put_file_str(const char* fname, char* format, ...)
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

