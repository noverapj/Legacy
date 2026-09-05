#ifndef __ioLocalSA_h__
#define __ioLocalSA_h__

#include "ioLocalParent.h"
#include "../NodeInfo/TestCashManager.h"
#include "../LoginManager/loginmanager.h"


#define SA_GAMETYPE 'O'
#define SA_TOKEN ';'
#define MAX_LOGIN_ARRAY 14
#define MAX_GETCASH_ARRAY 2
#define MAX_OUTPUTCASH_ARRAY 2

#if(_TEST)
	#define SA_LOCAL 1
	#define SA_SERVICEVALUE 1	//Billing Test IP 에 테스트 할 경우
#else if
	#define SA_LOCAL 2
	#define SA_SERVICEVALUE 2
#endif

class ioLocalSA  : public ioLocalParent
{
protected:
	ioHashString m_sLoginURL;	
	ioHashString m_sBillingGetURL;
	ioHashString m_sBillingOutPutURL;
	
protected:
	TestCashManager m_TestCashManager;

public:
	virtual ioLocalManager::LocalType GetType();

	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );

	virtual void Init();

public:
	virtual void ThreadLogin( const ioData &rData, LoginManager &rLoginMgr );
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );

public:
	ioLocalSA(void);
	virtual ~ioLocalSA(void);
};

#endif // __ioLocalSA_h__