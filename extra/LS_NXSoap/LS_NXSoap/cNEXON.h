#pragma once

#include <string>
#include "Nexon.h"

class NexonCashSoapProxy;

class cNEXON
{
public:
	cNEXON(void);
	~cNEXON(void);

	void Init();
	void Destroy();

public:
	BOOL InitializeSoap(
		const char* TS, 
		const char* IP, 
		const char* KEY,
		int& error);		// SOAP 초기화, 반드시 최초 1회 호출되어야 함

	BOOL InitializeSoapSEH(
		const char* TS, 
		const char* IP, 
		const char* KEY,
		int& error);		// SOAP 초기화, 반드시 최초 1회 호출되어야 함

	BOOL IsBillingUser(
		const char* ID,
		BYTE reason,
		const char* IP,
		int& error,
		time_t& lastDate);

	BOOL GetPurse(
		const char* ID, 
		const BYTE reason, 
		const char* IP, 
		int& error, 
		INT64& balance);		// 잔액조회(총잔액)

	BOOL Purchase(
		const char* ID, 
		const char* transactionID, 
		short productType, 
		const char* productCode, 
		const char* amount,
		const char* gameID, 
		const char* serverID, 
		const char* orderNO, 
		BYTE reason, 
		const char* IP, 
		int& error,
		BOOL& result);			// 아이템 구매

	BOOL Present(
		const char* ID, 
		const char* transactionID, 
		short productType, 
		const char* productCode, 
		const char* amount,
		const char *gameID, 
		const char *friendID, 
		BYTE age,
		const char* serverID, 
		const char* orderNO, 
		BYTE reason, 
		const char* IP, 
		int& error,
		BOOL& result);			// 선물하기

	BOOL UsageCancel(
		const char* ID, 
		const char* transactionID, 
		short productType, 
		int amount,
		int& result);			// 청약철회

	BOOL UsageCancelByUsageSN(
		const char* ID,
		int usageSN,
		int refundAmount,
		int& result);			// 청약철회 by UsageSN

	void GetError(char* buffer, int length);

protected:
	BOOL CreateHeader();
	void DestroyHeader();
	void GenerateHeader(const char* TS, const char* IP, const char* hashKey);
	
private:
	std::string m_TS;
	std::string m_IP;
	std::string m_Hash;

	NX_IsBillingUser m_isbillinguser;
	NX_GetPurse m_getpurse;
	NX_Purchase m_purchase;
	NX_Present m_present;
	NX_UsageCancel m_usagecancel;
	NX_UsageCancelByUsageSN m_usagecancelbyusagesn;
};

