#ifndef __ioLocalIndonesia_h__
#define __ioLocalIndonesia_h__

#include "ioLocalParent.h"
#include "../NodeInfo/TestCashManager.h"
#include "../LoginManager/loginmanager.h"

#define SHOP_KEY  "LS110121IOENTC"
#define SHOP_ID   "LSAGA"


class ioLocalIndonesia  : public ioLocalParent
{
protected:
	ioHashString m_sLoginURL;
	ioHashString m_sBillingGetURL;
	ioHashString m_sBillingOutPutURL;
	ioHashString m_sPCRoomURL;

protected:
	TestCashManager m_TestCashManager;

protected:
	bool ThreadPCRoom( IN const ioData &rData, IN const ioHashString &szReturn, OUT DWORD &rdwPCRoom );

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
	ioLocalIndonesia(void);
	virtual ~ioLocalIndonesia(void);
};

#endif // __ioLocalIndonesia_h__