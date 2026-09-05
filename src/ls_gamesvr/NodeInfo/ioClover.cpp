
#include "stdafx.h"
#include "ioClover.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

ioClover::ioClover()
{
	Init();
}

ioClover::~ioClover()
{
	Destroy();
}

void ioClover::Init()
{
	SetChangeData( false );
	m_iCloverCount = 0;
	m_iLastChargeTime = 0;
	m_sRemainTime = 0;
	m_dwCloverCheckTime = 0;
}

void ioClover::Destroy()
{
}

void ioClover::SetCloverCount( const int iCount )
{
	m_iCloverCount = iCount;
	SetChangeData( true );
}

void ioClover::SetLastChargeTime( const int iChargeTime )
{
	m_iLastChargeTime = iChargeTime;
	SetChangeData( true );
}

void ioClover::SetMaxRemainTime()
{
	m_sRemainTime = static_cast< short >( Help::GetCloverChargeTimeMinute() );
	m_dwCloverCheckTime = TIMEGETTIME();

	SetChangeData( true );
}

void ioClover::IncreaseCloverCount( const int iCount )
{
	int iResultCount = GetCloverCount() + iCount;

	if( iResultCount >= Help::GetCloverMaxCount() )
		SetCloverCount( Help::GetCloverMaxCount() );
	else
		SetCloverCount( iResultCount );
}

void ioClover::DecreaseCloverCount( const int iCount )
{
	if( GetCloverCount() >= Help::GetCloverMaxCount() )
	{
		// 꽉 차있을때 소모하면 시간 흐름.
		SetMaxRemainTime();
	}

	int iResultCount = GetCloverCount() - iCount;

	if( iResultCount <= 0 )
		SetCloverCount( 0 );
	else
		SetCloverCount( iResultCount );
}

void ioClover::Initialize( User *pUser )
{
	SetUser( pUser );
}

bool ioClover::DBtoNewIndex( DWORD dwIndex )
{
	return true;
}

void ioClover::DBtoData( CQueryResultData *query_data )
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	// Set : 멤버 변수에 바로
	PACKET_GUARD_VOID( query_data->GetValue( m_iCloverCount, sizeof( int ) ) );
	PACKET_GUARD_VOID( query_data->GetValue( m_iLastChargeTime, sizeof( int ) ) );
	PACKET_GUARD_VOID( query_data->GetValue( m_sRemainTime, sizeof( short ) ) );
	
	// Check : DB값 체크.
	CheckCloverCount();
	CheckCloverRemainTime();

	// 현재시간
	CTime current_time = CTime::GetCurrentTime();
	
	if( GetLastChargeTime() == 0 )
	{
		// 0 일경우, 30분 후 부터 충전가능. 초기 클로버갯수 ini 세팅.
		UpdateCloverInfo( CLOVER_TYPE_REFILL, Help::GetCloverMaxCount(), current_time );

		// LOGDB : Clover Charge
		g_LogDBClient.OnInsertCloverInfo( pUser->GetUserIndex(), 0, ioClover::CLOVER_TYPE_REFILL, GetCloverCount() );
	}
	else
	{
		// 테스트 아닐때...
		if( Help::GetCloverRefillTest() == 0 )
		{
			//------------------------------
			// 마지막 충전시간과 날짜 비교.
			//------------------------------
			
			CTimeSpan GapTime( 0, Help::GetCloverRefill_Hour(), Help::GetCloverRefill_Min(), 0 );

			// 현재 시간에서 gap 시간만큼 -
			CTime compareTime = CTime::GetCurrentTime() - GapTime;

			// int형으로 구하고 DB값과 비교.
			int compareDay = ConvertYYMMDDToDate( compareTime.GetYear(), compareTime.GetMonth(), compareTime.GetDay() );

			// 시, 분 빼고 년월일로 비교.
			int lastChargeTime	= m_iLastChargeTime / DATE_DAY_VALUE;

			// 같지 않으면 충전.
			if( lastChargeTime != compareDay )
			{
				// 리필.
				UpdateCloverInfo( CLOVER_TYPE_REFILL, Help::GetCloverMaxCount(), compareTime );

				// LOGDB : Clover Charge
				g_LogDBClient.OnInsertCloverInfo( pUser->GetUserIndex(), 0, ioClover::CLOVER_TYPE_REFILL, GetCloverCount() );
			}
		}
		else
		{
			// 리필 테스트...
			// 마지막 충전시간 과 로그인 시간 비교.

			// 로그아웃 시간.
			CTime LogoutTime = ConvertNumberToCTime( m_iLastChargeTime );

			// 현재시간
			SYSTEMTIME st;
			GetLocalTime( &st );

			if( LogoutTime.GetHour() != st.wHour )
			{
				// 리필
				UpdateCloverInfo( CLOVER_TYPE_REFILL, Help::GetCloverMaxCount(), current_time );

				// LOGDB : Clover Charge
				g_LogDBClient.OnInsertCloverInfo( pUser->GetUserIndex(), 0, ioClover::CLOVER_TYPE_REFILL, GetCloverCount() );
			}
		}
	}

	m_dwCloverCheckTime = TIMEGETTIME();
	
	// 유저에게 클로버 정보 Send.
	pUser->GiftCloverInfo( ioClover::GIFT_CLOVER_LOGIN );
}

void ioClover::SaveData()
{
	if( GetChangeData() == false )
		return;

	User* pUser = GetUser();
	if( pUser == NULL )
		return;
	
	g_DBClient.OnUpdateCloverInfo( pUser->GetUserDBAgentID(),
								pUser->GetAgentThreadID(),
								pUser->GetUserIndex(),
								GetCloverCount(),
								GetLastChargeTime(),
								GetCloverRemainTime() );

	SetChangeData( false );
}

void ioClover::FillMoveData( SP2Packet &rkPacket )
{
	rkPacket << GetCloverCount() << GetLastChargeTime() << GetCloverRemainTime();
}

void ioClover::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /*= false*/ )
{
	rkPacket >> m_iCloverCount >> m_iLastChargeTime >> m_sRemainTime;

	m_dwCloverCheckTime = TIMEGETTIME();
}

const int ioClover::GetCloverCount()
{
	CheckCloverCount();

	return m_iCloverCount;
}

const short ioClover::GetCloverRemainTime()
{
	short remainTime = max( 0, m_sRemainTime - ( ( TIMEGETTIME() - m_dwCloverCheckTime ) / 1000 ) );
	if( remainTime < 0 )
		return MAGIC_TIME;

	return remainTime;
}

CTime ioClover::ConvertNumberToCTime( const int iDateTime )
{
	int year	= iDateTime / DATE_YEAR_VALUE;
	int month	= ( iDateTime % DATE_YEAR_VALUE ) / DATE_MONTH_VALUE;
	int day		= ( iDateTime % DATE_MONTH_VALUE ) / DATE_DAY_VALUE;
	int hour	= ( iDateTime % DATE_DAY_VALUE ) / DATE_HOUR_VALUE;
	int min		= iDateTime % DATE_HOUR_VALUE;

	CTime time( Help::GetSafeValueForCTimeConstructor( year + DEFAULT_YEAR, month, day, hour, min, 0 ) );
	return time;
}

const int ioClover::ConvertYYMMDDToDate( const WORD wYear, const WORD wMonth, const WORD wDay )
{
	// ( year * 10000 ) + ( month * 100 ) + day
	int iReturnDate = ((wYear - DEFAULT_YEAR) * DATE_DAY_VALUE) + (wMonth * DATE_HOUR_VALUE) + wDay;
	return iReturnDate;
}

BOOL ioClover::IsValidCharge( CTime& CurrentTime )
{
	// RemainTime 추가로 RemainTime으로 체크.
	if( GetCloverRemainTime() - MAGIC_TIME * 60 > 0 )
		return FALSE;

	return TRUE;
}

void ioClover::IsSaveRemainTime()
{
	short sRemainTime = GetCloverRemainTime();

	if( sRemainTime != m_sRemainTime )
		SetChangeData( true );
}

void ioClover::UpdateCloverInfo( const int iType, const int iCount, CTime& CurrentTime )
{
	switch( iType )
	{
	case CLOVER_TYPE_CHARGE:
		UpdateCloverCharge( iCount );
		break;
	case CLOVER_TYPE_REFILL:
		UpdateCloverRefill( iCount, CurrentTime );
		break;
	case CLOVER_TYPE_SEND:
		DecreaseCloverCount( iCount );
		break;
	case CLOVER_TYPE_RECEIVE:
		IncreaseCloverCount( iCount );
		break;
	}
}

void ioClover::UpdateCloverCharge( const int iCount )
{
	// Count
	IncreaseCloverCount( iCount );

	// Remain Time
	SetMaxRemainTime();
}

void ioClover::UpdateCloverRefill( const int iCount, CTime& CurrentTime )
{
	// Count
	SetCloverCount( iCount );

	// 시간 변형.
	SetConvertDate( CurrentTime );

	// Remain Time
	SetMaxRemainTime();
}

void ioClover::SetConvertDate( CTime& CurrentTime )
{
	// 시간 변형.
	DWORD dwCurrentTime = Help::ConvertYYMMDDHHMMToDate( CurrentTime.GetYear(),
														CurrentTime.GetMonth(),
														CurrentTime.GetDay(),
														0,		// hour
														0 );	// min

	// time
	SetLastChargeTime( static_cast< int >( dwCurrentTime ) );
}

void ioClover::CheckCloverCount()
{
	// min
	if( m_iCloverCount < 0 )
		m_iCloverCount = 0;

	// max
	if( m_iCloverCount > Help::GetCloverMaxCount() )
		m_iCloverCount = Help::GetCloverMaxCount();
}

void ioClover::CheckCloverRemainTime()
{
	if( m_sRemainTime < 0 )
		m_sRemainTime = MAGIC_TIME;

	if( m_sRemainTime > Help::GetCloverChargeTimeMinute() )
		m_sRemainTime = Help::GetCloverChargeTimeMinute();
}
