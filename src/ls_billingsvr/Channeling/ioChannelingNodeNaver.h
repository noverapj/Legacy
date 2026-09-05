#ifndef __ioChannelingNodeNaver_h__
#define __ioChannelingNodeNaver_h__

#include "ioChannelingNodeParent.h"

#define NAVER_CP_ID        "P_LOSA"


#define NAVER_CHANNEL_INFO "PN"
#define NAVER_COIN_TYPE    "25"

class ioChannelingNodeNaver : public ioChannelingNodeParent
{
protected:
	ioHashString m_sURL;

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );



public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeNaver(void);
	virtual ~ioChannelingNodeNaver(void);
};

#endif // __ioChannelingNodeNaver_h__
