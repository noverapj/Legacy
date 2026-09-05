#ifndef __ioLocalThailand_h__
#define __ioLocalThailand_h__

#include "ioLocalParent.h"
#include "../NodeInfo/TestCashManager.h"
#include "ioLocalIndonesia.h"
#include "../ThailandLoginServer\ThailandUser.h"
#include "../../include/MemPooler.h"
#include <vector>

#define THAILAND_TOKEN '|'

class OTPInfo : public SuperParent
{
public:
	ioHashString m_sPrivateID;
	ioHashString m_sOTPID;
	ioHashString m_sOTPPW;
	DWORD        m_dwCreateTime;

	void Clear()
	{
		m_sPrivateID.Clear();
		m_sOTPID.Clear();
		m_sOTPPW.Clear();
		m_dwCreateTime = 0;
	}
public:
	OTPInfo(void){ Clear(); }
	virtual ~OTPInfo(void){}
};

typedef std::vector< OTPInfo* > vOTPInfo;

class ioLocalThailand  : public ioLocalParent
{
public:
	enum
	{
		MAX_PACKET_ALIVE_TIME = 30000,  // 30√ 
	};
protected:
	enum 
	{
		LOGIN_ARRAY_CODE   = 0,
		LOGIN_ARRAY_DESC   = 1,
		LOGIN_ARRAY_ID     = 2,
		LOGIN_ARRAY_OTPUSE = 3,
		LOGIN_ARRAY_OTPID  = 4,
		LOGIN_ARRAY_PID    = 5,
		MAX_LOGIN_ARRAY    = 6,

		OTP_ARRAY_TYPE     = 0,
		OTP_ARRAY_LENGTH   = 1,
		OTP_ARRAY_STATUS   = 2,
		OTP_ARRAY_DESC     = 3,
        OTP_ARRAY_END      = 4,
		MAX_OTP_ARRAY      = 5, 

		BILLING_GET_ARRAY_CODE  = 0,
		BILLING_GET_ARRAY_DESC  = 1,
		BILLING_GET_ARRAY_CASH  = 2,
		BILLING_GET_ARRAY_BONUS = 3,
		BILLING_GET_ARRAY_PID   = 4, 
		MAX_BILLING_GET_ARRAY   = 5, 

		BILLING_SET_ARRAY_CODE  = 0,
		BILLING_SET_ARRAY_DESC  = 1,
		BILLING_SET_ARRAY_CASH  = 2,
		BILLING_SET_ARRAY_BONUS = 3,
		BILLING_SET_ARRAY_DBID  = 4,
		BILLING_SET_ARRAY_PID   = 5, 
		MAX_BILLING_SET_ARRAY   = 6,

		IPBONUS_ARRAY_CODE = 0,
		IPBONUS_ARRAY_DESC = 1,
		IPBONUS_ARRAY_LEVEL= 2,
		IPBONUS_ARRAY_PID  = 3, 
		MAX_IPBONUS_ARRAY  = 4, 

		IPBONUS_OUT_ARRAY_CODE = 0,
		IPBONUS_OUT_ARRAY_DESC = 1,
		IPBONUS_OUT_ARRAY_LEVEL= 2,
		IPBONUS_OUT_ARRAY_PID  = 3, 
		MAX_IPBONUS_OUT_ARRAY  = 4, 
	};

protected:
	TestCashManager m_TestCashManager;

	// login
	LoginManager    m_LoginMgr;

	ioHashString    m_sLoginCheckAliveID;
	bool            m_bLoginSendAlive;

	// IPBonus
	ioHashString    m_sIPBonusCheckAliveID;
	bool            m_bIPBonusSendAlive;

	// IPBonus out
	ioHashString    m_sIPBonusOutCheckAliveID;
	bool            m_bIPBonusOutSendAlive;

	// Billing Get
	ioHashString    m_sBillingGetCheckAliveID;
	bool            m_bBillingGetSendAlive;

	// Billing Set
	ioHashString    m_sBillingSetCheckAliveID;
	bool            m_bBillingSetSendAlive;

	// total
	vThailandUser				 m_vUser;
	MemPooler<ThailandUser>      m_UserMemNode;

protected:
	void InitMemoryPool();
	void ReleaseMemoryPool();

	void RemoveUser( const ioHashString &rsPID );
	ThailandUser *GetUser( const ioHashString &rsPID );
	ThailandUser *InsertUser();

public:
	virtual ioLocalManager::LocalType GetType();

	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnOTP( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnIPBonus( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnIPBonusOut( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnRecieveLoginData( const ioHashString &rsResult );
	virtual void OnRecieveOTP( const ioHashString &rsResult, const ThailandUser &rUser );
	virtual void OnRecieveIPBonus( const ioHashString &rsResult );
	virtual void OnRecieveIPBonusOut( const ioHashString &rsResult );
	virtual void OnRecieveGetCash( const ioHashString &rsResult );
	virtual void OnRecieveOutputCash( const ioHashString &rsResult );

	virtual void Init();

	virtual int GetNodeSize(){ return m_LoginMgr.GetOTPNodeSize(); }
	virtual int RemainderNode(){ return m_LoginMgr.RemainderOTPNode(); }

	virtual int GetSecondNodeSize(){ return m_LoginMgr.GetLoginNodeSize(); }
	virtual int RemainderSecondNode(){ return m_LoginMgr.RemainderLoginNode(); }

	virtual int GetThirdNodeSize(){ return m_vUser.size(); }
	virtual int RemainderThirdNode(){ return m_UserMemNode.GetCount(); }

	void SendLogin( IN const char *szLoginID, IN const char *szPW, IN const char *szDomain, IN const char *szIP, IN bool bCheckAlive, OUT ioHashString &rsPID );
	void SendIPBonus( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID );
	void SendGetCash( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID );
	void SendOutputCash( IN const char *szPrivateID, IN const char *szPublicIP, IN const char *szPresentPrivateID, int iPayAmt, DWORD dwGoodsNo, IN bool bCheckAlive, OUT ioHashString &rsPID );
	void SendIPBonusOut( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID );

public:
	ioLocalThailand(void);
	virtual ~ioLocalThailand(void);
};

#endif // __ioLocalThailand_h__