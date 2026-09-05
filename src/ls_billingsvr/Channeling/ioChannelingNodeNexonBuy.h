#pragma once

#include "ioChannelingNodeParent.h"

class cNEXON; 

#define NEXON_GAME_CODE 112
#define NEXON_PATH      512

class ioChannelingNodeNexonBuy : public ioChannelingNodeParent
{
protected:
	ioHashString m_sSoapIP;
	ioHashString m_sHashCode;
	ioHashString m_sNexonPlayRock;

	char		 m_szNexonClientSecret[512];	
	int			 m_iNexonReqKey;


public:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);

public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadSubscriptionRetract( const ioData& rData);

public:
	BOOL NexonInitializeSoap(
		cNEXON& nxSoap,
		const char* TS, 
		const char* IP, 
		const char* KEY);

	BOOL NexonGetPurse(
		cNEXON& nxSoap,
		const char* ID, 
		const BYTE reason, 
		const char* IP, 
		int& error, 
		INT64& balance);		// 잔액조회(총잔액)

	BOOL NexonPurchase(
		cNEXON& nxSoap,
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

	BOOL NexonPresent(
		cNEXON& nxSoap,
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

	BOOL NexonUsageCancelByUsageSN(
		cNEXON& nxSoap,
		const char* ID,
		int usageSN,
		int refundAmount,
		int& result);	

public:
	virtual ChannelingType GetType();

public:
	//	cNEXON* GetNexonSoap() const { return m_nexonSoap; }
	//	void SetNexonSoap(cNEXON* val) { m_nexonSoap = val; }

private:
	int NexonGetCash( cNEXON& nxSoap, ioHashString& szNexonNo );

protected:
	void SetTS(char* TS);

protected:
	//	cNEXON* m_nexonSoap;
	ioHashString m_szHashCode;

protected:


public:
	ioChannelingNodeNexonBuy(void);
	virtual ~ioChannelingNodeNexonBuy(void);
};

