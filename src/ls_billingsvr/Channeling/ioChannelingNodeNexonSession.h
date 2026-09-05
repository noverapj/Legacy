#pragma once

#include "ioChannelingNodeParent.h"
#include "../NodeInfo/MemInfoManager.h"

class ioChannelingNodeNexonSession : public ioChannelingNodeParent
{
protected:
	BillInfoMgr m_BillInfoMgr;

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName ){}
	virtual void _OnPCRoom( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	void OnRecievePCRoom( const BOQPTS_CHECKPREMIUM2& rkResult );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeNexonSession(void);
	virtual ~ioChannelingNodeNexonSession(void);
};

