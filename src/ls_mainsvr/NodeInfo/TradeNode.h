#ifndef _TradeNode_h_
#define _TradeNode_h_
#include "../BoostPooler.h"

class SP2Packet;

class TradeNode : public BoostPooler<TradeNode>
{
protected:
	// 기본 정보
	DWORD m_dwTradeIndex;
	DWORD m_dwRegisterUserIndex;
	ioHashString m_szRegisterUserNick;
	
	DWORD m_dwItemType;
	DWORD m_dwItemMagicCode;
	DWORD m_dwItemValue;
	DWORD m_dwItemMaleCustom;
	DWORD m_dwItemFemaleCustom;
	__int64 m_iItemPrice;

	DWORD m_dwRegisterDate1;
	DWORD m_dwRegisterDate2;
	DWORD m_dwRegisterPeriod;

	ioHashString m_szRegisterIP;

protected:
	int m_iPriorityOrder;

protected:
	void InitData();

public:
	void CreateTradeItem( SP2Packet &rkPacket );					// 새로 등록될때
	void CreateTradeItem( CQueryResultData *pQueryData );		// DB에 저장된 것 로드

public:
	inline const DWORD GetTradeIndex() const { return m_dwTradeIndex; }
	inline DWORD GetRegisterUserIndex() { return m_dwRegisterUserIndex; }
	inline const ioHashString& GetRegisterUserNick() const { return m_szRegisterUserNick; }

	inline DWORD GetItemType() { return m_dwItemType; }
	inline DWORD GetItemMagicCode() const { return m_dwItemMagicCode; }
	inline DWORD GetItemValue() { return m_dwItemValue; }
	inline DWORD GetItemMaleCustom() { return m_dwItemMaleCustom; }
	inline DWORD GetItemFemaleCustom() { return m_dwItemFemaleCustom; }
	inline __int64 GetItemPrice() { return m_iItemPrice; }

	inline DWORD GetRegisterDate1() const { return m_dwRegisterDate1; }
	inline DWORD GetRegisterDate2() const { return m_dwRegisterDate2; }
	inline DWORD GetRegisterPeriod() { return m_dwRegisterPeriod; }

	inline const ioHashString& GetRegisterIP() const { return m_szRegisterIP; }

public:
	inline int GetPriorityOrder() const { return m_iPriorityOrder; }

public:
	void SetRegisterDate( int iYear , int iMonth, int iDay, int iHour, int iMinute, int iSec );
	SHORT GetYear();
	SHORT GetMonth();
	SHORT GetDay();
	SHORT GetHour();
	SHORT GetMinute();
	SHORT GetSec();

public:
	TradeNode();
	virtual ~TradeNode();
};

#endif