#include "stdafx.h"

#include "ioGameAnnMgr.h"
#include "ioLadderTeamMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"

template<> ioGameAnnMgr* Singleton< ioGameAnnMgr >::ms_Singleton = 0;

ioGameAnnMgr::ioGameAnnMgr()
{
	m_pBackground = NULL;
	
	m_fSecMoveWidth = 0.0f;
	m_iAddWidth		= 0;
	m_dwCurProcessTime = 0;
}

ioGameAnnMgr::~ioGameAnnMgr()
{	
	SAFEDELETE( m_pBackground );
}

ioGameAnnMgr& ioGameAnnMgr::GetSingleton()
{
	return Singleton< ioGameAnnMgr >::GetSingleton();
}

void ioGameAnnMgr::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_game_announce.ini" );
	char szBuf[MAX_PATH] = "";    
	kLoader.SetTitle_e( "common" );	
	SAFEDELETE( m_pBackground );
	kLoader.LoadString_e( "background", "", szBuf, MAX_PATH );
	m_pBackground = g_UIImageSetMgr.CreateImageByFullName( szBuf );	

	kLoader.LoadString_e( "start_sound", "", szBuf, MAX_PATH );
	m_szStartSnd = szBuf;

	m_fSecMoveWidth = kLoader.LoadFloat_e( "sec_move_width", 0.0f );
	m_iAddWidth		= kLoader.LoadInt_e( "add_width", 0 );

	m_AnnounceList.clear();
	m_CurrentAnnLIst.clear();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.
	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate ) return;
	// tm_year는 yeser-1900 값 2자리만 획득하기 위해서 -100
	DWORD dwCurrentDate = ( (pDate->tm_year - 100 ) * 100000000 ) + ( (pDate->tm_mon+1) * 1000000 ) + ( pDate->tm_mday * 10000 );

	int i = 0;
	int iMaxAnnounce = kLoader.LoadInt_e( "max_announce", 0 );
	for(i = 0;i < iMaxAnnounce;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "announce%d", i + 1 );
		kLoader.SetTitle( szTitle );

		//
		AnnounceData kAnnData;
		kAnnData.m_dwSeqIndex   = i + 1;
		if( !kLoader.LoadBool_e( "every_day", false ) )
		{
			kAnnData.m_dwStartDate	= kLoader.LoadInt_e( "start_date", 0 );
			kAnnData.m_dwEndDate	= kLoader.LoadInt_e( "end_date", 0 );
		}
		else
		{
			DWORD dwEveryStarDate	= kLoader.LoadInt_e( "every_start_date", 0 );
			DWORD dwEveryEndDate	= kLoader.LoadInt_e( "every_end_date", 0 );
			kAnnData.m_dwStartDate	= dwCurrentDate + dwEveryStarDate;
			kAnnData.m_dwEndDate	= dwCurrentDate + dwEveryEndDate;
		}

		kAnnData.m_dwRotateTime	= kLoader.LoadInt_e( "rotate_time", 0 );
		kAnnData.m_iCheckModeType = kLoader.LoadInt_e( "check_mode", -1 );
		kAnnData.m_iCheckRoomStyle= kLoader.LoadInt_e( "check_room_style", -1 );
		kAnnData.m_bShutDownUser  = kLoader.LoadBool_e( "shut_down_user", false );
		kAnnData.m_iPriority      = kLoader.LoadInt_e( "priority", 99999999 );

		int iLineCount = kLoader.LoadInt_e( "line_count", 0 );
		for(int k = 0;k < iLineCount;k++)
		{
			char szKey[MAX_PATH] = "";
			sprintf_e( szKey, "line%d_color", k + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			char *pTemp = NULL;
			DWORD dwColor = strtoul( szBuf, &pTemp, 16 );

			sprintf_e( szKey, "line%d_text", k + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Help::ChangeCharSet( szBuf, '#', ' ' );

			kAnnData.m_AnnounceText.SetTextStyle( TS_OUTLINE_FULL_2X );
			kAnnData.m_AnnounceText.SetBkColor( 0, 0, 0 );
			kAnnData.m_AnnounceText.SetTextColor( dwColor );
			kAnnData.m_AnnounceText.AddTextPiece( FONT_SIZE_25, STR(1), szBuf );
		}
		m_AnnounceList.push_back( kAnnData );
	}
}

bool ioGameAnnMgr::IsCurrentList( DWORD dwSeqIndex )
{
	int iMaxAnnounce = (int)m_CurrentAnnLIst.size();
	for(int i = 0;i < iMaxAnnounce;i++)
	{
		AnnounceData &rkData = m_CurrentAnnLIst[i];
		if( rkData.m_dwSeqIndex == dwSeqIndex )
			return true;
	}
	return false;
}

void ioGameAnnMgr::InsertServerAnnounce( const ioHashString &rkAnnounce, int iPriority )
{
	AnnounceData kAnnData;

	StringVector kStringVec = ioStringConverter::Split( rkAnnounce.c_str(), "#" );
	for(StringVector::const_iterator it = kStringVec.begin(); it != kStringVec.end(); ++it)
	{
		const std::string &szLine = *it;

		kAnnData.m_AnnounceText.SetTextStyle( TS_OUTLINE_FULL_2X );
		kAnnData.m_AnnounceText.SetBkColor( 0, 0, 0 );
		kAnnData.m_AnnounceText.SetTextColor( 255, 180, 0 );
		kAnnData.m_AnnounceText.AddTextPiece( FONT_SIZE_25, STR(1), szLine.c_str() );	
		kAnnData.m_iPriority    = iPriority;
		kAnnData.m_dwRenderTime = 0;
	}
    
	m_CurrentAnnLIst.push_back( kAnnData );
	if( !m_CurrentAnnLIst.empty() )
		std::sort( m_CurrentAnnLIst.begin(), m_CurrentAnnLIst.end(), ioGameAnnMgr::DataSort() );

	if( !m_szStartSnd.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szStartSnd );
	}
}

void ioGameAnnMgr::Process( ModeType eModeType )
{
	if( m_AnnounceList.empty() )
		return;

	if( FRAMEGETTIME() - m_dwCurProcessTime < 60000 )  
		return;

	m_dwCurProcessTime = FRAMEGETTIME();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.
	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate ) return;

	// tm_year는 yeser-1900 값 2자리만 획득하기 위해서 -100
	DWORD dwCurrentDate = ( (pDate->tm_year - 100 ) * 100000000 ) + ( (pDate->tm_mon+1) * 1000000 ) + ( pDate->tm_mday * 10000 ) + ( pDate->tm_hour * 100 ) + ( pDate->tm_min );
	
	int iMaxAnnounce = (int)m_AnnounceList.size();
	for(int i = 0;i < iMaxAnnounce;i++)
	{
		AnnounceData &rkData = m_AnnounceList[i];
		
		if( rkData.m_bShutDownUser )
		{
			if( !g_MyInfo.IsShutDownUser() )
				continue;
		}

		if( !COMPARE( dwCurrentDate, rkData.m_dwStartDate, rkData.m_dwEndDate ) )
			continue;

		if( FRAMEGETTIME() - rkData.m_dwCurRotateTime < rkData.m_dwRotateTime )
			continue;
		
		// 룸 스타일 체크 ( 전투 or 진영 )
		if( rkData.m_iCheckRoomStyle == BATTLE_STYLE )
		{
			if( !g_BattleRoomMgr.IsBattleRoom() )
				continue;
		}
		else if( rkData.m_iCheckRoomStyle == LADDER_STYLE )
		{
			if( !g_LadderTeamMgr.IsLadderTeam() )
				continue;
		}
		else if( rkData.m_iCheckRoomStyle == ALL_BATTLE_STYLE )
		{
			if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() &&
				!g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
				continue;
		}

		// 모드 체크	
		if( rkData.m_iCheckModeType != -1 )
		{
			if( eModeType != rkData.m_iCheckModeType )
				continue;
		}

		// 이미 진행중 체크
		if( IsCurrentList( rkData.m_dwSeqIndex ) )
			continue;

		rkData.m_dwRenderTime    = 0;
		rkData.m_dwCurRotateTime = FRAMEGETTIME();    //시간을 갱신해줘야지 다음 시간까지 처리된다.
		m_CurrentAnnLIst.push_back( rkData );
		
		if( !m_szStartSnd.IsEmpty() )
		{
			g_SoundMgr.PlaySound( m_szStartSnd );
		}
	}

	if( !m_CurrentAnnLIst.empty() )
		std::sort( m_CurrentAnnLIst.begin(), m_CurrentAnnLIst.end(), ioGameAnnMgr::DataSort() );
}

void ioGameAnnMgr::Render( ioUIRenderer *pUIRender )
{
	if( !m_pBackground ) return;
	if( m_CurrentAnnLIst.empty() ) return;

	ioRenderSystem *pRenderSystem = &RenderSystem();
	if( !pRenderSystem ) return;

	m_pBackground->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pBackground->SetSize( Setting::Width(), m_pBackground->GetHeight() );
	m_pBackground->Render( 0, 0 );

	AnnounceData &rkData = m_CurrentAnnLIst[0];
	if( rkData.m_dwRenderTime == 0 )
		rkData.m_dwRenderTime = FRAMEGETTIME();

	float fMaxMoveWidth  = Setting::Width() + rkData.m_AnnounceText.GetFullWidth() + m_iAddWidth + ( m_fSecMoveWidth * 2 );  // 2초간 더 표시해줘야하므로 
	float fCurrentRate   = (float)(FRAMEGETTIME() - rkData.m_dwRenderTime) / FLOAT1000;
	float fLeftXPos      = m_fSecMoveWidth * fCurrentRate;
	rkData.m_AnnounceText.PrintFullText( ( Setting::Width() + m_iAddWidth ) - fLeftXPos, 11, TAT_LEFT );
	
	if( fLeftXPos > fMaxMoveWidth )
	{
		m_CurrentAnnLIst.erase( m_CurrentAnnLIst.begin() );
		if( !m_CurrentAnnLIst.empty() )
		{
			AnnounceData &rkData = m_CurrentAnnLIst[0];
			rkData.m_dwRenderTime = 0; // 초기화
		}
	}

	if( pUIRender )
	{
		pUIRender->Render( pRenderSystem );
	}
}