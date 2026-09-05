
#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../Setting.h"
#include "../ioPlayStage.h"
#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../ioBattleRoomMgr.h"

#include "ioSP2GUIManager.h"
#include "ModeRankWnd.h"

ModeRankWnd::ModeRankWnd() : m_iRankType( NONE_TYPE ), m_pPlayStage( NULL ), m_bPosSave( false ), m_eModeType( MT_NONE ), m_iAlphaRate( MAX_ALPHA_RATE )
{
	m_pStonePoint	= NULL;
	m_pPrisonerPoint= NULL;
	m_pKingPoint	= NULL;
	m_pKillPoint	= NULL;
	m_pAttackPoint	= NULL;
	m_pDamagePoint	= NULL;
	m_pNpcKillPts	= NULL;
	m_pNpcTowerPts	= NULL;
	m_pTimePts		= NULL;
	m_pFlagPoint	= NULL;

	m_pGoalPoint		= NULL;
	m_pBallTouchPoint	= NULL;
	m_pBallTimePoint	= NULL;

	m_pGangsiInfectionPoint = NULL;
	m_pGangsiSurvivorPoint  = NULL;

	for(int i = 0;i < MAX_RANK;i++)
		m_pRank[i]		= NULL;

	m_dwCurTime = m_dwShowTime = m_dwStartTime = m_dwHideTime = m_dwGapTime = m_dwCurHideTime = 0;

	m_bRollingEnd = false;
	m_iDefaultX = m_iDefaultY = ioSP2GUIManager::SCREEN_OUT_POS;
}

ModeRankWnd::~ModeRankWnd()
{
	SAFEDELETE( m_pStonePoint );
	SAFEDELETE( m_pPrisonerPoint );
	SAFEDELETE( m_pKingPoint );
	SAFEDELETE( m_pKillPoint );
	SAFEDELETE( m_pAttackPoint );
	SAFEDELETE( m_pDamagePoint );
	SAFEDELETE(m_pNpcKillPts);
	SAFEDELETE(m_pNpcTowerPts);
	SAFEDELETE(m_pTimePts);
	SAFEDELETE( m_pFlagPoint );


	SAFEDELETE( m_pGoalPoint );
	SAFEDELETE( m_pBallTouchPoint );
	SAFEDELETE( m_pBallTimePoint );

	SAFEDELETE( m_pGangsiInfectionPoint );
	SAFEDELETE( m_pGangsiSurvivorPoint );

	for(int i = 0;i < MAX_RANK;i++)
		SAFEDELETE( m_pRank[i] );

	UIPositionSave();

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_RollingSound, 0 );
}

void ModeRankWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_RollingSound		= xElement.GetStringAttribute_e( "RollingSound" );
	m_RollingStopSound	= xElement.GetStringAttribute_e( "RollingStopSound" );
	m_dwShowTime		= xElement.GetIntAttribute_e( "ShowTime" ); 
	m_dwHideTime        = xElement.GetIntAttribute_e( "HideTime" ); 
	m_dwStartTime		= xElement.GetIntAttribute_e( "StartTime" ); 
	m_dwGapTime			= xElement.GetIntAttribute_e( "GapTime" );
}

void ModeRankWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{	
	if( szType == "StonePoint" )
	{
		SAFEDELETE( m_pStonePoint );
		m_pStonePoint = pImage;
	}
	else if( szType == "PrisonerPoint" )
	{
		SAFEDELETE( m_pPrisonerPoint );
		m_pPrisonerPoint = pImage;
	}	
	else if( szType == "KingPoint" )
	{
		SAFEDELETE( m_pKingPoint );
		m_pKingPoint = pImage;
	}	
	else if( szType == "KillPoint" )
	{
		SAFEDELETE( m_pKillPoint );
		m_pKillPoint = pImage;
	}	
	else if( szType == "AttackPoint" )
	{
		SAFEDELETE( m_pAttackPoint );
		m_pAttackPoint = pImage;
	}	
	else if( szType == "DamagePoint" )
	{
		SAFEDELETE( m_pDamagePoint );
		m_pDamagePoint = pImage;
	}
	else if( szType == "GoalPoint" )
	{
		SAFEDELETE( m_pGoalPoint );
		m_pGoalPoint = pImage;
	}
	else if( szType == "BallTouchPoint" )
	{
		SAFEDELETE( m_pBallTouchPoint );
		m_pBallTouchPoint = pImage;
	}
	else if( szType == "BallTimePoint" )
	{
		SAFEDELETE( m_pBallTimePoint );
		m_pBallTimePoint = pImage;
	}
	else if( szType == "GangsiInfectionPoint" )
	{
		SAFEDELETE( m_pGangsiInfectionPoint );
		m_pGangsiInfectionPoint = pImage;
	}
	else if( szType == "GangsiSurvivorPoint" )
	{
		SAFEDELETE( m_pGangsiSurvivorPoint );
		m_pGangsiSurvivorPoint = pImage;
	}
	else if( szType == "FlagPoint" )
	{
		SAFEDELETE( m_pFlagPoint );
		m_pFlagPoint = pImage;
	}
	else if( szType == "Rank_S" )
	{
		SAFEDELETE( m_pRank[0] );
		m_pRank[0] = pImage;
	}	
	else if( szType == "Rank_A" )
	{
		SAFEDELETE( m_pRank[1] );
		m_pRank[1] = pImage;
	}	
	else if( szType == "Rank_B" )
	{
		SAFEDELETE( m_pRank[2] );
		m_pRank[2] = pImage;
	}	
	else if( szType == "Rank_C" )
	{
		SAFEDELETE( m_pRank[3] );
		m_pRank[3] = pImage;
	}
	else if( szType == "Rank_Attack" )
	{
		SAFEDELETE(m_pNpcKillPts);
		m_pNpcKillPts = pImage;
	}
	else if( szType == "Rank_Defense" )
	{
		SAFEDELETE(m_pNpcTowerPts);
		m_pNpcTowerPts = pImage;
	}
	else if( szType == "Rank_Time" )
	{
		SAFEDELETE(m_pTimePts);
		m_pTimePts = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ModeRankWnd::UIPositionSave (bool bDefault )
{
	if( bDefault )
	{
		g_GUIMgr.GUIPosSaveInt( "ModeRank", "XPos", ioSP2GUIManager::SCREEN_OUT_POS, POS_SAVE_VERSION );
		g_GUIMgr.GUIPosSaveInt( "ModeRank", "YPos", ioSP2GUIManager::SCREEN_OUT_POS, POS_SAVE_VERSION );
	}

	if( m_iDefaultX == ioSP2GUIManager::SCREEN_OUT_POS ||
		m_iDefaultY == ioSP2GUIManager::SCREEN_OUT_POS )
		return;

	if( m_bPosSave )
	{
		if( m_iDefaultX != GetXPos() || m_iDefaultY != GetYPos() )
		{
			g_GUIMgr.GUIPosSaveInt( "ModeRank", "XPos", GetXPos(), POS_SAVE_VERSION );
			g_GUIMgr.GUIPosSaveInt( "ModeRank", "YPos", GetYPos(), POS_SAVE_VERSION );
		}	

		m_bPosSave = false;
	}	
}

void ModeRankWnd::iwm_show()
{
	switch( m_iRankType )
	{
	case DEATH_TYPE:
		SetPositionByRankType( DEATH_TYPE );
		break;
	case RESULT_TYPE:
		SetPositionByRankType( RESULT_TYPE );
		break;
	case TABKEY_TYPE:
		{
			int iXPos = g_GUIMgr.GUIPosLoadInt( "ModeRank", "XPos", ioSP2GUIManager::SCREEN_OUT_POS, POS_SAVE_VERSION );
			int iYPos = g_GUIMgr.GUIPosLoadInt( "ModeRank", "YPos", ioSP2GUIManager::SCREEN_OUT_POS, POS_SAVE_VERSION );

			if( COMPARE( iXPos, 0, Setting::Width() ) && COMPARE( iYPos, 0, Setting::Height() ) )
			{
				ioWnd::SetWndPos( iXPos, iYPos );
			}
			else       
			{
				SetPositionByRankType( TABKEY_TYPE );
			}

			m_iDefaultX = GetXPos();
			m_iDefaultY = GetYPos();

			m_bPosSave = true;
			m_dwCurHideTime = FRAMEGETTIME();
		}
		break;
	}

	if( m_eModeType == MT_FOOTBALL )
		return;

	if( m_pPlayStage )
	{
		m_bRollingEnd = true;
		m_dwCurTime = FRAMEGETTIME();

		int iMaxPlayer = m_pPlayStage->ContributeMyRank( m_RankData[0].m_iRank,
													     m_RankData[1].m_iRank,
														 m_RankData[2].m_iRank,
														 m_RankData[3].m_iRank );

		if( Help::IsMonsterDungeonMode(m_eModeType) ) 
		{
			for(int i = 0;i < MAX_RANK_ELEMENT;i++)
			{
				m_RankData[i].m_iCurRank = -1;
				m_RankData[i].m_dwRollingTime = 0;
				m_RankData[i].m_dwRankUPTime  = 0;

				if( m_RankData[i].m_iRank == -1 )
					continue;

				m_RankData[i].m_dwRollingTime = m_dwStartTime + ( i * m_dwGapTime );
				m_bRollingEnd = false;
			}
		}
		else
		{
			for(int i = 0;i < MAX_RANK_ELEMENT;i++)
			{
				m_RankData[i].m_iCurRank = -1;
				m_RankData[i].m_dwRollingTime = 0;
				m_RankData[i].m_dwRankUPTime  = 0;

				if( m_RankData[i].m_iRank == -1 )
					continue;

				if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.25f )
					m_RankData[i].m_iRank = S_RANK;
				else if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.50f )
					m_RankData[i].m_iRank = A_RANK;
				else if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.75f )
					m_RankData[i].m_iRank = B_RANK;
				else 
					m_RankData[i].m_iRank = C_RANK;

				m_RankData[i].m_dwRollingTime = m_dwStartTime + ( i * m_dwGapTime );
				m_bRollingEnd = false;
			}
		}

		if( !m_bRollingEnd )
		{	
			g_SoundMgr.PlaySound( m_RollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
		}
	}
}

void ModeRankWnd::iwm_hide()
{
	UIPositionSave();

	g_SoundMgr.StopSound( m_RollingSound, 0 );
	m_dwCurTime = m_dwCurHideTime = 0;
	m_iRankType = NONE_TYPE;
}

void ModeRankWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		SetPositionByRankType( m_iRankType );

		if( m_iRankType == DEATH_TYPE || m_iRankType == TABKEY_TYPE )
		{
			m_iDefaultX = GetXPos();
			m_iDefaultY = GetYPos();
		}

		m_bPosSave = false;
		UIPositionSave( true );
	}
}

void ModeRankWnd::SetPositionByRankType( int iRankType )
{
	int iXPos = 0;
	int iYPos = 0;

	switch( iRankType )
	{
	case DEATH_TYPE:
		iXPos = Setting::Width() / 2 - GetWidth() / 2;
		iYPos = Setting::Height() - 74;
		ioWnd::SetWndPos( iXPos, iYPos );
		break;
	case RESULT_TYPE:
		iXPos = Setting::Width() / 2 - GetWidth() / 2;
		iYPos = Setting::Height() / 2;

		iYPos -= (int)( (float)( Setting::Height() - 600 ) * 0.192f );
		ioWnd::SetWndPos( iXPos , iYPos - 89 );
		break;
	case TABKEY_TYPE:
		iXPos = Setting::Width() / 2 - GetWidth() / 2;
		iYPos = Setting::Height() - 197;

		if( m_eModeType == MT_SYMBOL ||
			m_eModeType == MT_KING ||
			m_eModeType == MT_TEAM_SURVIVAL ||
			m_eModeType == MT_TEAM_SURVIVAL_AI ||
			m_eModeType == MT_BOSS ||
			m_eModeType == MT_MONSTER_SURVIVAL ||
			m_eModeType == MT_FOOTBALL ||
			m_eModeType == MT_GANGSI ||
			m_eModeType == MT_DUNGEON_A || 
			m_eModeType == MT_DOUBLE_CROWN ||
			m_eModeType == MT_RAID ||
			m_eModeType == MT_FLAG_CAPTURE ||
			Help::IsMonsterDungeonMode(m_eModeType) )
		{
			iYPos = max( 0, iYPos - 21 );
		}

		iYPos -= ( Setting::Height() - 600 ) * 0.22f;
		ioWnd::SetWndPos( iXPos, iYPos );
		break;
	}
}

void ModeRankWnd::SetRank( int iRankType )
{
	if( !m_pPlayStage ) return;

	m_eModeType = m_pPlayStage->GetModeType();
	if( m_eModeType == MT_MYROOM || m_eModeType == MT_TRAINING || m_eModeType == MT_HEADQUARTERS  || m_eModeType == MT_HOUSE )
		return;

	m_iRankType = iRankType;
	if( m_iRankType == RESULT_TYPE ||
		m_iRankType == TABKEY_TYPE )
	{
		ShowWnd();
	}
	else 
	{
		m_dwCurTime = FRAMEGETTIME();
	}

	m_iAlphaRate = MAX_ALPHA_RATE;
}

void ModeRankWnd::ProcessRank()
{
	if( m_eModeType == MT_FOOTBALL )
		return;

	if( !m_bRollingEnd )
	{
		bool bRollingEnd = true;
		for(int i = 0;i < MAX_RANK_ELEMENT;i++)
		{
			if( m_RankData[i].m_iRank == -1 ) continue;
			if( m_RankData[i].m_dwRollingTime == 0 ) continue;

			if( FRAMEGETTIME() - m_dwCurTime >  m_RankData[i].m_dwRollingTime )
			{
				m_RankData[i].m_iCurRank = m_RankData[i].m_iRank;
				m_RankData[i].m_dwRollingTime = 0;

				if( !m_RollingStopSound.IsEmpty() )
				{
					g_SoundMgr.PlaySound( m_RollingStopSound );
				}
			}
			else 
			{
				bRollingEnd = false;
				if( FRAMEGETTIME() - m_RankData[i].m_dwRankUPTime > 50 )
				{
					m_RankData[i].m_dwRankUPTime = FRAMEGETTIME();		
					m_RankData[i].m_iCurRank++;

					if( !COMPARE( m_RankData[i].m_iCurRank, 0, MAX_RANK ) )
						m_RankData[i].m_iCurRank = S_RANK;					
				}
			}
		}

		if( bRollingEnd )
		{
			m_dwCurTime = FRAMEGETTIME();
			m_bRollingEnd = true;
			if( !m_RollingSound.IsEmpty() )
			{
				g_SoundMgr.StopSound( m_RollingSound, 0 );
			}
		}
	}	

	if( m_bRollingEnd )
	{
		if( FRAMEGETTIME() - m_dwCurTime > 1000 )
		{
			m_dwCurTime = FRAMEGETTIME();

			int iMaxPlayer = m_pPlayStage->ContributeMyRank( m_RankData[0].m_iRank,
															 m_RankData[1].m_iRank,
															 m_RankData[2].m_iRank,
															 m_RankData[3].m_iRank );

			if( Help::IsMonsterDungeonMode(m_eModeType) )
			{
				for(int i = 0;i < MAX_RANK_ELEMENT;i++)
				{
					if( m_RankData[i].m_iRank == -1 )
						continue;

					if( m_RankData[i].m_iCurRank != m_RankData[i].m_iRank )
					{					
						m_bRollingEnd = false;
						m_RankData[i].m_dwRollingTime = m_dwGapTime;					
					}
				}
			}
			else
			{
				for(int i = 0;i < MAX_RANK_ELEMENT;i++)
				{
					if( m_RankData[i].m_iRank == -1 )
						continue;

					if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.25f )
						m_RankData[i].m_iRank = S_RANK;
					else if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.50f )
						m_RankData[i].m_iRank = A_RANK;
					else if( (float)m_RankData[i].m_iRank < (float)iMaxPlayer * 0.75f )
						m_RankData[i].m_iRank = B_RANK;
					else 
						m_RankData[i].m_iRank = C_RANK;

					if( m_RankData[i].m_iCurRank != m_RankData[i].m_iRank )
					{					
						m_bRollingEnd = false;
						m_RankData[i].m_dwRollingTime = m_dwGapTime;					
					}
				}
			}

			if( !m_bRollingEnd )
			{
				g_SoundMgr.PlaySound( m_RollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
			}
		}
	}
}

void ModeRankWnd::DeathTypeProcessShow()
{
	if( m_iRankType != DEATH_TYPE ) return;
	if( m_dwCurTime == 0 ) return;

	if( FRAMEGETTIME() - m_dwCurTime > m_dwShowTime )
	{
		if( m_pPlayStage )
		{
			if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) ||
				g_GUIMgr.IsShow( TOTAL_RESULT_MAIN_WND ) ||
				g_GUIMgr.IsShow( BRIEFING_WND ) ||
				m_pPlayStage->IsRoundSetEnd() )
			{
				m_dwCurTime = 0;
				return;
			}
		}

		ShowWnd();
	}
}

void ModeRankWnd::TabTypeProcessHide()
{
	if( m_iRankType != TABKEY_TYPE ) return;
	if( m_dwCurHideTime == 0 ) return;

	if( FRAMEGETTIME() - m_dwCurHideTime > m_dwHideTime )
	{	
		HideWnd();
	}
}

void ModeRankWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		DeathTypeProcessShow();
		return;
	}

	if( m_iRankType == DEATH_TYPE || m_iRankType == TABKEY_TYPE )
	{
		ioMovingWnd::OnProcess( fTimePerSec );
	}

	ProcessRank();
	TabTypeProcessHide();
}

void ModeRankWnd::OnRender()
{
	// 옵저버는 출력 않함
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		return;

	// 축구모드는 출력하지 않음
	if( m_eModeType == MT_FOOTBALL )
		return;

	ioWnd::OnRender();

	switch( m_eModeType )
	{
	case MT_SURVIVAL:
	case MT_SHUFFLE_BONUS:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_SYMBOL:
		RenderRank( m_pStonePoint, m_pKillPoint, m_pAttackPoint, m_pDamagePoint );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		RenderRank( m_pNpcKillPts, m_pNpcTowerPts, m_pTimePts, NULL );
		break;
	case MT_RAID:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_KING:
	case MT_DOUBLE_CROWN:
		RenderRank( m_pKingPoint, m_pKillPoint, m_pAttackPoint, m_pDamagePoint );
		break;
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_FARMING:
		RenderRank( m_pPrisonerPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BATTLE:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_BOSS:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_MONSTER_SURVIVAL:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_FOOTBALL:
		RenderRank( m_pGoalPoint, m_pBallTouchPoint, m_pBallTimePoint, NULL );
		break;
	case MT_HEROMATCH:
		RenderRank( m_pPrisonerPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_GANGSI:
		RenderRank( m_pGangsiInfectionPoint, m_pGangsiSurvivorPoint, m_pDamagePoint, NULL );
		break;
	case MT_DUNGEON_A:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_FLAG_CAPTURE:
		RenderRank( m_pFlagPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	case MT_ARENA:
		RenderRank( m_pKillPoint, m_pAttackPoint, m_pDamagePoint, NULL );
		break;
	}
}

void ModeRankWnd::RenderRank( ioUIRenderImage *pRankValue1,
							  ioUIRenderImage *pRankValue2,
							  ioUIRenderImage *pRankValue3,
							  ioUIRenderImage *pRankValue4 )
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !pRankValue3 )
		iXPos += 35;
	if( !pRankValue4 )
		iXPos += 35;	

	if( pRankValue1 )
	{
		pRankValue1->SetAlpha( (BYTE)m_iAlphaRate );
		pRankValue1->Render( iXPos, iYPos );
	}

	if( pRankValue2 )
	{
		pRankValue2->SetAlpha( (BYTE)m_iAlphaRate );
		pRankValue2->Render( iXPos + 71, iYPos );
	}

	if( pRankValue3 )
	{
		pRankValue3->SetAlpha( (BYTE)m_iAlphaRate );
		pRankValue3->Render( iXPos + 142, iYPos );
	}

	if( pRankValue4 )
	{
		pRankValue4->SetAlpha( (BYTE)m_iAlphaRate );
		pRankValue4->Render( iXPos + 213, iYPos );
	}

	for(int i = 0;i < MAX_RANK_ELEMENT;i++)
	{
		int iCurRank = m_RankData[i].m_iCurRank;

		if( COMPARE( iCurRank, 0, MAX_RANK ) && m_pRank[iCurRank] )
		{
			m_pRank[iCurRank]->SetAlpha( (BYTE)m_iAlphaRate );
			m_pRank[iCurRank]->Render( iXPos + (i*71), iYPos );
		}
	}
}

