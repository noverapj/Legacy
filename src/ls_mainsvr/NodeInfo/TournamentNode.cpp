#include "../stdafx.h"

#include "../EtcHelpFunc.h"
#include "../ioProcessChecker.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../QueryData/QueryResultData.h"
#include "../Util/cSerialize.h"

#include "ServerNodeManager.h"
#include "TournamentNode.h"
#include "../MainProcess.h"
#include <random>

TournamentNode::TournamentNode( DWORD dwIndex, DWORD dwOwnerIndex, DWORD dwStartDate, DWORD dwEndDate, BYTE Type, BYTE State ) :
m_dwIndex( dwIndex ),
m_dwOwnerIndex( dwOwnerIndex ),
m_dwStartDate( dwStartDate ),
m_dwEndDate( dwEndDate ),
m_Type( Type ),
m_State( State )
{
	m_RegularInfo.Init();
	m_CustomInfo.Init();

	m_dwTournamentTeamCount = 0;
	m_dwTournamentMaxRound  = 0;
	m_dwStateChangeTime     = 0;
	m_bTournamentRoundSet	= false;
	m_MaxPlayer				= 0;
	m_iPlayMode				= 0;
	m_dwAdjustCheerTeamPeso = 0;
	m_dwAdjustCheerUserPeso = 0;
	m_bDisableTournament    = false;

	if( GetType() == TYPE_REGULAR )
	{
		// 정규 리그
		LoadRegularINI();

		g_DBClient.OnSelectPrevTournamentChampInfo( GetIndex() );
	}
	else if( GetType() == TYPE_CUSTOM )
	{
		// 유저 리그
		g_DBClient.OnSelectTournamentCustomInfo( GetIndex() );
	}

	//
	LoadINI();
}

TournamentNode::~TournamentNode()
{
	AllTeamDelete();
}

void TournamentNode::AllTeamDelete()
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		SAFEDELETE( *iter );
	}
	m_TournamentTeamList.clear();
	m_RegularCheerTeamList.clear();

	TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.begin();
	for(;iCreator != m_TournamentRoundMap.end();++iCreator)
	{
		TournamentRoundData &rkRoundData = iCreator->second;
		rkRoundData.m_TeamList.clear();
	}
	m_TournamentRoundMap.clear();
	m_TournamentBattleList.clear();
}

DWORD TournamentNode::GetPlusDate( DWORD dwStartDate, DWORD dwPlusMinute )
{
	if( dwPlusMinute == 0 )
		return dwStartDate;

	CTime kStartDate = Help::ConvertDateToCTime( dwStartDate );
	CTimeSpan kGapTime( 0, 0, dwPlusMinute, 0 );
	kStartDate = kStartDate + kGapTime;
	return Help::ConvertCTimeToDate( kStartDate );
}

void TournamentNode::AddStateMinuteToDate( StateDate &rkStateDate, DWORD dwStartMinute, DWORD dwEndMinute )
{
	rkStateDate.m_dwStartDate = GetPlusDate( m_dwStartDate, dwStartMinute );		
	rkStateDate.m_dwEndDate   = GetPlusDate( m_dwStartDate, dwEndMinute );	
	m_StateDate.push_back( rkStateDate );
}

int TournamentNode::GetCustomRewardEmptySlot( BYTE TourPos )
{
	if( GetType() != TYPE_CUSTOM ) return 0;
	if( !COMPARE( TourPos - 1, 0, TOURNAMENT_CUSTOM_MAX_ROUND ) ) return 0;	

	int iEmptyPos = 0;
	if( m_CustomInfo.m_CustomReward[ TourPos - 1 ].m_dwRewardA == 0 )
		iEmptyPos++;
	if( m_CustomInfo.m_CustomReward[ TourPos - 1 ].m_dwRewardB == 0 )
		iEmptyPos++;
	if( m_CustomInfo.m_CustomReward[ TourPos - 1 ].m_dwRewardC == 0 )
		iEmptyPos++;
	if( m_CustomInfo.m_CustomReward[ TourPos - 1 ].m_dwRewardD == 0 )
		iEmptyPos++;
	return iEmptyPos;
}

void TournamentNode::InitTournamentMap()
{
	TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.begin();
	for(;iCreator != m_TournamentRoundMap.end();++iCreator)
	{
		TournamentRoundData &rkRoundData = iCreator->second;
		rkRoundData.m_TeamList.clear();
	}

	for(int i = 0;i < (int)m_dwTournamentMaxRound;i++)
	{
		iCreator = m_TournamentRoundMap.find( i + 1 );
		if( iCreator == m_TournamentRoundMap.end() )
		{
			TournamentRoundData kRoundData;
			m_TournamentRoundMap.insert( TournamentRoundMap::value_type( i + 1, kRoundData ) );
		}
	}
}

int TournamentNode::GetTournamentRoundTeamCount( DWORD dwRound )
{
	TournamentRoundData &rkRoundData = GetTournamentRound( dwRound );
	return rkRoundData.m_TeamList.size();
}

TournamentNode::TournamentRoundData &TournamentNode::GetTournamentRound( DWORD dwRound )
{
	TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.find( dwRound );
	if( iCreator == m_TournamentRoundMap.end() )
	{
		TournamentRoundData kRoundData;
		m_TournamentRoundMap.insert( TournamentRoundMap::value_type( dwRound, kRoundData ) );
	}

	iCreator = m_TournamentRoundMap.find( dwRound );
	if( iCreator == m_TournamentRoundMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "GetTournamentRound == Create RoundMap Fail!!" );

		static TournamentRoundData kNoneData;
		return kNoneData;
	}
	return iCreator->second;
}

TournamentNode::RoundTeamData &TournamentNode::GetTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundTeamList, SHORT Position )
{
	RoundTeamVec::iterator iter = rkRoundTeamList.m_TeamList.begin();
	for(;iter != rkRoundTeamList.m_TeamList.end();++iter)
	{
		RoundTeamData &rkTeamData = *iter;
		if( rkTeamData.m_Position == Position )
			return rkTeamData;
	}

	static RoundTeamData kNoneTeam;
	return kNoneTeam;
}

bool TournamentNode::IsTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundTeamList, SHORT Position )
{
	RoundTeamVec::iterator iter = rkRoundTeamList.m_TeamList.begin();
	for(;iter != rkRoundTeamList.m_TeamList.end();++iter)
	{
		RoundTeamData &rkTeamData = *iter;
		if( rkTeamData.m_Position == Position )
			return true;
	}
	return false;
}

void TournamentNode::LoadRegularINI( bool bReLoad )
{
	// LoadINI보다 먼저 로딩되어야하는 INI
	// 정규 리그 세팅 사항
	ioINILoader kLoader( "config/sp2_tournament_regular.ini" );
	if( bReLoad )
	{
		if( kLoader.ReloadFile( "config/sp2_tournament_regular.ini" ) )
		{
			LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
		}
		else
		{
			LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
		}
	}

	kLoader.SetTitle( "default" );

	m_RegularInfo.m_dwNextRegularHour = kLoader.LoadInt( "next_tournament_hour", 0 );
}

void TournamentNode::LoadINI( bool bReLoad )
{
	// 
	if( GetType() == TYPE_REGULAR )
	{
		m_StateDate.clear();
		m_RegularInfo.Init();

		// 정규 리그 세팅 사항
		ioINILoader kLoader( "config/sp2_tournament_regular.ini" );
		if( bReLoad )
		{
			if( kLoader.ReloadFile( "config/sp2_tournament_regular.ini" ) )
			{
				LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
			}
		}

		kLoader.SetTitle( "default" );

		m_dwTournamentTeamCount = kLoader.LoadInt( "start_tournament_team_count", 0 );
		m_dwTournamentMaxRound  = (short)kLoader.LoadInt( "tournament_max_round", 0 );

		LOG.PrintTimeAndLog( 0, "Reqular Tournament 1: %d ~ %d : %d, %d", m_dwStartDate, m_dwEndDate, m_RegularInfo.m_dwNextRegularHour, m_dwTournamentTeamCount );
		StateDate kStateDate;
		DWORD dwStartMinute, dwEndMinute;
		// 팀 등록 기간
		dwStartMinute = 0;
		dwEndMinute   = kLoader.LoadInt( "state_team_app_end_minute", 0 );
		AddStateMinuteToDate( kStateDate, dwStartMinute, dwEndMinute );
		LOG.PrintTimeAndLog( 0, "Regular Tournament TeamAppState: %d ~ %d", kStateDate.m_dwStartDate, kStateDate.m_dwEndDate );

		// 팀 등록 대기 기간
		dwStartMinute = dwEndMinute;
		dwEndMinute   = kLoader.LoadInt( "state_team_delay_end_minute", 0 );
		AddStateMinuteToDate( kStateDate, dwStartMinute, dwEndMinute );
		LOG.PrintTimeAndLog( 0, "Regular Tournament TeamDelayState: %d ~ %d", kStateDate.m_dwStartDate, kStateDate.m_dwEndDate );

		// 토너먼트 기간
		char szKey[MAX_PATH] = "";
		int iMaxTournament = kLoader.LoadInt( "max_tournament", 0 );
		for(int i = 0;i < iMaxTournament;i++)
		{
			sprintf_s( szKey, sizeof( szKey ), "state_tournament_start_minute_%d", i + 1 );
			dwStartMinute = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, sizeof( szKey ), "state_tournament_end_minute_%d", i + 1 );
			dwEndMinute   = kLoader.LoadInt( szKey, 0 );

			AddStateMinuteToDate( kStateDate, dwStartMinute, dwEndMinute );
			LOG.PrintTimeAndLog( 0, "Regular Tournament TournamentState %d: %d ~ %d", i + 1, kStateDate.m_dwStartDate, kStateDate.m_dwEndDate );
		}

		// 리소스
		m_RegularInfo.m_iRegularResourceType = kLoader.LoadInt( "resource_type", 1 );

		// 설정
		m_MaxPlayer				= (BYTE)kLoader.LoadInt( "max_player", 1 );
		m_iPlayMode				= kLoader.LoadInt( "play_mode", 1 );
		m_bDisableTournament	= kLoader.LoadBool( "disable_tournament", false );
		m_dwAdjustCheerTeamPeso = kLoader.LoadInt( "adjust_cheer_team_peso", 0 );
		m_dwAdjustCheerUserPeso = kLoader.LoadInt( "adjust_cheer_user_peso", 0 );

		char szBuf[MAX_PATH] = "";
		kLoader.LoadString( "title", "로스트사가", szBuf, MAX_PATH );
		m_RegularInfo.m_szTitle = szBuf;

		kLoader.LoadString( "blue_camp_name", "유미라", szBuf, MAX_PATH );
		m_RegularInfo.m_szBlueCampName = szBuf;

		kLoader.LoadString( "red_camp_name", "마이", szBuf, MAX_PATH );
		m_RegularInfo.m_szRedCampName = szBuf;

		// 기간별로 다른 적용
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, sizeof( szTitle ), "%d", m_dwStartDate );
		kLoader.SetTitle( szTitle );

		m_RegularInfo.m_iRegularResourceType = kLoader.LoadInt( "resource_type", m_RegularInfo.m_iRegularResourceType );
		m_MaxPlayer			   = (BYTE)kLoader.LoadInt( "max_player", m_MaxPlayer );
		m_iPlayMode			   = kLoader.LoadInt( "play_mode", m_iPlayMode );

		kLoader.LoadString( "title", m_RegularInfo.m_szTitle.c_str(), szBuf, MAX_PATH );
		m_RegularInfo.m_szTitle = szBuf;

		kLoader.LoadString( "blue_camp_name", m_RegularInfo.m_szBlueCampName.c_str(), szBuf, MAX_PATH );
		m_RegularInfo.m_szBlueCampName = szBuf;

		kLoader.LoadString( "red_camp_name", m_RegularInfo.m_szRedCampName.c_str(), szBuf, MAX_PATH );
		m_RegularInfo.m_szRedCampName = szBuf;

		m_RegularInfo.m_bDisableTournament	  = kLoader.LoadBool( "disable_tournament", m_bDisableTournament );
		m_RegularInfo.m_dwAdjustCheerTeamPeso = kLoader.LoadInt( "adjust_cheer_team_peso", m_dwAdjustCheerTeamPeso );
		m_RegularInfo.m_dwAdjustCheerUserPeso = kLoader.LoadInt( "adjust_cheer_user_peso", m_dwAdjustCheerUserPeso );
		
		m_RegularInfo.m_WinTeamCamp   = CAMP_NONE;

		LOG.PrintTimeAndLog( 0, "Regular Tournament CheerAdjustValue %d, %d", m_dwAdjustCheerTeamPeso, m_dwAdjustCheerUserPeso );

		// 대진표
		InitTournamentMap();
	}
	else if( GetType() == TYPE_CUSTOM )
	{
		// 유저 리그 세팅 사항
	}
}

void TournamentNode::SaveDB()
{
	g_DBClient.OnUpdateTournamentData( GetIndex(), GetStartDate(), GetEndDate(), GetType(), GetState() );
}

void TournamentNode::SaveInfoDB()
{
	if( GetType() == TYPE_REGULAR )
		return;  // 정규 리그는 정보 데이터가 없다.
	if( m_CustomInfo.m_dwInfoIndex == 0 )
		return;
	if( m_CustomInfo.m_bInfoDataChange == false )
		return;  // 변경된 사항이 없다.
	
	m_CustomInfo.m_bInfoDataChange = false;
	g_DBClient.OnUpdateTournamentInfoDataSave( m_CustomInfo.m_dwInfoIndex, m_CustomInfo.m_szAnnounce, GetRoundEndDate( STATE_TEAM_APP ), GetRoundEndDate( STATE_TEAM_DELAY ) );
}

void TournamentNode::SaveRoundDB()
{
	if( GetType() == TYPE_REGULAR )
		return;  // 정규 리그는 라운드 데이터가 없다.
	if( m_CustomInfo.m_dwRoundIndex == 0 )
		return;
	if( m_CustomInfo.m_bRoundDataChange == false )
		return;  // 변경된 사항이 없다.

	m_CustomInfo.m_bRoundDataChange = false;
	//2050=game_league_round_save INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT 
	//                            INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT  

	cSerialize v_FT;
	v_FT.Write( m_CustomInfo.m_dwRoundIndex );
	for(int i = 0;i < TOURNAMENT_CUSTOM_MAX_ROUND;i++)
	{
		v_FT.Write( GetRoundEndDate( STATE_TOURNAMENT + i ) );
		v_FT.Write( m_CustomInfo.m_CustomReward[i].m_dwRewardA );
		v_FT.Write( m_CustomInfo.m_CustomReward[i].m_dwRewardB );
		v_FT.Write( m_CustomInfo.m_CustomReward[i].m_dwRewardC );
		v_FT.Write( m_CustomInfo.m_CustomReward[i].m_dwRewardD );
	}

	g_DBClient.OnUpdateTournamentRoundDataSave( v_FT );
}

bool TournamentNode::IsTournamentEnd( CTime &rkCurTime )
{
	DWORD dwCurTime = Help::ConvertCTimeToDate( rkCurTime );

	if( dwCurTime < m_dwStartDate )
		return false;             // 시작 시간이 우선 지나야한다.

	if( !COMPARE( dwCurTime, m_dwStartDate, m_dwEndDate ) )
	{
		return true;
	}
	return false;
}

bool TournamentNode::IsCustomTournamentEnd()
{
	if( GetType() == TYPE_REGULAR )
		return false;
	return m_CustomInfo.m_bTournamentEnd;
}

void TournamentNode::Process()
{
	CTime cCurrentTime = CTime::GetCurrentTime();

	// 기간 종료되는지 확인
	if( IsTournamentEnd( cCurrentTime ) )
	{
		if( GetType() == TYPE_REGULAR )
		{
			// 정규리그는 다음 토너먼트 시작
			SetNextTournament();
		}
		else if( GetType() == TYPE_CUSTOM )
		{
			// 유저리그
			if( m_CustomInfo.m_bTournamentEnd == false )
			{
				SendEndProcess();
				m_CustomInfo.m_bTournamentEnd = true;
			}
		}
	}
	else
	{
		// 저장
		SaveInfoDB();
		SaveRoundDB();
		SaveTeamProcess( true );

		// 상태 체크
		StateProcess();
	}
	// 
}

void TournamentNode::StateProcess()
{
	CTime cCurrentTime = CTime::GetCurrentTime();

	// 상태 체크
	switch( m_State )
	{
	case STATE_WAITING:
		StateWaitingProcess( cCurrentTime );
		break;
	case STATE_TEAM_APP:
		StateTeamAppProcess( cCurrentTime );
		break;
	case STATE_TEAM_DELAY:
		StateTeamDelayProcess( cCurrentTime );
		break;
	default:
		StateTournamentProcess( cCurrentTime );
		break;
	}
}

void TournamentNode::SetState( BYTE State )
{
	if( m_State == State ) return;

	m_State				= State;
	m_dwStateChangeTime = TIMEGETTIME();
	m_bTournamentRoundSet = false;

	SendServerSync( NULL );
}

void TournamentNode::StateWaitingProcess( CTime &rkCurTime )
{
}

void TournamentNode::StateTeamAppProcess( CTime &rkCurTime )
{
	// 팀 모집 & 리그 기간
	if( m_State > (int)m_StateDate.size() )
	{
		LOG.PrintTimeAndLog( 0, "StateTeamAppProcess( %d ) None Check Time : %d", GetIndex(), (int)m_State );
		return;
	}

	StateDate &rkStateDate = m_StateDate[m_State - 1];
	DWORD dwCurTime = Help::ConvertCTimeToDate( rkCurTime );
	if( rkStateDate.m_dwEndDate == 0 )
	{
		// 유저 리그 - 수동 모드
	}
	else if( dwCurTime >= rkStateDate.m_dwEndDate )
	{
		// 팀 모집 기간 종료 처리 & 대진표 생성
		SetState( m_State + 1 );

		SaveDB();
	}
	else
	{
		// 팀 모집중에 할 일 없음.
	}
}

void TournamentNode::StateTeamDelayProcess( CTime &rkCurTime )
{
	// 팀 대기 & 응원
	if( m_State > (int)m_StateDate.size() )
	{
		LOG.PrintTimeAndLog( 0, "StateTeamDelayProcess( %d ) None Check Time : %d", GetIndex(), (int)m_State );
		return;
	}

	StateDate &rkStateDate = m_StateDate[m_State - 1];
	DWORD dwCurTime = Help::ConvertCTimeToDate( rkCurTime );
	if( rkStateDate.m_dwEndDate == 0 )
	{
		// 유저 리그 - 수동 모드
	}
	else if( dwCurTime >= rkStateDate.m_dwEndDate )
	{
		// 응원 기간 종료 처리
		SetState( m_State + 1 );
		
		// 유저리그는 대기기간종료시에 대전 배정이된다.
		if( GetType() == TYPE_CUSTOM )
		{
			CreateNewTournamentRoundCustom();
		}

		SaveDB();
//		LOG.PrintTimeAndLog( 0, "StateTeamDelayProcess Next State : %d", (int)m_State );
	}
	else
	{
		// 정규리그는 대기기간중에 대전 배정이된다.
		if( GetType() == TYPE_REGULAR )
		{
			if( !m_bTournamentRoundSet )
			{
				if( TIMEGETTIME() - m_dwStateChangeTime > TOURNAMENT_SET_TIME )
				{
					m_bTournamentRoundSet = true;
					CreateNewTournamentRoundRegular();
					CreateNewRegularCheerTeamList();
				}
			}
		}
	}
}

void TournamentNode::StateTournamentProcess( CTime &rkCurTime )
{
	// 토너먼트
	if( m_State > (int)m_StateDate.size() )
	{
		LOG.PrintTimeAndLog( 0, "StateTournamentProcess( %d ) None Check Time : %d", GetIndex(), (int)m_State );
		return;
	}

	// 토너먼트 라운드 시작 확인
	StateDate &rkStateDate = m_StateDate[m_State - 1];
	DWORD dwCurTime = Help::ConvertCTimeToDate( rkCurTime );	

	// 토너먼트 종료 시간을 체크하여 해당 전투 시간 발송
	if( rkStateDate.m_dwEndDate == 0 )
	{
		// 유저 리그 - 수동 모드
	}
	else if( dwCurTime >= rkStateDate.m_dwEndDate )
	{
		
		// 대전 발송하고 바로 다음 강에 대한 프로세스가 돌아간다.
		StateRoundStartProcess();	

		// 다음 강 갱신
		SetState( m_State + 1 );

		// 결승전까지 종료되었는지 확인
		if( m_State > (int)m_StateDate.size() )
		{
			SetState( STATE_WAITING );
		}
//		LOG.PrintTimeAndLog( 0, "StateTournamentProcess Next State : %d", (int)m_State );
	}
	else
	{
		//
	}
}

void TournamentNode::SetNextTournament()
{
	// 정규 대회 프로세스

	// 모든 팀 해체 & 로그 기록 & 보상 프로세스
	SendEndProcess();         // 토너먼트 종료 알림
	
	//
	if( GetType() == TYPE_REGULAR )
	{
		LoadRegularINI( true );
	}

	// 기간
	m_dwStartDate = m_dwEndDate;
	m_dwEndDate   = GetPlusDate( m_dwStartDate, m_RegularInfo.m_dwNextRegularHour * 60 ); //분단위로 체크
	
	g_DBClient.OnDeleteTournamentAllTeam( GetIndex() );

	// 존재하는 팀 삭제
	AllTeamDelete();

	// 데이터 로드
	LoadINI( true );

	// 새로운 대회
	SetState( STATE_TEAM_APP );	

	SaveDB();
}

void TournamentNode::SaveTeamProcess( bool bTimeCheck )
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;

		if( bTimeCheck )
		{
			if( pTeam->IsDataSaveTimeCheck() )
				pTeam->SaveData();
		}
		else			
		{
			if( pTeam->IsDataSave() )
				pTeam->SaveData();
		}
	}
}

DWORD TournamentNode::GetStateEndDate()
{
	if( m_State == STATE_WAITING )
		return m_dwEndDate;
	if( m_State > (int)m_StateDate.size() )
		return m_dwEndDate;
	return m_StateDate[m_State - 1].m_dwEndDate;
}

DWORD TournamentNode::GetRoundStartDate( BYTE State )
{
	if( !COMPARE( State - 1, 0, (BYTE)m_StateDate.size() ) )
		return 0;

	return m_StateDate[State - 1].m_dwStartDate;
}

DWORD TournamentNode::GetRoundEndDate( BYTE State )
{
	if( !COMPARE( State - 1, 0, (BYTE)m_StateDate.size() ) )
		return 0;

	return m_StateDate[State - 1].m_dwEndDate;
}

const ioHashString &TournamentNode::GetTitle()
{
	if( GetType() == TYPE_REGULAR )
		return m_RegularInfo.m_szTitle;
	return m_CustomInfo.m_szTitle;
}

void TournamentNode::FillMainInfo( SP2Packet &rkPacket )
{
	rkPacket << GetIndex() << GetOwnerIndex() << GetStartDate() << GetEndDate() << GetType() << GetState() 
		     << m_dwTournamentTeamCount << GetStateEndDate() << GetMaxPlayer() << GetPlayMode();

	if( GetType() == TYPE_REGULAR )
	{
		// 정규 리그 정보
		rkPacket << GetRegularResourceType() << m_RegularInfo.m_WinTeamCamp << IsDisableTournament() << GetTotalCheerCount();
	}
	else
	{
		// 유저 리그 정보
		rkPacket << m_CustomInfo.m_szTitle << GetBannerBig() << GetBannerSmall() << m_CustomInfo.m_TournamentMethod << m_CustomInfo.m_szAnnounce << (int)m_TournamentTeamList.size();
	}
}

void TournamentNode::FillScheduleInfo( SP2Packet &rkPacket )
{
	rkPacket << GetIndex();

	int iScheduleSize = m_StateDate.size();
	rkPacket << iScheduleSize;

	for(int i = 0;i < iScheduleSize;i++)
	{
		StateDate &rkDate = m_StateDate[i];
		rkPacket << rkDate.m_dwStartDate << rkDate.m_dwEndDate;
	}
}

void TournamentNode::SendServerSync( ServerNode *pServerNode )
{
	SP2Packet kPacket( MSTPK_TOURNAMENT_SERVER_SYNC );
	kPacket << GetIndex() << GetType() << GetState() << GetStartDate() << GetEndDate() << IsDisableTournament();
	if( GetType() == TYPE_REGULAR )
	{
		kPacket << m_RegularInfo.m_PrevChampTeamCamp << m_RegularInfo.m_PrevChampTeamName;
	}

	if( pServerNode )
	{
		pServerNode->SendMessage( kPacket );
	}
	else
	{
		g_ServerNodeManager.SendMessageAllNode( kPacket );
	}
}

void TournamentNode::SendPrevChampSync()
{
	//정규대회만이 이전 우승자 정보를 가지고 있는다.
	if( GetType() != TYPE_REGULAR )
		return;

	SP2Packet kPacket( MSTPK_TOURNAMENT_PREV_CHAMP_SYNC );
	kPacket << GetIndex() << m_RegularInfo.m_PrevChampTeamCamp << m_RegularInfo.m_PrevChampTeamName;
	g_ServerNodeManager.SendMessageAllNode( kPacket );	

	LOG.PrintTimeAndLog(0 ,"%s - prev_champ_info sync", __FUNCTION__ );
}

void TournamentNode::StateRoundStartProcess()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	int i = 0;
	m_TournamentBattleList.clear();

	BYTE TourPos = ( GetState() - STATE_TOURNAMENT ) + 1;
	BYTE NextTourPos = TourPos + 1;

	TournamentRoundData &rkRoundData = GetTournamentRound( TourPos );

	if( rkRoundData.m_TeamList.empty() )
	{
		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 1 : None Team : %d", __FUNCTION__, (int)TourPos );
		}
		return;      // 경기가 없다.
	}

	TournamentRoundData &rkNextRoundData = GetTournamentRound( NextTourPos );

	if( GetType() == TYPE_REGULAR ) 
	{
		LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d Round Start :", (int)TourPos );
	}

	int iMaxTeamCount = ( (int)pow( (double)2, (double)( m_dwTournamentMaxRound - ( GetState() - STATE_TOURNAMENT ) ) ) );
	for(i = 0;i < iMaxTeamCount; i += 2)
	{
		RoundTeamData &rkBlueTeam = GetTournamentRoundTeam( rkRoundData, i + 1 );
		RoundTeamData &rkRedTeam  = GetTournamentRoundTeam( rkRoundData, i + 2 );

		TournamentBattleData kBattleRoom;
		kBattleRoom.m_dwBlueIndex = rkBlueTeam.m_dwTeamIndex;
		kBattleRoom.m_szBlueName  = rkBlueTeam.m_szTeamName;
		kBattleRoom.m_BlueCamp    = rkBlueTeam.m_CampPos;
		kBattleRoom.m_BlueTourPos = rkBlueTeam.m_TourPos;
		kBattleRoom.m_BluePosition= rkBlueTeam.m_Position;

		kBattleRoom.m_dwRedIndex  = rkRedTeam.m_dwTeamIndex;
		kBattleRoom.m_szRedName   = rkRedTeam.m_szTeamName;
		kBattleRoom.m_RedCamp     = rkRedTeam.m_CampPos;
		kBattleRoom.m_RedTourPos  = rkRedTeam.m_TourPos;
		kBattleRoom.m_RedPosition = rkRedTeam.m_Position;

		if( kBattleRoom.m_dwBlueIndex == 0 && kBattleRoom.m_dwRedIndex == 0 ) 
		{
			if( GetType() == TYPE_REGULAR ) 
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d Round - 팀이 존재하지 않음, 현재 라운드의 팀 위치 %d, %d",  TourPos, i + 1, i + 2 );
			}
			continue;             // 두 팀다 없다.
		}

		if( kBattleRoom.m_dwRedIndex == 0 )
		{
			if( GetType() == TYPE_REGULAR )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d Round - 블루팀 부전승(레드팀이 존재하지 않음) : 레드 : %s, 블루 : %s", TourPos, rkRedTeam.m_szTeamName.c_str(), rkBlueTeam.m_szTeamName.c_str() );
			}
			
			// 블루팀 부전승
			TournamentTeamNode *pTeamNode = GetTeamNode( kBattleRoom.m_dwBlueIndex );
			if( pTeamNode )
			{
				pTeamNode->SetTourPos( NextTourPos );
				pTeamNode->SetPosition( ( rkBlueTeam.m_Position + 1 ) / 2 );

				RoundTeamData kTeamData;
				kTeamData.m_dwTeamIndex = pTeamNode->GetTeamIndex();
				kTeamData.m_szTeamName  = pTeamNode->GetTeamName();
				kTeamData.m_Position    = pTeamNode->GetPosition();
				kTeamData.m_TourPos     = pTeamNode->GetTourPos();
				kTeamData.m_CampPos     = pTeamNode->GetCampPos();

				rkNextRoundData.m_TeamList.push_back( kTeamData );
				if( (SHORT)NextTourPos > m_dwTournamentMaxRound )
				{
					StateFinalWinTeamProcess( pTeamNode );
				}
			}
			else
			{
				if( GetType() == TYPE_REGULAR ) 
				{
					LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d - none Team Pointer 1: %d", TourPos, kBattleRoom.m_dwBlueIndex );
				}
				
			}
		}
		else if( kBattleRoom.m_dwBlueIndex == 0 )
		{
			if( GetType() == TYPE_REGULAR ) 
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d Round - 레드팀 부전승(블루팀이 존재하지 않음) : 레드 : %s, 블루 : %s", TourPos, rkRedTeam.m_szTeamName.c_str(), rkBlueTeam.m_szTeamName.c_str() );
			}

			// 레드팀 부전승
			TournamentTeamNode *pTeamNode = GetTeamNode( kBattleRoom.m_dwRedIndex );
			if( pTeamNode )
			{
				pTeamNode->SetTourPos( NextTourPos );
				pTeamNode->SetPosition( ( rkRedTeam.m_Position + 1 ) / 2 );

				RoundTeamData kTeamData;
				kTeamData.m_dwTeamIndex = pTeamNode->GetTeamIndex();
				kTeamData.m_szTeamName  = pTeamNode->GetTeamName();
				kTeamData.m_Position    = pTeamNode->GetPosition();
				kTeamData.m_TourPos     = pTeamNode->GetTourPos();
				kTeamData.m_CampPos     = pTeamNode->GetCampPos();

				rkNextRoundData.m_TeamList.push_back( kTeamData );
				if( (SHORT)NextTourPos > m_dwTournamentMaxRound )
				{
					StateFinalWinTeamProcess( pTeamNode );
				}
			}
			else
			{
				if( GetType() == TYPE_REGULAR ) 
				{
					LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d Round - none Team Pointer 2: %d", TourPos, kBattleRoom.m_dwBlueIndex );
				}
			}
		}
		else
		{
			// 대회 경기 진행 알림
			m_TournamentBattleList.push_back( kBattleRoom );
		}
	}

	if( !rkNextRoundData.m_TeamList.empty() )
	{
		// 부전승 게임서버에 전송
		int iMaxTeam = (int)rkNextRoundData.m_TeamList.size();
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
		kPacket << iMaxTeam;
		for( i = 0;i < iMaxTeam; i++ )
		{
			RoundTeamData &rkTeam = rkNextRoundData.m_TeamList[i];
			kPacket << rkTeam.m_dwTeamIndex << rkTeam.m_Position << rkTeam.m_TourPos << true;       // 정보 유저에게 전송
		}
		g_ServerNodeManager.SendMessageAllNode( kPacket );

		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess 부전승 전송" );
		}
	}

	// 대회 경기 생성 전송
	int iCreateBattleRoomCount = (int)m_TournamentBattleList.size();
	if( iCreateBattleRoomCount == 0 )
	{
		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess %d : None Team", (int)TourPos );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "StateRoundStartProcess %d : None Team", (int)TourPos );
		}

		return;       // 경기가 없다
	}
		
	int iMaxServerCount = g_ServerNodeManager.GetNodeSize();

	// 경기수보다 서버가 더 많으면 1경기씩!!
	int iCreateBattleRoom = 1;

	if( iCreateBattleRoomCount > iMaxServerCount )
	{									// 52 / 32
		iCreateBattleRoom = (int)( (float)iCreateBattleRoomCount / iMaxServerCount ) + 1;   
	}

	int iStartPosition = 0;
	for(i = 0;i < iMaxServerCount;i++)
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_ROUND_START );
		kPacket << GetIndex() << GetState() << GetPlayMode() << GetMaxPlayer();
		
		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess 배틀룸 생성 발송(라운드 시작) - %d", (int)TourPos );
		}

		kPacket << iCreateBattleRoom;
		for(int k = 0;k < iCreateBattleRoom;k++)
		{
			if( iStartPosition < iCreateBattleRoomCount )
			{
				TournamentBattleData &rkBattleData = m_TournamentBattleList[iStartPosition++];
				kPacket << rkBattleData.m_dwBlueIndex << rkBattleData.m_BluePosition;
				kPacket << rkBattleData.m_dwRedIndex  << rkBattleData.m_RedPosition;

				if( GetType() == TYPE_REGULAR ) 
				{
					LOG.PrintTimeAndLog( 0, "[대회로그] StateRoundStartProcess 배틀룸 정보 - 서버 : %d, 블루팀 : %d(%d), 레드팀 : %d(%d)", i, 
						rkBattleData.m_dwBlueIndex, rkBattleData.m_BluePosition, rkBattleData.m_dwRedIndex, rkBattleData.m_RedPosition );
				}
			}
			else
			{
				kPacket << 0 << (SHORT)0 << 0 << (SHORT)0;
				LOG.PrintTimeAndLog( 0, "StateRoundStartProcess Battle Room StartPosition Error!" );
			}
		}
		g_ServerNodeManager.SendMessageArray( i, kPacket );
		
		if( iStartPosition >= iCreateBattleRoomCount )
			break;			// 모든 경기방이 생성되었다.

		// 다음 서버가 마지막 서버 !!
		if( i + 1 == iMaxServerCount - 1 )          
		{
			// 마지막 서버에는 남은 모든 서버를 전송한다.
			iCreateBattleRoom = iCreateBattleRoomCount - iStartPosition;
		}
	}	
}

void TournamentNode::StateFinalWinTeamProcess( TournamentTeamNode *pWinTeam )
{
	if( pWinTeam == NULL ) 
	{
		m_RegularInfo.m_PrevChampTeamCamp = CAMP_NONE;
		m_RegularInfo.m_PrevChampTeamName.Clear();
		SendPrevChampSync();
		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] StateFinalWinTeamProcess None Team (TourIdx : %d) ", GetIndex() );
		}
		return;
	}
	
	SaveTeamProcess( false );        // 모든 팀 저장

	if( GetType() == TYPE_REGULAR )
	{
		// 정규 대회 승리팀 처리
		ioHashString kCampName = "None";
		if( pWinTeam->GetCampPos() == CAMP_BLUE )
			kCampName = m_RegularInfo.m_szBlueCampName;
		else if( pWinTeam->GetCampPos() == CAMP_RED )
			kCampName = m_RegularInfo.m_szRedCampName;

		m_RegularInfo.m_WinTeamCamp		  = pWinTeam->GetCampPos();
		m_RegularInfo.m_PrevChampTeamCamp = pWinTeam->GetCampPos();
		m_RegularInfo.m_PrevChampTeamName = pWinTeam->GetTeamName();
		SendPrevChampSync();

		g_DBClient.OnInsertTournamentWinnerHistory( m_RegularInfo.m_szTitle, GetStartDate(), GetEndDate(), pWinTeam->GetTeamIndex(), pWinTeam->GetTeamName(), kCampName, pWinTeam->GetCampPos() );
		LOG.PrintTimeAndLog( 0, "[대회로그] StateFinalWinTeamProcess Regular Champion : %d - %s - %d Win", GetIndex(), pWinTeam->GetTeamName().c_str(), pWinTeam->GetTeamIndex() );
	}
	else
	{
		// 유저 대회 승리팀 처리
		LOG.PrintTimeAndLog( 0, "StateFinalWinTeamProcess Custom Champion : %d - %s - %d Win", GetIndex(), pWinTeam->GetTeamName().c_str(), pWinTeam->GetTeamIndex() );
	}
}

void TournamentNode::TournamentRoundCreateBattleRoom( DWORD dwServerIndex, int iMaxBattleRoom, SP2Packet &rkPacket )
{
	SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLEROOM_INVITE );
	kPacket << GetIndex() << dwServerIndex << iMaxBattleRoom;
	
	if( GetType() == TYPE_REGULAR ) 
	{
		LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회팀 초대 발송 시작 - 게임서버 : %d (생성완료된 배틀룸 갯수 : %d)", __FUNCTION__, dwServerIndex, iMaxBattleRoom );
	}

	for( int i = 0; i < iMaxBattleRoom; i++ )
	{
		SHORT Position;
		DWORD dwBattleRoomIndex;
		rkPacket >> Position >> dwBattleRoomIndex;

		TournamentBattleVec::iterator iter = m_TournamentBattleList.begin();
		for(;iter != m_TournamentBattleList.end();iter++)
		{
			TournamentBattleData &rkTournamentBattle = *iter;
			if( rkTournamentBattle.m_BluePosition == Position )
			{
				kPacket << dwBattleRoomIndex << rkTournamentBattle.m_dwBlueIndex << rkTournamentBattle.m_szBlueName << rkTournamentBattle.m_BlueCamp
					                         << rkTournamentBattle.m_dwRedIndex << rkTournamentBattle.m_szRedName << rkTournamentBattle.m_RedCamp;

				if( GetType() == TYPE_REGULAR ) 
				{
					LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회팀 초대 발송 시작 - 블루팀 :%d, 블루팀 이름 : %s / 레드팀 : %d, 레드팀 이름 : %s", __FUNCTION__, 
						rkTournamentBattle.m_dwBlueIndex, rkTournamentBattle.m_szBlueName.c_str(), 
						rkTournamentBattle.m_dwRedIndex, rkTournamentBattle.m_szRedName.c_str() );
				}
			}
		}
	}
	// 전체 서버에 전송
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void TournamentNode::TournamentRoundChangeBattleTeam( SP2Packet &rkPacket )
{
	// 예비 엔트리로 교체
	DWORD dwTeamIndex, dwBattleRoomIndex;
	rkPacket >> dwTeamIndex >> dwBattleRoomIndex;

	TournamentTeamNode *pDropTeam = GetTeamNode( dwTeamIndex );
	if( pDropTeam )
	{
		//
		pDropTeam->SetDropEntry( true );         // 미 참여로 탈락!!!!

		// 다음 순번 팀을 찾는다. - 이미 정렬된 데이터이므로 순서대로 처리
		TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
		for(;iter != m_TournamentTeamList.end();++iter)
		{
			TournamentTeamNode *pTeam = *iter;
			if( pTeam == NULL ) continue;
			if( pTeam->IsDropEntry() ) continue;
			if( pTeam->GetTourPos() != 0 ) continue;
			if( pTeam->GetCampPos() != pDropTeam->GetCampPos() ) continue;

			// 교체팀으로 투입!!!
			pTeam->SetPosition( pDropTeam->GetPosition() );
			pTeam->SetTourPos( pDropTeam->GetTourPos() );
			if( pDropTeam->GetTourPos() == 1 )      // 첫 라운드면
				pTeam->SetStartPosition( pTeam->GetPosition() );

			// 이전팀 드랍
			pDropTeam->SetPosition( 0 );
			pDropTeam->SetTourPos( 0 );
			pDropTeam->SetStartPosition( 0 );

			// 라운드 테이블 교체
			TournamentRoundData &rkRoundData = GetTournamentRound( pTeam->GetTourPos() );
			RoundTeamData &rkRoundTeamData   = GetTournamentRoundTeam( rkRoundData, pTeam->GetPosition() );

			rkRoundTeamData.m_dwTeamIndex    = pTeam->GetTeamIndex();
			rkRoundTeamData.m_szTeamName     = pTeam->GetTeamName();
			rkRoundTeamData.m_Position       = pTeam->GetPosition();
			rkRoundTeamData.m_TourPos        = pTeam->GetTourPos();
			rkRoundTeamData.m_CampPos        = pTeam->GetCampPos();

			// 전체 서버에 전송
			SP2Packet kPacket1( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
			kPacket1 << 2 << pDropTeam->GetTeamIndex() << pDropTeam->GetPosition() << pDropTeam->GetTourPos() << false
				<< pTeam->GetTeamIndex() << pTeam->GetPosition() << pTeam->GetTourPos() << false;
			g_ServerNodeManager.SendMessageAllNode( kPacket1 );

			LOG.PrintTimeAndLog( 0, "[대회로그] %s 예비엔트리팀 교체 - 탈락처리팀 :%s(%d), 교체팀 : %s(%d)", __FUNCTION__, 
				pDropTeam->GetTeamName().c_str(), pDropTeam->GetTeamIndex(), pTeam->GetTeamName().c_str(), pTeam->GetTeamIndex() );

			// 예비 엔트리팀 룸 입장
			for(int i = 0;i < (int)m_TournamentBattleList.size();i++)
			{
				TournamentBattleData &rkBattleData = m_TournamentBattleList[i];
				if( rkBattleData.m_dwBlueIndex == pDropTeam->GetTeamIndex() )
				{
					// 블루팀 변경됨
					rkBattleData.m_dwBlueIndex = pTeam->GetTeamIndex();
					rkBattleData.m_szBlueName  = pTeam->GetTeamName();
					rkBattleData.m_BlueCamp    = pTeam->GetCampPos();
					break;
				}
				else if( rkBattleData.m_dwRedIndex == pDropTeam->GetTeamIndex() )
				{
					// 레드팀 변경됨
					rkBattleData.m_dwRedIndex = pTeam->GetTeamIndex();
					rkBattleData.m_szRedName  = pTeam->GetTeamName();
					rkBattleData.m_RedCamp    = pTeam->GetCampPos();
					break;
				}
			}	

			SP2Packet kPacket2( MSTPK_TOURNAMENT_BATTLE_TEAM_CHANGE );
			kPacket2 << GetIndex() << dwBattleRoomIndex << pDropTeam->GetTeamIndex() << pTeam->GetTeamIndex() << pTeam->GetTeamName() << pTeam->GetCampPos();
			g_ServerNodeManager.SendMessageAllNode( kPacket2 );

			LOG.PrintTimeAndLog( 0, "[대회로그] %s 예비엔트리팀 입장 - 탈락처리팀 :%s(%d), 교체팀 : %s(%d)", __FUNCTION__, 
									pDropTeam->GetTeamName().c_str(), pDropTeam->GetTeamIndex(), pTeam->GetTeamName().c_str(), pTeam->GetTeamIndex() );
			
			return;
		}
	}
}

void TournamentNode::TournamentTeamAllocateList( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	int iCurPage, iMaxCount;
	rkPacket >> iCurPage >> iMaxCount;

	if( GetType() == TYPE_REGULAR )
	{
		DWORD dwTeamIdx;
		rkPacket >> dwTeamIdx;

		if( m_RegularCheerTeamList.empty() )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_LIST );
			kPacket << dwUserIndex << GetIndex() << 0 << 0 << 0 << 0 << 0;
			pSender->SendMessage( kPacket );
		}
		else
		{
			int iMaxList = (int)m_RegularCheerTeamList.size();
			int iStartPos = iCurPage * iMaxCount;
			int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_LIST );
			kPacket << dwUserIndex << GetIndex() << iCurPage << iMaxList << iCurSize << GetTotalCheerCount();
			
			//dwTeamIdx팀이 있는 페이지 계산
			int iNodeArrayIdx = GetTeamNodeArrayIndex( GetTeamNode( dwTeamIdx ) );
			int iMyTeamPage = -1;
			if( 0 < iNodeArrayIdx )
			{
				iMyTeamPage = iNodeArrayIdx / iMaxCount;
			}
			kPacket << iMyTeamPage;

			for( int i = iStartPos; i < iStartPos + iCurSize; i++ )
			{
				TournamentTeamNode *pTeamNode = GetTeamNodeByCheerArray( i );
				if( pTeamNode == NULL )
				{
					kPacket << 0 << 0 << 0 << "";
				}
				else
				{
					kPacket << pTeamNode->GetTeamIndex() << pTeamNode->GetCampPos() << pTeamNode->GetCheerPoint() << pTeamNode->GetTeamName();
				}
			}
			pSender->SendMessage( kPacket );			
		}
	}
	else
	{
		int iMaxList = (int)m_TournamentTeamList.size();
		if( iMaxList == 0 )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_LIST );
			kPacket << dwUserIndex << GetIndex() << 0 << 0;
			pSender->SendMessage( kPacket );
		}
		else
		{
			int iStartPos = iCurPage * iMaxCount;
			int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );
			
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_LIST );
			kPacket << dwUserIndex << GetIndex() << iCurPage << iMaxList << iCurSize;
			for(int i = iStartPos; i < iStartPos + iCurSize;i++)
			{
				TournamentTeamNode *pTeamNode = GetTeamNodeByArray( i );
				if( pTeamNode == NULL )
				{
					kPacket << 0 << "";
				}
				else
				{
					kPacket << pTeamNode->GetTeamIndex() << pTeamNode->GetTeamName();
				}
			}
			pSender->SendMessage( kPacket );			
		}
	}
}

void TournamentNode::_TeamAllocateRandomCreate( DWORDVec &rkTeamList )
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;
		if( pTeam->GetPosition() != 0 ) continue;

		rkTeamList.push_back( pTeam->GetTeamIndex() );
	}

	if (!rkTeamList.empty())
	{
		std::random_device kRandom;
		std::mt19937 kGenerator(kRandom());
		std::shuffle(rkTeamList.begin(), rkTeamList.end(), kGenerator);
	}
}

void TournamentNode::TournamentTeamAllocateData( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	bool bRemainingTeamRand;
	rkPacket >> bRemainingTeamRand;            // 남은팀 랜덤 여부

	int i = 0;
	int iMaxTeam;
	rkPacket >> iMaxTeam;
		
	// 1라운드
	BYTE TourPos = 1;
	TournamentRoundData &rkRoundData = GetTournamentRound( TourPos );
	rkRoundData.m_TeamList.clear();

	// 주최자가 세팅한 팀
	for(i = 0;i < iMaxTeam;i++)
	{
		//
		SHORT Position;
		DWORD dwTeamIndex;
		rkPacket >> dwTeamIndex >> Position;

		TournamentTeamNode *pTeam = GetTeamNode( dwTeamIndex );
		if( pTeam == NULL )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] TournamentNode::TournamentTeamAllocateData None Team Index : %d - %d", GetIndex(), dwTeamIndex );
		}
		else
		{
			pTeam->SetPosition( Position );
			pTeam->SetTourPos( TourPos );              
			pTeam->SetStartPosition( pTeam->GetPosition() );

			RoundTeamData kTeamData;
			kTeamData.m_dwTeamIndex = pTeam->GetTeamIndex();
			kTeamData.m_szTeamName  = pTeam->GetTeamName();
			kTeamData.m_Position    = pTeam->GetPosition();
			kTeamData.m_TourPos     = pTeam->GetTourPos();
			kTeamData.m_CampPos     = pTeam->GetCampPos();
			rkRoundData.m_TeamList.push_back( kTeamData );
		}
	}

	if( bRemainingTeamRand )
	{
		// 랜덤 세팅
		int iRandTeamArray = 0;
		DWORDVec BattleTeamList;
		_TeamAllocateRandomCreate( BattleTeamList );
		for(i = 1;i < (int)m_dwTournamentTeamCount + 1;i++)
		{
			if( IsTournamentRoundTeam( rkRoundData, i ) ) continue;

			if( iRandTeamArray >= (int)BattleTeamList.size() )
				break;

			DWORD dwTeamIndex = BattleTeamList[iRandTeamArray++];
			TournamentTeamNode *pTeam = GetTeamNode( dwTeamIndex );
			if( pTeam == NULL )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] TournamentNode::TournamentTeamAllocateData None Rand Team Index : %d - %d", GetIndex(), dwTeamIndex );
			}
			else
			{
				pTeam->SetPosition( i );
				pTeam->SetTourPos( TourPos );    
				pTeam->SetStartPosition( pTeam->GetPosition() );          

				RoundTeamData kTeamData;
				kTeamData.m_dwTeamIndex = pTeam->GetTeamIndex();
				kTeamData.m_szTeamName  = pTeam->GetTeamName();
				kTeamData.m_Position    = pTeam->GetPosition();
				kTeamData.m_TourPos     = pTeam->GetTourPos();
				kTeamData.m_CampPos     = pTeam->GetCampPos();
				rkRoundData.m_TeamList.push_back( kTeamData );
			}
		}
		BattleTeamList.clear();
	}

	// 게임서버에 전송 - 
	iMaxTeam = (int)rkRoundData.m_TeamList.size();
	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
	kPacket << iMaxTeam;
	for(i = 0;i < iMaxTeam;i++)
	{
		RoundTeamData &rkTeam = rkRoundData.m_TeamList[i];
		kPacket << rkTeam.m_dwTeamIndex << rkTeam.m_Position << rkTeam.m_TourPos << false;            // 정보 유저에게 전송하지 않음
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	// 주최자에게 결과 전송
	SP2Packet kUserPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_DATA );
	kUserPacket << dwUserIndex << TOURNAMENT_TEAM_ALLOCATE_OK << GetIndex();
	pSender->SendMessage( kUserPacket );

	LOG.PrintTimeAndLog( 0, "[대회로그] TournamentNode::TournamentTeamAllocateData Start : %d - %d - %d = %d회전", GetIndex(), (int)GetState(), (int)rkRoundData.m_TeamList.size(), (int)TourPos );
}

void TournamentNode::SendTournamentTotalTeamList( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	int iCurPage, iMaxCount;
	rkPacket >> iCurPage >> iMaxCount;

	int iMaxList = (int)m_TournamentTeamList.size();
	if( iMaxList == 0 )
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_TOTAL_TEAM_LIST );
		kPacket << dwUserIndex << GetIndex() << 0 << 0 << 0;
		pSender->SendMessage( kPacket );
	}
	else
	{
		int iStartPos = iCurPage * iMaxCount;
		int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

		SP2Packet kPacket( MSTPK_TOURNAMENT_TOTAL_TEAM_LIST );
		kPacket << dwUserIndex << GetIndex() << iCurPage << iMaxList << iCurSize;
		for(int i = iStartPos; i < iStartPos + iCurSize;i++)
		{
			TournamentTeamNode *pTeamNode = GetTeamNodeByArray( i );
			if( pTeamNode == NULL )
			{
				kPacket << 0 << "";
			}
			else
			{
				kPacket << pTeamNode->GetTeamIndex() << pTeamNode->GetTeamName();
			}
		}
		pSender->SendMessage( kPacket );

		if( GetType() == TYPE_REGULAR ) 
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] SendTournamentTotalTeamList : %d - %d - %d - %d - %d", GetIndex(), dwUserIndex, iMaxList, iCurSize, iCurPage );
		}
	}

}

void TournamentNode::TournamentCustomStateStart( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	if( GetOwnerIndex() != dwUserIndex )
	{
		// 주최자가 아니다.
		SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_STATE_START );
		kPacket << dwUserIndex << TOURNAMENT_CUSTOM_STATE_START_FAILED << GetIndex();
		pSender->SendMessage( kPacket );
	}
	else
	{
		int iNowStateArray = (int)GetState() - 1;
		if( COMPARE( iNowStateArray, 0, (int)m_StateDate.size() ) )
		{
			CTime kCurTime = CTime::GetCurrentTime();
			m_StateDate[iNowStateArray].m_dwEndDate = Help::ConvertCTimeToDate( kCurTime );
			if( COMPARE( iNowStateArray + 1, 0, (int)m_StateDate.size() ) )
			{
				m_StateDate[iNowStateArray + 1].m_dwStartDate = m_StateDate[iNowStateArray].m_dwEndDate;
			}
						
			// 저장 요청
			if( GetState() == STATE_TEAM_APP || GetState() == STATE_TEAM_DELAY )
			{
				m_CustomInfo.m_bInfoDataChange = true;
			}
			else
			{
				m_CustomInfo.m_bRoundDataChange = true;
			}

			// 상태를 변경한다
			StateProcess();

			// 주최자에게 전송
			SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_STATE_START );
			kPacket << dwUserIndex << TOURNAMENT_CUSTOM_STATE_START_OK << GetIndex() << GetState()
					<< m_StateDate[iNowStateArray].m_dwStartDate << m_StateDate[iNowStateArray].m_dwEndDate;
			pSender->SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "TournamentCustomStateStart : %d - %d - %d - %d - %d", GetIndex(), dwUserIndex, iNowStateArray, m_StateDate[iNowStateArray].m_dwStartDate, m_StateDate[iNowStateArray].m_dwEndDate );
		}
		else
		{
			// 시간 세팅 가능한 상태가 아니다.
			SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_STATE_START );
			kPacket << dwUserIndex << TOURNAMENT_CUSTOM_STATE_START_FAILED << GetIndex();
			pSender->SendMessage( kPacket );
		}
	}
}

void TournamentNode::SendTournamentCustomRewardList( ServerNode *pSender, DWORD dwUserIndex )
{
	if( pSender == NULL ) return;

	SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_REWARD_LIST );
	kPacket << dwUserIndex << GetIndex() << TOURNAMENT_CUSTOM_MAX_ROUND;
	for(int i = 0;i < TOURNAMENT_CUSTOM_MAX_ROUND;i++)
	{
		kPacket << m_CustomInfo.m_CustomReward[i].m_dwRewardA;
		kPacket << m_CustomInfo.m_CustomReward[i].m_dwRewardB;
		kPacket << m_CustomInfo.m_CustomReward[i].m_dwRewardC;
		kPacket << m_CustomInfo.m_CustomReward[i].m_dwRewardD;
	}
	pSender->SendMessage( kPacket );
}

void TournamentNode::ApplyTournamentCustomRewardRegCheck( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	if( GetOwnerIndex() != dwUserIndex )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_REWARD_REG_CHECK );
		kPacket << dwUserIndex << TOURNAMENT_CUSTOM_REWARD_REG_NOT_OWNER << GetIndex();
		pSender->SendMessage( kPacket );		
	}
	else
	{
		BYTE TourPos;
		rkPacket >> TourPos;

		int iRewardSize;
		rkPacket >> iRewardSize;

		int i = 0;
		DWORDVec vRewardEtcType;
		for(i = 0;i < iRewardSize;i++)
		{
			DWORD dwEtcType;
			rkPacket >> dwEtcType;
			if( dwEtcType == 0 ) continue;

			vRewardEtcType.push_back( dwEtcType );
		}

		if( (int)vRewardEtcType.size() <= GetCustomRewardEmptySlot( TourPos ) )
		{
			int iRewardTotalCount = max( 1,( Help::TournamentCurrentRoundWithTeam( m_dwTournamentTeamCount, TourPos - 1 ) / 2 ) ) * m_MaxPlayer;

			SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_REWARD_REG_CHECK );
			kPacket << dwUserIndex << TOURNAMENT_CUSTOM_REWARD_REG_CHECK_OK << GetIndex();
			kPacket << TourPos << iRewardTotalCount << (int)vRewardEtcType.size();
			for(i = 0;i < (int)vRewardEtcType.size();i++)
			{
				kPacket << vRewardEtcType[i];
			}
			pSender->SendMessage( kPacket );
			vRewardEtcType.clear();
			LOG.PrintTimeAndLog( 0, "ApplyTournamentCustomRewardRegCheck : %d - %d - %d - %d", GetIndex(), (int)TourPos, iRewardSize, iRewardTotalCount );
		}
		else
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_REWARD_REG_CHECK );
			kPacket << dwUserIndex << TOURNAMENT_CUSTOM_REWARD_REG_EMPTY_SLOT << GetIndex();
			pSender->SendMessage( kPacket );
		}
		vRewardEtcType.clear();
	}
}

void TournamentNode::ApplyTournamentCustomRewardRegUpdate( SP2Packet &rkPacket )
{
	if( GetType() != TYPE_CUSTOM ) return;
	
	ioHashString kOwnerID;
	rkPacket >> kOwnerID;

	BYTE TourPos;
	rkPacket >> TourPos;

	if( !COMPARE( TourPos - 1, 0, TOURNAMENT_CUSTOM_MAX_ROUND ) )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentCustomRewardRegUpdate Error : %d - %d", GetIndex(), (int)TourPos );
		return;	
	}		

	CustomRewardInfo &rkCustomReward = m_CustomInfo.m_CustomReward[ TourPos - 1 ];
	LOG.PrintTimeAndLog( 0, "ApplyTournamentCustomRewardRegUpdate Prev : %d - %d : %d - %d - %d - %d", GetIndex(), (int)TourPos, rkCustomReward.m_dwRewardA, rkCustomReward.m_dwRewardB, rkCustomReward.m_dwRewardC, rkCustomReward.m_dwRewardD );

	int iRewardSize;
	rkPacket >> iRewardSize;

	int i = 0;
	for(i = 0;i < iRewardSize;i++)
	{
		DWORD dwEtcType;
		rkPacket >> dwEtcType;
		if( dwEtcType == 0 ) continue;		

		if( rkCustomReward.m_dwRewardA == 0 )
			rkCustomReward.m_dwRewardA = dwEtcType;
		else if( rkCustomReward.m_dwRewardB == 0 )
			rkCustomReward.m_dwRewardB = dwEtcType;
		else if( rkCustomReward.m_dwRewardC == 0 )
			rkCustomReward.m_dwRewardC = dwEtcType;
		else if( rkCustomReward.m_dwRewardD == 0 )
			rkCustomReward.m_dwRewardD = dwEtcType;			
	}
	m_CustomInfo.m_bRoundDataChange = true;
	LOG.PrintTimeAndLog( 0, "ApplyTournamentCustomRewardRegUpdate Next : %d - %d : %d - %d - %d - %d", GetIndex(), (int)TourPos, rkCustomReward.m_dwRewardA, rkCustomReward.m_dwRewardB, rkCustomReward.m_dwRewardC, rkCustomReward.m_dwRewardD );
	g_LogDBClient.OnInsertTournamentRewardSet( GetIndex(), kOwnerID, GetOwnerIndex(), TourPos, rkCustomReward.m_dwRewardA, rkCustomReward.m_dwRewardB, rkCustomReward.m_dwRewardC, rkCustomReward.m_dwRewardD );
}

void TournamentNode::ApplyTournamentCheerDecision( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwTeamIndex;
	rkPacket >> dwTeamIndex;

	TournamentTeamNode* pNode = GetTeamNode( dwTeamIndex );
	if( pNode )
	{
		pNode->IncreaseCheerPoint();

		SP2Packet kPacket( MSTPK_TOURNAMENT_CHEER_DECISION );
		kPacket << TOURNAMENT_CHEER_DECISION_OK;
		kPacket << dwUserIndex;
		kPacket << GetIndex();
		kPacket << dwTeamIndex;
		kPacket << pNode->GetCheerPoint();
		g_ServerNodeManager.SendMessageAllNode( kPacket );
	}
	else
	{
		//여기에 진입 할 수 없음
		LOG.PrintTimeAndLog(0, "%s - not find team",  __FUNCTION__ );
	}
}

bool TournamentNode::IsTournamentJoinConfirm( DWORD dwUserIndex )
{
	ConfirmUserDataVec::iterator iter = m_CustomConfirmUserList.begin();
	for(;iter != m_CustomConfirmUserList.end();iter++)
	{
		ConfirmUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == dwUserIndex )
		{
			return true;
		}
	}
	return false;
}

void TournamentNode::TournamentJoinConfirmReg( SP2Packet &rkPacket )
{
	DWORD dwOwnerIndex;
	rkPacket >> dwOwnerIndex;

	if( GetOwnerIndex() != dwOwnerIndex )
	{
		LOG.PrintTimeAndLog( 0, "TournamentJoinConfirmReg Owner Index Error : %d - %d - %d", GetIndex(), GetOwnerIndex(), dwOwnerIndex );
	}
	else
	{
		ConfirmUserData kUserData;
		rkPacket >> kUserData.m_dwUserIndex >> kUserData.m_szNickName >> kUserData.m_iGradeLevel;
		if( !IsTournamentJoinConfirm( kUserData.m_dwUserIndex ) )
		{
			m_CustomConfirmUserList.push_back( kUserData );
			g_DBClient.OnInsertTournamentConfirmUser( GetIndex(), kUserData.m_dwUserIndex );
		}
	}
}

bool TournamentNode::ChangeAnnounce( DWORD dwUserIndex, ioHashString szAnnounce )
{
	if( GetOwnerIndex() != dwUserIndex )
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::ChangeAnnounce Not Owner : %d - %d - %d", GetIndex(), GetOwnerIndex(), dwUserIndex );	
		return false;
	}
	else if( GetType() != TYPE_CUSTOM )
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::ChangeAnnounce Not Custom Tournament : %d - %d", GetIndex(), dwUserIndex );	
		return false;
	}		

	m_CustomInfo.m_szAnnounce      = szAnnounce;
	m_CustomInfo.m_bInfoDataChange = true;    // 저장
	LOG.PrintTimeAndLog( 0, "TournamentNode::ChangeAnnounce : %d - %d - ", GetIndex(), dwUserIndex, szAnnounce.c_str() );
	return true;
}

void TournamentNode::SendEndProcess()
{
	SP2Packet kPacket( MSTPK_TOURNAMENT_END_PROCESS );
	kPacket << GetIndex() << GetType();
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void TournamentNode::SendRoundTeamData( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray, SP2Packet &rkPacket )
{
	int iTeamCount = iRoundTeamCount;
	for(int i = iStartRound;i < iStartRound + iTotalRoundCount;i++)
	{
		TournamentRoundData &rkRoundData = GetTournamentRound( i );

		int iStartArray = iTeamCount * iStartRountTeamArray;
		for(int j = iStartArray;j < iStartArray + iTeamCount;j++)
		{
			RoundTeamData kRoundTeamData = GetTournamentRoundTeam( rkRoundData, j + 1 );
			if( kRoundTeamData.m_dwTeamIndex == 0 )
				rkPacket << false;
			else
			{
				rkPacket << true << kRoundTeamData.m_dwTeamIndex << kRoundTeamData.m_szTeamName << kRoundTeamData.m_CampPos;
			}
		}
		iTeamCount /= 2;
	}
}

void TournamentNode::ApplyTournamentBattleResult( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	BYTE NextTourPos;
	DWORD dwWinTeamIndex, dwLoseTeamIndex;
	rkPacket >> dwWinTeamIndex >> dwLoseTeamIndex >> NextTourPos;

	TournamentTeamNode *pWinTeam = GetTeamNode( dwWinTeamIndex );
	if( pWinTeam == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentBattleResult None Win Team : %d - %d", GetIndex(), dwWinTeamIndex );
		return;
	}

	if( pWinTeam->GetTourPos() >= NextTourPos )
	{
		// 치명적인 버그 대회 경기 결과 패킷이 2번 날라온다. + 1 / 2가 2번 발생하여 팀을 덮음.
		LOG.PrintTimeAndLog( 0, "ApplyTournamentBattleResult NextTourPos Already Position : %s - %d - %d - %d - %d", 
			                    pWinTeam->GetTeamName().c_str(), pWinTeam->GetTeamIndex(), 
								(int)pWinTeam->GetTourPos(), (int)NextTourPos, (int)pWinTeam->GetPosition() );
		return;
	}

	TournamentRoundData &rkNextRoundData = GetTournamentRound( NextTourPos );

	pWinTeam->SetTourPos( NextTourPos );
	pWinTeam->SetPosition( ( pWinTeam->GetPosition() + 1 ) / 2 );

	// 다음 라운드
	RoundTeamData kTeamData;
	kTeamData.m_dwTeamIndex = pWinTeam->GetTeamIndex();
	kTeamData.m_szTeamName  = pWinTeam->GetTeamName();
	kTeamData.m_Position    = pWinTeam->GetPosition();
	kTeamData.m_TourPos     = pWinTeam->GetTourPos();
	kTeamData.m_CampPos     = pWinTeam->GetCampPos();

	rkNextRoundData.m_TeamList.push_back( kTeamData );
	
	//
	if( (SHORT)NextTourPos > m_dwTournamentMaxRound )
	{
		StateFinalWinTeamProcess( pWinTeam );
	}

	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
	kPacket << 1 << kTeamData.m_dwTeamIndex << kTeamData.m_Position << kTeamData.m_TourPos << true;         // 정보 유저에게 전송
	g_ServerNodeManager.SendMessageAllNode( kPacket );
	LOG.PrintTimeAndLog( 0, "ApplyTournamentBattleResult : Round : %d - WinTeam : %d - LoseTeam : %d", (int)NextTourPos, dwWinTeamIndex, dwLoseTeamIndex );
}

TournamentTeamNode *TournamentNode::CreateTeamNode( DWORD dwTeamIndex, ioHashString szTeamName, DWORD dwOwnerIndex, int iLadderPoint, BYTE CampPos )
{
	if( GetTeamNode( dwTeamIndex ) )
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTeamNode Already Team : %d - %d - %s", GetIndex(), dwTeamIndex, szTeamName.c_str() );
		return NULL;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTeamNode : %d - %d - %s", GetIndex(), dwTeamIndex, szTeamName.c_str() );
	}

	TournamentTeamNode *pTeamNode = new TournamentTeamNode( GetIndex(), dwTeamIndex, szTeamName, dwOwnerIndex, iLadderPoint, CampPos, GetMaxPlayer() );
	m_TournamentTeamList.push_back( pTeamNode );

	return pTeamNode;
}

TournamentTeamNode *TournamentNode::CreateTeamNode( DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer, int iCheerPoint, SHORT Position, SHORT StartPosition, BYTE TourPos )
{
	TournamentTeamNode *pTeamNode = GetTeamNode( dwTeamIndex );
	if( pTeamNode == NULL )
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTeamNode : %d - %d - %s", GetIndex(), dwTeamIndex, szTeamName.c_str() );
		pTeamNode = new TournamentTeamNode( GetIndex(), dwTeamIndex, szTeamName, dwOwnerIndex, iLadderPoint, CampPos, MaxPlayer, iCheerPoint, Position, StartPosition, TourPos );
		m_TournamentTeamList.push_back( pTeamNode );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTeamNode DB Already Team : %d - %d - %s", GetIndex(), dwTeamIndex, szTeamName.c_str() );		
	}
	return pTeamNode;
}

void TournamentNode::DeleteTeamNode( DWORD dwTeamIndex )
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;
		if( pTeam->GetTeamIndex() != dwTeamIndex ) continue;

		SAFEDELETE( pTeam );
		m_TournamentTeamList.erase( iter );

		return; 
	}
}

TournamentTeamNode *TournamentNode::GetTeamNode( DWORD dwTeamIndex )
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;
		if( pTeam->GetTeamIndex() == dwTeamIndex )
			return pTeam;	
	}
	return NULL;
}

TournamentTeamNode *TournamentNode::GetTeamNode( SHORT Position, BYTE TourPos )
{
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;
		if( pTeam->GetPosition() == Position && pTeam->GetTourPos() == TourPos )
			return pTeam;	
	}
	return NULL;
}

TournamentTeamNode *TournamentNode::GetTeamNodeByArray( int iArray )
{
	if( COMPARE( iArray, 0, (int)m_TournamentTeamList.size() ) )
		return m_TournamentTeamList[iArray];
	return NULL;
}

TournamentTeamNode *TournamentNode::GetTeamNodeByCheerArray( int iArray )
{
	if( COMPARE( iArray, 0, (int)m_RegularCheerTeamList.size() ) )
		return m_RegularCheerTeamList[iArray];
	return NULL;
}

int TournamentNode::GetTeamNodeArrayIndex( TournamentTeamNode* pNode )
{
	if( pNode == NULL )
		return -1;

	TournamentTeamVec::iterator iter = m_RegularCheerTeamList.begin();
	for( int i = 0; iter != m_RegularCheerTeamList.end(); ++iter, ++i )
	{
		if( *iter == pNode )
		{
			return i;
		}
	}
	return -1;
}

void TournamentNode::ApplyTournamentInfoDB( CQueryResultData *pQueryData )
{
	m_StateDate.clear();

	if( GetType() == TYPE_CUSTOM )
	{
		// - 대회 정보 인덱스 : int / 리그명 : nvarchar(20) / 맥스라운드 : smallint / 배너1 : int	/ 배너2 : int / 모드타입 : int / 팀최대인원 : tinyint / 대회실행타입 : tinyint / 공지 : nvarchar(512) / 모집 기간 / 대기 기간
		DWORD dwAppDate=0, dwDelayDate=0;
		char  szTournamentTitle[TOURNAMENT_NAME_NUM_PLUS_ONE] = "";
		char  szTournamentAnnounce[TOURNAMENT_ANNOUNCE_NUM_PLUS_ONE] = "";

		if(!pQueryData->GetValue( m_CustomInfo.m_dwInfoIndex ))			return;
		if(!pQueryData->GetValue( szTournamentTitle, TOURNAMENT_NAME_NUM_PLUS_ONE ))		return;
		if(!pQueryData->GetValue( m_dwTournamentMaxRound ))				return;
		if(!pQueryData->GetValue( m_CustomInfo.m_dwBannerBig ))			return;
		if(!pQueryData->GetValue( m_CustomInfo.m_dwBannerSmall ))			return;
		if(!pQueryData->GetValue( m_iPlayMode ))								return;
		if(!pQueryData->GetValue( m_MaxPlayer ))							return;
		if(!pQueryData->GetValue( m_CustomInfo.m_TournamentMethod ))		return;
		if(!pQueryData->GetValue( szTournamentAnnounce, TOURNAMENT_ANNOUNCE_NUM_PLUS_ONE ))	return;
		if(!pQueryData->GetValue( dwAppDate ))								return;
		if(!pQueryData->GetValue( dwDelayDate ))							return;

		m_CustomInfo.m_szTitle		= szTournamentTitle;
		m_CustomInfo.m_szAnnounce	= szTournamentAnnounce;		
		m_dwTournamentTeamCount		= (DWORD)pow( 2, (double)m_dwTournamentMaxRound );
		LOG.PrintTimeAndLog( 0, "ApplyTournamentInfoDB : %d - %d - %s - %d - %d - %d - %d - %d - %d - %d - %s - %d - %d", 
								GetIndex(), m_CustomInfo.m_dwInfoIndex, m_CustomInfo.m_szTitle.c_str(), m_dwTournamentMaxRound, m_dwTournamentTeamCount, m_CustomInfo.m_dwBannerBig, m_CustomInfo.m_dwBannerSmall,
								m_iPlayMode, (int)m_MaxPlayer, (int)m_CustomInfo.m_TournamentMethod, m_CustomInfo.m_szAnnounce.c_str(), dwAppDate, dwDelayDate );

		StateDate kStateDate;

		// 모집 기간
		kStateDate.m_dwStartDate = m_dwStartDate;
		kStateDate.m_dwEndDate   = dwAppDate;
		m_StateDate.push_back( kStateDate );

		// 대기 기간
		kStateDate.m_dwStartDate = dwAppDate;
		kStateDate.m_dwEndDate   = dwDelayDate;
		m_StateDate.push_back( kStateDate );

		// 라운드 정보 요청
		g_DBClient.OnSelectTournamentCustomRound( GetIndex(), m_CustomInfo.m_dwInfoIndex );

		// 수동 대회면 참여 가능 리스트 요청
		if( m_CustomInfo.m_TournamentMethod == CPT_OFFLINE )
		{
			g_DBClient.OnSelectTournamentConfirmUserList( GetIndex(), 0, TOURNAMENT_CUSTOM_CONFIRM_USER_LIST_COUNT );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::ApplyTournamentInfoDB : None Type : %d", GetIndex() );
	}
}

void TournamentNode::ApplyTournamentRoundDB( CQueryResultData *pQueryData )
{
	if( GetType() == TYPE_CUSTOM )
	{
		//  - 라운드인덱스 : int / 라운드1시간 : int / 라운드1선물1 : int / 라운드1선물2 : int / 라운드1선물3 : int / 라운드1선물4 : int x TOURNAMENT_CUSTOM_MAX_ROUND
		DWORD dwStartDate = GetRoundEndDate( STATE_TEAM_APP );
	
		if(!pQueryData->GetValue( m_CustomInfo.m_dwRoundIndex )) return;

		for(int i = 0;i < TOURNAMENT_CUSTOM_MAX_ROUND;i++)
		{
			DWORD dwTourRoundDate = 0;
			if(!pQueryData->GetValue( dwTourRoundDate ))								break;
			if(!pQueryData->GetValue( m_CustomInfo.m_CustomReward[i].m_dwRewardA ))		break;
			if(!pQueryData->GetValue( m_CustomInfo.m_CustomReward[i].m_dwRewardB ))		break;
			if(!pQueryData->GetValue( m_CustomInfo.m_CustomReward[i].m_dwRewardC ))		break;
			if(!pQueryData->GetValue( m_CustomInfo.m_CustomReward[i].m_dwRewardD ))		break;
			
			if( i < (int)m_dwTournamentMaxRound )
			{
				// 라운드 시간
				StateDate kStateDate;
				kStateDate.m_dwStartDate = dwStartDate;
				kStateDate.m_dwEndDate   = dwTourRoundDate;
				m_StateDate.push_back( kStateDate );

				dwStartDate = dwTourRoundDate;

				LOG.PrintTimeAndLog( 0, "ApplyTournamentRoundDB : %d - %d - %d - %d - %d - %d - %d", 
										GetIndex(), m_CustomInfo.m_dwRoundIndex, dwTourRoundDate, m_CustomInfo.m_CustomReward[i].m_dwRewardA, 
										m_CustomInfo.m_CustomReward[i].m_dwRewardB, m_CustomInfo.m_CustomReward[i].m_dwRewardC, m_CustomInfo.m_CustomReward[i].m_dwRewardD );
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::ApplyTournamentRoundDB : None Type : %d", GetIndex() );
	}
}

void TournamentNode::ApplyTournamentConfirmUserListDB( CQueryResultData *pQueryData )
{
	if( pQueryData == NULL )
		return;

	int iCount = 0;
	DWORD dwLastIndex = 0;
	while( pQueryData->IsExist() )
	{
		ConfirmUserData kUserData;
		
		// - 유저인덱스, 닉네임, 레벨
		char szUserNick[ID_NUM_PLUS_ONE] = "";
		if(!pQueryData->GetValue( kUserData.m_dwUserIndex )) break;
		if(!pQueryData->GetValue( szUserNick, ID_NUM_PLUS_ONE ))			break;
		if(!pQueryData->GetValue( kUserData.m_iGradeLevel ))	break;

		kUserData.m_szNickName = szUserNick;
		dwLastIndex = kUserData.m_dwUserIndex;
		m_CustomConfirmUserList.push_back( kUserData );

		iCount++;
	}

	if( dwLastIndex == 0 )
		return;

	if( iCount >= TOURNAMENT_CUSTOM_CONFIRM_USER_LIST_COUNT )
	{
		g_DBClient.OnSelectTournamentConfirmUserList( GetIndex(), dwLastIndex, TOURNAMENT_CUSTOM_CONFIRM_USER_LIST_COUNT );
	}	
}

void TournamentNode::ApplyTournamentPrevChampInfoDB( CQueryResultData *pQueryData )
{
	char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";
	BYTE CampPos;

	pQueryData->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE );
	pQueryData->GetValue( CampPos );
		
	m_RegularInfo.m_PrevChampTeamName = szTeamName;
	m_RegularInfo.m_PrevChampTeamCamp = CampPos;

	LOG.PrintTimeAndLog(0, "%s - %d, %s", __FUNCTION__, CampPos, szTeamName );
}

void TournamentNode::CreateTournamentRoundHistory()
{
	LOG.PrintTimeAndLog(0, "%s - m_TournamentTeamList.size() : %d", __FUNCTION__, m_TournamentTeamList.size() );

	bool bHistorySet = false;
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;
		if( pTeam->GetStartPosition() == 0 ) continue;

		SHORT Position = pTeam->GetStartPosition();
		for(BYTE TourPos = 0;TourPos < pTeam->GetTourPos();TourPos++)
		{				
			RoundTeamData kTeamData;
			kTeamData.m_dwTeamIndex = pTeam->GetTeamIndex();
			kTeamData.m_szTeamName  = pTeam->GetTeamName();
			kTeamData.m_Position    = Position;
			kTeamData.m_TourPos     = TourPos + 1;
			kTeamData.m_CampPos     = pTeam->GetCampPos();

			TournamentRoundData &rkRoundData = GetTournamentRound( kTeamData.m_TourPos );
			RoundTeamData &rkRoundTeamData = GetTournamentRoundTeam( rkRoundData, kTeamData.m_Position );
			if( rkRoundTeamData.m_dwTeamIndex == 0 )
			{
				rkRoundData.m_TeamList.push_back( kTeamData );
			}
			else
			{
				rkRoundTeamData.m_dwTeamIndex = kTeamData.m_dwTeamIndex;
				rkRoundTeamData.m_szTeamName  = kTeamData.m_szTeamName;
				rkRoundTeamData.m_Position    = kTeamData.m_Position;
				rkRoundTeamData.m_TourPos     = kTeamData.m_TourPos;
				rkRoundTeamData.m_CampPos     = kTeamData.m_CampPos;
			}
			Position = ( Position + 1 ) / 2;
		}
		bHistorySet = true;
	}

	if( bHistorySet )
	{
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTournamentRoundHistory Create DB Set %d: %d", GetIndex(), (int)GetState() );
	}
	else
	{
		switch( GetType() )
		{
		case TYPE_REGULAR:
			{
				// 팀 모집 종료 후 대기 상태에서 서버가 재부팅되면 리그 라운드를 생성한다.
				if( GetState() == STATE_TEAM_DELAY )
				{
					CreateNewTournamentRoundRegular();
					LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTournamentRoundHistory Create State Set %d : %d", GetIndex(), (int)GetState() );
				}
			}
			break;
		case TYPE_CUSTOM:
			{
				// 1라운드가 시작되었는데 서버가 재부팅되면 리그 라운드를 생성한다.
				if( GetState() == STATE_TOURNAMENT )
				{
					CreateNewTournamentRoundCustom();
					LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTournamentRoundHistory Create State Set %d : %d", GetIndex(), (int)GetState() );
				}
			}
			break;
		}
	}

	// 로그
	TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.begin();
	for(;iCreator != m_TournamentRoundMap.end();++iCreator)
	{
		TournamentRoundData &rkRoundData = iCreator->second;
		LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTournamentRoundHistory Load : %d - %d - %d = %d회전", GetIndex(), (int)GetState(), (int)rkRoundData.m_TeamList.size(), (int)iCreator->first );
	}
}

bool TournamentNode::CreateNewTournamentRoundRegular()
{
	if( m_TournamentTeamList.size() > 1 )
	{
		std::sort( m_TournamentTeamList.begin(), m_TournamentTeamList.end(), TournamentTeamSort() );
	}

	// 첫 시작 대진표 - 첫 시작에서 상위 50%씩 나눠서 블루 & 레드로 분류
	BYTE  TourPos  = 1;
	TournamentRoundData &rkRoundData = GetTournamentRound( TourPos );
	rkRoundData.m_TeamList.clear();

	enum { MAX_CAMP = 2, };
	int i = 0;           // 양 진영에 대해서 반씩 나눠서 시작
	int iMaxTeam = m_dwTournamentTeamCount / 2;		
	for(i = 0;i < MAX_CAMP;i++)
	{
		int iTeamCount = 0;
		SHORT Position = ( iMaxTeam * i ) + 1;
		TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
		for(;iter != m_TournamentTeamList.end();++iter)
		{
			TournamentTeamNode *pTeam = *iter;
			if( pTeam && pTeam->GetCampPos() == (BYTE)i + 1 )     
			{
				pTeam->SetPosition( Position++ );
				pTeam->SetTourPos( TourPos );               // 시작값이 1이다. 256강 안에 들어있다는 뜻!!!!
				pTeam->SetStartPosition( pTeam->GetPosition() );

				RoundTeamData kTeamData;
				kTeamData.m_dwTeamIndex = pTeam->GetTeamIndex();
				kTeamData.m_szTeamName  = pTeam->GetTeamName();
				kTeamData.m_Position    = pTeam->GetPosition();
				kTeamData.m_TourPos     = pTeam->GetTourPos();
				kTeamData.m_CampPos     = pTeam->GetCampPos();
				rkRoundData.m_TeamList.push_back( kTeamData );

				iTeamCount++;
			}

			if( iTeamCount >= iMaxTeam )
				break;
		}
		LOG.PrintTimeAndLog( 0, "[대회로그] TournamentNode::CreateTournamentRoundRegular Start %d Camp : %d - %d - %d = %d회전", i + 1, GetIndex(), (int)GetState(), (int)rkRoundData.m_TeamList.size(), (int)TourPos );
	}

	// 게임서버에 전송 - 
	iMaxTeam = (int)rkRoundData.m_TeamList.size();
	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
	kPacket << iMaxTeam;
	for(i = 0;i < iMaxTeam;i++)
	{
		RoundTeamData &rkTeam = rkRoundData.m_TeamList[i];
		kPacket << rkTeam.m_dwTeamIndex << rkTeam.m_Position << rkTeam.m_TourPos << false;            // 정보 유저에게 전송하지 않음
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );
	return true;
}

bool TournamentNode::CreateNewTournamentRoundCustom()
{
	BYTE TourPos = 1;
	TournamentRoundData &rkRoundData = GetTournamentRound( TourPos );
	if( !rkRoundData.m_TeamList.empty() )
		return false;        // 이미 배정이 되었다.

	// 랜덤 배정 및 1라운드 세팅
	if( m_TournamentTeamList.size() > 1 )
	{
		std::random_device kRandom;
		std::mt19937 kGenerator(kRandom());
		std::shuffle(m_TournamentTeamList.begin(), m_TournamentTeamList.end(), kGenerator);
	}

	SHORT Position = 1;
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;

		pTeam->SetPosition( Position++ );
		pTeam->SetTourPos( TourPos );               // 시작값이 1이다. 256강 안에 들어있다는 뜻!!!!
		pTeam->SetStartPosition( pTeam->GetPosition() );

		RoundTeamData kTeamData;
		kTeamData.m_dwTeamIndex = pTeam->GetTeamIndex();
		kTeamData.m_szTeamName  = pTeam->GetTeamName();
		kTeamData.m_Position    = pTeam->GetPosition();
		kTeamData.m_TourPos     = pTeam->GetTourPos();
		kTeamData.m_CampPos     = pTeam->GetCampPos();
		rkRoundData.m_TeamList.push_back( kTeamData );

		if( Position > (int)m_dwTournamentTeamCount )
			break;
	}
	LOG.PrintTimeAndLog( 0, "TournamentNode::CreateTournamentRoundCustom Start : %d - %d - %d = %d회전", GetIndex(), (int)GetState(), (int)rkRoundData.m_TeamList.size(), (int)TourPos );

	// 게임서버에 전송 - 
	int iMaxTeam = (int)rkRoundData.m_TeamList.size();
	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_POSITION_SYNC );
	kPacket << iMaxTeam;
	for(int i = 0;i < iMaxTeam;i++)
	{
		RoundTeamData &rkTeam = rkRoundData.m_TeamList[i];
		kPacket << rkTeam.m_dwTeamIndex << rkTeam.m_Position << rkTeam.m_TourPos << false;            // 정보 유저에게 전송하지 않음
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );
	return true;
}

void TournamentNode::CreateNewRegularCheerTeamList()
{
	if( m_TournamentTeamList.empty() )
	{
		LOG.PrintTimeAndLog( 0 ,"%s - Empty", __FUNCTION__ );
		return;
	}

	if( GetState() == STATE_TEAM_APP )
	{
		return;
	}

	//첫라운드 기준으로 작성
	TournamentRoundData &rkRoundData = GetTournamentRound( 1 );
	TournamentTeamVec vBlue;
	TournamentTeamVec vRed;

	RoundTeamVec::iterator iter = rkRoundData.m_TeamList.begin();
	for(;iter != rkRoundData.m_TeamList.end();++iter )
	{
		RoundTeamData& rkTeam = *iter;
		TournamentTeamNode *pTeam = GetTeamNode( rkTeam.m_dwTeamIndex );

		if( pTeam == NULL ) continue;		
		if( pTeam->GetCampPos() == CAMP_BLUE )
		{
			vBlue.push_back( pTeam );
		}
		else
		{
			vRed.push_back( pTeam );
		}
	}
	m_RegularCheerTeamList.clear();

	unsigned int iBlueIdx = 0;
	unsigned int iRedIdx  = 0;
	unsigned int iCampPos = CAMP_BLUE;

	while( true )
	{
		if( iCampPos == CAMP_BLUE )
		{
			if( iBlueIdx < vBlue.size() )
			{
				m_RegularCheerTeamList.push_back( vBlue[iBlueIdx++] );
			}
			iCampPos = CAMP_RED;
		}
		else if( iCampPos == CAMP_RED )
		{
			if( iRedIdx < vRed.size() )
			{
				m_RegularCheerTeamList.push_back( vRed[iRedIdx++] );
			}
			iCampPos = CAMP_BLUE;
		}

		if( vBlue.size() <= iBlueIdx && vRed.size() <= iRedIdx )
			break;
	}

	LOG.PrintTimeAndLog( 0 ,"[대회로그] %s - Create OK : %d", __FUNCTION__, m_RegularCheerTeamList.size() );
}

DWORD TournamentNode::GetTotalCheerCount()
{
	if( m_TournamentTeamList.empty() )
		return 0;

	DWORD dwCheerCount = 0;
	TournamentTeamVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TournamentTeamNode *pTeam = *iter;
		if( pTeam == NULL ) continue;

		dwCheerCount += pTeam->GetCheerPoint();
	}
	return dwCheerCount;
}

void TournamentNode::TestSetState(BYTE state)
{
	SetState(state);
}

void TournamentNode::TestRoundStartProcess()
{
	StateRoundStartProcess();
}