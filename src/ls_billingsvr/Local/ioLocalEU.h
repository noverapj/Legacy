#ifndef __ioLocalEU_h__
#define __ioLocalLatin_h__

#include "ioLocalParent.h"
#include "../NodeInfo/TestCashManager.h"
#include "../LoginManager/loginmanager.h"
#include "../NodeInfo/MemInfoManager.h" 
#include "../Channeling/ioChannelingNodeParent.h"
#include "../Util/IORandom.h"
#include "../NexonEUDefine.h"

#define LOGIN_OK				     0
#define LOGIN_INVALID_PASSPORT	     2
#define	LOGIN_EXPIRED			     3
#define LOGIN_USER_IP_MISMATCHED     8
#define LOGIN_LOW_PASSPORT_LEVEL     10
#define LOGIN_GAME_CODE_MISMATCH     11
#define	LOGIN_SESSION_DATA_NOT_EXIST 12
#define LOGIN_DISCONNECTED			 13
#define LOGIN_INVALID_CHANNEL_CODE   14
#define LOGIN_SERVER_FAILED			 100

class ioLocalEU : public ioLocalParent
{


public:
	virtual ioLocalManager::LocalType GetType();

public:
	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnRecieveLoginData( const EU_SESSION4_REPLY &rsResult );
	virtual void OnReceiveGetCash( const EU_GETCASH_RESPONSE& rkResult );

public:
	virtual void ThreadLogin( const ioData &rData, LoginManager &rLoginMgr );
	virtual void OnRecieveOutputCash( const EU_PURCAHSEITEM_RESPONSE &rkResult );
	virtual void OnRecieveOutputCashAmount( const  EU_GETCASH_AMOUNT_RESPONSE &rkResult );
	virtual void OnRecieveProductList( const EU_PRODUCT_INQUIRY_RESPONSE &rkResult );

	virtual void OnEUNexonPixel( ServerNode *pServerNode, SP2Packet &rkPacket );
	
	virtual void Init();

	DWORD GetGoodsNo( DWORD nismsGoodsNo );
	DWORD GetGoodsPrice( DWORD nismsGoodsNo );
	
	DWORD m_dwReqKey;		//NISMS 에서 사용하는 ReqKey
	DWORD m_dwSSOReqKey;	//SSO 에서 사용하는 ReqKey
	
	//typedef struct NISMS_LIST; // nexon 패킷 
	typedef std::map< DWORD, NISMS_LIST > ProductListMap;
	

	ProductListMap	m_ProductListMap;
	ioHashString	m_sBillingPixelURL;
	
public:
	ioLocalEU(void);
	~ioLocalEU(void);
};
#endif // __ioLocalEU_h__
