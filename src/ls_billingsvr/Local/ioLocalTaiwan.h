#ifndef __ioLocalTaiwan_h__
#define __ioLocalTaiwan_h__

#include "ioLocalParent.h"
#include "../LoginManager/loginmanager.h"
#include "../NodeInfo/TestCashManager.h"

#define TAIWAN_CODE "00015"
#define TAIWAN_TOKEN '|'
#define TAIWAN_EXTEND_TOKEN '='

class ioLocalTaiwan  : public ioLocalParent
{
protected:
	enum
	{
		MAX_LOGIN_ARRAY    = 2,
		LOGIN_ARRAY_KEY    = 0,
		LOGIN_ARRAY_USERNO = 1,
	};
protected:
	TestCashManager m_TestCashManager;

	ioHashString  m_sLoginURL;
	ioHashString  m_sBillingGetURL;
	ioHashString  m_sBillingOutPutURL;
	ioHashString  m_sBillingCancelURL;
	ioHashString  m_sKey;

protected:
	void GetReturnParsing( OUT int &riReturnCode, OUT int &riTotalCash, OUT int &riPurchasedCash, OUT char *szVerify, IN int iVerifySize, IN const char *szReturn );

public:
	virtual ioLocalManager::LocalType GetType();

	virtual void Init();

	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	virtual void ThreadLogin( const ioData &rData, LoginManager &rLoginMgr );
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadCancelCash( const ioData &rData );

public:
	ioLocalTaiwan(void);
	virtual ~ioLocalTaiwan(void);
};

#endif // __ioLocalTaiwan_h__