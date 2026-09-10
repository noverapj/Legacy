#include "StdAfx.h"

#include "../ioBaseChar.h"

#include "MiniMap.h"

//////////////////////////////////////////////////////////////////////////
MiniMap::MiniMap()
{
	m_pModeParent = NULL;

	m_fMapStartX = 0.0f;
	m_fMapStartZ = 0.0f;
	m_fMapRealWidth  = FLOAT100;
	m_fMapRealHeight = FLOAT100;
	
	m_iMiniMapOffX = 0;
	m_iMiniMapOffY = 0;
	m_iPointOffX = 0;
	m_iPointOffY = 0;

	m_iBlindOffSetY    = 0;
	m_iPreBlindOffSetY = 0;

	m_pMiniMap		 = NULL;
	m_pAreaMe        = NULL;
	
	for(int i = SPOT_USER; i < MAX_SPOT; i++)
		m_pSpot[i] = NULL;

	for(int i = TIME_ICON; i < MAX_IMAGE; i++)
		m_pExtraImg[i] = NULL;

	m_iDefaultX = 0;
	m_iDefaultY = 0;

	m_dwSpotDuration = 0;
	m_dwSpotVisibleGap = 0;
	m_dwSpotHideGap = 0;
	m_dwTime = 0;
	
	m_fSpotPositionRate = 0.0f;

	m_bShowTime = false;

	InitShowAniState();
}

MiniMap::~MiniMap()
{
	SAFEDELETE( m_pMiniMap );
	SAFEDELETE( m_pAreaMe );

	for(int i = SPOT_USER; i < MAX_SPOT; i++)
		SAFEDELETE(m_pSpot[i]);

	for(int i = TIME_ICON; i < MAX_IMAGE; i++)
		SAFEDELETE(m_pExtraImg[i]);

	if( COMPARE( m_iDefaultX, 0, Setting::Width() ) && 
		COMPARE( m_iDefaultY, 0, Setting::Height() ) )
	{
		g_GUIMgr.GUIPosSaveInt( "MiniMap", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
		g_GUIMgr.GUIPosSaveInt( "MiniMap", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
	}
}

void MiniMap::InitMapInfo( ioINILoader &rkLoader,
						   const ioAxisAlignBox &rkWorldBox,
						   ioPlayMode *pModeParent )
{
	m_pModeParent = pModeParent;

	m_iMiniMapOffX = rkLoader.LoadInt_e( "map_x_offset", 0 );
	m_iMiniMapOffY = rkLoader.LoadInt_e( "map_y_offset", 0 );

	m_iPointOffX = rkLoader.LoadInt_e( "point_x_offset", 0 );
	m_iPointOffY = rkLoader.LoadInt_e( "point_y_offset", 0 );

	m_dwTime = 0;

	D3DXVECTOR3 vMin, vMax;
	vMin = rkWorldBox.GetMinPos();
	vMax = rkWorldBox.GetMaxPos();

	m_fMapStartX = vMin.x;
	m_fMapStartZ = vMin.z;
	m_fMapRealWidth  = vMax.x - vMin.x;
	m_fMapRealHeight = vMax.z - vMin.z;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "image", "", szBuf, MAX_PATH );

	SAFEDELETE( m_pMiniMap );

	for(int i = TIME_ICON; i < MAX_IMAGE; i++)
		SAFEDELETE(m_pExtraImg[i]);

	m_bShowTime = false;

	if( strcmp( szBuf, "" ) != 0 )
	{
		m_pMiniMap = g_UIImageSetMgr.CreateImageByFullName( szBuf );
		if( !m_pMiniMap )
		{
			LOG.PrintTimeAndLog( 0, "Not MiniMap Resource!!(%s)", szBuf );
		}
	}
}

void MiniMap::SetTimeImage(ioINILoader &rkLoader)
{
	rkLoader.SetTitle_e( "MiniMapExtra" );

	m_ptOffSet[TIME_ICON].x = rkLoader.LoadInt_e( "timeicon_x", 0 );
	m_ptOffSet[TIME_ICON].y = rkLoader.LoadInt_e( "timeicon_y", 0 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "timeicon", "", szBuf, MAX_PATH );
	m_pExtraImg[TIME_ICON] = g_UIImageSetMgr.CreateImageByFullName(szBuf);


	m_ptOffSet[MAP_TITLE].x = rkLoader.LoadInt_e( "maptitle_x", 0 );
	m_ptOffSet[MAP_TITLE].y = rkLoader.LoadInt_e( "maptitle_y", 0 );

	rkLoader.LoadString_e( "maptitle", "", szBuf, MAX_PATH );
	m_pExtraImg[MAP_TITLE] = g_UIImageSetMgr.CreateImageByFullName(szBuf);

	m_ptOffSet[MAP_DIFFICULTY].x = rkLoader.LoadInt_e( "mapdifficulty_x", 0 );
	m_ptOffSet[MAP_DIFFICULTY].y = rkLoader.LoadInt_e( "mapdifficulty_y", 0 );

	rkLoader.LoadString_e( "mapdifficulty", "", szBuf, MAX_PATH );
	m_pExtraImg[MAP_DIFFICULTY] = g_UIImageSetMgr.CreateImageByFullName(szBuf);


	m_ptOffSet[TIME_NUMBER].x = rkLoader.LoadInt_e( "timenum_x", 0 );
	m_ptOffSet[TIME_NUMBER].y = rkLoader.LoadInt_e( "timenum_y", 0 );

	rkLoader.LoadString_e( "timenum", "BaseImage_Defense003#timeui_easy_num", szBuf, MAX_PATH );
	m_pExtraImg[TIME_NUMBER] = g_UIImageSetMgr.CreateImageByFullName(szBuf);

	m_ptOffSet[TIME_COLON].x = rkLoader.LoadInt_e( "timecolon_x", 0 );
	m_ptOffSet[TIME_COLON].y = rkLoader.LoadInt_e( "timecolon_y", 0 );

	rkLoader.LoadString_e( "timecolon", "", szBuf, MAX_PATH );
	m_pExtraImg[TIME_COLON] = g_UIImageSetMgr.CreateImageByFullName(szBuf);

	m_ptOffSet[TIME_DOT].x = rkLoader.LoadInt_e( "timedot_x", 0 );
	m_ptOffSet[TIME_DOT].y = rkLoader.LoadInt_e( "timedot_y", 0 );

	rkLoader.LoadString_e( "timedot", "", szBuf, MAX_PATH );
	m_pExtraImg[TIME_DOT] = g_UIImageSetMgr.CreateImageByFullName(szBuf);

	m_bShowTime = true;
}

TeamType MiniMap::GetOwnerTeam()
{
	if( m_pModeParent )
		return m_pModeParent->GetOwnerTeam();

	return TEAM_NONE;
}

int MiniMap::GetPointOffsetX( float fXPos )
{
	if( !m_pMiniMap )
		return 0;

	fXPos -= m_fMapStartX;

	int iMiniWidth =m_pMiniMap->GetWidth();
	int iOffsetX = (int)( fXPos * ( (float)iMiniWidth / m_fMapRealWidth ) );
	iOffsetX = max( 0, min( iOffsetX, iMiniWidth ) ) + m_iPointOffX;

	return iOffsetX;
}

int MiniMap::GetPointOffsetY( float fZPos )
{
	if( !m_pMiniMap )
		return 0;

	fZPos -= m_fMapStartZ;

	int iMiniHeight =m_pMiniMap->GetHeight();
	int iOffsetY = iMiniHeight - (int)( fZPos * ( (float)iMiniHeight / m_fMapRealHeight ) );
	iOffsetY = max( 0, min( iOffsetY, iMiniHeight ) ) + m_iPointOffY;

	return iOffsetY;
}

bool MiniMap::CheckEnableRenderPoint( DWORD dwGapTime )
{
	if( dwGapTime == 0 ) return true;

	if( m_dwSpotHideGap == 0 ) return true;
	if( m_dwSpotDuration == 0 ) return true;

	if( m_dwSpotVisibleGap == 0 ) return false;
	if( dwGapTime >= m_dwSpotDuration ) return true;

	DWORD dwCurGap = dwGapTime;

	bool bVisible = true;
	while( dwGapTime != 0 )
	{
		if( bVisible )
		{
			if( dwGapTime >= m_dwSpotVisibleGap )
			{
				dwGapTime -= m_dwSpotVisibleGap;
				bVisible = false;
			}
			else
			{
				dwGapTime = 0;
			}
		}
		else
		{
			if( dwGapTime >= m_dwSpotHideGap )
			{
				dwGapTime -= m_dwSpotHideGap;
				bVisible = true;
			}
			else
			{
				dwGapTime = 0;
			}
		}
	}

	return bVisible;
}

void MiniMap::AddPoint( float fXPos, float fZPos, SpotType eSpot, TeamType eTeam, DWORD dwGapTime )
{
	if( !m_pMiniMap )	return;

	SpotPoint kNewPoint;
	kNewPoint.m_Type = eSpot;
	kNewPoint.m_eTeamType = eTeam;
	kNewPoint.m_iXOffset  = GetPointOffsetX( fXPos );
	kNewPoint.m_iYOffset  = GetPointOffsetY( fZPos );
	kNewPoint.m_bRender = CheckEnableRenderPoint( dwGapTime );

	m_SpotPointList.push_back( kNewPoint );	
}

void MiniMap::AddEvent( float fXPos, float fZPos, EventType eEvent, TeamType eTeam )
{
	// 사용하지 않음
}

void MiniMap::AddNextSymbolEvent( const D3DXVECTOR3 &vPos,
								  const D3DXVECTOR3 &vDir,
								  TeamType eTeam )
{
	// 사용하지 않음
}

void MiniMap::ClearPointList()
{
	m_SpotPointList.erase( m_SpotPointList.begin(), m_SpotPointList.end() );
}

void MiniMap::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SpotUserBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_USER] );
		m_pSpot[SPOT_USER] = pImage;
	}
	else if( szType == "SpotDropUserBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_DROP_USER] );
		m_pSpot[SPOT_DROP_USER] = pImage;
	}
	else if( szType == "SpotUserRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_USER_RED] );
		m_pSpot[SPOT_USER_RED] = pImage;
	}
	else if( szType == "SpotDropUserRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_DROP_USER_RED] );
		m_pSpot[SPOT_DROP_USER_RED] = pImage;
	}
	else if( szType == "SpotUserMe" )
	{
		SAFEDELETE( m_pSpot[SPOT_ME] );
		m_pSpot[SPOT_ME] = pImage;
	}
	else if( szType == "SpotDropMe" )
	{
		SAFEDELETE( m_pSpot[SPOT_DROP_ME] );		// 같은 이미지 사용
		m_pSpot[SPOT_DROP_ME] = pImage;
	}
	else if( szType == "SpotPrisonerBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_PRISON_USER] );
		m_pSpot[SPOT_PRISON_USER] = pImage;
	}
	else if( szType == "SpotPrisonerRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_PRISON_USER_RED] );
		m_pSpot[SPOT_PRISON_USER_RED] = pImage;
	}
	else if( szType == "SpotPrisonerMe" )
	{
		SAFEDELETE( m_pSpot[SPOT_PRISON_ME] );
		m_pSpot[SPOT_PRISON_ME] = pImage;
	}
	else if( szType == "SpotSymbolBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_SYMBOL] );
		m_pSpot[SPOT_SYMBOL] = pImage;
	}
	else if( szType == "SpotSymbolRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_SYMBOL_RED] );
		m_pSpot[SPOT_SYMBOL_RED] = pImage;
	}
	else if( szType == "SpotSymbolNeutral" )
	{
		SAFEDELETE( m_pSpot[SPOT_SYMBOL_NEUTRAL] );
		m_pSpot[SPOT_SYMBOL_NEUTRAL] = pImage;
	}
	else if( szType == "SpotCrownBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_KING_USER] );
		m_pSpot[SPOT_KING_USER] = pImage;
	}
	else if( szType == "SpotCrownRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_KING_USER_RED] );
		m_pSpot[SPOT_KING_USER_RED] = pImage;
	}
	else if( szType == "SpotCrownNeutral" )
	{
		SAFEDELETE( m_pSpot[SPOT_CROWN] );
		m_pSpot[SPOT_CROWN] = pImage;
	}
	else if( szType == "SpotCrownMe" )
	{
		SAFEDELETE( m_pSpot[SPOT_KING_ME] );
		m_pSpot[SPOT_KING_ME] = pImage;
	}
	else if( szType == "SpotBossMon" )
	{
		SAFEDELETE( m_pSpot[SPOT_BOSS_MON] );
		m_pSpot[SPOT_BOSS_MON] = pImage;
	}
	else if( szType == "SpotDropBossMon" )
	{
		SAFEDELETE( m_pSpot[SPOT_DROP_BOSS_MON] );
		m_pSpot[SPOT_DROP_BOSS_MON] = pImage;
	}
	else if( szType == "SpotDeathMon" )
	{
		SAFEDELETE( m_pSpot[SPOT_DEATH_MON] );
		m_pSpot[SPOT_DEATH_MON] = pImage;
	}
	else if( szType == "SpotBall" )
	{
		SAFEDELETE( m_pSpot[SPOT_BALL] );
		m_pSpot[SPOT_BALL] = pImage;
	}
	else if( szType == "SpotBlueNexus" )
	{
		SAFEDELETE( m_pSpot[SPOT_NEXUS] );
		m_pSpot[SPOT_NEXUS] = pImage;
	}
	else if( szType == "SpotRedNexus" )
	{
		SAFEDELETE( m_pSpot[SPOT_NEXUS_RED] );
		m_pSpot[SPOT_NEXUS_RED] = pImage;
	}
	else if( szType == "SpotBlueLair" )
	{
		SAFEDELETE( m_pSpot[SPOT_LAIR] );
		m_pSpot[SPOT_LAIR] = pImage;
	}
	else if( szType == "SpotRedLair" )
	{
		SAFEDELETE( m_pSpot[SPOT_LAIR_RED] );
		m_pSpot[SPOT_LAIR_RED] = pImage;
	}
	else if( szType == "SpotBlueTower" )
	{
		SAFEDELETE( m_pSpot[SPOT_TOWER] );
		m_pSpot[SPOT_TOWER] = pImage;
	}
	else if( szType == "SpotRedTower" )
	{
		SAFEDELETE( m_pSpot[SPOT_TOWER_RED] );
		m_pSpot[SPOT_TOWER_RED] = pImage;
	}
	else if( szType == "SpotNpcBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_NPC] );
		m_pSpot[SPOT_NPC] = pImage;
	}
	else if( szType == "SpotNpcRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_NPC_RED] );
		m_pSpot[SPOT_NPC_RED] = pImage;
	}
	else if( szType == "SpotRoulette" )
	{
		SAFEDELETE( m_pSpot[SPOT_ROULETTE] );
		m_pSpot[SPOT_ROULETTE] = pImage;
	}
	else if( szType == "SpotStar" )
	{
		SAFEDELETE( m_pSpot[SPOT_STAR] );
		m_pSpot[SPOT_STAR] = pImage;
	}
	else if( szType == "SpotStarOn" )
	{
		SAFEDELETE( m_pSpot[SPOT_STAR_ON] );
		m_pSpot[SPOT_STAR_ON] = pImage;
	}
	else if( szType == "SpotShuffleMonster" )
	{
		SAFEDELETE( m_pSpot[SPOT_SHUFFFLE_MONSTER] );
		m_pSpot[SPOT_SHUFFFLE_MONSTER] = pImage;
	}
	else if( szType == "SpotShuffleBuffBox" )
	{
		SAFEDELETE( m_pSpot[SPOT_SHUFFLE_BUFFBOX] );
		m_pSpot[SPOT_SHUFFLE_BUFFBOX] = pImage;
	}
	else if( szType == "SpotAreaMe" )
	{
		SAFEDELETE( m_pAreaMe );
		m_pAreaMe = pImage;
	}
	else if( szType == "SpotDobuleCrownDropRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_DOUBLECROWN_DROP_RED] );
		m_pSpot[SPOT_DOUBLECROWN_DROP_RED] = pImage;
	}
	else if( szType == "SpotDobuleCrownDropBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_DOUBLECROWN_DROP_BLUE] );
		m_pSpot[SPOT_DOUBLECROWN_DROP_BLUE] = pImage;
	}
	else if( szType == "SpotBomb" )
	{
		SAFEDELETE( m_pSpot[SPOT_BOMB] );
		m_pSpot[SPOT_BOMB] = pImage;
	}
	else if( szType == "SpotGoal" )
	{
		SAFEDELETE( m_pSpot[SPOT_GOAL] );
		m_pSpot[SPOT_GOAL] = pImage;
	}
	else if( szType == "SpotItem" )
	{
		SAFEDELETE( m_pSpot[SPOT_ITEM] );
		m_pSpot[SPOT_ITEM] = pImage;
	}
	else if( szType == "SpotCheckPoint" )
	{
		SAFEDELETE( m_pSpot[SPOT_CHECK_POINT] );
		m_pSpot[SPOT_CHECK_POINT] = pImage;
	}
	else if( szType == "FlagNeutral" )
	{
		SAFEDELETE( m_pSpot[SPOT_FLAG_NEUTRAL] );
		m_pSpot[SPOT_FLAG_NEUTRAL] = pImage;
	}
	else if( szType == "FlagBlue" )
	{
		SAFEDELETE( m_pSpot[SPOT_FLAG] );
		m_pSpot[SPOT_FLAG] = pImage;
	}
	else if( szType == "FlagRed" )
	{
		SAFEDELETE( m_pSpot[SPOT_FLAG_RED] );
		m_pSpot[SPOT_FLAG_RED] = pImage;
	}
	else if( szType == "FlagMe" )
	{
		SAFEDELETE( m_pSpot[SPOT_FLAG_ME] );
		m_pSpot[SPOT_FLAG_ME] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MiniMap::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwSpotHideGap     = (DWORD)xElement.GetIntAttribute_e( "SpotHideGap" );
	m_dwSpotDuration    = (DWORD)xElement.GetIntAttribute_e( "SpotDuration" );
	m_dwSpotVisibleGap  = (DWORD)xElement.GetIntAttribute_e( "SpotVisibleGap" );
	m_fSpotPositionRate = xElement.GetFloatAttribute_e( "SpotPositionRate" );
}

void MiniMap::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "MiniMap", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "MiniMap", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();
}

void MiniMap::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	UpdateAniState();	

	ioMovingWnd::OnProcess( fTimePerSec );
}

void MiniMap::OnRender()
{
	if( !m_pMiniMap )	return;

	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_bShowTime)
		RenderDigitTime(iXPos, iYPos);

	iXPos += m_iMiniMapOffX;
	iYPos += m_iMiniMapOffY;

	m_pMiniMap->Render( iXPos, iYPos );

	if( m_pAreaMe )
	{
		int iSize = m_SpotPointList.size();
		for (int i = iSize - 1; i >= 0 ; i--)
		{
			const SpotPoint &rkSpot = m_SpotPointList[i];

			if( rkSpot.m_Type == SPOT_ME ||
				rkSpot.m_Type == SPOT_PRISON_ME ||
				rkSpot.m_Type == SPOT_KING_ME )
			{
				m_pAreaMe->Render( iXPos + rkSpot.m_iXOffset, iYPos + rkSpot.m_iYOffset );
				break;
			}
		}
	}

	SpotPointList::iterator iter=m_SpotPointList.begin();
	for( ; iter!=m_SpotPointList.end(); ++iter )
	{
		RenderSpotPoint( iXPos, iYPos, *iter );
	}

	
}

void MiniMap::RenderDigitTime( int nPosX, int nPosY )
{
	if( !m_pExtraImg[TIME_NUMBER] || !m_pExtraImg[TIME_DOT])
		return;

	if (m_pExtraImg[TIME_ICON])
		m_pExtraImg[TIME_ICON]->Render(nPosX + m_ptOffSet[TIME_ICON].x, nPosY + m_ptOffSet[TIME_ICON].y);

	if (m_pExtraImg[MAP_TITLE])
		m_pExtraImg[MAP_TITLE]->Render(nPosX + m_ptOffSet[MAP_TITLE].x, nPosY + m_ptOffSet[MAP_TITLE].y);

	if (m_pExtraImg[MAP_DIFFICULTY])
		m_pExtraImg[MAP_DIFFICULTY]->Render(nPosX + m_ptOffSet[MAP_DIFFICULTY].x, nPosY + m_ptOffSet[MAP_DIFFICULTY].y);

	int msec= (m_dwTime%1000) / 10;
	int sec = m_dwTime/1000;
	int hour= sec/3600;
	int min = (sec%3600)/60;
	sec = sec%60;

	int nGap = 18;

	const int nShowNum[8] = {hour/10, hour%10, min/10, min%10, sec/10, sec%10, msec/10, msec%10 };

	if( hour > 0)
	{
		int nExtraGap = 0;

		for(int i = 0; i < 8; i++)
		{
			if( (i != 0) && (i % 2 == 0))
				nExtraGap += 6;

			m_pExtraImg[TIME_NUMBER]->RenderNum(nPosX + m_ptOffSet[TIME_NUMBER].x + nExtraGap + (nGap * i), 
				nPosY + m_ptOffSet[TIME_NUMBER].y, nShowNum[i], -6.f );
		}

		m_pExtraImg[TIME_COLON]->Render(nPosX + m_ptOffSet[TIME_COLON].x, nPosY + m_ptOffSet[TIME_COLON].y);
		m_pExtraImg[TIME_COLON]->Render(nPosX + m_ptOffSet[TIME_COLON].x + 41, nPosY + m_ptOffSet[TIME_COLON].y);
		m_pExtraImg[TIME_DOT]->Render(nPosX + m_ptOffSet[TIME_DOT].x + 41, nPosY + m_ptOffSet[TIME_DOT].y);
	}
	else
	{
		int nExtraGap = 0;

		for(int i = 0; i < 6; i++)
		{
			if( (i != 0) && (i % 2 == 0))
				nExtraGap += 6;

			m_pExtraImg[TIME_NUMBER]->RenderNum(nPosX + m_ptOffSet[TIME_NUMBER].x + nExtraGap + (nGap * i), 
				nPosY + m_ptOffSet[TIME_NUMBER].y, nShowNum[i+2], -6.f );
		}

		m_pExtraImg[TIME_COLON]->Render(nPosX + m_ptOffSet[TIME_COLON].x, nPosY + m_ptOffSet[TIME_COLON].y);
		m_pExtraImg[TIME_DOT]->Render(nPosX + m_ptOffSet[TIME_DOT].x, nPosY + m_ptOffSet[TIME_DOT].y);
	}
}

void MiniMap::RenderSpotPoint( int iXPos, int iYPos, const SpotPoint &rkPoint )
{
	if( !rkPoint.m_bRender ) return;

	switch( rkPoint.m_Type )
	{
	case SPOT_USER:
	case SPOT_DROP_USER:
	case SPOT_PRISON_USER:
	case SPOT_NPC:
	case SPOT_NEXUS:
	case SPOT_LAIR:
	case SPOT_TOWER:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if(	rkPoint.m_Type < MAX_SPOT && m_pSpot[rkPoint.m_Type] )
				m_pSpot[rkPoint.m_Type]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( (rkPoint.m_Type+1) < MAX_SPOT && m_pSpot[rkPoint.m_Type+1] )
				m_pSpot[rkPoint.m_Type+1]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;

	case SPOT_ME:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_USER] )
				m_pSpot[SPOT_USER]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( m_pSpot[SPOT_USER_RED] )
				m_pSpot[SPOT_USER_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		if( m_pSpot[SPOT_ME] )
			m_pSpot[SPOT_ME]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;
	case SPOT_DROP_ME:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_DROP_USER] )
				m_pSpot[SPOT_DROP_USER]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( m_pSpot[SPOT_DROP_USER_RED] )
				m_pSpot[SPOT_DROP_USER_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		if( m_pSpot[SPOT_DROP_ME] )
			m_pSpot[SPOT_DROP_ME]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;

	case SPOT_SYMBOL:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_SYMBOL] )
				m_pSpot[SPOT_SYMBOL]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else if( rkPoint.m_eTeamType == TEAM_RED )
		{
			if( m_pSpot[SPOT_SYMBOL_RED] )
				m_pSpot[SPOT_SYMBOL_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( m_pSpot[SPOT_SYMBOL_NEUTRAL] )
				m_pSpot[SPOT_SYMBOL_NEUTRAL]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;
	case SPOT_KING_USER:
	case SPOT_CROWN:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_KING_USER] )
				m_pSpot[SPOT_KING_USER]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else if( rkPoint.m_eTeamType == TEAM_RED )
		{
			if( m_pSpot[SPOT_KING_USER_RED] )
				m_pSpot[SPOT_KING_USER_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( m_pSpot[SPOT_CROWN] )
				m_pSpot[SPOT_CROWN]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;
	case SPOT_DOUBLECROWN_DROP_RED:
		{
			if( m_pSpot[SPOT_DOUBLECROWN_DROP_RED] )
				m_pSpot[SPOT_DOUBLECROWN_DROP_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;
	case SPOT_DOUBLECROWN_DROP_BLUE:
		{
			if( m_pSpot[SPOT_DOUBLECROWN_DROP_BLUE] )
				m_pSpot[SPOT_DOUBLECROWN_DROP_BLUE]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;
	case SPOT_PRISON_ME:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_PRISON_USER] )
				m_pSpot[SPOT_PRISON_USER]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else
		{
			if( m_pSpot[SPOT_PRISON_USER_RED] )
				m_pSpot[SPOT_PRISON_USER_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		if( m_pSpot[SPOT_PRISON_ME] )
			m_pSpot[SPOT_PRISON_ME]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;

	case SPOT_KING_ME:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_KING_USER] )
				m_pSpot[SPOT_KING_USER]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else if( rkPoint.m_eTeamType == TEAM_RED )
		{
			if( m_pSpot[SPOT_KING_USER_RED] )
				m_pSpot[SPOT_KING_USER_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		if( m_pSpot[SPOT_KING_ME] )
			m_pSpot[SPOT_KING_ME]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;

	case SPOT_FLAG:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_FLAG] )
				m_pSpot[SPOT_FLAG]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else if( rkPoint.m_eTeamType == TEAM_RED )
		{
			if( m_pSpot[SPOT_FLAG_RED] )
				m_pSpot[SPOT_FLAG_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		break;

	case SPOT_FLAG_ME:
		if( rkPoint.m_eTeamType == TEAM_BLUE )
		{
			if( m_pSpot[SPOT_FLAG] )
				m_pSpot[SPOT_FLAG]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		else if( rkPoint.m_eTeamType == TEAM_RED )
		{
			if( m_pSpot[SPOT_FLAG_RED] )
				m_pSpot[SPOT_FLAG_RED]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		}
		if( m_pSpot[SPOT_FLAG_ME] )
			m_pSpot[SPOT_FLAG_ME]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;

	case SPOT_FLAG_NEUTRAL:
		if( m_pSpot[SPOT_FLAG_NEUTRAL] )
			m_pSpot[SPOT_FLAG_NEUTRAL]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;

	case SPOT_BOMB:
	case SPOT_CHECK_POINT:
	case SPOT_GOAL:
	case SPOT_ITEM:
	case SPOT_DEATH_MON:
	case SPOT_BOSS_MON:
	case SPOT_DROP_BOSS_MON:
	case SPOT_BALL:
	case SPOT_ROULETTE:
	case SPOT_STAR:
	case SPOT_STAR_ON:
	case SPOT_SHUFFFLE_MONSTER:
	case SPOT_SHUFFLE_BUFFBOX:
		if(	rkPoint.m_Type < MAX_SPOT && m_pSpot[rkPoint.m_Type] )
			m_pSpot[rkPoint.m_Type]->Render( iXPos + rkPoint.m_iXOffset, iYPos + rkPoint.m_iYOffset );
		break;
	}
}

void MiniMap::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY);
		pState->Add( SAT_NONE,  SAT_NONE);
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void MiniMap::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// blind ani
	if( m_iBlindOffSetY != m_iPreBlindOffSetY )
	{
		int iCurYPos = m_iDefaultY + m_iBlindOffSetY;
		if( COMPARE( iCurYPos, 0, ( Setting::Height() - GetHeight() ) + 1) )
		{
			SetWndPos( m_iDefaultX,  iCurYPos );
			m_iPreBlindOffSetY = m_iBlindOffSetY;
		}
	}
}

void MiniMap::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool MiniMap::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { MINUS_Y_POS = 140 };
		int iStartYPos = MINUS_Y_POS + m_iDefaultY;
		int iCurYpos = m_iDefaultY - ( cos( (D3DX_PI * FLOAT05 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (D3DX_PI * FLOAT05 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (D3DX_PI * FLOAT05 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
		return true;
	}

	return false;
}

int MiniMap::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void MiniMap::UpdateDefaultPos()
{
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos() - m_iBlindOffSetY;
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = iYPos;
		}
	}
}

void MiniMap::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos - fPosGap;
	m_iDefYPos = iTempDefYPos + fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();

		SetWndPos( m_iDefaultX, m_iDefaultY + m_iBlindOffSetY );
	}
}
