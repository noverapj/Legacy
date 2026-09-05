#ifndef __ioChannelingNodeTooniland_h__
#define __ioChannelingNodeTooniland_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeTooniland : public ioChannelingNodeParent
{
protected:
	virtual bool AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType );
	virtual bool OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType );
	virtual void SendCancelCash( IN SP2Packet &rkPacket );

	virtual bool AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex = 0);
	virtual bool CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo );
	virtual bool CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual bool AddReuestSubscriptionRetractCashPacket( IN User *pUser, OUT SP2Packet & rkSendPacket );
	virtual bool OnReceiveSubscriptionRetractCash( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType );

public:
	virtual bool IsSubscriptionRetractCheck() { return false; } //kyg Tooniland는 지원 하지 않음 
	virtual bool IsSubscrptionRetract() { return true; } //kyg 청약철회는 지원 

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeTooniland(void);
	virtual ~ioChannelingNodeTooniland(void);
};

#endif // __ioChannelingNodeTooniland_h__