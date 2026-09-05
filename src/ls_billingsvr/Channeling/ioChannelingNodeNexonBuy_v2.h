#pragma once

#include "ioChannelingNodeParent.h"

class ioRestAPI;

#define NEXON_GAME_CODE 112
#define NEXON_PATH      512

class ioChannelingNodeNexonBuy_v2 : public ioChannelingNodeParent
{
protected:
	ioHashString m_sGetURL;
	ioHashString m_sOutputURL;
	ioHashString m_sSubscriptionRetractURL;
	ioHashString m_sClientSecret;

public:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);
	
public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadSubscriptionRetract( const ioData& rData);

protected:
	BOOL NexonGetCash(const ioHashString& szUserID, int& iReturnCash, int& iPurchase, int& iResult, ioHashString& szErrString);
	BOOL NexGetCashParse(const char* szReturnData, int& iRealCash, int& iBonus, int& iResult, ioHashString& szErrString);

	BOOL NexonOutputCash(const ioHashString& szUserID, ioHashString& szBuyNo, ioData& rData, ioHashString& szErrString, int& iResult);
	BOOL NexonOutputCashParse(const char* szReturnData, ioHashString& szBuyNo, ioData& rData, ioHashString& szErrString);
	BOOL NexonOutputJsonParam(const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, ioHashString& szJsonData);


	BOOL NexonSubscription(const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, int& iResult);
	BOOL NexonSubscriptionParse(const char* szReturnData, ioData& rData, ioHashString& szErrString);
	BOOL NexonSubsJsonParam(const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, ioHashString& szJsonData, ioHashString& szCharNo);

	void MakeTransactionID( char* szTransactionID, ioData &rData );

protected:
	void SendExecptMessage(DWORD dwPacketID, int iErrCode, const ioData& rData, ioHashString& szErrString);

protected:
	void UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize );

public:
	virtual ChannelingType GetType();

protected:
 

public:
	ioChannelingNodeNexonBuy_v2(void);
	virtual ~ioChannelingNodeNexonBuy_v2(void);
};

 