#pragma once
#include "ioChannelingNodeParent.h"

#define DAUM_SUCCESS 0

class ioChannelingNodeDaum_v2 : public ioChannelingNodeParent
{
protected:
	ioHashString m_sGetURL;
	ioHashString m_sOutputURL;
	ioHashString m_sApikey;
	ioHashString m_sSubscriptionRetractURL;
	ioHashString m_sFillURL;
	ioHashString m_sSendFillURL;
	ioHashString m_sShutDownCheckURL;

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);

public:
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	virtual void ThreadGetCash( const ioData &rData );
	BOOL DaumGetCash( const ioData &rData, int& iPurchasedCash, int& iReturnCash, char* szErrString, int iErrSize );

	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadSubscriptionRetract( const ioData& rData);
	virtual void ThreadFillCashUrl( const ioData &rData );

protected:
	BOOL ParseGetCashReturnData( char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, char* szErrString, int iErrSize);
	BOOL ParseOutPutCashReturnData( char* szReturnData, const int iSize, int& iChargedAmt, char* szErrString, int iErrSize, ioHashString& szCharNo);
	BOOL ParseSubsRetract( char* szReturnData, const int iSize, int& iCancelAmt, char* szErrString, int iErrSize);
	BOOL ParseFillCashURL(char* szReturnData, const int iSize, int& iRsvSeq, char* szErrString, int iErrSize);

public:
	BOOL ParseShutDownCheck(char* szReturnData, int& iResult);

protected:
	void SendExecptMessage(DWORD dwPacketID, int iErrCode, const ioData& rData, ioHashString& szErrorString);

public:
	virtual ChannelingType GetType();

protected:
	void UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize );

public:
	BOOL CheckShutDownCheck(ioHashString& szDaumUserID);

protected:
//-------------------------------------------------------------------------DAUM
void decode(unsigned char* s, unsigned char* ret);
char *pt(unsigned char *md, char* buf);
char *getSig(char *apiKey, char* data, char* ts, char* nonce);
void getNewNonce(char *nonce); 
void getNewTimeStamp(char *ts); 
//-------------------------------------------------------------------------

public:
	ioChannelingNodeDaum_v2(void);
	virtual ~ioChannelingNodeDaum_v2(void);
};

