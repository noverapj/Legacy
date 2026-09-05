#ifndef __LoginManager_h__
#define __LoginManager_h__

#include "../../include/MemPooler.h"

class ioData;
class LoginInfo : public SuperParent
{
public: 
	enum PasswordType
	{
		PT_ENCODE_LOGIN_PW = 1,
		PT_DECODE_LOGIN_PW = 2,
		PT_DECODE_OTP_PW   = 3,
	};
public:
	ioHashString m_sPrivateID;
	ioHashString m_sOTPID;
	ioHashString m_sPW; 
	ioHashString m_sReturnValue;
	bool         m_bOTP;
	DWORD        m_dwPCRoomNum; // 0은 PC방 아님, 1보다 큰 수는 PC방
	DWORD        m_dwCreateTime;
	PasswordType m_ePasswordType;

	void Clear()
	{
		m_sPrivateID.Clear();
		m_sPW.Clear();
		m_sReturnValue.Clear();
		m_bOTP = false;
		m_dwPCRoomNum  = 0;
		m_dwCreateTime = 0;
		m_ePasswordType = PT_DECODE_LOGIN_PW;
	}
public:
	LoginInfo(void){ Clear(); }
	virtual ~LoginInfo(void){}
};

class LoginInfoCompare // set비교 class
{
public:
	bool operator()(const LoginInfo *lhs , const LoginInfo *rhs) const
	{
		if(lhs->m_sPrivateID.GetHashCode() < rhs->m_sPrivateID.GetHashCode() ) 
			return true; 
		else if( lhs->m_sPrivateID.GetHashCode() == rhs->m_sPrivateID.GetHashCode()) 
		{ 
			int iCmpValue = _stricmp( lhs->m_sPrivateID.c_str(), rhs->m_sPrivateID.c_str() );
			if( iCmpValue < 0 )
				return true; 
			else 
				return false; 
		} 
		else 
			return false; 
	}
};

typedef std::set< LoginInfo*, LoginInfoCompare > sLoginInfo;

class ThailandUser;

class LoginManager
{
protected:
	enum
	{
		MAX_ALIVE_TIME   = 300000, // 5분 
	};

protected:
	sLoginInfo					m_sLoginInfo;
	MemPooler<LoginInfo>		m_LoginMemNode;

	sLoginInfo					m_sOTPInfo;
	MemPooler<LoginInfo>		m_OTPMemNode;

protected:
	void ReleaseMemoryPool();

	LoginInfo *GetLoginInfo( const ioHashString &rsPrivateID );
	void RemoveLoginInfo( const ioHashString &rsPrivateID );

	LoginInfo *GetOTPInfo( const ioHashString &rsPrivateID );
	void RemoveOTPInfo( const ioHashString &rsPrivateID );

public:
	void InitMemoryPool();

	bool CheckLogin( const ioData &rData );
	bool InsertLogin( const ioData &rData, const char *pReturnValue, DWORD dwPCRoomNum, bool bOTP, LoginInfo::PasswordType ePasswordType );

	int GetLoginNodeSize(){ return m_sLoginInfo.size(); }
	int RemainderLoginNode(){ return m_LoginMemNode.GetCount(); }

	bool CheckOTP( const ioData &rData, const ThailandUser &rkUser, bool bOTP );
	bool InsertOTP( const ThailandUser &rUser );

	int GetOTPNodeSize(){ return m_sOTPInfo.size(); }
	int RemainderOTPNode(){ return m_OTPMemNode.GetCount(); }

	void GetReturnValue(const ioHashString &rsPrivateID, ioHashString &szReturn);
	void SetPCroomNum(const ioHashString &rsPrivateID, DWORD dwPCRoom);


public:
	LoginManager(void);
	virtual ~LoginManager(void);
};

#endif // __LoginManager_h__