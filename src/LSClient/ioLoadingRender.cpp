

#include "stdafx.h"

#include "ioLoadingRender.h"

#include "GUI/ioInterfaceBackground.h"
#include "Channeling/ioChannelingNodeParent.h"
#include "Channeling/ioChannelingNodeManager.h"

#include "ioTournamentManager.h"
#include "ioShuffleRoomMgr.h"
#include "ioModeResourceManager.h"
#include "ioLoadingToolTipRender.h"
#include "ioLadderTeamMgr.h"

ioLoadingRender::ioLoadingRender()
{
	m_pGameLimit		= NULL;

	m_pLoadGaugeBack	= NULL;
	m_pLoadGauge		= NULL;

	m_pHelpBack			= NULL;
	m_pTitle			= NULL;
	m_pTitleSub			= NULL;
	m_pTitleLine		= NULL;
	m_pModeIcon			= NULL;
	m_pMiniMap			= NULL;
	m_pHeroMark			= NULL;
	m_pTipBack			= NULL;

	m_pShuffleTitle		= NULL;

	m_iHelpXPos			= 0;
	m_iHelpYPos			= 0;
	m_iTitleXPos		= 0;
	m_iTitleYPos		= 0;
	m_iModeIconXPos		= 0;
	m_iModeIconYPos		= 0;

	m_dwFirstState		= STATE_LOGO;
	m_dwCurLogoTime		= 0;
	m_iLogoCurRate		= 0;
	m_iCurrentRate		= 0;
	m_iFullRate			= 0;
	m_dwLoadingTime		= 0;
	m_bCampBattle		= false;
	m_bCampHeroBattle	= false;

	m_iModeType			= -1;
	m_iMyRoomType		= 0;

	m_isAppearTip		= false;
	m_nTipRandomSeed	= 0;

	m_pCampVersus = NULL;
	m_pCampBlueTeam = NULL;
	m_pCampRedTeam = NULL;

	m_pSuccessionRevenge = NULL;
	m_iRevengeTitleXPos = 0;
	m_iRevengeTitleYPos = 0;
}

ioLoadingRender::~ioLoadingRender()
{
	ClearResource();

	m_ModePathMap.clear();
}

void ioLoadingRender::LoadModeINIPATH()
{
	m_ModePathMap.clear();

	char szBuf[MAX_PATH]="";
	char szKey[MAX_PATH]="";

	ioINILoader_e kLoader( "config/sp2_mode.ini" );
	kLoader.SetTitle_e( "common_info" );

	int iModePathCnt = kLoader.LoadInt_e( "max_mode_cnt", 0 );
	int iModeType = 0;
	for( int i=0; i < iModePathCnt; ++i )
	{
		wsprintf_e( szKey, "mode%d_type", i+1 );
		iModeType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "mode%d_path", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ModePathMap.insert( ModePathMap::value_type(iModeType, szBuf) );
	}
}

const char* ioLoadingRender::GetModeINIPATH( ModeType eType )
{
	int iType = eType;
	if( iType == (int)MT_MYROOM || iType == (int)MT_NONE )
	{
		iType = MT_HEADQUARTERS;
	}

	ModePathMap::iterator iter = m_ModePathMap.find( iType );
	if( iter != m_ModePathMap.end() )
	{
		return iter->second.c_str();
	}

	LOG.PrintTimeAndLog( 0, "ioLoadingRender::GetModeINIPATH - %d Unknown Mode", iType );

	return "";
}

DWORD ioLoadingRender::GetLoadingBackColor()
{
	if( m_dwFirstState == STATE_LOGO )
		return g_App.GetLoadingLogoBackColor();
	return g_App.GetLoadingIssueBackColor();
}

const char* ioLoadingRender::GetModeMapINIFileName( int iModeType, int iSubNum, int iModeMapNum )
{
	m_CurMapFileName.Clear();

	if( iSubNum == -1 )
		return m_CurMapFileName.c_str();

	char szBuf[MAX_PATH];
	switch( iModeType )
	{
	case MT_SYMBOL:
		wsprintf_e( szBuf, "%ssp2_symbol_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TOWER_DEFENSE:
		wsprintf_e( szBuf, "%ssp2_towerdefense_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DARK_XMAS:
		wsprintf_e( szBuf, "%ssp2_darkxmas_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FIRE_TEMPLE:
		wsprintf_e( szBuf, "%ssp2_firetemple_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_CATCH:
	case MT_UNDERWEAR:
	case MT_CBT:
		wsprintf_e( szBuf, "%ssp2_catch_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_CATCH_RUNNINGMAN:
		wsprintf_e( szBuf, "%ssp2_catch_runningman_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_KING:
		wsprintf_e( szBuf, "%ssp2_hiddenking_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_survival_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FIGHT_CLUB:
		wsprintf_e( szBuf, "%ssp2_fight_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TEAM_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_teamsurvival_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TEAM_SURVIVAL_AI:
		wsprintf_e( szBuf, "%ssp2_teamsurvivalAI_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_BOSS:
		wsprintf_e( szBuf, "%ssp2_boss_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_MONSTER_SURVIVAL:
		wsprintf_e( szBuf, "%ssp2_monstersurvival_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_TRAINING:
		wsprintf_e( szBuf, "%ssp2_training_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FOOTBALL:
		wsprintf_e( szBuf, "%ssp2_football_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HEROMATCH:
		wsprintf_e( szBuf, "%ssp2_heromatch_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_GANGSI:
		wsprintf_e( szBuf, "%ssp2_gangsi_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DUNGEON_A:
		wsprintf_e( szBuf, "%ssp2_dungeon_a_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_DOUBLE_CROWN:
		wsprintf_e( szBuf, "%ssp2_doublecrown_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HEADQUARTERS:
		wsprintf_e( szBuf, "%ssp2_headquarters_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_SHUFFLE_BONUS:
		wsprintf_e( szBuf, "%ssp2_shufflebonus_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FACTORY:
		wsprintf_e( szBuf, "%ssp2_factory_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_HOUSE:
		wsprintf_e( szBuf, "%ssp2_house_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_RAID:
		wsprintf_e( szBuf, "%ssp2_raid_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_SUCCESSION:
		wsprintf_e( szBuf, "%ssp2_succession_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_MYROOM:
	case MT_NONE:
		wsprintf_e( szBuf, "%ssp2_headquarters_map%d.ini", GetModeINIPATH((ModeType)iModeType), iModeMapNum+1 );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_PRACTICE://이거 변경해야됨
		wsprintf_e( szBuf, "%ssp2_practice_map%d.ini", GetModeINIPATH((ModeType)iModeType), iModeMapNum+1 );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FLAG_CAPTURE:
		wsprintf_e( szBuf, "%ssp2_flagcapture_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_ARENA:
		wsprintf_e( szBuf, "%ssp2_arena_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_BATTLE:
		wsprintf_e( szBuf, "%ssp2_battle_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	case MT_FARMING:
		wsprintf_e( szBuf, "%ssp2_farming_mode%d_map%d.ini", GetModeINIPATH((ModeType)iModeType), iSubNum, iModeMapNum );
		m_CurMapFileName = szBuf;
		return m_CurMapFileName.c_str();
	}

	LOG.PrintTimeAndLog( 0, "ioLoadingRender::GetModeMapINIFileName - %d Unknown Mode", iModeType );

	return "";
}

void ioLoadingRender::RenderFirstLoading( ioRenderSystem *pRenderSystem, ioUIRenderer *pUIRenderer, int iCurRate, int iFullRate, DWORD dwLogoTime, DWORD dwRegulationTime )
{
	if( !pRenderSystem || !pUIRenderer ) return;

	if( !g_BackgroundMgr.GetLoading( true, m_iModeType, m_iMyRoomType ) )
	{
		ClearResource();
		if( m_dwFirstState == STATE_LOGO )
		{
			ioHashString sImgSet = "LogoDefault";
			ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
			if( pChannelingNode )
				sImgSet = pChannelingNode->GetLogoSetName();
			g_BackgroundMgr.SetLoading( sImgSet );
			m_dwLoadingTime = dwLogoTime;
		}
		else if( m_dwFirstState == STATE_REGULATION )
		{
			g_BackgroundMgr.SetLoading( "LoadingDefault2" );

			m_pGameLimit   = g_UIImageSetMgr.CreateImage( "BaseImage000", "Loading_info", true );
			if( m_pGameLimit )
			{
				m_pGameLimit->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
				m_pGameLimit->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

				float fWidthRate  = (float)Setting::Width() / DEF_SCREEN_W;
				float fHeightRate = (float)Setting::Height() / DEF_SCREEN_H;
				m_pGameLimit->SetSize( (float)m_pGameLimit->GetWidth() * fWidthRate, (float)m_pGameLimit->GetHeight() * fHeightRate );
			}
			m_dwLoadingTime = dwRegulationTime;
		}
		else if( m_dwFirstState == STATE_LOADING )
		{
			g_BackgroundMgr.SetLoading( "LoadingDefault" );

			m_pGameLimit   = g_UIImageSetMgr.CreateImage( "BaseImage000", "Loading_info", true );
			if( m_pGameLimit )
			{
				m_pGameLimit->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
				m_pGameLimit->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

				float fWidthRate  = (float)Setting::Width() / DEF_SCREEN_W;
				float fHeightRate = (float)Setting::Height() / DEF_SCREEN_H;
				m_pGameLimit->SetSize( (float)m_pGameLimit->GetWidth() * fWidthRate, (float)m_pGameLimit->GetHeight() * fHeightRate );
			}

			m_pLoadGaugeBack   = g_UIImageSetMgr.CreateImage( "BaseImage000", "gauge_finishbig_bg", true );
			m_pLoadGauge	   = g_UIImageSetMgr.CreateImage( "BaseImage000", "gauge_finishbig_gauge_orange", true );
			m_dwLoadingTime    = 1000;
		}		
	}
	m_iCurrentRate  = iCurRate;
	m_iFullRate     = iFullRate;

	switch( m_dwFirstState )
	{
	case STATE_LOGO:
		{	
			// 페이드 작동
			if( m_dwCurLogoTime == 0 )
			{
				m_dwCurLogoTime = timeGetTime();
				for(int i = 0;i < MAX_ALPHA_RATE;i++)
				{
					pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
					RenderFirstBackGround( i );
					pRenderSystem->SetFogEnable( false );
					pUIRenderer->Render( pRenderSystem );
					pRenderSystem->Present();
				}
			}

			if( timeGetTime() - m_dwCurLogoTime >= m_dwLoadingTime )
			{
				for(int i = MAX_ALPHA_RATE;i > 0;i--)
				{
					pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
					RenderFirstBackGround( i );
					pRenderSystem->SetFogEnable( false );
					pUIRenderer->Render( pRenderSystem );
					pRenderSystem->Present();
				}
				m_dwCurLogoTime = timeGetTime();
				m_dwFirstState  = STATE_REGULATION;
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal && !pLocal->IsShowRegulation() )
					m_dwFirstState = STATE_LOADING;
					
				m_iLogoCurRate  = m_iCurrentRate;
			
				ClearResource();
				g_BackgroundMgr.SetLoading( "" );   // 메모리 해제
				pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
				RenderFirstLoading( pRenderSystem, pUIRenderer, m_iCurrentRate, m_iFullRate, dwLogoTime, dwRegulationTime );
			}			
			else
			{
				pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
				RenderFirstBackGround();
				pRenderSystem->SetFogEnable( false );
				pUIRenderer->Render( pRenderSystem );
				pRenderSystem->Present();
			}
		}
		break;	
	case STATE_REGULATION:
		{	
			if( timeGetTime() - m_dwCurLogoTime >= m_dwLoadingTime )
			{
				for(int i = MAX_ALPHA_RATE;i > 0;i--)
				{
					pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
					RenderFirstBackGround( i );
					RenderGameLimit( i );
					pRenderSystem->SetFogEnable( false );
					pUIRenderer->Render( pRenderSystem );
					pRenderSystem->Present();
				}
				m_dwCurLogoTime = 0;
				m_dwFirstState  = STATE_LOADING;
				m_iLogoCurRate  = m_iCurrentRate;

				ClearResource();
				g_BackgroundMgr.SetLoading( "" );   // 메모리 해제				
				RenderFirstLoading( pRenderSystem, pUIRenderer, m_iCurrentRate, m_iFullRate, dwLogoTime, dwRegulationTime );
			}			
			else
			{
				pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
				RenderFirstBackGround();
				RenderGameLimit();
				pRenderSystem->SetFogEnable( false );
				pUIRenderer->Render( pRenderSystem );
				pRenderSystem->Present();
			}
		}
		break;
	case STATE_LOADING:
		{
			// 페이드 작동
			if( m_dwCurLogoTime == 0 )
			{
				m_dwCurLogoTime = timeGetTime();
				for(int i = 0;i < MAX_ALPHA_RATE;i++)
				{
					pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
					RenderFirstBackGround( i );
					RenderGameLimit( i );
					pRenderSystem->SetFogEnable( false );
					pUIRenderer->Render( pRenderSystem );
					pRenderSystem->Present();
				}

				float fRate = FLOAT1;
				if( m_iLogoCurRate != m_iFullRate )
				{
					iFullRate = max( m_iFullRate - m_iLogoCurRate, 1 );
					iCurRate  = max( 0, min( m_iCurrentRate - m_iLogoCurRate, m_iFullRate ) );
					fRate = (float)iCurRate/(float)iFullRate;
				}

				pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
				RenderFirstBackGround( i );
				RenderLoadBar( fRate );	
				RenderGameLimit();
				pRenderSystem->SetFogEnable( false );
				pUIRenderer->Render( pRenderSystem );
				pRenderSystem->Present();
			}
			else
			{
				pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, GetLoadingBackColor() );
				RenderFirstBackGround();

				float fRate = FLOAT1;
				if( m_iLogoCurRate != m_iFullRate )
				{
					iFullRate = max( m_iFullRate - m_iLogoCurRate, 1 );
					iCurRate  = max( 0, min( m_iCurrentRate - m_iLogoCurRate, m_iFullRate ) );
					fRate = (float)iCurRate/(float)iFullRate;
				}

				RenderLoadBar( fRate );	
				RenderGameLimit();
				pRenderSystem->SetFogEnable( false );
				pUIRenderer->Render( pRenderSystem );
				pRenderSystem->Present();
			}
		}
		break;
	}	
}

bool ioLoadingRender::IsCloseFirstLoading()
{
	if( m_dwFirstState == STATE_LOGO ) return false;
	if( m_dwFirstState == STATE_REGULATION ) return false;
	if( timeGetTime() - m_dwCurLogoTime < m_dwLoadingTime )	return false;
	if( m_iCurrentRate < m_iFullRate )	return false;	

	return true;
}

void ioLoadingRender::InitTipInfo()
{
	m_nTipRandomSeed = rand();
}

void ioLoadingRender::RenderModeLoading( int iModeType, int iSubNum, int iModeMapNum, bool bCampBattle, bool bCampHeroBattle, PlazaType ePlazaType, MyRoomType eMyRoomType, bool bExitRoomBySearchMode )
{
	ClearResource();

	m_iModeType		  = iModeType;
	m_iMyRoomType	  = (int)eMyRoomType;
	m_bCampBattle	  = bCampBattle;
	m_bCampHeroBattle = bCampHeroBattle;

	ioINILoader_e kLoader( "config/sp2_mode_loading.ini" );	

	//로딩을 위한 타이틀 세팅
	ioHashString szTitle;
	if( iModeType == MT_TRAINING )
	{
		g_ModeResourceMgr.GetPlazaTitle( szTitle, ePlazaType );
	}
	else if( iModeType == MT_NONE || iModeType == MT_MYROOM )
	{
		g_ModeResourceMgr.GetModeRoomTitle( szTitle, eMyRoomType, bCampHeroBattle );
	}
	else
	{
		g_ModeResourceMgr.GetModeTitle( szTitle, iModeType );
	}
	
	if( szTitle.IsEmpty() )
		szTitle = "LoadingDefault";

	//로딩 배경이미지 설정
	LoadLoadingMapImage( kLoader, szTitle, iModeType, iSubNum, bExitRoomBySearchMode );


	if( iModeType == MT_TEAM_SURVIVAL_AI )
	{
		//hard모드
		if( g_BattleRoomMgr.GetAiLevel() == 1 )
		{
			szTitle += "_hard";
		}
	}
	//테투리, 모드 이름 등 이미지 설정
	LoadLoadingModePart( kLoader, szTitle, iModeType, iSubNum );
			
	//모드 아이콘
	LoadLoadingModeIcon( kLoader, szTitle, iModeType, ePlazaType, eMyRoomType, bCampHeroBattle );

	//모드 설명
	LoadLoadingModeDesc( kLoader, szTitle, iModeType, iSubNum );
			
	//모드 공통 사용정보
	LoadLoadingModeCommon( kLoader, szTitle, iModeType );

	//오늘의 모드 로딩
	LoadLoadingShuffle( kLoader, szTitle, eMyRoomType );
	
	if( m_bCampBattle )
	{
		//진영전
		LoadLoadingCamp( kLoader, szTitle, iModeType );
	}
	else
	{		
		//미니맵
		LoadMiniMapLoading( iModeType, iSubNum, iModeMapNum );
	}

	//로딩 툴팁
	LoadModeToolTip( szTitle );

	//배경
	RenderLoading( bExitRoomBySearchMode );
}

void ioLoadingRender::LoadLoadingMapImage( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSubNum, bool bExitRoomBySearchMode )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";	

	rkLoader.SetTitle( szTitle.c_str() );
	wsprintf( szKey, "mode_sub%d", iSubNum );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	
	if( !bExitRoomBySearchMode )
		g_BackgroundMgr.SetLoading( szBuf );
}

void ioLoadingRender::LoadLoadingModePart( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSubNum )
{
	char szBuf[MAX_PATH] = "";

	rkLoader.SetTitle( szTitle.c_str() );

	
	bool bExtraOption = g_BattleRoomMgr.IsUseExtraOption();
	bool bSuccessionRevenge = g_MyInfo.GetSuccessionRevengeMode();

	if( ( iModeType == MT_UNDERWEAR || iModeType == MT_CBT || iModeType == MT_FARMING ) )
		bExtraOption = false;

	if( bExtraOption )
		rkLoader.LoadString( "title_image_user_mode", "", szBuf, MAX_PATH );			
	else if(iModeType == MT_PRACTICE)
	{
		wsprintf( szBuf, "title_image%d", iSubNum );
		rkLoader.LoadString(szBuf,"", szBuf, MAX_PATH);
	}
	else
		rkLoader.LoadString( "title_image", "", szBuf, MAX_PATH );

	m_pTitle = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	if ( iModeType == MT_SUCCESSION && bSuccessionRevenge )
	{
		rkLoader.LoadString( "title_image_revenge_mode", "", szBuf, MAX_PATH );
		m_pSuccessionRevenge = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		m_iTitleXPos = rkLoader.LoadInt( "revenge_1vs1_title_x", 0 );
		m_iTitleYPos = rkLoader.LoadInt( "revenge_1vs1_title_y", 0 );
		m_iRevengeTitleXPos = rkLoader.LoadInt( "revenge_title_x", 0 );
		m_iRevengeTitleYPos = rkLoader.LoadInt( "revenge_title_y", 0 );
	}
	else
	{
		m_iTitleXPos = rkLoader.LoadInt( "title_x", 0 );
		m_iTitleYPos = rkLoader.LoadInt( "title_y", 0 );
	}
	

	rkLoader.LoadString_e( "title_sub_image", "", szBuf, MAX_PATH );			
	m_pTitleSub	= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	m_iTitleSubXPos = rkLoader.LoadInt( "title_sub_x", 0 );
	m_iTitleSubYPos = rkLoader.LoadInt( "title_sub_y", 0 );

	rkLoader.LoadString( "title_line_image", "", szBuf, MAX_PATH );
	m_pTitleLine = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	m_iTitleLineXPos = rkLoader.LoadInt( "title_line_x", 0 );
	m_iTitleLineYPos = rkLoader.LoadInt( "title_line_y", 0 );

	if( Help::IsMonsterDungeonMode( (ModeType)iModeType) )
		m_iTitleLineYPos -= 100;
}

void ioLoadingRender::LoadLoadingModeIcon( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, PlazaType ePlazaType, MyRoomType eMyRoomType, bool bCampHeroBattle )
{
	char szBuf[MAX_PATH] = "";

	rkLoader.SetTitle( szTitle.c_str() );

	
	bool bExtraOption = g_BattleRoomMgr.IsUseExtraOption();
	bool bSuccessionRevenge = g_MyInfo.GetSuccessionRevengeMode();

	if( ( iModeType == MT_UNDERWEAR || iModeType == MT_CBT || iModeType == MT_FARMING ) )
		bExtraOption = false;

	if( bExtraOption )
		rkLoader.LoadString( "icon_image_user_mode", "", szBuf, MAX_PATH );			
	else
		rkLoader.LoadString( "icon_image", "", szBuf, MAX_PATH );			

	if( iModeType == MT_TRAINING )
	{
		m_pModeIcon = g_ModeResourceMgr.GetPlazaIcon( ePlazaType );
	}
	else if( iModeType == MT_NONE || iModeType == MT_MYROOM )
	{
		m_pModeIcon = g_ModeResourceMgr.GetModeRoomIcon( eMyRoomType, bCampHeroBattle );
	}
	else if ( iModeType == MT_SUCCESSION )
	{
		if ( bSuccessionRevenge )
			m_pModeIcon = g_ModeResourceMgr.GetModeIconByUserMode( iModeType );
		else
			m_pModeIcon = g_ModeResourceMgr.GetModeIcon( iModeType );
	}
	else
	{
		if( bExtraOption )
			m_pModeIcon = g_ModeResourceMgr.GetModeIconByUserMode( iModeType );
		else
			m_pModeIcon = g_ModeResourceMgr.GetModeIcon( iModeType );
	}

	if ( iModeType == MT_SUCCESSION && bSuccessionRevenge )
	{
		m_iModeIconXPos = rkLoader.LoadInt_e( "revenge_icon_x", 0 );
		m_iModeIconYPos = rkLoader.LoadInt_e( "revenge_icon_y", 0 );
	}
	else
	{
		m_iModeIconXPos = rkLoader.LoadInt_e( "icon_x", 0 );
		m_iModeIconYPos = rkLoader.LoadInt_e( "icon_y", 0 );
	}
}

void ioLoadingRender::LoadLoadingModeDesc( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSubNum )
{	
	char szBuf[MAX_PATH] = "";

	rkLoader.SetTitle( szTitle.c_str() );
	if(iModeType != MT_PRACTICE)
		rkLoader.LoadString( "help_desc", "", szBuf, MAX_PATH );
	else
	{
		wsprintf( szBuf, "help_desc%d", iSubNum );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );

	}
	m_szHelpDesc = szBuf;
}

void ioLoadingRender::LoadLoadingModeCommon( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType )
{
	char szBuf[MAX_PATH] = "";	

	// 공통 이미지
	rkLoader.SetTitle( "common" );
	rkLoader.LoadString_e( "help_bar", "", szBuf, MAX_PATH );
	m_pHelpBack = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	m_iHelpXPos = rkLoader.LoadInt( "help_bar_x", 0 );
	m_iHelpYPos = rkLoader.LoadInt( "help_bar_y", 0 );

	if( Help::IsMonsterDungeonMode((ModeType)iModeType) )
		m_iHelpYPos -= 100;
}

void ioLoadingRender::LoadLoadingShuffle( ioINILoader& rkLoader, const ioHashString& szTitle, int eMyRoomType )
{
	char szBuf[MAX_PATH] = "";

	//오늘의 모드는 공통이미지에서 로드
	rkLoader.SetTitle( "common" );

	if( eMyRoomType == MRT_SHUFFLE )
	{
		rkLoader.LoadString( "shuffle_title", "", szBuf, MAX_PATH );
		m_pShuffleTitle = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	}
}

void ioLoadingRender::LoadLoadingCamp( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.SetTitle( "common" );

	if ( iModeType == MT_NONE )
	{
		//로딩 화면이라면
		int iType = g_TournamentMgr.GetRegularResourceType();
		sprintf_s_e( szKey, "camp_battle_mark%d_team_blue", iType );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_pCampBlueTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

		sprintf_s_e( szKey, "camp_battle_mark%d_team_red", iType );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_pCampRedTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

		//레더전일 경우
		if( m_bCampHeroBattle )
		{
			rkLoader.LoadString( "camp_battle_hero_mark", "", szBuf, MAX_PATH );
			m_pHeroMark = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
		//진영전일 경우.
		else
		{
			rkLoader.LoadString( "camp_battle_mark", "", szBuf, MAX_PATH );
			m_pCampVersus = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}

		return;
	}

	//적팀이 블루
	int iOtherTeampType = g_LadderTeamMgr.GetOtherTeamType();
	int iOtherCampType = g_LadderTeamMgr.GetOtherCampType();

	//패킷정보를 제대로 받지 못했을 경우 진영,레더전을 그려주지 않는다.
	if ( iOtherTeampType == TEAM_NONE || iOtherCampType == CAMP_NONE )
		return;
	int iType = g_TournamentMgr.GetRegularResourceType();
	if ( iOtherTeampType == TEAM_BLUE )
	{
		if ( iOtherCampType == CAMP_BLUE )
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_blue", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampBlueTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
		else
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_red", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampBlueTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}

		if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_blue", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampRedTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
		else
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_red", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampRedTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
	}
	else
	{
		if ( iOtherCampType == CAMP_BLUE )
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_blue", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampRedTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
		else
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_red", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampRedTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}

		if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_blue", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampBlueTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
		else
		{
			sprintf_s_e( szKey, "camp_battle_mark%d_team_red", iType );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_pCampBlueTeam = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		}
	}

	if ( m_pCampRedTeam )
		m_pCampRedTeam->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );

	//레더전일 경우
	if( m_bCampHeroBattle )
	{
		rkLoader.LoadString( "camp_battle_hero_mark", "", szBuf, MAX_PATH );
		m_pHeroMark = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	}
	//진영전일 경우.
	else
	{
		rkLoader.LoadString( "camp_battle_mark", "", szBuf, MAX_PATH );
		m_pCampVersus = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	}
}

void ioLoadingRender::LoadModeToolTip( const ioHashString& szTtitle )
{
	m_TipPrinter.ClearList();
	m_TipPrinter.SetBkColor( 0, 0, 0 );
	m_TipPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );

	m_TipRandom.SetRandomSeed( m_nTipRandomSeed );

	m_pTipBack		= g_UIImageSetMgr.CreateImageByFullName( "BaseImage001#icon0_rec_bg", true );
	m_isAppearTip	= TipAnalysis( szTtitle.c_str(), m_TipPrinter, m_TipRandom );
}

void ioLoadingRender::LoadMiniMapLoading( int iModeType, int iSubNum, int iModeMapNum )
{
	if( !Help::IsMonsterDungeonMode((ModeType)iModeType) )
		return;

	ioHashString szMapININame = GetModeMapINIFileName( iModeType, iSubNum, iModeMapNum );
	if( szMapININame.IsEmpty() )
		return;

	ioINILoader rkLoader( szMapININame.c_str() );
	rkLoader.SetTitle( "minimap" );
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "image", "", szBuf, MAX_PATH );

	if( strcmp( szBuf, "" ) != 0 )
	{
		m_pMiniMap = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
		if( !m_pMiniMap )
		{
			LOG.PrintTimeAndLog( 0, "Not MiniMap Resource!!(%s)", szBuf );
		}
	}
}

void ioLoadingRender::ClearResource()
{	
	SAFEDELETE( m_pGameLimit );

	SAFEDELETE( m_pLoadGaugeBack );
	SAFEDELETE( m_pLoadGauge );

	SAFEDELETE( m_pHelpBack );
	SAFEDELETE( m_pTitle );
	SAFEDELETE( m_pTitleSub );
	SAFEDELETE( m_pTitleLine );
	SAFEDELETE( m_pMiniMap );
	SAFEDELETE( m_pHeroMark );
	SAFEDELETE( m_pShuffleTitle );

	SAFEDELETE( m_pCampVersus );
	SAFEDELETE( m_pCampBlueTeam );
	SAFEDELETE( m_pCampRedTeam );
	
	m_szHelpDesc.Clear();

	SAFEDELETE( m_pTipBack );
	m_isAppearTip = false;
	m_TipPrinter.ClearList();

	SAFEDELETE( m_pSuccessionRevenge );
}

// 최초 로딩
void ioLoadingRender::RenderFirstBackGround( int iBackAlpha /* = MAX_ALPHA_RATE  */ )
{	
	ioUIRenderImage *pLoadingRes = g_BackgroundMgr.GetLoading( true, m_iModeType, m_iMyRoomType );

	if( !pLoadingRes )
		return;

	pLoadingRes->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	pLoadingRes->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	pLoadingRes->SetSize( Setting::Width(), Setting::Height() );
	pLoadingRes->SetAlpha( iBackAlpha );

	if( Setting::Width() != DEF_SCREEN_W || Setting::Height() != DEF_SCREEN_H )
	{
		pLoadingRes->Render( 0, 0, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		pLoadingRes->Render( 0, 0 );
	}
}

void ioLoadingRender::RenderLoadBar( float fRate )
{
	if( !IsLoadedBar() )	return;

	int iGaugeX = ( Setting::Width() / 2 ) - ( m_pLoadGaugeBack->GetWidth() / 2 );
	int iGaugeY = Setting::Height() - 74;
	m_pLoadGaugeBack->Render( iGaugeX, iGaugeY );
	m_pLoadGauge->RenderWidthCut( iGaugeX, iGaugeY, 0, (float)m_pLoadGauge->GetWidth() * fRate );
}

void ioLoadingRender::RenderShuffleProcess( int iXPos, int iYPos )
{
	if( m_pShuffleTitle )
		m_pShuffleTitle->Render( iXPos - 168, iYPos - 175 );
	
	if( m_pTitle )
		m_pTitle->Render( iXPos + 58, iYPos - 184 );
}

void ioLoadingRender::RenderGameLimit( int iBackAlpha /* = MAX_ALPHA_RATE  */ )
{
	if( !m_pGameLimit )	return;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsKoreaGameLimit() ) return;

	// 전체 이용가
	m_pGameLimit->SetAlpha( iBackAlpha );
	m_pGameLimit->Render( Setting::Width() - ( m_pGameLimit->GetWidth() + 30.0f ), 30, UI_RENDER_NORMAL, TFO_BILINEAR );
}

// 일반 로딩
void ioLoadingRender::RenderLoading( bool bExitRoomBySearchMode )
{
	ioUIRenderImage *pLoadingRes = NULL;
	if( bExitRoomBySearchMode )
		pLoadingRes = g_BackgroundMgr.GetBackgroundRes( BGT_MODE );
	else
		pLoadingRes = g_BackgroundMgr.GetLoading( false, m_iModeType, m_iMyRoomType );

	if( !pLoadingRes )
		return;

	pLoadingRes->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	pLoadingRes->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	pLoadingRes->SetSize( Setting::Width(), Setting::Height() );
	pLoadingRes->Render( 0, 0, UI_RENDER_NORMAL, TFO_BILINEAR );

	int iXPos = Setting::HalfWidth();
	int iYPos = Setting::HalfHeight();
	if( m_iMyRoomType == MRT_SHUFFLE && g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		RenderShuffleProcess( iXPos, iYPos );
	}
	else	
	{
		if( m_pModeIcon )
			m_pModeIcon->Render( iXPos + m_iModeIconXPos, iYPos + m_iModeIconYPos );
		if( m_pTitleLine )
			m_pTitleLine->Render( iXPos + m_iTitleLineXPos, iYPos + m_iTitleLineYPos );
		if( m_pTitle )
			m_pTitle->Render( iXPos + m_iTitleXPos, iYPos + m_iTitleYPos );
		if( m_pTitleSub )
			m_pTitleSub->Render( iXPos + m_iTitleSubXPos, iYPos + m_iTitleSubYPos );
		if ( m_pSuccessionRevenge )
			m_pSuccessionRevenge->Render( iXPos + m_iRevengeTitleXPos, iYPos + m_iRevengeTitleYPos );
		if( m_pMiniMap )
		{
			/*
			if( m_bCampBattle )
			{
				m_pMiniMap->Render( iXPos, iYPos - 11 );
				if( m_bCampHeroBattle && m_pHeroMark )
					m_pHeroMark->Render( iXPos, iYPos + 79 );
			}
			else
				m_pMiniMap->Render( iXPos - ( m_pMiniMap->GetWidth() / 2), iYPos - 6 );
			*/
			m_pMiniMap->Render( iXPos - ( m_pMiniMap->GetWidth() / 2), iYPos - 6 );
		}
	}

	if( !m_szHelpDesc.IsEmpty() )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 229, 229, 229 );
		if( m_iMyRoomType == MRT_SHUFFLE && g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			if( m_pHelpBack )
				m_pHelpBack->Render( iXPos + m_iHelpXPos, iYPos + m_iHelpYPos - 53 );

			g_FontMgr.PrintText( iXPos, iYPos + m_iHelpYPos - 51, FONT_SIZE_13, m_szHelpDesc.c_str() );
		}
		else
		{
			if( m_pHelpBack )
				m_pHelpBack->Render( iXPos + m_iHelpXPos, iYPos + m_iHelpYPos );

			g_FontMgr.PrintText( iXPos, iYPos + m_iHelpYPos + 2, FONT_SIZE_13, m_szHelpDesc.c_str() );
		}
	}
	if( !m_szChangeServerHelp.IsEmpty() )
	{
		if( m_pHelpBack )
			m_pHelpBack->Render( iXPos + m_iHelpXPos, iYPos + m_iHelpYPos + CHNAGE_HELP_Y_OFFSET );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 229, 229, 229 );
		g_FontMgr.PrintText( iXPos, iYPos + m_iHelpYPos + 2 + CHNAGE_HELP_Y_OFFSET, FONT_SIZE_13, m_szChangeServerHelp.c_str() );
	}
	if( m_isAppearTip )
	{
		int nTipHeight = m_pTipBack->GetHeight();
		if( m_pTipBack )
		{
			m_pTipBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pTipBack->SetReverseFormatting( ioUIRenderImage::RF_VERT_REVERSE );
			m_pTipBack->SetSize( Setting::Width(), nTipHeight );
			m_pTipBack->Render( 0, Setting::Height() - nTipHeight );
		}
		m_TipPrinter.PrintFullText( iXPos, Setting::Height() - 34, TAT_CENTER );
	}

	if ( m_bCampBattle || m_bCampHeroBattle )
	{
		if ( m_pCampBlueTeam )
			m_pCampBlueTeam->Render( iXPos - 204 , iYPos + 2, UI_RENDER_NORMAL, TFO_BILINEAR );

		if ( m_pCampRedTeam )
			m_pCampRedTeam->Render( iXPos + 44 , iYPos + 2, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bCampHeroBattle && m_pHeroMark )
			m_pHeroMark->Render( iXPos, iYPos + 79 );
		else
		{
			if ( m_pCampVersus )
				m_pCampVersus->Render( iXPos - 47, iYPos + 19, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

bool ioLoadingRender::IsLoadedBar() const
{
	if( m_pLoadGaugeBack && m_pLoadGauge )
		return true;

	return false;
}


