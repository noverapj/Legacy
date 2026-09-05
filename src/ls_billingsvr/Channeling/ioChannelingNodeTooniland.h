#ifndef __ioChannelingNodeTooniland_h__
#define __ioChannelingNodeTooniland_h__

#include "ioChannelingNodeParent.h"
#include "../NodeInfo/MemInfoManager.h" 
#define TOONILAND_TOKEN '|'
#define TOONILAND_EXTEND_TOKEN '-'

class ioChannelingNodeTooniland : public ioChannelingNodeParent
{
protected:
	enum 
	{
		MAX_GET_CASH_ARRAY = 3,
		GET_CASH_ARRAY_TYPE= 0,
		GET_CASH_ARRAY_CPID= 1,
		GET_CASH_ARRAY_RESULT = 2,

		MAX_GET_CASH_EXTEND_ARRAY    = 2,
		GET_CASH_EXTEND_ARRAY_RESULT = 0,
		GET_CASH_EXTEND_ARRAY_CASH   = 1,

		MAX_BUY_CASH_ARRAY = 3,
		BUY_CASH_ARRAY_TYPE = 0,
		BUY_CASH_ARRAY_CPID = 1,
		BUY_CASH_ARRAY_RESULT = 2,

		MAX_BUY_CASH_EXTEND_ARRAY       = 3,
		BUY_CASH_EXTEND_ARRAY_RESULT    = 0,
		BUY_CASH_EXTEND_ARRAY_BOUGHTID  = 1,
		BUY_CASH_EXTEND_ARRAY_CASH      = 2,
		BUY_CASH_EXTEND_ARRAY_ERROR_DESC= 2,

		MAX_SUBSCRIPTION_RETRACT_ARRAY		= 3,
		SUBSCRIPTION_RETRACT_ARRAY_RESULT	= 0,
		SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID = 1,
		SUBSCRIPTION_RETRACT_ARRAY_ERROR_DESC	= 2,
	};


protected:
	void AnsiToUTF8( IN const char *szAnsi, OUT char *szUTF8, OUT int &riReturnUTF8Size, IN int iUTF8BufferSize );

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
//test
public:
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);


public:
	virtual ChannelingType GetType();

public:
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	void OnRecieveGetCash( const ioHashString &rsPacket );
	void OnRecieveOutputCash( const ioHashString &rsPacket );
	void OnReceiveRetractCash( const ioHashString &rsPacket );

public:
	ioChannelingNodeTooniland(void);
	virtual ~ioChannelingNodeTooniland(void);
};

#endif // __ioChannelingNodeTooniland_h__