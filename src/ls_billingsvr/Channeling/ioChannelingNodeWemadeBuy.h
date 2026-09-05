#ifndef __ioChannelingNodeWemadeBuy_h__
#define __ioChannelingNodeWemadeBuy_h__

#include "ioChannelingNodeParent.h"
#include "../NodeInfo/MemInfoManager.h"
#include "../../include/MemPooler.h" //kyg ??? 

 

class ioChannelingNodeWemadeBuy : public ioChannelingNodeParent
{
protected:
	enum
	{
		RESULT_CODE_SUCCESS = 200,
	};

protected:
	ioHashString m_sURL;
	long m_iReqKey;
//	MemPooler<BillInfoMgr::BillingInfo> m_BillInfoPool; //SubscriptionRetract 부분만 시험적 사용 

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
public:
	virtual void _OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket);
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);
	virtual void _OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPacket);
	
public:
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket );

	virtual void ThreadFillCashUrl( const ioData &rData );
	
public:
	void OnRecieveGetCash( const SC_BALANCE& rkResult );
	void OnRecieveOutputCash( const SC_PURCHASEITEM& rkResult );
	void OnRecievePresentCash( const SC_GIFTITEM& rkResult );
	void OnReceiveRetractCash( SC_RETRACT& rkResult );
	void OnReceiveRetractCheckCash( SC_RETRACT_PAYBACK& rkResult );
	void OnReceiveRetractCancelCash( const SC_RETRACT_CANCEL& rkResult );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeWemadeBuy(void);
	virtual ~ioChannelingNodeWemadeBuy(void);
};

#endif // __ioChannelingNodeWemadeBuy_h__
