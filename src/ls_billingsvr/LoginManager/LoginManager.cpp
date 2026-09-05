#include "../stdafx.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "./loginmanager.h"
#include "../ThailandLoginServer/ThailandUser.h"
#include "../Local/ioLocalManager.h"


extern CLog LOG;

LoginManager::LoginManager(void)
{
}

LoginManager::~LoginManager(void)
{
	ReleaseMemoryPool();
}

void LoginManager::InitMemoryPool()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );

	int iMax = kLoader.LoadInt( "LoginInfoPool", 0 );
	for(int i = 0;i < iMax * 2 ;i++) //kyg Max 값 조정 
	{
		LoginInfo *pInfo = new LoginInfo();
		if( !pInfo )
			continue;
		m_LoginMemNode.Push( pInfo );
	}

	iMax = kLoader.LoadInt( "OTPInfoPool", 0 );
	for(int i = 0;i < iMax * 2; i++) //kyg MAx 값 조정 
	{
		LoginInfo *pInfo = new LoginInfo();
		if( !pInfo )
			continue;
		m_OTPMemNode.Push( pInfo );
	}
}


void LoginManager::ReleaseMemoryPool()
{
	for(sLoginInfo::iterator iter = m_sLoginInfo.begin(); iter != m_sLoginInfo.end(); ++iter)
	{
		LoginInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		m_LoginMemNode.Push( pInfo );
	}
	m_sLoginInfo.clear();
	m_LoginMemNode.DestroyPool();

	for(sLoginInfo::iterator iter = m_sOTPInfo.begin(); iter != m_sOTPInfo.end(); ++iter)
	{
		LoginInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		m_OTPMemNode.Push( pInfo );
	}
	m_sOTPInfo.clear();
	m_OTPMemNode.DestroyPool();
}


LoginInfo * LoginManager::GetLoginInfo( const ioHashString &rsPrivateID )
{
	LoginInfo kInfo;
	kInfo.m_sPrivateID = rsPrivateID;

	sLoginInfo::iterator iter = m_sLoginInfo.find( &kInfo );
	if( iter == m_sLoginInfo.end() )
		return NULL;

	LoginInfo *pInfo = *iter;
	return pInfo;
}

void LoginManager::RemoveLoginInfo( const ioHashString &rsPrivateID )
{
	LoginInfo kInfo;
	kInfo.m_sPrivateID = rsPrivateID;

	enum { MAX_LOOP = 100, };
	for (int i = 0; i < MAX_LOOP ; i++) // 쓰레기 데이터가 있을 수 있으므로 최고 100번 확인
	{
		sLoginInfo::iterator iter = m_sLoginInfo.find( &kInfo );
		if( iter == m_sLoginInfo.end() )
			break;	
		LoginInfo *pInfo = *iter;
		if( !pInfo )
			break;
		pInfo->Clear();
		m_sLoginInfo.erase( iter );
		m_LoginMemNode.Push( pInfo );
	}
}

bool LoginManager::CheckLogin( const ioData &rData )
{
	if( rData.GetReturnMsgType() != BSTPK_LOGIN_RESULT ) 
		return false;
	
	LoginInfo *pInfo = GetLoginInfo( rData.GetPrivateID() );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %s:%s", __FUNCTION__ , rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		return true;
	}

	if(ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA)
	{
		if(g_App.IsTestMode())
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_SUCCESS;
			kPacket << "jal";
			kPacket << 1;

			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			return true;
		}
	}

	ThailandUser kUser;
	ioHashString sPW;
	if( pInfo->m_ePasswordType == LoginInfo::PT_DECODE_LOGIN_PW )
	{
		kUser.SetData( 0, rData.GetBillingGUID(), rData.GetPrivateID(), rData.GetEncodePW(), rData.GetUserIP(), rData.GetReturnMsgType(), rData.GetServerIP(), rData.GetServerPort() );
		sPW = kUser.GetPW();
	}
	else
	{
		sPW = rData.GetEncodePW();
	}

	if( pInfo->m_sPW != sPW )
	{
		LOG.PrintTimeAndLog( 0, "%s Error PW. %s:%s[%s:%s]", __FUNCTION__ , rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str(), pInfo->m_sPW.c_str(), sPW.c_str() );
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		return true;
	}

	if( CheckOTP( rData, kUser, pInfo->m_bOTP ) )
		return true;

	SP2Packet kPacket( rData.GetReturnMsgType() );
	kPacket << rData.GetPrivateID();
	kPacket << rData.GetBillingGUID();
	kPacket << BILLING_LOGIN_RESULT_SUCCESS;
	if( !pInfo->m_sReturnValue.IsEmpty() )
	{
		kPacket << pInfo->m_sReturnValue;
		kPacket << pInfo->m_dwPCRoomNum;
	}
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
	//LOG.PrintTimeAndLog( 0, "%s Delete LoginInfo %s:%s:%s", __FUNCTION__, rData.GetPrivateID().c_str(), sPW.c_str(), pInfo->m_sReturnValue.c_str() );
	RemoveLoginInfo( rData.GetPrivateID() );
	return true;
}

bool LoginManager::InsertLogin( const ioData &rData, const char *pReturnValue, DWORD dwPCRoomNum, bool bOTP, LoginInfo::PasswordType ePasswordType )
{
	if( rData.GetReturnMsgType() != BSTPK_AUTOUPGRADE_LOGIN_RESULT )   
		return true;
	
	bool bExist = false;
	LoginInfo *pInfo = NULL;
	pInfo = GetLoginInfo( rData.GetPrivateID() );
	if( !pInfo )
		pInfo = (LoginInfo*)m_LoginMemNode.Pop();
	else
		bExist = true;

	if( !pInfo )
	{
		for(sLoginInfo::iterator iter = m_sLoginInfo.begin(); iter != m_sLoginInfo.end(); ++iter)
		{
			LoginInfo *pInfo = *iter;
			if( !pInfo )
				continue;
			if( TIMEGETTIME() - pInfo->m_dwCreateTime < MAX_ALIVE_TIME )
				continue;
			LOG.PrintTimeAndLog( 0, "%s Delete Login Info %s:%d", __FUNCTION__, pInfo->m_sPrivateID.c_str(), pInfo->m_dwCreateTime );
			m_sLoginInfo.erase( iter );
			m_LoginMemNode.Push( pInfo );
			break;
		}

		pInfo = (LoginInfo*)m_LoginMemNode.Pop();
		if( !pInfo )
		{
			LOG.PrintTimeAndLog(0, "%s Pool Over:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str()  );
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			return false;
		}
	}

	if( pInfo )
	{
		pInfo->m_sPrivateID   = rData.GetPrivateID();
		pInfo->m_sPW          = rData.GetEncodePW();
		pInfo->m_ePasswordType= ePasswordType;
		pInfo->m_bOTP         = bOTP;
		pInfo->m_dwCreateTime = TIMEGETTIME();
		if( pReturnValue != NULL )
		{
			pInfo->m_sReturnValue = pReturnValue;
			pInfo->m_dwPCRoomNum  = dwPCRoomNum;
		}
		

		if( !bExist )
			m_sLoginInfo.insert( pInfo );
		LOG.PrintTimeAndLog( 0, "%s InsertLoginInfo %s:%s:%s:%d", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetEncodePW().c_str(), (int) bExist );
	}

	return true;
}

LoginInfo * LoginManager::GetOTPInfo( const ioHashString &rsPrivateID )
{
	LoginInfo kInfo;
	kInfo.m_sPrivateID = rsPrivateID;

	sLoginInfo::iterator iter = m_sOTPInfo.find( &kInfo );
	if( iter == m_sOTPInfo.end() )
		return NULL;

	LoginInfo *pInfo = *iter;
	return pInfo;
}

void LoginManager::RemoveOTPInfo( const ioHashString &rsPrivateID )
{
	LoginInfo kInfo;
	kInfo.m_sPrivateID = rsPrivateID;

	enum { MAX_LOOP = 100, };
	for (int i = 0; i < MAX_LOOP ; i++) // 쓰레기 데이터가 있을 수 있으므로 최고 100번 확인
	{
		sLoginInfo::iterator iter = m_sOTPInfo.find( &kInfo );
		if( iter == m_sOTPInfo.end() )
			break;	
		LoginInfo *pInfo = *iter;
		if( !pInfo )
			break;
		pInfo->Clear();
		m_sOTPInfo.erase( iter );
		m_OTPMemNode.Push( pInfo );
	}
}

bool LoginManager::CheckOTP( const ioData &rData, const ThailandUser &rkUser, bool bOTP )
{
	if( !bOTP )
		return false;

	LoginInfo *pInfo = GetOTPInfo( rData.GetPrivateID() );
	if( !pInfo )
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s OTP Check NULL:%s:%s", __FUNCTION__, rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
		return true;
	}

	if( pInfo->m_sOTPID != rkUser.GetOTPID() || pInfo->m_sPW != rkUser.GetOTPPW() ) 
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s OTP Check Error:%s:%s[%s:%s:%s:%s]", __FUNCTION__, rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str(), pInfo->m_sOTPID.c_str() , rkUser.GetOTPID().c_str() ,pInfo->m_sPW.c_str() , rkUser.GetOTPPW().c_str() );
		RemoveOTPInfo( rData.GetPrivateID() );
		return true;
	}

	//LOG.PrintTimeAndLog( 0, "%s RemoveOTPInfo %s %s %s", __FUNCTION__, rData.GetPrivateID().c_str(), rkUser.GetOTPID().c_str(), rkUser.GetOTPPW().c_str() );
	RemoveOTPInfo( rData.GetPrivateID() );
	return false;
}

bool LoginManager::InsertOTP( const ThailandUser &rUser )
{
	LoginInfo *pInfo = NULL;
	bool bExist = false;
	pInfo = GetOTPInfo( rUser.GetPrivateID() );
	if( !pInfo )
		pInfo = (LoginInfo*)m_OTPMemNode.Pop();
	else
		bExist = true;

	if( !pInfo )
	{
		for(sLoginInfo::iterator iter = m_sOTPInfo.begin(); iter != m_sOTPInfo.end(); ++iter)
		{
			LoginInfo *pInfo = *iter;
			if( !pInfo )
				continue;
			if( TIMEGETTIME() - pInfo->m_dwCreateTime < MAX_ALIVE_TIME )
				continue;
			LOG.PrintTimeAndLog( 0, "%s Delete OTP Info %s:%s:%s:%d", __FUNCTION__, pInfo->m_sPrivateID.c_str(), pInfo->m_sOTPID.c_str(), pInfo->m_sPW.c_str(), pInfo->m_dwCreateTime );
			m_sOTPInfo.erase( iter );
			m_OTPMemNode.Push( pInfo );
			break;
		}

		pInfo = (LoginInfo*)m_OTPMemNode.Pop();
		if( !pInfo )
		{
			SP2Packet kPacket( BSTPK_OTP_RESULT );
			kPacket << rUser.GetPrivateID();
			kPacket << rUser.GetBillingGUID();
			kPacket << BILLING_OTP_RESULT_FAIL;
			kPacket << false;
			if( !g_ServerNodeManager.SendMessageIP( (ioHashString)rUser.GetServerIP(), rUser.GetServerPort(), kPacket ) )
			{	
				LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
			}
			LOG.PrintTimeAndLog(0, "%s Pool Over:[%s]%s:%s", __FUNCTION__, rUser.GetOTPID().c_str(), rUser.GetPrivateID().c_str(), rUser.GetBillingGUID().c_str() );
			return false;
		}
	}

	if( pInfo )
	{
		pInfo->m_dwCreateTime = TIMEGETTIME();
		pInfo->m_sPrivateID   = rUser.GetPrivateID();
		pInfo->m_sOTPID       = rUser.GetOTPID();
		pInfo->m_ePasswordType= LoginInfo::PT_DECODE_OTP_PW;
		pInfo->m_sPW          = rUser.GetOTPPW();
		m_sOTPInfo.insert( pInfo );
		//LOG.PrintTimeAndLog( 0, "%s InsertOTPInfo %s %s %s", __FUNCTION__, rUser.GetPrivateID().c_str(), rUser.GetOTPID().c_str(), rUser.GetOTPPW().c_str() );
	}

	return true;
}


void LoginManager::GetReturnValue(const ioHashString &rsPrivateID, ioHashString &szReturn)
{
	LoginInfo *pInfo = NULL;
	pInfo = GetLoginInfo(rsPrivateID);

	if( pInfo )
	{
		szReturn = pInfo->m_sReturnValue;
	}
}


void LoginManager::SetPCroomNum(const ioHashString &rsPrivateID, DWORD dwPCRoom)
{
	LoginInfo *pInfo = NULL;
	pInfo = GetLoginInfo(rsPrivateID);

	if( pInfo )
		pInfo->m_dwPCRoomNum = dwPCRoom;
}