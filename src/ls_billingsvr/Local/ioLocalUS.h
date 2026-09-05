#ifndef __ioLocalUS_h__
#define __ioLocalUS_h__

#include "ioLocalParent.h"
#include "../NodeInfo/TestCashManager.h"
#include "../NodeInfo/MemInfoManager.h" 

#define USER_TYPE_NORMAL "WMU"
#define USER_TYPE_FB     "FB"

class ioLocalUS : public ioLocalParent
{
protected:
	enum 
	{
		RESULT_SUCCESS   = 0,
	};

protected:
	TestCashManager m_TestCashManager;
	ioHashString    m_sLoginURL;
//	BillInfoMgr     m_BillInfoMgr;

public:
	virtual ioLocalManager::LocalType GetType();
	virtual void Init();

public:
	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	
public:
	virtual void ThreadLogin( const ioData &rData, LoginManager &rLoginMgr );
	virtual void OnRecieveGetCash( const BILL_PACK_GETBALANCE &rkResult );
	virtual void OnRecieveOutputCash( const BILL_PACK_PURCHASEITEM &rkResult );

public:
	ioLocalUS(void);
	virtual ~ioLocalUS(void);
};

#endif // __ioLocalUS_h__