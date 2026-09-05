
#include "stdafx.h"
#include "ioOakBarrel.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "ioOakBarrelManager.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "../Util/IORandom.h"
#include "UserNodeManager.h"
#include "../MainServerNode/MainServerNode.h"

ioOakBarrel::ioOakBarrel()
{
	Init();
}

ioOakBarrel::~ioOakBarrel()
{
	Destroy();
}

void ioOakBarrel::Init()
{
	m_byOakBarrelStep			= 0;
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_byOakBarrelHole[i]	= 0;
	m_dwOakBarrelTime			= 0;
	m_byHoleIndex				= 0;
	m_iLimitSword				= 0;

	m_bChangeOakBarrelData		= false;
	m_bTimeCheckStart			= false;
}

void ioOakBarrel::Destroy()
{
	m_byOakBarrelStep			= 0;
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_byOakBarrelHole[i]	= 0;
	m_dwOakBarrelTime			= 0;
	m_byHoleIndex				= 0;
	m_iLimitSword				= 0;

	m_bChangeOakBarrelData		= false;
	m_bTimeCheckStart			= false;
}

void ioOakBarrel::Initialize( User *pUser )
{
	SetUser( pUser );
	Init();
}

bool ioOakBarrel::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}

void ioOakBarrel::DBtoData( CQueryResultData *query_data )
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	if( query_data == NULL )
		return;

	while( query_data->IsExist() )
	{
		PACKET_GUARD_BREAK( query_data->GetValue( m_byOakBarrelStep, sizeof(m_byOakBarrelStep) ) );				// 오크통 진행 단계
		for( int i = 0; i < OAK_BARREL_HOLE; ++i )
			PACKET_GUARD_BREAK( query_data->GetValue( m_byOakBarrelHole[i], sizeof(m_byOakBarrelHole[i]) ) );	// 12개의 오크통 구멍 상태
		PACKET_GUARD_BREAK( query_data->GetValue( m_dwOakBarrelTime, sizeof( m_dwOakBarrelTime ) ) );			// 칼 개수가 3개 미만 일때 수량 변동 된 시각값
	}
}

void ioOakBarrel::DBtoTimeData( CQueryResultData *query_data )
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	if( query_data == NULL )
		return;
	
	if( query_data->IsExist())
		query_data->GetValue( m_dwOakBarrelTime, sizeof( m_dwOakBarrelTime ) );				// 칼 개수가 3개 미만 일때 수량 변동 된 시각값
}

void ioOakBarrel::SaveData()
{
	//if( GetChangeOakBarrelData() == false )
		//return;

	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	BYTE ArrayHole[OAK_BARREL_HOLE] = {0, };
	this->GetOakBarrelHole( ArrayHole );

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] savedata [%d] iLimitSword[%d]", pUser->GetUserIndex(), this->GetLimitSword() );

	g_DBClient.OnUpdateOakBarrelData( pUser->GetUserDBAgentID()
		, pUser->GetAgentThreadID()
		, pUser->GetUserIndex()
		, OAK_BARREL_SAVE
		, this->GetOakBarrelStep()
		, ArrayHole
		, this->GetLimitSword() );

	//SetChangeOakBarrelData( false );
}


void ioOakBarrel::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( m_byOakBarrelStep ) );
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		PACKET_GUARD_VOID( rkPacket.Write( m_byOakBarrelHole[i] ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_dwOakBarrelTime ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iLimitSword ) );
}


void ioOakBarrel::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /*= false*/ )
{
	PACKET_GUARD_VOID( rkPacket.Read( m_byOakBarrelStep ) );
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		PACKET_GUARD_VOID( rkPacket.Read( m_byOakBarrelHole[i] ) );
	PACKET_GUARD_VOID( rkPacket.Read( m_dwOakBarrelTime ) );
	PACKET_GUARD_VOID( rkPacket.Read( m_iLimitSword ) );
}

void ioOakBarrel::GetOakBarrelHole( BYTE *pArray )
{
	for( int i = 0 ; i < OAK_BARREL_HOLE ; ++i )
		pArray[i] = static_cast<BYTE>(m_byOakBarrelHole[i]);
}

void ioOakBarrel::SetOakBarrelHole( int iHoleIndex )
{
	m_byOakBarrelHole[iHoleIndex] = OAK_HOLE_USED;
}

void ioOakBarrel::InitOakBarrelHole()
{
	for( int i = 0 ; i < OAK_BARREL_HOLE ; ++i )
		m_byOakBarrelHole[i] = OAK_HOLE_UNUSED;
}

// S->C 로그인 시 오크통 정보 전송
void ioOakBarrel::SendOakBarrelData( User *pUser )
{
	SP2Packet kPacket( STPK_OAK_BARREL_GET_INFO );
	PACKET_GUARD_VOID( kPacket.Write( pUser->GetOakBarrelOpen() ) );
	PACKET_GUARD_VOID( kPacket.Write( m_byOakBarrelStep ) );
	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		PACKET_GUARD_VOID( kPacket.Write( m_byOakBarrelHole[i] ) );

	pUser->SendMessage( kPacket );
}

// 칼 사용에 대한 결과
void ioOakBarrel::UseSwordResult( BYTE byIndex )
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// 구멍 인덱스 체크
	if( byIndex > OAK_BARREL_HOLE )
	{
		SP2Packet kPacket( STPK_OAK_BARREL_USE_SWORD );
		PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_ERR_HOLE_INDEX ) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::UseSword Error Hole index - %s, %d",  m_pUser->GetPublicID().c_str(), byIndex );
		return;
	}
	// 중복 구멍 체크
	if( m_byOakBarrelHole[byIndex] != OAK_HOLE_UNUSED )
	{
		SP2Packet kPacket( STPK_OAK_BARREL_USE_SWORD );
		PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_ERR_HOLE_DUPLICATED ) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::UseSword Error duplicated hole - %s, %d",  m_pUser->GetPublicID().c_str(), byIndex );
		return;
	}

	// 일일 한도 수량 차감
	int iLimitSword = this->GetLimitSword() - 1;
	this->SetLimitSword( iLimitSword );

	// 각 단계 별 개발자K 날아갈 확률
	DWORD dwRandStep = g_OakBarrelMgr.GetInvalidityRate( GetOakBarrelStep() );

	// 현재 랜덤 시드값
	IORandom mRand;
	mRand.SetRandomSeed( timeGetTime() );
	DWORD dwCurRand = mRand.Random( 0, 10000 );
	
	// 실패, 개발자 K 날아감
	if( this->GetOakBarrelStep() != 0 && dwCurRand < dwRandStep )
	{
		this->InitOakBarrelHole();
		this->SetHoleIndex( byIndex );
		
		BYTE ArrayHole[OAK_BARREL_HOLE] = {0, };
		this->GetOakBarrelHole( ArrayHole );

		g_DBClient.OnUpdateOakBarrelData( pUser->GetUserDBAgentID()
			, pUser->GetAgentThreadID()
			, pUser->GetUserIndex()
			, OAK_BARREL_FAIL
			, 0
			, ArrayHole
			, this->GetLimitSword() );
	}
	// 성공
	else
	{
		this->SetOakBarrelStep( this->GetOakBarrelStep() + 1 );
		this->SetOakBarrelHole( byIndex );
		this->SetHoleIndex( byIndex );

		BYTE ArrayHole[OAK_BARREL_HOLE] = {0, };
		this->GetOakBarrelHole( ArrayHole );

		g_DBClient.OnUpdateOakBarrelData( pUser->GetUserDBAgentID()
			, pUser->GetAgentThreadID()
			, pUser->GetUserIndex()
			, OAK_BARREL_SUCCESS
			, this->GetOakBarrelStep()
			, ArrayHole
			, this->GetLimitSword() );
	}
}


// S->C 오크통 결과 전송
void ioOakBarrel::SendOakBarrelResult( User *pUser, bool bSuccess, BYTE byStep )
{
	BYTE byIndex = 0;

	SP2Packet kPacket( STPK_OAK_BARREL_USE_SWORD );
	
	// 실패 시 현재 단계에서의 보상 상품 인덱스를 전송
	if( bSuccess == false )
	{
		g_OakBarrelMgr.GetOneStepReward( byStep, byIndex );

		int iType = 0;
		DWORD dwValue1 = 0;
		DWORD dwValue2 = 0;

		// 보상 상품 선물함 전송
		ioOakBarrelManager::mapAllReward &mapRewardAll = g_OakBarrelMgr.GetMapOakBarrelReward();
		ioOakBarrelManager::mapAllReward::iterator iter1 = mapRewardAll.find( byStep );
		if( iter1 == mapRewardAll.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::SendOakBarrelResult mapAllReward find error" );
			
			PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_ERR_REWARD_STEP ) );
			pUser->SendMessage( kPacket );
			
			return;
		}
		ioOakBarrelManager::mapOneStepReward::iterator iter2 = iter1->second.find( byIndex );
		if( iter2 == iter1->second.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::SendOakBarrelResult mapOneStepReward find error" );

			PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_ERR_REWARD_STEP ) );
			pUser->SendMessage( kPacket );

			return;
		}
		iType = iter2->second.m_iType;
		dwValue1 = iter2->second.m_dwValue1;
		dwValue2 = iter2->second.m_dwValue2;

		CTimeSpan cPresentGapTime( g_OakBarrelMgr.GetPeriod(), 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( g_MainServer.GetSendID(), iType, dwValue1, dwValue2, 0, 0, g_OakBarrelMgr.GetMent(), kPresentTime, g_OakBarrelMgr.GetState() );
		pUser->SendPresentMemory();

		g_LogDBClient.OnInsertOakbarrelInfo( pUser, OAK_BARREL_FAIL, byStep+2, iType, dwValue1, dwValue2 );
	}
	
	PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( bSuccess ) );
	PACKET_GUARD_VOID( kPacket.Write( this->GetHoleIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( m_byOakBarrelStep ) );

	if( bSuccess == false )
	{
		PACKET_GUARD_VOID( kPacket.Write( byIndex ) );
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] 2. [%d] byHoleIndex[%d] oakStep[%d] byPreStep[%d] byIndex[%d]", pUser->GetUserIndex(), this->GetHoleIndex(), m_byOakBarrelStep, byStep, byIndex );

		// 마지막 단계일 때 알림
		if( byStep+2 == g_OakBarrelMgr.GetHoleMax() )
		{
			SP2Packet kPacket1( SUPK_SERVER_ALARM_MENT );
			kPacket1 << UDP_SERVER_ALARM_OAK_RESULT << pUser->GetPublicID() << bSuccess << byStep+2;
			g_UserNodeManager.SendAllServerAlarmMent( kPacket1 );
		}
	}
	else
	{
		// 마지막 단계일 때 알림
		if( byStep+1 == g_OakBarrelMgr.GetHoleMax() )
		{
			SP2Packet kPacket1( SUPK_SERVER_ALARM_MENT );
			kPacket1 << UDP_SERVER_ALARM_OAK_RESULT << pUser->GetPublicID() << bSuccess << byStep+1;
			g_UserNodeManager.SendAllServerAlarmMent( kPacket1 );
		}
	}

	pUser->SendMessage( kPacket );
}

// 보상 요청
void ioOakBarrel::RewardReq( User *pUser )
{
	if( pUser == NULL )
		return;

	this->InitOakBarrelHole();
	
	BYTE ArrayHole[OAK_BARREL_HOLE] = {0, };
	this->GetOakBarrelHole( ArrayHole );

	g_DBClient.OnUpdateOakBarrelData( pUser->GetUserDBAgentID()
		, pUser->GetAgentThreadID()
		, pUser->GetUserIndex()
		, OAK_BARREL_REWARD
		, 0
		, ArrayHole
		, this->GetLimitSword() );
}

// S->C 요청 된 보상 전송
void ioOakBarrel::SendOakBarrelReward( User *pUser, BYTE byStep )
{
	if( pUser == NULL || byStep == 0 )
		return;

	BYTE byIndex = 0;
	std::vector<BYTE> vecIndex;
	vecIndex.clear();

	for( int i = 0; i < byStep; ++i )
	{
		g_OakBarrelMgr.GetOneStepReward( i, byIndex );
		vecIndex.push_back( byIndex );
	}
	
	SP2Packet kPacket( STPK_OAK_BARREL_GET_REWARD );
	PACKET_GUARD_VOID( kPacket.Write( OAK_BARREL_OK ) );
	PACKET_GUARD_VOID( kPacket.Write( byStep ) );
	std::vector<BYTE>::iterator iter = vecIndex.begin();
	for( ; iter != vecIndex.end(); ++iter )
		PACKET_GUARD_VOID( kPacket.Write( *iter ) );

	pUser->SendMessage( kPacket );

	int iType = 0;
	DWORD dwValue1 = 0;
	DWORD dwValue2 = 0;

	// 보상 상품 선물함 전송
	ioOakBarrelManager::mapAllReward &mapRewardAll = g_OakBarrelMgr.GetMapOakBarrelReward();
	for( int i = 0; i < byStep; ++i )
	{
		ioOakBarrelManager::mapAllReward::iterator iter1 = mapRewardAll.find( i );
		if( iter1 == mapRewardAll.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::SendOakBarrelReward mapAllReward find error" );
			continue;
		}
		ioOakBarrelManager::mapOneStepReward::iterator iter2 = iter1->second.find( vecIndex[i] );
		if( iter2 == iter1->second.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioOakBarrel::SendOakBarrelReward mapOneStepReward find error" );
			continue;
		}
		iType = iter2->second.m_iType;
		dwValue1 = iter2->second.m_dwValue1;
		dwValue2 = iter2->second.m_dwValue2;

		CTimeSpan cPresentGapTime( g_OakBarrelMgr.GetPeriod(), 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( g_MainServer.GetSendID(), iType, dwValue1, dwValue2, 0, 0, g_OakBarrelMgr.GetMent(), kPresentTime, g_OakBarrelMgr.GetState() );
		pUser->SendPresentMemory();

		if( i == 0 )
			g_LogDBClient.OnInsertOakbarrelInfo( pUser, OAK_BARREL_REWARD, byStep, iType, dwValue1, dwValue2 );
		else
			g_LogDBClient.OnInsertOakbarrelInfo( pUser, OAK_BARREL_REWARD, 0, iType, dwValue1, dwValue2 );
	}

	vecIndex.clear();
}

// DB로 부터 오크통 정보를 받아와 메모리 저장.
void ioOakBarrel::SetOakBarrelData( BYTE byStep, BYTE byHole[OAK_BARREL_HOLE], DWORD dwTime, int iLimitSword, bool bInit )
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	if( byStep <= 0 )
		byStep = 0;

	// 진행 단계 저장
	this->SetOakBarrelStep( byStep );
	this->SetLimitSword(iLimitSword);
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][oak] set data [%d] iLimitSword[%d]", pUser->GetUserIndex(), iLimitSword );
	
	// 구멍 상태 저장
	if( bInit == true )
	{
		this->InitOakBarrelHole();
		//if( bFirst )
			//SetLimitSword(iLimitSword);
	}
	else
	{
		for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		{
			if( byHole[i] == OAK_HOLE_USED )
				this->SetOakBarrelHole( i );
		}
	}

	// 하루가 변경 시 일일 한도 수량 초기화
	//CTime prevTime = Help::ConvertDateToCTime( dwTime, false );	// DEFAULT_YEAR : 2000
	//CTime curtime = CTime::GetCurrentTime();
	//CTimeSpan kInitTime = curtime - prevTime;

	CTime curtime = CTime::GetCurrentTime();
	DWORD dwCurTime = Help::ConvertCTimeToDate( curtime );

	DWORD dwCurYear	= dwCurTime / 100000000;
	dwCurTime		= dwCurTime - (dwCurYear * 100000000);
	DWORD dwCurMonth= dwCurTime / 1000000;
	dwCurTime		= dwCurTime - (dwCurMonth * 1000000);
	DWORD dwCurDay	= dwCurTime / 10000;

	DWORD dwLastTime = dwTime;
	DWORD dwLastYear= dwTime / 100000000;
	dwTime			= dwTime - (dwLastYear * 100000000);
	DWORD dwLastMonth= dwTime / 1000000;
	dwTime			= dwTime - (dwLastMonth * 1000000);
	DWORD dwLastDay	= dwTime / 10000;

	if( dwCurMonth == dwLastMonth && dwCurDay == dwLastDay )
	{
		// 시간값 저장
		this->SetOakBarrelTimeStamp( dwLastTime );
	}
	else
	{
		// 일일 한도 수량 초기화 디비에 세팅
		g_DBClient.OnUpdateOakBarrelLimitSwordInit( pUser->GetUserDBAgentID()
			, pUser->GetAgentThreadID()
			, pUser->GetUserIndex()
			, g_OakBarrelMgr.GetLimitSwordMax() );

		// 초기화 시각 디비에 세팅
		g_DBClient.OnUpdateOakBarrelTimeData( pUser->GetUserDBAgentID()
			, pUser->GetAgentThreadID()
			, pUser->GetUserIndex() );
	}
}

// DB로 부터 오크통 초기화 시간값을 받아와 메모리 저장
void ioOakBarrel::SetOakBarrelTimeData( DWORD dwTime )
{
	this->SetOakBarrelTimeStamp( dwTime );	
}