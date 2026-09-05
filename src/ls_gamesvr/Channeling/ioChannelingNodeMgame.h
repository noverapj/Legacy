#ifndef __ioChannelingNodeMgame_h__
#define __ioChannelingNodeMgame_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeMgame : public ioChannelingNodeParent
{
protected:
	virtual bool AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType );
	virtual bool OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual bool AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex = 0);
	virtual bool CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo);
	virtual bool CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType );
	virtual bool UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType );
	
public:
	virtual bool IsSubscriptionRetractCheck() { return false; } //kyg mgame은 지원 하지 않음 
	virtual bool IsSubscrptionRetract() { return false; } //kyg 청약철회 미지원 
public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeMgame(void);
	virtual ~ioChannelingNodeMgame(void);
};

#endif // __ioChannelingNodeMgame_h__