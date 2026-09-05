#pragma once

#include "ioChannelingNodeParent.h"

class ioChannelingNodeHangame : public ioChannelingNodeParent
{
public:
	ioChannelingNodeHangame();
	virtual ~ioChannelingNodeHangame();

	void Init();
	void Destroy();

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );

public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );

public:
	virtual ChannelingType GetType() { return CNT_HANGAME; }

public:
	void TestGetURL();

protected:
	ioHashString m_szGetURL;
	ioHashString m_szBuyURL;
};