
#include "stdafx.h"
#include "TrainingModeRoulette.h"

#include "../MainProcess.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"

CEventRoulette::CEventRoulette()
{
	Init();
}

CEventRoulette::~CEventRoulette()
{
	Destroy();
}

void CEventRoulette::Init()
{
	m_iBoardingTime = 0;
	m_iSpinTime = 0;
	m_iCoinCount = 0;
	m_vecGroupRange.clear();
	m_vecGroupAngle.clear();

	InitMember();

	m_Random.Randomize();
}

void CEventRoulette::InitMember()
{
	m_bStart = FALSE;
	m_dwStartTime = 0;
	m_dwJoinEndTime = 0;
	m_dwEndTime = 0;
	m_iAngle = -1;
	m_kResultPresent.Init();
	m_mapBoardingUser.clear();
}

void CEventRoulette::Destroy()
{
	Init();
}

void CEventRoulette::SetSpinTime( const int iSpinTime )
{
	m_iSpinTime = iSpinTime;
}

void CEventRoulette::SetBoardingTime( const int iBoardingTime )
{
	m_iBoardingTime = iBoardingTime;
}

void CEventRoulette::SetCoinCount( const int iCoinCount )
{
	m_iCoinCount = iCoinCount;
}

void CEventRoulette::InsertGroupRange( GROUPRANGE& rGroupRange )
{
	m_vecGroupRange.push_back( rGroupRange );
}

int CEventRoulette::GetGroupRangePosition( const int iRoomUserCount )
{
	int Pos = 0;
	int GroupRange = static_cast< int >( m_vecGroupRange.size() );

	for( Pos ; Pos < GroupRange ; ++Pos )
	{
		if( m_vecGroupRange[ Pos ].m_min <= iRoomUserCount && iRoomUserCount <= m_vecGroupRange[ Pos ].m_max )
			break;
	}

	return Pos;
}

void CEventRoulette::InsertAngleData( std::vector< ANGLEDATA >& rAngleData )
{
	m_vecGroupAngle.push_back( rAngleData );
}

const int CEventRoulette::GetNewAngle( const int iPos )
{
	// Declare) angel
	int angle = -1;

	// 당첨 확률 구하고.
	int state = m_Random.Random( 0, 10000 );

	std::vector< ANGLEDATA >::iterator iter		= m_vecGroupAngle[ iPos ].begin();
	std::vector< ANGLEDATA >::iterator iterEnd	= m_vecGroupAngle[ iPos ].end();

	while( iter != iterEnd )
	{
		// 당첨 확률에 대한 범위 체크.
		if( (*iter).m_maxRealPercent < state )
		{
			++iter;
			continue;
		}

		// 당첨 확률에 대한 범위 체크로 angle값 구해서.
		angle = m_Random.Random( (*iter).m_angle_min, (*iter).m_angle_max );

		// Set : Present
		m_kResultPresent = (*iter);
		break;
	}

	// set
	SetAngle( angle );
	
	// return
	return angle;
}

void CEventRoulette::SetRouletteStart( User* pUser )
{
	InitMember();

	SetState( TRUE );
	m_mapBoardingUser.insert( std::map< int, User* >::value_type( pUser->GetUserIndex(), pUser ) );
	SetStartTime( TIMEGETTIME() );
	SetJoinEndTime( GetStartTime() + GetBoardingTime() * 1000 );
	SetEndTime( GetStartTime() + GetSpinTime() * 1000 );
}

BOOL CEventRoulette::SetRouletteJoin( User* pUser )
{
	// Check : 조인가능 시간
	if( GetJoinEndTime() < TIMEGETTIME() )
	{
		// 탑승시간 종료.
		SP2Packet kPacket( STPK_ROULETTE_START );
		kPacket << static_cast< int >( ROULETTE_JOIN_TIME_OVER );
		pUser->SendMessage( kPacket );
		return FALSE;
	}

	// Check : 코인이 있는지만 체크하자. ( 두번째 파라미터 TRUE )
	if( pUser->UseRouletteCoin( GetUseCoinCount(), TRUE ) == false )
	{
		// 코인 부족.
		SP2Packet kPacket( STPK_ROULETTE_START );
		kPacket << static_cast< int >( ROULETTE_START_NOT_ENOUGH_COIN );
		pUser->SendMessage( kPacket );
		return FALSE;
	}

	// Check : 이미 등록되어 있는데 탑승이 되면 안되므로..
	std::map< int, User* >::iterator	iter = m_mapBoardingUser.find( pUser->GetUserIndex() );
	if( iter != m_mapBoardingUser.end() )
	{
		// 이미 조인
		SP2Packet kPacket( STPK_ROULETTE_START );
		kPacket << static_cast< int >( ROULETTE_JOIN_ALREADY );
		pUser->SendMessage( kPacket );
		return FALSE;
	}

	// insert
	m_mapBoardingUser.insert( std::map< int, User* >::value_type( pUser->GetUserIndex(), pUser ) );

	// return
	return TRUE;
}

void CEventRoulette::SetRouletteEnd( User* pSend )
{
	// Check : 종료 시간
	if( GetEndTime() > TIMEGETTIME() + 10000 )	// 10 초
	{
		LOG.PrintTimeAndLog( 0, "Not End Time... %u > %u", GetEndTime(), TIMEGETTIME() + 10000 );

		// PacketSend : 종료때까지 남은 시간 전달.
		SP2Packet kPacket( STPK_ROULETTE_EXCEPTION );
		kPacket << GetEndTime() - TIMEGETTIME();
		pSend->SendMessage( kPacket );
		return;
	}

	// Get : 선물 Data
	ANGLEDATA& presentInfo = GetPresentInfo();

	// 유저들 돌면서 코인 차감. / 선물 지급.
	std::map< int, User* >::iterator	iter	= m_mapBoardingUser.begin();
	std::map< int, User* >::iterator	iterEnd	= m_mapBoardingUser.end();

	while( iter != iterEnd )
	{
		if( ( (*iter).second )->UseRouletteCoin( GetUseCoinCount(), FALSE ) == true )
		{
			// 선물지급.
			CTimeSpan cPresentGapTime( presentInfo.m_angle_PresentPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

			// To Memory
			( (*iter).second )->AddPresentMemory( g_MainServer.GetSendID(), presentInfo.m_angle_PresentType, presentInfo.m_angle_PresentValue1,
				presentInfo.m_angle_PresentValue2, 0, 0, presentInfo.m_angle_PresentMent, kPresentTime, presentInfo.m_angle_PresentState );
			
			// LogDB
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), ( (*iter).second )->GetUserIndex(), presentInfo.m_angle_PresentType,
				presentInfo.m_angle_PresentValue1, presentInfo.m_angle_PresentValue2, 0, 0, LogDBClient::PST_RECIEVE, "Roulette" );

			// 한번만.
			( (*iter).second )->SendPresentMemory();
		}
		else
		{
			// Log.
			LOG.PrintTimeAndLog( 0, "%s : coin not enough.", __FUNCTION__ );
		}

		++iter;
	}

	// 데이터 초기화.
	InitMember();
}

const int CEventRoulette::GetBoardingMemberCount()
{
	return static_cast< int >( m_mapBoardingUser.size() );
}

void CEventRoulette::GetBoardingMember( std::vector< User* >& rMember )
{
	std::map< int, User* >::iterator	iter	= m_mapBoardingUser.begin();
	std::map< int, User* >::iterator	iterEnd	= m_mapBoardingUser.end();

	while( iter != iterEnd )
	{
		rMember.push_back( (*iter).second );

		++iter;
	}
}

BOOL CEventRoulette::GetRouletteMaster( ioHashString& name )
{
	if( m_mapBoardingUser.empty() )
		return FALSE;

	// Name
	std::map< int, User* >::iterator	iter	= m_mapBoardingUser.begin();
	name = ( (*iter).second )->GetPublicID();
	return TRUE;
}

BOOL CEventRoulette::RemoveUser( User* pUser )
{
	if( m_mapBoardingUser.empty() )
		return FALSE;

	// find
	std::map< int, User* >::iterator	iter = m_mapBoardingUser.find( pUser->GetUserIndex() );
	if( iter != m_mapBoardingUser.end() )
	{
		m_mapBoardingUser.erase( iter );
	}

	// 삭제 후 아무도 없다면.
	if( m_mapBoardingUser.empty() )
	{
		InitMember();

		return TRUE;
	}

	return FALSE;
}

void CEventRoulette::ModeInfo( SP2Packet& rkPacket )
{
	//	- 룰렛상태(T/F) / 정지까지 남은시간 / 인원수 / 룰렛참여인원 이름 / T / angle

	BOOL bState = GetState();
	if( bState )
	{
		// declare)
		DWORD dwRemainTime = GetEndTime() - TIMEGETTIME();
		int iBoardingUserCount = GetBoardingMemberCount();

		rkPacket << true;	// 룰렛 상태.
		rkPacket << dwRemainTime;
		rkPacket << iBoardingUserCount;

		// Declare)
		std::vector< User* > vecData;
		GetBoardingMember( vecData );

		std::vector< User* >::iterator	iter	= vecData.begin();
		std::vector< User* >::iterator	iterEnd	= vecData.end();
		while( iter != iterEnd )
		{
			rkPacket << (*iter)->GetPublicID();

			++iter;
		}

		//
		int angle = GetAngle();
		if( angle == -1 )
		{
			rkPacket << false;
		}
		else
		{
			rkPacket << true;
			rkPacket << angle;
		}
	}
	else
	{
		rkPacket << false;
	}
}

