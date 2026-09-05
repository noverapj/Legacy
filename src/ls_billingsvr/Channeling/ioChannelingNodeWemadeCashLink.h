#ifndef __ioChannelingNodeWemadeCashLink_h__
#define __ioChannelingNodeWemadeCashLink_h__

#include "ioChannelingNodeParent.h"

#define WEMADE_SUCCESS_STRING "success"
#define WEMADE_FAIL_STRINg	"fail"

class ioChannelingNodeWemadeCashLink : public ioChannelingNodeParent
{
protected:
	ioHashString m_sGetURL;
	ioHashString m_sOutputURL;
	ioHashString m_sSubscriptionRetractURL;
	ioHashString m_sSubscriptionRetractCheckURL;
	ioHashString m_sPresentURL;
	ioHashString m_sURL;
	ioHashString m_sPresentCashURL;
	DWORD		 m_dwMakeCode;

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);
	virtual void _OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket);

public:
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket );
	//기간 캐쉬 지급 상자.
	void _OnPresentCash(ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadSubscriptionRetract( const ioData& rData);
	virtual void ThreadSubscriptionRetractCheck (const ioData &rData );
	virtual void ThreadFillCashUrl( const ioData &rData );

	void ThreadPresentCash(const ioData &rData);
	void TestPresentCash();

public:
	virtual ChannelingType GetType();

protected:
	void UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize );

protected:
	void GetCashURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const DWORD dwUserIndex, const ioHashString& szPrivateID );
	BOOL ParseGetCashReturnData( char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, const ioData& rData, char* errString, int iErrSize);
	
	void GetOutputCashURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData );
	BOOL ParseOutPutCashReturnData( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iChargedAmt, ioData& rData, char* errString, int iErrSize);

	void GetPresentURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData );
	BOOL ParsePresentReturnData( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iChargedAmt, ioData& rData, char* errString, int iErrSize);

	void GetSubsCheck( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData );
	BOOL ParseSubsCheckReturnData( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iCancelAmt, int& iRealCancelCash, int& iBonusCancelCash, ioData& rData, char* errString, int iErrSize);

	void GetSubsRetract( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData );
	BOOL ParseSubsRetract( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iCancelAmt, int& iRealCancelCash, int& iBonusCancelCash, ioData& rData, char* errString, int iErrSize);

	BOOL ParsePresentCashReturnData(char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, const ioData& rData, char* errString, int iErrSize);

protected:
	void SendExecptMessage(DWORD dwPacketID, int iErrCode, const ioData& rData);

protected:
	DWORD GetMakeCode() const	{ return m_dwMakeCode; }
	void SetMakeCode(DWORD val) { m_dwMakeCode = val; }

public:
	ioChannelingNodeWemadeCashLink(void);
	virtual ~ioChannelingNodeWemadeCashLink(void);
};

#endif // __ioChannelingNodeWemadeCashLink_h__