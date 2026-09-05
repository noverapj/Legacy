#ifndef __ioLocalParent_h__
#define __ioLocalParent_h__

#include "../LoginManager/LoginManager.h"
#include "ioLocalManager.h"
#include "../NexonEUDefine.h"

class SP2Packet;
class ServerNode;
class ThailandUser;
class ioData;

class ioLocalParent
{
public:
	enum
	{
		MAX_AGENCY_NO_CNT      = 99999,
		MAX_AGENCY_NO_PLUS_ONE = 21,
	};

protected:
	int          m_iAgencyNoCnt;

public:
	virtual ioLocalManager::LocalType GetType() = 0;
	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void OnLogoutData( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket ) = 0;
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName ) = 0;
	virtual void OnRefundCash( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void OnUserInfo( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void OnOTP( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnIPBonus( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnIPBonusOut( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnAddCash( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void Init() {}
	virtual bool IsLoadGoodsNameList() { return true; }
	virtual void OnRecieveLoginData( const ioHashString &rsResult ){}
	virtual void OnRecieveLoginData( const EU_SESSION4_REPLY &rsResult ) {}
	virtual void OnRecieveOTP( const ioHashString &rsResult, const ThailandUser &rUser ){}
	virtual void OnRecieveIPBonus( const ioHashString &rsResult ){}
	virtual void OnRecieveIPBonusOut( const ioHashString &rsResult ){}
	virtual void OnRecieveGetCash( const ioHashString &rsResult ) {}
	virtual void OnRecieveGetCash( const BILL_PACK_GETBALANCE &rkResult ) {}
	virtual void OnReceiveGetCash( const EU_GETCASH_RESPONSE& rkResult ) {}

	virtual void OnRecieveOutputCash( const ioHashString &rsResult ) {}
	virtual void OnRecieveOutputCash( const BILL_PACK_PURCHASEITEM &rkResult ) {}
	virtual void OnRecieveOutputCash( const EU_PURCAHSEITEM_RESPONSE &rkResult ) {}
	//virtual void OnRecieveOutputCashAmount( const  EU_AMOUNT_RESPONSE &rkResult ) {}
	virtual void OnRecieveOutputCashAmount( const EU_GETCASH_AMOUNT_RESPONSE &rkResult ) {}
	
	virtual void OnGetMileage( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnAddMileage( ServerNode *pServerNode, SP2Packet &rkPacket ) {}

	virtual int GetNodeSize(){ return 0; }
	virtual int RemainderNode(){ return 0; }
	virtual int GetSecondNodeSize(){ return 0; }
	virtual int RemainderSecondNode(){ return 0; }
	virtual int GetThirdNodeSize(){ return 0; }
	virtual int RemainderThirdNode(){ return 0; }

	virtual void ThreadLogin( const ioData &rData, LoginManager &rLoginMgr ) {}
	virtual void ThreadLogout( const ioData &rData, LoginManager &rLoginMgr ) {}
	virtual void ThreadGetCash( const ioData &rData ) {}
	virtual void ThreadOutputCash( const ioData &rData ) {}
	virtual void ThreadCancelCash( const ioData &rData ) {}

	virtual void OnLogoutLog( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnCCUCount( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnEUNexonPixel( ServerNode *pServerNode, SP2Packet &rkPacket ) {} 

	void GetAgencyNo( OUT char *szAgencyNo, IN int iAgencyNoSize, IN bool bYear );
	void GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource );

public:
	ioLocalParent(void);
	virtual ~ioLocalParent(void);
};

#endif // __ioLocalParent_h__
