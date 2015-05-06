#pragma once
#include <map>
#include <string>
using namespace std;

struct DesTradeTable
{
	char tk[10];
	char tkname[10];
	int desshr;
	char direction[2];
	char stime[10];
	char etime[10];
	int brokeshr;
	int actshr;
	char tradestauts[10];
	float despx;
	float tradepx;
	float proportion;
};

struct ActHoldingTable
{
	char tk[10];
	char tkname[10];
	int shr;
	float px;
	float rtn;//return
	float pnl;
};

struct TradeTable
{
	char tk[10];
	char tkname[10];
	int shr;
	float px;
	char dealtime[20];
};

struct EntrustTable
{
	char tk[10];
	char tkname[10];
	int shr;
	float px;
	char entrusttime[20];
	char type[10];
};

class DisplayTable
{
private:
	DisplayTable();//不允许常态的创造实例.
	static DisplayTable* single;
public:
	~DisplayTable();
	//单例
	static DisplayTable* GetInstance()
	{
		if (single == NULL)
		{
			single = new DisplayTable();
		}
		return single;
	}
	map<string, DesTradeTable*> m_des_map;
	map<string, ActHoldingTable*> m_actholding_map;
	map<string, TradeTable*> m_trade_map;
	map<string, EntrustTable*> m_entrust_map;
};

DisplayTable* DisplayTable::single = NULL;