#include "StdAfx.h"
#include "ioMissionManager.h"
#include "GUI/MissionWnd.h"

template<> ioMissionManager* Singleton< ioMissionManager >::ms_Singleton = 0;
ioMissionManager::ioMissionManager(void) : 
m_bChangeMission( false ),
m_RequestDateTime( 0 )
{
}

ioMissionManager::~ioMissionManager(void)
{
	ClreaAllMissionList();
	ClreaCurMissionList();
}

ioMissionManager& ioMissionManager::GetSingleton()
{
	return Singleton< ioMissionManager >::GetSingleton();
}

void ioMissionManager::ClreaAllMissionList()
{
	for ( int iType = MDT_DAILY ; iType<MDT_MAX ; ++iType)
	{
		for ( int i=0 ; i<(int)m_AllDayMissionList[iType].size() ; ++i )
			SAFEDELETE( m_AllDayMissionList[iType][i] );
		m_AllDayMissionList[iType].clear();
	}
}

void ioMissionManager::ClreaCurMissionList()
{
	for ( int iType = MDT_DAILY ; iType<MDT_MAX ; ++iType)
	{
		for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
			SAFEDELETE( m_CurDayMissionList[iType][i] );
		m_CurDayMissionList[iType].clear();
	}
}

void ioMissionManager::Initialize()
{
	char szBuf[MAX_PATH],szInIKey[MAX_PATH];
	enum{ MAX_MISSION_CODE = 1000, };
	ioINILoader_e kLoader( "config/sp2_daily_mission.ini" );
	for ( int iDateType = MDT_DAILY ; iDateType<MDT_MAX ; ++iDateType )
	{
		if ( iDateType == MDT_WEEKLY )
			kLoader.SetFileName_e( "config/sp2_weekly_mission.ini" );
		else if (iDateType == MDT_MONTHLY)
			kLoader.SetFileName_e( "config/sp2_monthly_mission.ini" );
		
		kLoader.SetTitle_e( "common" );
		int iMaxMissionCount = kLoader.LoadInt_e( "max_count", 0 );
		if( iMaxMissionCount == 0 )
			iMaxMissionCount = 1000;

		for(int iDate = 1; iDate <= iMaxMissionCount; ++iDate )
		{
			sprintf_s_e( szBuf, "date%d", iDate );
			kLoader.SetTitle( szBuf );
			int iStartDay = kLoader.LoadInt_e( "start_date", 0 );
			if ( iStartDay == 0 )
				continue;

			for(int iCode = 1; iCode < MAX_MISSION_CODE; ++iCode )
			{
				sprintf_s_e( szBuf, "mission%d_code", iCode );
				int iINIMissionCode = kLoader.LoadInt( szBuf, 0 );
				if ( iINIMissionCode == 0 )
					break;

				sprintf_s_e( szBuf, "mission%d_type", iCode );
				int iType = kLoader.LoadInt( szBuf, 0 );
				ioMission* pMission = CreateMission( iType );
				if ( !pMission ) return;

				sprintf_s_e( szInIKey, "mission%d", iCode );
				pMission->Init( szInIKey, kLoader );

				m_AllDayMissionList[iDateType].push_back( pMission );
			}
		}
	}
}

ioMission* ioMissionManager::CreateMission( const int& iType )
{
	switch( iType )
	{
	case MISSION_CLASS_LOGINTIME_CHECK:
		return new ioLoginTimeMission;
	case MISSION_CLASS_FISHING:
		return new ioFishingMission;
	case MISSION_CLASS_PESO_COLLECT:
		return new ioPesoCollectMission;
	case MISSION_CLASS_MONSTER_KILL:
		return new ioMonsterKillMission;
	case MISSION_CLASS_ITEM_BUY:
		return new ioItemBuyMission;
	case MISSION_CLASS_CLOVER:
		return new ioCloverMission;
	case MISSION_CLASS_EXCAVATION:
		return new ioExcavationMission;
	case MISSION_CLASS_ITEM_USE:
		return new ioItemUseMission;
	case MISSION_CLASS_ITEMREINFORCE:
		return new ioItemReinforceMission;
	case MISSION_CLASS_MODEPLAY:
		return new ioModePlayMission;
	case MISSION_CLASS_DAILY_COMPLETE:
		return new ioDailyCompleteMisson;
	case MISSION_CLASS_DAILY_ALL_CLEAR:
		return new ioDailyAllClearMisson;
	case MISSION_CLASS_WEEKLY_COMPLETE:
		return new ioWeeklyCompleteMission;
	case MISSION_CLASS_WEEKLY_ALL_CLEAR:
		return new ioWeeklyAllClearMission;
	case MISSION_CLASS_MONTHLY_ALL_CLEAR:
		return new ioMonthlyAllClearMission;
	case MISSION_CLASS_ADDITIVE_USE:
		return new ioAdditiveUseMission;
	default:
		{
			LOG.PrintTimeAndLog( 0, "%s UNKOWN Mission Type :%d", __FUNCTION__, iType );
			return NULL;
		}
	}
}

//패킷
void ioMissionManager::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_MISSION_STATE_CHANGE:
		OnMissionStateChange( rkPacket );
		break;
	case STPK_MISSION_INFO:
		OnSetMissionInfo( rkPacket );
		break;
	case STPK_MISSION_COMPENSATION_RECV:
		OnCompensationReceive( rkPacket );
		break;
	}
	for ( int iType = MDT_DAILY ; iType<MDT_MAX ; ++iType)
		std::sort( m_CurDayMissionList[iType].begin(), m_CurDayMissionList[iType].end(), MissionDataSort() );
	ChangeMission();
}

//STPK_MISSION_STATE_CHANGE
void ioMissionManager::OnMissionStateChange( SP2Packet &rkPacket )
{
	int iCode,iValue;
	BYTE byClearState;
	rkPacket >> iCode >> byClearState >> iValue ;
	SetMissionToCode( iCode, iValue, byClearState );
}

//STPK_MISSION_INFO
void ioMissionManager::OnSetMissionInfo( SP2Packet &rkPacket )
{
	BYTE byType;
	rkPacket >> byType; 
	if ( byType == MISSION_INFO_FAIL )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Update Mission Info ", __FUNCTION__ );
	}
	else if ( byType == MISSION_INFO_SUCCESS )
	{
		DWORD dwNextDayTime = 0;
		rkPacket >> dwNextDayTime; 

		//다음 미션 요청 날짜 갱신
		m_RequestDateTime = CTime( dwNextDayTime );
		ClreaCurMissionList();
		int iMissionCnt = 0;
		rkPacket >> iMissionCnt;
		if( g_App.IsMeDeveloper() )
		{
			LOG.PrintTimeAndLog( 0, "%s Request Date Time %d %d %d %d, Mission Size : %d  ", __FUNCTION__, m_RequestDateTime.GetYear(),
				m_RequestDateTime.GetMonth(), m_RequestDateTime.GetDay(), m_RequestDateTime.GetHour(), iMissionCnt );
		}
		for( int i = 0;i < iMissionCnt;++i)
			LoadMission( rkPacket ); 
	}
}

//STPK_MISSION_INFO
void ioMissionManager::LoadMission( SP2Packet &rkPacket )
{
	BYTE iDateType;
	int iPacketMissionCode, iPacketCurValue;
	BYTE PacketMissionState;
	rkPacket >> iDateType;
	rkPacket >> iPacketMissionCode;
	rkPacket >> PacketMissionState;
	rkPacket >> iPacketCurValue;

	if ( !COMPARE( iDateType, MDT_DAILY, MDT_MAX ) )
		LOG.PrintTimeAndLog( 0, "%s UNKOWN Mission Date Type :%d", __FUNCTION__, iDateType );

	ioMission* pMission = NULL;
	for( int i=0 ; i < (int)m_AllDayMissionList[iDateType].size() ; ++i )
	{
		if ( m_AllDayMissionList[iDateType][i]->GetMagicCode() == iPacketMissionCode )
		{
			pMission = m_AllDayMissionList[iDateType][i]->Clone();
			break;
		}
	}
	if ( pMission )
	{
		pMission->SetMissionState( iPacketCurValue, PacketMissionState );
		m_CurDayMissionList[iDateType].push_back( pMission );
		if( g_App.IsMeDeveloper() )
			LOG.PrintTimeAndLog( 0, "%s Success Mission! Code :%d", __FUNCTION__, iPacketMissionCode );
	}
	else
		LOG.PrintTimeAndLog( 0, "%s UNKOWN Mission Code :%d", __FUNCTION__, iPacketMissionCode );
}

void ioMissionManager::OnCompensationReceive( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	if ( iType == COMPENSATION_RECV_OK )
	{
		//UI Open
		int iCode;
		rkPacket >> iCode;
		MissionAttainStateWnd* pMissionAttainStateWnd = dynamic_cast<MissionAttainStateWnd*>( g_GUIMgr.FindWnd( MISSION_ATTAIN_STATE_WND ) );
		if ( pMissionAttainStateWnd )
			pMissionAttainStateWnd->ShowMissionAttainWnd( GetMissionToCode( iCode ) );
	}
	else if ( iType == COMPENSATION_UNKNOWN_MISSION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "존재하지 않는 미션입니다." );
	}
	else if ( iType == COMPENSATION_PROGRESS )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보상받기를 기다리고 있는 미션입니다." );
	}
	else if ( iType == COMPENSATION_ALREADY_RECV )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 받은 보상입니다." );
	}
	else if ( iType == COMPENSATION_EXCEPTION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "제외된 보상입니다." );
	}
}

//Get Func
const ioMission* const ioMissionManager::GetMissionToArray( const int& iType, const int& iArray ) const
{
	if ( !COMPARE( iType, MDT_DAILY, MDT_MAX ) )
		return NULL;

	int iSize = (int)m_CurDayMissionList[iType].size();
	if ( !COMPARE( iArray, 0, iSize ) )
		return NULL;

	return m_CurDayMissionList[iType][iArray];
}

const ioMission* const ioMissionManager::GetMissionToCode( const int& iType, const int& iCode ) const
{
	if ( !COMPARE( iType, MDT_DAILY, MDT_MAX ) )
		return NULL;

	for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
	{
		if ( m_CurDayMissionList[iType][i]->GetMagicCode() == iCode )
			return m_CurDayMissionList[iType][i];
	}

	return NULL;
}

const ioMission* const ioMissionManager::GetMissionToCode( const int& iCode ) const
{
	for( int iType=MDT_DAILY ; iType<MDT_MAX ; ++iType )
	{
		for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
		{
			if ( m_CurDayMissionList[iType][i]->GetMagicCode() == iCode )
				return m_CurDayMissionList[iType][i];
		}
	}

	return NULL;
}

int	ioMissionManager::GetMissionCountToType( const int& iType ) const
{
	if ( !COMPARE( iType, MDT_DAILY, MDT_MAX ) )
		return -1;

	return m_CurDayMissionList[iType].size();
}

int ioMissionManager::GetClearTypeCountToType( const int& iType, const int& iClearType ) const
{
	if ( !COMPARE( iType, MDT_DAILY, MDT_MAX ) )
		return -1;

	if ( !COMPARE( iClearType, MIS_PROGRESS, MIS_COMPLETE + 1 ) )
		return -1;

	int iCount = 0;
	for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
	{
		if ( m_CurDayMissionList[iType][i]->GetClearMissionType() == iClearType )
			iCount++;
	}

	return iCount;
}

int ioMissionManager::GetAttinCountToType( const int& iType ) const
{
	if ( !COMPARE( iType, MDT_DAILY, MDT_MAX ) )
		return -1;

	int iCount = 0;
	for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
	{
		if ( m_CurDayMissionList[iType][i]->GetClearMissionType() == MIS_ATTAIN )
			iCount++;
	}

	return iCount;
}

//Set Func
void ioMissionManager::SetMissionToCode( int iCode, int iValue, BYTE iClearMissionType )
{
	ioMission* pMission = NULL;
	for( int iType=MDT_DAILY ; iType<MDT_MAX ; ++iType )
	{
		for ( int i=0 ; i<(int)m_CurDayMissionList[iType].size() ; ++i )
		{
			if ( m_CurDayMissionList[iType][i]->GetMagicCode() == iCode )
			{
				pMission= m_CurDayMissionList[iType][i];
				break;
			}
		}
	}
	if ( pMission )
		pMission->SetMissionState( iValue, iClearMissionType );
	else
		LOG.PrintTimeAndLog( 0, "ioMissionManager::SetMissionToCode : MissionCode Not Exist, code : %d", iCode );
		
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MissionDataSort::operator()( const ioMission *lhs , const ioMission *rhs ) const
{
	if( !lhs || !rhs )
		return false;
	if( lhs->GetMissionSortType() < rhs->GetMissionSortType() )
		return true;
	else if( lhs->GetMissionSortType() == rhs->GetMissionSortType() )
	{
		if( lhs->GetMagicCode() < rhs->GetMagicCode() )
			return true;
		else if( lhs->GetMagicCode() == rhs->GetMagicCode() )
		{
			LOG.PrintTimeAndLog( 0, "MissionDataSort::operator : Exist Same MagicCode Left code : %d , Right code : %d", lhs->GetMagicCode(), rhs->GetMagicCode() );
			if( lhs->GetMissionType() < rhs->GetMissionType() )
				return true;
		}
	}
	return false;
}