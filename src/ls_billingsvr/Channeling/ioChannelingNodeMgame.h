#ifndef __ioChannelingNodeMgame_h__
#define __ioChannelingNodeMgame_h__

#include "ioChannelingNodeParent.h"
#include "../NodeInfo/MemInfoManager.h" 
#include "../NodeInfo/BillInfoManager.h"

class ioChannelingNodeMgame : public ioChannelingNodeParent
{

protected:
	void OnRecieveGetCash( const MgameCashResult& rkResult, BillInfoManager::BillingInfo *pInfo );
	void OnRecieveOutputCash( const MgameCashResult& rkResult, BillInfoManager::BillingInfo *pInfo );

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );

public:
	virtual ChannelingType GetType();

public:
	void OnRecieveCashAction( const MgameCashResult& rkResult );

public:
	ioChannelingNodeMgame(void);
	virtual ~ioChannelingNodeMgame(void);
};

#endif // __ioChannelingNodeMgame_h__