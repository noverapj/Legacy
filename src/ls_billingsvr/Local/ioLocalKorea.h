#ifndef __ioLocalKorea_h__
#define __ioLocalKorea_h__

#include "ioLocalParent.h"

class ioLocalKorea : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();

	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnRefundCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnUserInfo( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnAddCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual bool IsLoadGoodsNameList() { return false; }

public:
	ioLocalKorea(void);
	virtual ~ioLocalKorea(void);
};

#endif // __ioLocalKorea_h__