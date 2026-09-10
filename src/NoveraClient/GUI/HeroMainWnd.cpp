

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioDateHelp.h"
#include "../ioRankBattleManager.h"

#include "LobbyMainWnd.h"
#include "GuildWnd.h"
#include "LadderTeamWnd.h"
#include "CampMainWnd.h"
#include "HeroMainWnd.h"

HeroMainWnd::HeroMainWnd()
{
	m_dwTabID = ID_HERO_MATCH_BTN;
}

HeroMainWnd::~HeroMainWnd()
{
	
}

void HeroMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/heromatchwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/influencewnd.xml", this );
}

void HeroMainWnd::iwm_show()
{	
	//
	g_CampMgr.SyncCampData();

	if( g_CampMgr.GetNextTodayBattleSec() == -1 )
	{	
		CheckCampButton( ID_CAMP_INFO_BTN );
	}
	else
	{
		CheckCampButton( ID_HERO_MATCH_BTN );	
	}	
}

void HeroMainWnd::iwm_hide()
{
}

void HeroMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_HERO_MATCH_BTN:
	case ID_CAMP_INFO_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckCampButton( dwID );
		}
		break;
	}
}

void HeroMainWnd::CheckCampButton( DWORD dwID )
{
	m_dwTabID = dwID;
	CheckRadioButton( ID_HERO_MATCH_BTN, ID_CAMP_INFO_BTN, m_dwTabID );

	HideChildWnd( ID_HERO_MATCH_WND );
	HideChildWnd( ID_CAMP_INFO_WND );
	switch( m_dwTabID )
	{
	case ID_HERO_MATCH_BTN:
		{
			ShowChildWnd( ID_HERO_MATCH_WND );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>(g_GUIMgr.FindWnd( LOBBY_MAIN_WND ));
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
				{
					pLobbyMainWnd->ShowChatWnd( true );
				}
			}
			else
			{
				//광장에서는 진영전 내부 윈도우에 채팅 없음
			}
		}
		break;
	case ID_CAMP_INFO_BTN:
		{
			InfluenceWnd *pCampInfoWnd = dynamic_cast<InfluenceWnd*>(FindChildWnd( ID_CAMP_INFO_WND ));
			if( pCampInfoWnd )
			{
				pCampInfoWnd->ShowWnd();
			}

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>(g_GUIMgr.FindWnd( LOBBY_MAIN_WND ));
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
				{
					pLobbyMainWnd->ShowChatWnd( false );
				}
			}
			else
			{
				//광장에서는 진영전 내부 윈도우에 채팅 없음				
			}
		}
		break;
	}
}

void HeroMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void HeroMainWnd::OnRender()
{
	ioWnd::OnRender();
}
//////////////////////////////////////////////////////////////////////////
HeroMatchRankBtn::HeroMatchRankBtn()
{
	
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;

	m_pBlueBar = NULL;
	m_pRedBar  = NULL;
	m_pGrayBar = NULL;
	m_pBlueOverBar	= NULL;
	m_pRedOverBar	= NULL;
	m_pGrayOverBar	= NULL;

	m_pBlueGra = NULL;
	m_pRedGra  = NULL;
	m_pGrayGra = NULL;

	m_pBlueOverGra = NULL;
	m_pRedOverGra  = NULL;
	m_pGrayOverGra = NULL;

	m_pIconBack		= NULL;
	m_pNoneIcon     = NULL;
	m_pHeroRankTitle = NULL;
	m_pNoneText		 = NULL;
	m_pNumber		 = NULL;
	m_pRankText		 = NULL;

	m_pCurrentBar    = NULL;
	m_pCurrentOverBar= NULL;
	m_pCurrentGra	 = NULL;
	m_pCurrentOverGra= NULL;
}

HeroMatchRankBtn::~HeroMatchRankBtn()
{
	SAFEDELETE( m_pBlueBar );
	SAFEDELETE( m_pRedBar );
	SAFEDELETE( m_pGrayBar );
	SAFEDELETE( m_pBlueOverBar );
	SAFEDELETE( m_pRedOverBar );
	SAFEDELETE( m_pGrayOverBar );
	SAFEDELETE( m_pBlueGra );
	SAFEDELETE( m_pRedGra );
	SAFEDELETE( m_pGrayGra );
	SAFEDELETE( m_pBlueOverGra );
	SAFEDELETE( m_pRedOverGra );
	SAFEDELETE( m_pGrayOverGra );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pHeroRankTitle );
	SAFEDELETE( m_pNoneText );
	SAFEDELETE( m_pNumber );
	SAFEDELETE( m_pRankText );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void HeroMatchRankBtn::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void HeroMatchRankBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueBar" )
	{
		SAFEDELETE( m_pBlueBar );
		m_pBlueBar = pFrame;
	}
	else if( szType == "RedBar" )
	{
		SAFEDELETE( m_pRedBar );
		m_pRedBar = pFrame;
	}
	else if( szType == "GrayBar" )
	{
		SAFEDELETE( m_pGrayBar );
		m_pGrayBar = pFrame;
	}
	else if( szType == "BlueOverBar" )
	{
		SAFEDELETE( m_pBlueOverBar );
		m_pBlueOverBar = pFrame;
	}
	else if( szType == "RedOverBar" )
	{
		SAFEDELETE( m_pRedOverBar );
		m_pRedOverBar = pFrame;
	}
	else if( szType == "GrayOverBar" )
	{
		SAFEDELETE( m_pGrayOverBar );
		m_pGrayOverBar = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void HeroMatchRankBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueGra" )
	{
		SAFEDELETE( m_pBlueGra );
		m_pBlueGra = pImage;
	}
	else if( szType == "RedGra" )
	{
		SAFEDELETE( m_pRedGra );
		m_pRedGra = pImage;
	}
	else if( szType == "GrayGra" )
	{
		SAFEDELETE( m_pGrayGra );
		m_pGrayGra = pImage;
	}
	else if( szType == "BlueOverGra" )
	{
		SAFEDELETE( m_pBlueOverGra );
		m_pBlueOverGra = pImage;
	}
	else if( szType == "RedOverGra" )
	{
		SAFEDELETE( m_pRedOverGra );
		m_pRedOverGra = pImage;
	}
	else if( szType == "GrayOverGra" )
	{
		SAFEDELETE( m_pGrayOverGra );
		m_pGrayOverGra = pImage;
	}
	else if( szType == "HeroRankTitle" )
	{
		SAFEDELETE( m_pHeroRankTitle );
		m_pHeroRankTitle = pImage;
	}
	else if( szType == "Number" )
	{
		SAFEDELETE( m_pNumber );
		m_pNumber = pImage;
	}
	else if( szType == "RankText" )
	{
		SAFEDELETE( m_pRankText );
		m_pRankText = pImage;
	}
	else if( szType == "NoneText" )
	{
		SAFEDELETE( m_pNoneText );
		m_pNoneText = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "NoneIcon" )
	{
		SAFEDELETE( m_pNoneIcon );
		m_pNoneIcon = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void HeroMatchRankBtn::CreateButton()
{
	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		m_pCurrentBar		= m_pBlueBar;
		m_pCurrentOverBar	= m_pBlueOverBar;
		m_pCurrentGra		= m_pBlueGra;
		m_pCurrentOverGra	= m_pBlueOverGra;
	}
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
	{
		m_pCurrentBar		= m_pRedBar;
		m_pCurrentOverBar	= m_pRedOverBar;
		m_pCurrentGra		= m_pRedGra;
		m_pCurrentOverGra	= m_pRedOverGra;
	}
	else
	{
		m_pCurrentBar		= m_pGrayBar;
		m_pCurrentOverBar	= m_pGrayOverBar;
		m_pCurrentGra		= m_pGrayGra;
		m_pCurrentOverGra	= m_pGrayOverGra;
	}

	SAFEDELETE( m_aMyChar );

	DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
	m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
	if( m_CharInfo.m_class_type == 0 )
		return;

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( m_CharInfo, "ResultChar" );
	if( !pChar ) return;

	pChar->SetPosition( 0.0f, -FLOAT90, 900.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (??)
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			DWORD dwItemCode = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			DWORD dwItemMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwItemFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwItemMaleCustom = 0;
					dwItemFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
			else
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwItemMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
					dwItemFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
				}				
				pChar->EquipItem( dwItemCode, dwItemMaleCustom, dwItemFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
		}
	}

	ioHashString kMotion = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_7, m_aMyChar->IsMale(), "" );
	if( kMotion.IsEmpty() )
	{
		if( !m_aMyChar->SetWeaponShopMotion() )
		{
			ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
			if( !szUIAnimation.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( szUIAnimation );
			}
		}
	}
	else
	{
		m_aMyChar->SetMotionItemAni( kMotion );
	}
}

void HeroMatchRankBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void HeroMatchRankBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( ( IsOver() || IsClicked() ) && m_pCurrentOverBar )
		m_pCurrentOverBar->Render( iXPos, iYPos );
	else if( m_pCurrentBar )
		m_pCurrentBar->Render( iXPos, iYPos );
	// 3D Char
	if( m_aMyChar )
	{
		OnRenderSoldier( iXPos + 3, iYPos + 3 );
	}
	else if( m_pNoneIcon && m_pIconBack )
	{
		if( m_pIconBack )
			m_pIconBack->Render( iXPos + 63, iYPos + 61, UI_RENDER_MULTIPLY );
		if( m_pNoneIcon )
			m_pNoneIcon->Render( iXPos + 63, iYPos + 61 );
	}

	if( ( IsOver() || IsClicked() ) && m_pCurrentOverGra )
		m_pCurrentOverGra->Render( iXPos, iYPos );
	else if( m_pCurrentGra )
		m_pCurrentGra->Render( iXPos, iYPos );

	//
	if( m_pHeroRankTitle )
		m_pHeroRankTitle->Render( iXPos + 63, iYPos + 113 );

	int iHeroRank = g_MyInfo.GetHeroTodayRank();
	int iNumberCount = GetNumberCount( iHeroRank );
	if( iHeroRank == 0 || iNumberCount >= 6 )
	{
		if( m_pNoneText )
			m_pNoneText->Render( iXPos + 63, iYPos + 132 );
	}
	else
	{
		enum { NUM_W = 27, NUM_GAP = 5, TEXT_W = 27,};
		if( iNumberCount >= 5 )
		{
			// 위 텍스트 찍지 않음
			if( m_pNumber )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pNumber->RenderNum( iXPos + 63, iYPos + 132, iHeroRank, -NUM_GAP );
			}
		}
		else
		{
			int iNumberSize = (iNumberCount*NUM_W) - ((iNumberCount-1)*NUM_GAP);
			int iStartPos   = (iNumberSize + TEXT_W) / 2;
			if( m_pNumber && m_pRankText )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
				m_pNumber->RenderNum( (iXPos + 63) - iStartPos, iYPos + 132, iHeroRank, -NUM_GAP );

				m_pRankText->Render( ((iXPos + 63) - iStartPos)+(iNumberSize-NUM_GAP), iYPos + 132 );
			}
		}
	}
}

void HeroMatchRankBtn::OnRenderSoldier( int iXPos, int iYPos )
{
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}
//////////////////////////////////////////////////////////////////////////
HeroMatchWnd::HeroMatchWnd()
{
	m_pBlueBar           = NULL;
	m_pRedBar            = NULL;
	m_pGrayBar           = NULL;
	m_pBlueGra           = NULL;
	m_pRedGra            = NULL;
	m_pGrayGra           = NULL;
	m_pBlueOverGra       = NULL;
	m_pRedOverGra        = NULL;
	m_pGrayOverGra       = NULL;
	m_pHeroRankTitle     = NULL;
	m_pSeasonHeroTitle	 = NULL;
	m_pSeasonRecordTitle = NULL;
	m_pIconBack		     = NULL;
	m_pRecordIcon	     = NULL;
	m_pNumber		     = NULL;
	m_pRankText		     = NULL;
	m_pPercentText	     = NULL;
	m_pNoneText          = NULL;
	m_pNoneIcon          = NULL;

	m_pCurrentBar        = NULL;
	m_pCurrentGra        = NULL;
	m_pCurrentOverGra    = NULL;
	m_iMyCampType        = CAMP_NONE;

	m_pDarkBar           = NULL;
	m_pLightBar          = NULL;

	m_pBlueBarByPlaza    = NULL;
	m_pRedBarByPlaza     = NULL;
	m_pGrayBarByPlaza    = NULL;

	m_pBlueGraByPlaza    = NULL;
	m_pRedGraByPlaza     = NULL;
	m_pGrayGraByPlaza    = NULL;

	m_pTextLine			 = NULL;
}

HeroMatchWnd::~HeroMatchWnd()
{
	SAFEDELETE( m_pBlueBar );
	SAFEDELETE( m_pRedBar );
	SAFEDELETE( m_pGrayBar );
	SAFEDELETE( m_pBlueGra );
	SAFEDELETE( m_pRedGra );
	SAFEDELETE( m_pGrayGra );
	SAFEDELETE( m_pBlueOverGra );
	SAFEDELETE( m_pRedOverGra );
	SAFEDELETE( m_pGrayOverGra );
	SAFEDELETE( m_pHeroRankTitle );
	SAFEDELETE( m_pSeasonHeroTitle );
	SAFEDELETE( m_pSeasonRecordTitle );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pRecordIcon );
	SAFEDELETE( m_pNumber );
	SAFEDELETE( m_pRankText );
	SAFEDELETE( m_pPercentText );
	SAFEDELETE( m_pNoneText );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );

	SAFEDELETE( m_pBlueBarByPlaza );
	SAFEDELETE( m_pRedBarByPlaza );
	SAFEDELETE( m_pGrayBarByPlaza );
	SAFEDELETE( m_pBlueGraByPlaza );
	SAFEDELETE( m_pRedGraByPlaza );
	SAFEDELETE( m_pGrayGraByPlaza );

	SAFEDELETE( m_pTextLine );
}

void HeroMatchWnd::iwm_create()
{
}

void HeroMatchWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueBar" )
	{
		SAFEDELETE( m_pBlueBar );
		m_pBlueBar = pFrame;
	}
	else if( szType == "RedBar" )
	{
		SAFEDELETE( m_pRedBar );
		m_pRedBar = pFrame;
	}
	else if( szType == "GrayBar" )
	{
		SAFEDELETE( m_pGrayBar );
		m_pGrayBar = pFrame;
	}
	else if( szType == "BlueBarByPlaza" )
	{
		SAFEDELETE( m_pBlueBarByPlaza );
		m_pBlueBarByPlaza = pFrame;
	}
	else if( szType == "RedBarByPlaza" )
	{
		SAFEDELETE( m_pRedBarByPlaza );
		m_pRedBarByPlaza = pFrame;
	}
	else if( szType == "GrayBarByPlaza" )
	{
		SAFEDELETE( m_pGrayBarByPlaza );
		m_pGrayBarByPlaza = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void HeroMatchWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueGra" )
	{
		SAFEDELETE( m_pBlueGra );
		m_pBlueGra = pImage;
	}
	else if( szType == "RedGra" )
	{
		SAFEDELETE( m_pRedGra );
		m_pRedGra = pImage;
	}
	else if( szType == "GrayGra" )
	{
		SAFEDELETE( m_pGrayGra );
		m_pGrayGra = pImage;
	}
	else if( szType == "BlueOverGra" )
	{
		SAFEDELETE( m_pBlueOverGra );
		m_pBlueOverGra = pImage;
	}
	else if( szType == "RedOverGra" )
	{
		SAFEDELETE( m_pRedOverGra );
		m_pRedOverGra = pImage;
	}
	else if( szType == "GrayOverGra" )
	{
		SAFEDELETE( m_pGrayOverGra );
		m_pGrayOverGra = pImage;
	}
	else if( szType == "HeroRankTitle" )
	{
		SAFEDELETE( m_pHeroRankTitle );
		m_pHeroRankTitle = pImage;
	}
	else if( szType == "SeasonHeroTitle" )
	{
		SAFEDELETE( m_pSeasonHeroTitle );
		m_pSeasonHeroTitle = pImage;
	}
	else if( szType == "SeasonRecordTitle" )
	{
		SAFEDELETE( m_pSeasonRecordTitle );
		m_pSeasonRecordTitle = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "RecordIcon" )
	{
		SAFEDELETE( m_pRecordIcon );
		m_pRecordIcon = pImage;
	}
	else if( szType == "Number" )
	{
		SAFEDELETE( m_pNumber );
		m_pNumber = pImage;
	}
	else if( szType == "RankText" )
	{
		SAFEDELETE( m_pRankText );
		m_pRankText = pImage;
	}
	else if( szType == "PercentText" )
	{
		SAFEDELETE( m_pPercentText );
		m_pPercentText = pImage;
	}
	else if( szType == "NoneText" )
	{
		SAFEDELETE( m_pNoneText );
		m_pNoneText = pImage;
	}
	else if( szType == "NoneIcon" )
	{
		SAFEDELETE( m_pNoneIcon );
		m_pNoneIcon = pImage;
	}
	else if( szType == "DarkBar" )
	{
		SAFEDELETE( m_pDarkBar );
		m_pDarkBar = pImage;
	}
	else if( szType == "LightBar" )
	{
		SAFEDELETE( m_pLightBar );
		m_pLightBar = pImage;
	}
	else if( szType == "BlueGraByPlaza" )
	{
		SAFEDELETE( m_pBlueGraByPlaza );
		m_pBlueGraByPlaza = pImage;
	}
	else if( szType == "RedGraByPlaza" )
	{
		SAFEDELETE( m_pRedGraByPlaza );
		m_pRedGraByPlaza = pImage;
	}
	else if( szType == "GrayGraByPlaza" )
	{
		SAFEDELETE( m_pGrayGraByPlaza );
		m_pGrayGraByPlaza = pImage;
	}
	else if( szType == "TextLine" )
	{
		SAFEDELETE( m_pTextLine );
		m_pTextLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void HeroMatchWnd::ChangeCamp()
{
	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		HideChildWnd( ID_RED_TEAM_JOIN );
		HideChildWnd( ID_NONE_TEAM_JOIN );
		ShowChildWnd( ID_BLUE_TEAM_JOIN );

		HideChildWnd( ID_RED_RANKING );
		HideChildWnd( ID_NONE_RANKING );
		ShowChildWnd( ID_BLUE_RANKING );	

		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			m_pCurrentBar = m_pBlueBar;
			m_pCurrentGra = m_pBlueGra;
			m_pCurrentOverGra = m_pBlueOverGra;
		}
		else
		{
			m_pCurrentBar = m_pBlueBarByPlaza;
			m_pCurrentGra = m_pBlueGraByPlaza;
			m_pCurrentOverGra = m_pBlueOverGra;
		}

		m_iMyCampType = g_MyInfo.GetUserCampPos();
	}
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
	{
		ShowChildWnd( ID_RED_TEAM_JOIN );
		HideChildWnd( ID_BLUE_TEAM_JOIN );
		HideChildWnd( ID_NONE_TEAM_JOIN );

		ShowChildWnd( ID_RED_RANKING );
		HideChildWnd( ID_BLUE_RANKING );
		HideChildWnd( ID_NONE_RANKING );

		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			m_pCurrentBar = m_pRedBar;
			m_pCurrentGra = m_pRedGra;
			m_pCurrentOverGra = m_pRedOverGra;
		}
		else
		{
			m_pCurrentBar = m_pRedBarByPlaza;
			m_pCurrentGra = m_pRedGraByPlaza;
			m_pCurrentOverGra = m_pRedOverGra;
		}
		m_iMyCampType = g_MyInfo.GetUserCampPos();
	}
	else
	{
		HideChildWnd( ID_RED_TEAM_JOIN );
		HideChildWnd( ID_BLUE_TEAM_JOIN );
		ShowChildWnd( ID_NONE_TEAM_JOIN );

		HideChildWnd( ID_RED_RANKING );
		HideChildWnd( ID_BLUE_RANKING );
		ShowChildWnd( ID_NONE_RANKING );
		
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			m_pCurrentBar = m_pGrayBar;
			m_pCurrentGra = m_pGrayGra;
			m_pCurrentOverGra = m_pGrayOverGra;
		}
		else
		{
			m_pCurrentBar = m_pGrayBarByPlaza;
			m_pCurrentGra = m_pGrayGraByPlaza;
			m_pCurrentOverGra = m_pGrayOverGra;
		}
		m_iMyCampType = g_MyInfo.GetUserCampPos();
	}

	HideChildWnd( ID_MY_RANK_BTN );
	HideChildWnd( PLAZA_HERO_MATCH_INFO_BTN );
	HideChildWnd( PLAZA_HERO_MATCH_TEXT );

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		HeroMatchRankBtn *pMatchRankBtn = dynamic_cast<HeroMatchRankBtn *>( FindChildWnd( ID_MY_RANK_BTN ) );
		if( pMatchRankBtn )
			pMatchRankBtn->CreateButton();

		ShowChildWnd( ID_MY_RANK_BTN );
	}
	else
	{
		LadderBattleHeroRankBtn *pBattleRankBtn = dynamic_cast<LadderBattleHeroRankBtn *>( FindChildWnd( PLAZA_HERO_MATCH_INFO_BTN ) );
		if( pBattleRankBtn )
			pBattleRankBtn->CreateButton();

		ShowChildWnd( PLAZA_HERO_MATCH_INFO_BTN );
		ShowChildWnd( PLAZA_HERO_MATCH_TEXT );
	}
}

void HeroMatchWnd::iwm_show()
{	
	ChangeCamp();
}

void HeroMatchWnd::iwm_hide()
{
}

void HeroMatchWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_BLUE_TEAM_JOIN:
	case ID_RED_TEAM_JOIN:
	case ID_NONE_TEAM_JOIN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				if( g_MyInfo.GetUserCampPos() == CAMP_NONE )
				{
					g_GUIMgr.ShowWnd( CAMP_JOIN_NEED_POPUP );
				}
				else if( g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetHeroMatchLimitGrade() )
				{
					char szGradeName[MAX_PATH] = "";
					g_LevelMgr.GetGradeName( g_LadderTeamMgr.GetHeroMatchLimitGrade(), szGradeName, sizeof( szGradeName ), false );
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szGradeName );
				}
				else if( g_CampMgr.IsCloseCampBattle() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), g_CampMgr.GetPlayPreceedMent().c_str() );
				}
				else if( g_BattleRoomMgr.IsBattleRoom() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );					
				}
				else if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				}
				else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
				}
				else
				{
					// 생성
					int iModeIndex = g_ModeSelectInfo.GetLadderTeamHeroMatchIndex();
					g_LadderTeamMgr.SetSelectModeNMap( iModeIndex, -1 );
					SP2Packet kPacket( CTPK_CREATE_LADDERTEAM );
					kPacket << iModeIndex << g_MyInfo.GetPublicID() << "" << g_ModeSelectInfo.GetLadderTeamDefaultPlayer( iModeIndex ) 
							<< 0 << g_ModeSelectInfo.IsLadderTeamHeroMatchMode( iModeIndex );
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	case ID_BLUE_RANKING:
	case ID_RED_RANKING:
	case ID_NONE_RANKING:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( LADDER_RANKING_LIST_WND ) )
				g_GUIMgr.HideWnd( LADDER_RANKING_LIST_WND );
			else
			{
				LadderRankingListWnd *pRankList = dynamic_cast<LadderRankingListWnd*>(g_GUIMgr.FindWnd( LADDER_RANKING_LIST_WND ));
				if( pRankList )
					pRankList->ShowTabWnd( LadderRankingListWnd::ID_HERO_RANKING );
			}
		}
		break;
	case ID_MY_RANK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenMannerWnd( g_MyInfo.GetPublicID().c_str(), true );
		}
		break;
	}
}

void HeroMatchWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_iMyCampType != g_MyInfo.GetUserCampPos() )
		ChangeCamp();
}

void HeroMatchWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		OnRenderHeroTitle( iXPos + 138, iYPos + 58 );
		OnRenderSeasonRecord( iXPos + 266, iYPos + 58 );
	}
	else
	{
		OnRenderHeroTitleByPlaza( iXPos + 138, iYPos + 58 );
		OnRenderSeasonRecordByPlaza( iXPos + 266, iYPos + 58 );

		if( m_pTextLine )
		{
			int iOffset = 0;
			for( int i = 0; i < 5; ++i )
			{
				m_pTextLine->Render( iXPos, iYPos + iOffset, UI_RENDER_MULTIPLY );
				iOffset += 18;
			}
		}
	}
}

void HeroMatchWnd::OnRenderHeroTitle( int iXPos, int iYPos )
{
	if( m_pCurrentBar )
		m_pCurrentBar->Render( iXPos, iYPos );
	if( m_pCurrentGra )
		m_pCurrentGra->Render( iXPos, iYPos );

	//
	if( m_pSeasonHeroTitle )
		m_pSeasonHeroTitle->Render( iXPos + 63, iYPos + 113 );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 63, iYPos + 61, UI_RENDER_MULTIPLY );

	g_LevelMgr.RenderHeroTitleIcon( g_MyInfo.GetHeroTitle(), iXPos + 63, iYPos + 61 );
	g_LevelMgr.RenderHeroTitleText( g_MyInfo.GetHeroTitle(), iXPos + 63, iYPos + 132 );
}

void HeroMatchWnd::OnRenderSeasonRecord( int iXPos, int iYPos )
{
	if( m_pCurrentBar )
		m_pCurrentBar->Render( iXPos, iYPos );
	if( m_pCurrentGra )
		m_pCurrentGra->Render( iXPos, iYPos );

	//
	if( m_pSeasonRecordTitle )
		m_pSeasonRecordTitle->Render( iXPos + 63, iYPos + 113 );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 63, iYPos + 61, UI_RENDER_MULTIPLY );
	if( m_pRecordIcon )
		m_pRecordIcon->Render( iXPos + 63, iYPos + 61 );

	int iRecordRate  = g_MyInfo.GetHeroSeasonWinLoseRate() * 100;
	int iNumberCount = GetNumberCount( iRecordRate );
	enum { NUM_W = 27, NUM_GAP = 5, TEXT_W = 31,};
	int iNumberSize = (iNumberCount*NUM_W) - ((iNumberCount-1)*NUM_GAP);
	int iStartPos   = (iNumberSize + TEXT_W) / 2;
	if( m_pNumber && m_pPercentText )
	{
		m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pNumber->RenderNum( (iXPos + 63) - iStartPos, iYPos + 132, iRecordRate, -NUM_GAP );

		m_pPercentText->Render( ((iXPos + 63) - iStartPos)+(iNumberSize-NUM_GAP), iYPos + 132 );
	}
}

void HeroMatchWnd::OnRenderHeroTitleByPlaza( int iXPos, int iYPos )
{
	if( m_pCurrentBar )
		m_pCurrentBar->Render( iXPos, iYPos );
	if( m_pCurrentGra )
		m_pCurrentGra->Render( iXPos, iYPos );

	//
	if( m_pSeasonHeroTitle )
		m_pSeasonHeroTitle->Render( iXPos + 63, iYPos + 113 );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 63, iYPos + 61, UI_RENDER_MULTIPLY );

	g_LevelMgr.RenderHeroTitleIcon( g_MyInfo.GetHeroTitle(), iXPos + 63, iYPos + 61 );
	g_LevelMgr.RenderHeroTitleText( g_MyInfo.GetHeroTitle(), iXPos + 63, iYPos + 132 );

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	//if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		g_FontMgr.SetBkColor( 25, 70, 125 );
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
		g_FontMgr.SetBkColor( 120, 31, 19 );
	else
		g_FontMgr.SetBkColor( 110, 110, 110 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 182, FONT_SIZE_13, "-" );
	if( g_MyInfo.GetHeroTitle() == 0 )
		g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, "-" );
	else
	{
		int iBonus = (float)( g_LevelMgr.GetHeroTitleBonus( g_MyInfo.GetHeroTitle() ) * FLOAT100 ) + 0.001f;
		g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, "페소 + %d％", iBonus );
	}
	g_FontMgr.PrintText( iXPos + 63, iYPos + 224, FONT_SIZE_13, "-" );
}

void HeroMatchWnd::OnRenderSeasonRecordByPlaza( int iXPos, int iYPos )
{
	if( m_pCurrentBar )
		m_pCurrentBar->Render( iXPos, iYPos );
	if( m_pCurrentGra )
		m_pCurrentGra->Render( iXPos, iYPos );

	//
	if( m_pSeasonRecordTitle )
		m_pSeasonRecordTitle->Render( iXPos + 63, iYPos + 113 );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 63, iYPos + 61, UI_RENDER_MULTIPLY );
	if( m_pRecordIcon )
		m_pRecordIcon->Render( iXPos + 63, iYPos + 61 );

	int iRecordRate = g_MyInfo.GetHeroSeasonWinLoseRate() * 100;
	int iNumberCount = GetNumberCount( iRecordRate );

	enum { NUM_W = 27, NUM_GAP = 5, TEXT_W = 31,};
	int iNumberSize = (iNumberCount*NUM_W) - ((iNumberCount-1)*NUM_GAP);
	int iStartPos   = (iNumberSize + TEXT_W) / 2;
	if( m_pNumber && m_pPercentText )
	{
		m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pNumber->RenderNum( (iXPos + 63) - iStartPos, iYPos + 132, iRecordRate, -NUM_GAP );

		m_pPercentText->Render( ((iXPos + 63) - iStartPos)+(iNumberSize-NUM_GAP), iYPos + 132 );
	}

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	if( /*g_LadderTeamMgr.GetLadderCampType()*/g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		g_FontMgr.SetBkColor( 25, 70, 125 );
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
		g_FontMgr.SetBkColor( 120, 31, 19 );
	else
		g_FontMgr.SetBkColor( 110, 110, 110 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 182, FONT_SIZE_13, "승", g_MyInfo.GetHeroSeasonWin() );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, "패", g_MyInfo.GetHeroSeasonLose() );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 224, FONT_SIZE_13, "-" );
}

//////////////////////////////////////////////////////////////////////////
GuildHeroRankingListWnd::GuildHeroRankingListWnd()
{
	m_dwTabID   = 0;
}

GuildHeroRankingListWnd::~GuildHeroRankingListWnd()
{
}

void GuildHeroRankingListWnd::SetRankRadioBtn( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	CheckRadioButton( ID_GUILD_BTN, ID_HERO_TOP100_BTN, m_dwTabID );

	HideChildWnd( ID_GUILD_RANKING_WND );
	HideChildWnd( ID_HERO_RANKING_WND );

	switch( m_dwTabID )
	{
	case ID_GUILD_BTN:
		{
			GuildRankingListWnd *pRankingList = dynamic_cast<GuildRankingListWnd*>( FindChildWnd( ID_GUILD_RANKING_WND ) );
			if( pRankingList )
			{
				pRankingList->SetCampRadioBtn( m_dwTabID );
				pRankingList->ShowWnd();
			}
		}
		break;
	case ID_HERO_TOP100_BTN:
		{
			ShowChildWnd( ID_HERO_RANKING_WND );
		}
		break;
	}
}

void GuildHeroRankingListWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/guildrankinglistwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/herorankinglistwnd.xml", this );
}

void GuildHeroRankingListWnd::iwm_show()
{	
	if( m_dwTabID == 0 )
	{
		SetRankRadioBtn( ID_GUILD_BTN );
	}
	else
	{
		SetRankRadioBtn( m_dwTabID );
	}
}

void GuildHeroRankingListWnd::iwm_hide()
{
	m_dwTabID   = 0;
	HideChildWnd( ID_GUILD_RANKING_WND );
	HideChildWnd( ID_HERO_RANKING_WND );
}

void GuildHeroRankingListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_GUILD_BTN:
	case ID_HERO_TOP100_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			SetRankRadioBtn( dwID );
		}
		break;
	}
}

void GuildHeroRankingListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void GuildHeroRankingListWnd::OnRender()
{
	ioWnd::OnRender();
}

void GuildHeroRankingListWnd::ApplyGuildList( SP2Packet &rkPacket )
{
	GuildRankingListWnd *pRankingList = dynamic_cast<GuildRankingListWnd*>( FindChildWnd( ID_GUILD_RANKING_WND ) );
	if( pRankingList && pRankingList->IsShow() )
	{
		pRankingList->ApplyGuildList( rkPacket );
	}
}
//////////////////////////////////////////////////////////////////////////
HeroRankingListBtn::HeroRankingListBtn()
{
	m_iHeroRank = 0;

	m_szBlueCampName.Clear();
	m_szRedCampName.Clear();
}

HeroRankingListBtn::~HeroRankingListBtn()
{
}

void HeroRankingListBtn::iwm_mouseover( const ioMouse& mouse )
{
	HeroRankData kRankData = g_CampMgr.GetHeroData( m_iHeroRank );
	if( kRankData.m_szNick.IsEmpty() )
		return;
	ioButton::iwm_mouseover( mouse );
}

void HeroRankingListBtn::iwm_lbuttondown( const ioMouse& mouse )
{
	HeroRankData kRankData = g_CampMgr.GetHeroData( m_iHeroRank );
	if( kRankData.m_szNick.IsEmpty() )
		return;
	ioButton::iwm_lbuttondown( mouse );
}

void HeroRankingListBtn::SetHeroInfo( int iHeroRank )
{
	m_iHeroRank   = iHeroRank;	

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament == NULL ) return;

	m_szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
	m_szRedCampName = pTournament->GetCampName( CAMP_RED );
}

void HeroRankingListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	HeroRankData kRankData = g_CampMgr.GetHeroData( m_iHeroRank );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	if( m_bOver || m_bClicked )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	g_FontMgr.PrintText( iXPos + 6, iYPos + 3, FONT_SIZE_12, STR(1), kRankData.m_iRank );

	if( kRankData.m_szNick.IsEmpty() )
	{
		g_FontMgr.PrintText( iXPos + 50, iYPos + 3, FONT_SIZE_12, "-" );
	}
	else
	{
		// 계급
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( kRankData.m_szNick, iXPos + 50, iYPos + 2, kRankData.m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, TCT_DEFAULT_WHITE );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
		}
		else
		{
			g_LevelMgr.RenderGrade( kRankData.m_szNick, iXPos + 50, iYPos + 2, kRankData.m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		}

		// 닉네임
		g_FontMgr.PrintTextWidthCut( iXPos + 69, iYPos + 3, FONT_SIZE_12, 105.0f, kRankData.m_szNick.c_str() );
		
		if( m_bOver || m_bClicked )
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
		else if( kRankData.m_iCampPos == CAMP_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE ); 
		else if( kRankData.m_iCampPos == CAMP_RED )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED ); 
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 236, iYPos + 3, FONT_SIZE_12, "%s", g_LevelMgr.GetHeroTitleString( kRankData.m_iHeroTitle ).c_str() );

		int iWin = kRankData.m_iHeroWin / 30;
		int iLose= kRankData.m_iHeroLose / 10;
		if( m_bOver || m_bClicked )
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		g_FontMgr.PrintText( iXPos + 333, iYPos + 3, FONT_SIZE_12, STR(6), iWin, iLose );
		
		g_FontMgr.SetAlignType( TAT_RIGHT );
		int iWinRate = ( (float)( (float)iWin / (float)max( 1, iWin + iLose ) ) ) * 100;
		g_FontMgr.PrintText( iXPos + 417, iYPos + 3, FONT_SIZE_12, STR(7), iWinRate );
	}
}
//////////////////////////////////////////////////////////////////////////
HeroRankingListWnd::HeroRankingListWnd()
{
	m_iCurPage = m_iMaxPage = 0;
}

HeroRankingListWnd::~HeroRankingListWnd()
{
}


void HeroRankingListWnd::iwm_show()
{	
	m_iMaxPage = max( 0, MAX_LIST - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( m_iCurPage );
}

void HeroRankingListWnd::iwm_hide()
{
	m_iCurPage = 0;
}

void HeroRankingListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
	case ID_LIST_9:
	case ID_LIST_10:
		if( cmd == IOBN_BTNUP )
		{	
			HeroRankingListBtn *pHeroBtn = dynamic_cast<HeroRankingListBtn*>(pWnd);
			HeroRankData kRankData = g_CampMgr.GetHeroData( pHeroBtn->GetHeroRank() );
			if( !kRankData.m_szNick.IsEmpty() )
			{
				g_App.OpenMannerWnd( kRankData.m_szNick.c_str(), kRankData.m_bLogIn );
			}
		}
		break;
	}
}

void HeroRankingListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void HeroRankingListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL);
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 95, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 69, iYPos + 95, FONT_SIZE_12, STR(2) );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + 253, iYPos + 95, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 345, iYPos + 95, FONT_SIZE_12, STR(4) );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 434, iYPos + 95, FONT_SIZE_12, STR(5) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(6), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(7), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 229, iYPos + 332, TAT_CENTER );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL);	
	g_FontMgr.SetAlignType( TAT_CENTER );
	
	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	
	char szMyRanking[MAX_PATH] = "";
	char szMyHeroTitle[MAX_PATH] = "";
	if( g_MyInfo.GetHeroTodayRank() == 0 )
		SafeSprintf( szMyRanking, sizeof( szMyRanking ), STR(8) );
	else
		SafeSprintf( szMyRanking, sizeof( szMyRanking ), STR(9), g_MyInfo.GetHeroTodayRank() );
	
	if( g_MyInfo.GetHeroTitle() == 0 )
		SafeSprintf( szMyHeroTitle, sizeof( szMyHeroTitle ), STR(10) );
	else 
		sprintf( szMyHeroTitle, "%s", g_LevelMgr.GetHeroTitleString( g_MyInfo.GetHeroTitle() ).c_str() );

	int iWinRate = g_MyInfo.GetRecordWinLoseRate( RECORD_HEROMATCH ) * 100;
	g_FontMgr.PrintText( iXPos + 229, iYPos + 377, FONT_SIZE_13, STR(11),
						 szMyRanking, szMyHeroTitle, g_MyInfo.GetRecordWin( RECORD_HEROMATCH ), 
						 g_MyInfo.GetRecordLose( RECORD_HEROMATCH ), iWinRate );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL);	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 229, iYPos + 397, FONT_SIZE_13, STR(12) );
}

void HeroRankingListWnd::UpdateSetPage( int iCurPage )
{
	m_iCurPage = iCurPage;
	for(int i = 0;i < MAX_PAGE_LIST;i++)
	{
		HeroRankingListBtn *pListBtn = dynamic_cast<HeroRankingListBtn*>( FindChildWnd( ID_LIST_1 + i ) );
		if( pListBtn )
		{
			pListBtn->SetHeroInfo( ( m_iCurPage * MAX_PAGE_LIST ) + i + 1 );
		}
	}
	g_CampMgr.SendHeroTop100( iCurPage, MAX_PAGE_LIST );
}
//////////////////////////////////////////////////////////////////////////
HeroMatchHistoryBtn::HeroMatchHistoryBtn()
{
	m_pIconBack	  = NULL;
	m_pResultWin  = NULL;
	m_pResultLose = NULL;
	m_pResultDraw = NULL;
	m_pNoneData   = NULL;

	m_HeroHistory.Init();
	m_iMode = HERO_MATCH;
}

HeroMatchHistoryBtn::~HeroMatchHistoryBtn()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pResultWin );
	SAFEDELETE( m_pResultLose );
	SAFEDELETE( m_pResultDraw );
	SAFEDELETE( m_pNoneData );
}
	
void HeroMatchHistoryBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "ResultWin" )
	{	
		SAFEDELETE( m_pResultWin );
		m_pResultWin = pImage;
	}
	else if( szType == "ResultLose" )
	{
		SAFEDELETE( m_pResultLose );
		m_pResultLose = pImage;
	}
	else if( szType == "ResultDraw" )
	{
		SAFEDELETE( m_pResultDraw );
		m_pResultDraw = pImage;
	}
	else if( szType == "NoneData" )
	{
		SAFEDELETE( m_pNoneData );
		m_pNoneData = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void HeroMatchHistoryBtn::SetMatchHistory( int iArray, int iMode )
{
	m_HeroHistory.Init();
	m_BattleHistory.Init();

	m_iMode = iMode;
	bool bActive = false;
	if( m_iMode == HERO_MATCH )
	{
		bActive = g_CampMgr.GetHeroMatchHistory( iArray, m_HeroHistory );
		
		if ( m_HeroHistory.m_eWinTeam != WTT_NONE )
			LOG.PrintTimeAndLog( 0, " %s CampType : %d, TeamType : %d, Win Team : %d", __FUNCTION__, m_HeroHistory.m_iCampType, m_HeroHistory.m_iTeamType, m_HeroHistory.m_eWinTeam );
	}
	else
	{
		bActive = g_RankBattleMgr.GetBattleHistory( iArray, m_BattleHistory );
	}
	if( !bActive )
		SetInActive();
	else
		SetActive();
}

void HeroMatchHistoryBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( IsDisabled() )
	{
		if( m_pNoneData )
		{
			m_pNoneData->SetScale( 0.48f );
			m_pNoneData->Render( iXPos + 35, iYPos + 25, UI_RENDER_MULTIPLY );
			m_pNoneData->Render( iXPos + 35, iYPos + 25, UI_RENDER_MULTIPLY );
			m_pNoneData->Render( iXPos + 35, iYPos + 25, UI_RENDER_MULTIPLY );
		}

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( 128, 128, 128 );
		if( m_iMode == HERO_MATCH )
		{
			g_FontMgr.PrintText( iXPos + 64, iYPos + 10, FONT_SIZE_12, STR(1) );
			g_FontMgr.PrintText( iXPos + 64, iYPos + 27, FONT_SIZE_12, STR(2) );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 64, iYPos + 18, FONT_SIZE_14, STR(1) );
		}
	}
	else
	{
		ioUIRenderImage *pResultIcon = NULL;
		if( m_iMode == HERO_MATCH )
		{
			char szRankNTitle[MAX_PATH] = "";
			if( m_HeroHistory.m_iHeroRank == 0 )
			{
				if( m_HeroHistory.m_iHeroTitle == 0)
					SafeSprintf( szRankNTitle, sizeof( szRankNTitle ), STR(3) );
				else 
					SafeSprintf( szRankNTitle, sizeof( szRankNTitle ), STR(4), g_LevelMgr.GetHeroTitleString( m_HeroHistory.m_iHeroTitle ).c_str() );
			}
			else
			{
				if( m_HeroHistory.m_iHeroTitle == 0)
					SafeSprintf( szRankNTitle, sizeof( szRankNTitle ), STR(5), m_HeroHistory.m_iHeroRank );
				else 
					SafeSprintf( szRankNTitle, sizeof( szRankNTitle ), STR(6), m_HeroHistory.m_iHeroRank, g_LevelMgr.GetHeroTitleString( m_HeroHistory.m_iHeroTitle ).c_str() );
			}

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			// 순위 & 타이틀
			g_FontMgr.PrintText( iXPos + 64, iYPos + 10, FONT_SIZE_12, szRankNTitle );

			// 계급
			int iCurrentXPos = g_FontMgr.GetTextWidth( szRankNTitle, TS_NORMAL, FONT_SIZE_12 ) + 3;
			g_LevelMgr.RenderGrade( m_HeroHistory.m_szNickName, iXPos + 64 + iCurrentXPos, iYPos + 9, m_HeroHistory.m_iGradeLevel, TEAM_PRIVATE_1 );
			iCurrentXPos += 19;
		
			// 아이디
			g_FontMgr.PrintTextWidthCut( iXPos + 64 + iCurrentXPos, iYPos + 10, FONT_SIZE_12, 110.0f, m_HeroHistory.m_szNickName.c_str() );
			iCurrentXPos += min( 110.0f, g_FontMgr.GetTextWidth( m_HeroHistory.m_szNickName.c_str(), TS_NORMAL, FONT_SIZE_12 ) ) + 3;

			// 길드마크
			DWORD dwGuildIndex, dwGuildMark;
			g_UserInfoMgr.GetGuildInfo( m_HeroHistory.m_szNickName, dwGuildIndex, dwGuildMark );
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 64 + iCurrentXPos, iYPos + 11 );	

			// 승&패/시간/이탈
			int iSec  = ( FRAMEGETTIME() - m_HeroHistory.m_dwResultTime ) / 1000;
			int iMin  = max( 1, iSec / 60 );
			int iHour = iMin / 60;
			int iDay  = iHour / 24;

			char szResult[MAX_PATH] = "";
			char szResultTime[MAX_PATH] = "";
			// 시간
			if( iDay != 0 )
				SafeSprintf( szResultTime, sizeof( szResultTime ), STR(7), iDay );
			else if( iHour != 0 )
			{
				if( iMin != 0 )
					SafeSprintf( szResultTime, sizeof( szResultTime ), STR(8), iHour, iMin % 60 );
				else
					SafeSprintf( szResultTime, sizeof( szResultTime ), STR(9), iHour );
			}
			else
				SafeSprintf( szResultTime, sizeof( szResultTime ), STR(10), iMin );
        
			// 결과
			if( !m_HeroHistory.m_bResultCheck )
			{
				SafeSprintf( szResult, sizeof( szResult ), STR(11) );		
				pResultIcon = m_pResultLose;
			}
			else
			{
				if( m_HeroHistory.m_iTeamType == TEAM_BLUE )
				{
					if( m_HeroHistory.m_eWinTeam == WTT_BLUE_TEAM || m_HeroHistory.m_bLeave )
					{
						SafeSprintf( szResult, sizeof( szResult ), STR(12), m_HeroHistory.m_iRedScore, m_HeroHistory.m_iBlueScore );
						pResultIcon = m_pResultLose;
					}
					else if( m_HeroHistory.m_eWinTeam == WTT_RED_TEAM )
					{
						SafeSprintf( szResult, sizeof( szResult ), STR(13), m_HeroHistory.m_iRedScore, m_HeroHistory.m_iBlueScore );
						pResultIcon = m_pResultWin;
					}
					else
					{
						SafeSprintf( szResult, sizeof( szResult ), STR(14), m_HeroHistory.m_iRedScore, m_HeroHistory.m_iBlueScore  );
						pResultIcon = m_pResultDraw;
					}
				}
				else
				{
					if( m_HeroHistory.m_eWinTeam == WTT_RED_TEAM || m_HeroHistory.m_bLeave )
					{
						SafeSprintf( szResult, sizeof( szResult ), STR(15), m_HeroHistory.m_iBlueScore, m_HeroHistory.m_iRedScore );
						pResultIcon = m_pResultLose;
					}
					else if( m_HeroHistory.m_eWinTeam == WTT_BLUE_TEAM )
					{	
						SafeSprintf( szResult, sizeof( szResult ), STR(16), m_HeroHistory.m_iBlueScore, m_HeroHistory.m_iRedScore );
						pResultIcon = m_pResultWin;
					}
					else
					{
						SafeSprintf( szResult, sizeof( szResult ), STR(17), m_HeroHistory.m_iBlueScore, m_HeroHistory.m_iRedScore  );
						pResultIcon = m_pResultDraw;
					}
				}
			}

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 64, iYPos + 27, FONT_SIZE_12, STR(18), szResult, szResultTime );
		}
		else if( m_iMode == RANK_BATTLE )
		{
			char szResult[MAX_PATH] = "";
			if( m_BattleHistory.m_byWinState == 0 )
			{
				SafeSprintf( szResult, sizeof( szResult ), STR(11) );		
				pResultIcon = m_pResultLose;
			}
			else if( m_BattleHistory.m_byWinState == 1 )
			{
				SafeSprintf( szResult, sizeof( szResult ), STR(13), m_BattleHistory.m_iWinGame, m_BattleHistory.m_iLoseGame );
				pResultIcon = m_pResultWin;
			}
			else
			{
				SafeSprintf( szResult, sizeof( szResult ), STR(12), m_BattleHistory.m_iWinGame, m_BattleHistory.m_iLoseGame );
				pResultIcon = m_pResultLose;
			}
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 64, iYPos + 18, FONT_SIZE_14, szResult );
		}
		if( m_pIconBack && pResultIcon )
		{
			m_pIconBack->SetScale( 0.48f );
			m_pIconBack->Render( iXPos + 35, iYPos + 25, UI_RENDER_MULTIPLY );
			
			pResultIcon->SetScale( 0.48f );
			pResultIcon->Render( iXPos + 35, iYPos + 25 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
HeroMatchHistoryWnd::HeroMatchHistoryWnd()
{
	m_iCurPos = 0;
	m_iMode = HERO_MATCH;
}

HeroMatchHistoryWnd::~HeroMatchHistoryWnd()
{
}

void HeroMatchHistoryWnd::iwm_show()
{
	m_iCurPos = 0;
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( m_iCurPos, g_CampMgr.GetMaxHeroMatch() );
		pScroll->SetScrollPos( m_iCurPos );
	}
	if( m_iMode != HERO_MATCH )
		g_RankBattleMgr.SendHistory();
}

void HeroMatchHistoryWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( pScroll->HasWndStyle( IWS_INACTIVE ) ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void HeroMatchHistoryWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	UpdateSetPage( curPos );
}

void HeroMatchHistoryWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( m_iMode != HERO_MATCH )
		return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
		if( cmd == IOBN_BTNUP )
		{
			HeroMatchHistoryBtn *pHistoryBtn = dynamic_cast<HeroMatchHistoryBtn*>(pWnd);
			if( !pHistoryBtn->GetNickName().IsEmpty() )
				g_App.OpenMannerWnd( pHistoryBtn->GetNickName().c_str(), false );
		}
		break;
	}
}

void HeroMatchHistoryWnd::UpdateSetPage( int iCurPos )
{
	m_iCurPos = iCurPos;

	for(int i = 0;i < MAX_PAGE_LIST;i++)
	{
		HeroMatchHistoryBtn *pHistoryBtn = dynamic_cast<HeroMatchHistoryBtn*>(FindChildWnd( ID_LIST_1 + i ));
		if( pHistoryBtn )
		{
			pHistoryBtn->SetMatchHistory( (m_iCurPos*MAX_PAGE_LIST) + i, m_iMode );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
HeroMatchTop100Btn::HeroMatchTop100Btn()
{
	m_pCustomDisableFrm = NULL;
	m_pBlueCampBar = NULL;
	m_pRedCampBar  = NULL;
	m_iHeroRank    = 0;
	m_iMode = HERO_MATCH;
}

HeroMatchTop100Btn::~HeroMatchTop100Btn()
{
	SAFEDELETE( m_pCustomDisableFrm );
	SAFEDELETE( m_pBlueCampBar );
	SAFEDELETE( m_pRedCampBar );
}

void HeroMatchTop100Btn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "CustomDisable" )
	{
		SAFEDELETE( m_pCustomDisableFrm );
		m_pCustomDisableFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void HeroMatchTop100Btn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueCampBar" )
	{
		SAFEDELETE( m_pBlueCampBar );
		m_pBlueCampBar = pImage;
	}
	else if( szType == "RedCampBar" )
	{
		SAFEDELETE( m_pRedCampBar );
		m_pRedCampBar = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void HeroMatchTop100Btn::SetTop100Rank( int iRank, int iMode )
{
	m_iHeroRank = iRank;
	m_iMode = iMode;
}

void HeroMatchTop100Btn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iMode == HERO_MATCH )
	{
		HeroRankData kRankData = g_CampMgr.GetHeroData( m_iHeroRank );
		if( !kRankData.m_szNick.IsEmpty() )
		{
			if( kRankData.m_iCampPos == CAMP_BLUE && m_pBlueCampBar )
				m_pBlueCampBar->Render( iXPos + 5, iYPos + 5 );
			else if( kRankData.m_iCampPos == CAMP_RED && m_pRedCampBar )
				m_pRedCampBar->Render( iXPos + 5, iYPos + 5 );
			else if( m_pBlueCampBar )
				m_pBlueCampBar->Render( iXPos + 5, iYPos + 5, UI_RENDER_GRAY );

			// 순위
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
			g_FontMgr.PrintText( iXPos + 24, iYPos + 5, FONT_SIZE_12, STR(2), kRankData.m_iRank );

			// 계급
			g_LevelMgr.RenderGrade( kRankData.m_szNick, iXPos + 47, iYPos + 4, kRankData.m_iGradeLevel, TEAM_PRIVATE_1 );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 

			// 닉네임
			g_FontMgr.PrintTextWidthCut( iXPos + 66, iYPos + 5, FONT_SIZE_12, 80.0f, kRankData.m_szNick.c_str() );

			// 승패
			int iWin = kRankData.m_iHeroWin / 30;
			int iLose= kRankData.m_iHeroLose / 10;
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
			g_FontMgr.PrintText( iXPos + 220, iYPos + 5, FONT_SIZE_12, STR(3), iWin, iLose );

			g_FontMgr.SetAlignType( TAT_RIGHT );
			int iWinRate = ( (float)( (float)iWin / (float)max( 1, iWin + iLose ) ) ) * 100;
			g_FontMgr.PrintText( iXPos + 303, iYPos + 5, FONT_SIZE_12, STR(4), iWinRate );
			return;
		}
	}
	else if( m_iMode == RANK_BATTLE )
	{
		UserBattleRecord kInfo = g_RankBattleMgr.GetUserBattleRecord( m_iHeroRank );
		if( !kInfo.m_szName.IsEmpty() )
		{
			if( m_pBlueCampBar )
				m_pBlueCampBar->Render( iXPos + 5, iYPos + 5, UI_RENDER_GRAY );

			// 순위
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
			g_FontMgr.PrintText( iXPos + 24, iYPos + 5, FONT_SIZE_12, STR(2), kInfo.m_iRank );

			// 계급
			g_LevelMgr.RenderGrade( kInfo.m_szName, iXPos + 47, iYPos + 4, kInfo.m_iGradeLevel, TEAM_PRIVATE_1 );
			
			// 닉네임
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 			
			g_FontMgr.PrintTextWidthCut( iXPos + 66, iYPos + 5, FONT_SIZE_12, 80.0f, kInfo.m_szName.c_str() );

			// 승패
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
			g_FontMgr.PrintText( iXPos + 220, iYPos + 5, FONT_SIZE_12, STR(3), kInfo.m_iWin, kInfo.m_iLose );

			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.PrintText( iXPos + 303, iYPos + 5, FONT_SIZE_12, "%d점", kInfo.m_iRankPoint );
			return;
		}
	}
	if( m_pCustomDisableFrm )
		m_pCustomDisableFrm->Render( iXPos, iYPos );

	if( m_pBlueCampBar )
		m_pBlueCampBar->Render( iXPos + 5, iYPos + 5, UI_RENDER_GRAY );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE ); 
	g_FontMgr.PrintText( iXPos + 24, iYPos + 5, FONT_SIZE_12, "-" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY ); 
	g_FontMgr.PrintText( iXPos + 174, iYPos + 5, FONT_SIZE_12, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
HeroMatchTop100Wnd::HeroMatchTop100Wnd()
{
	m_iCurPos = 0;
	m_iMode = HERO_MATCH;
}

HeroMatchTop100Wnd::~HeroMatchTop100Wnd()
{
}

void HeroMatchTop100Wnd::iwm_show()
{
	m_iSyncArray.clear();
	m_iCurPos = 0;
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		if( m_iMode == HERO_MATCH )
			pScroll->SetScrollRange( 0, ( MAX_TOP100 + MAX_PAGE_LIST ) / MAX_PAGE_LIST );
		else
			pScroll->SetScrollRange( 0, MAX_TOP100 - MAX_PAGE_LIST + 1 );

		pScroll->SetScrollPos( m_iCurPos );
	}
	if( m_iMode == HERO_MATCH )
		g_CampMgr.SendHeroTop100( m_iCurPos, MAX_PAGE_LIST );
	else
		g_RankBattleMgr.SendTop100();

	m_iSyncArray.push_back( m_iCurPos );
}

void HeroMatchTop100Wnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( pScroll->HasWndStyle( IWS_INACTIVE ) ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void HeroMatchTop100Wnd::iwm_vscroll(DWORD dwID, int curPos )
{
	UpdateSetPage( curPos );
}

void HeroMatchTop100Wnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
		if( cmd == IOBN_BTNUP )
		{
			HeroMatchTop100Btn *pHistoryBtn = dynamic_cast<HeroMatchTop100Btn*>(pWnd);
			if( m_iMode == HERO_MATCH )
			{
				HeroRankData kRankData = g_CampMgr.GetHeroData( pHistoryBtn->GetTop100Rank() );
				if( !kRankData.m_szNick.IsEmpty() )
				{
					g_App.OpenMannerWnd( kRankData.m_szNick.c_str(), kRankData.m_bLogIn );
				}
			}
			else
			{
				UserBattleRecord kInfo = g_RankBattleMgr.GetUserBattleRecord( pHistoryBtn->GetTop100Rank() );
				if( !kInfo.m_szName.IsEmpty() )
					g_App.OpenMannerWnd( kInfo.m_szName.c_str(), kInfo.m_bLogin );
			}
		}
		break;
	}
}

void HeroMatchTop100Wnd::UpdateSetPage( int iCurPos )
{
	int i = 0;
	m_iCurPos = iCurPos;
	if( m_iMode == HERO_MATCH )
	{
		bool bSend = true;
		for(i = 0;i < (int)m_iSyncArray.size();i++)
		{
			if( m_iSyncArray[i] == m_iCurPos )
			{
				bSend = false;
				break;
			}
		}	

		if( bSend )
		{
			g_CampMgr.SendHeroTop100( m_iCurPos, MAX_PAGE_LIST );
			m_iSyncArray.push_back( m_iCurPos );
		}
	}
	for(i = 0;i < MAX_PAGE_LIST;i++)
	{
		HeroMatchTop100Btn *pHistoryBtn = dynamic_cast<HeroMatchTop100Btn*>(FindChildWnd( ID_LIST_1 + i ));
		if( pHistoryBtn )
		{
			if( m_iMode == HERO_MATCH )
				pHistoryBtn->SetTop100Rank( ( (m_iCurPos*MAX_PAGE_LIST) + i ) + 1, m_iMode );
			else
				pHistoryBtn->SetTop100Rank( ( m_iCurPos + i ) + 1, m_iMode );
		}
	}
}

void HeroMatchTop100Wnd::ResizeScroll()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		if( m_iMode != HERO_MATCH )
			pScroll->SetScrollRange( 0, g_RankBattleMgr.GetUserBattleRecordCount() - MAX_PAGE_LIST + 1 );

		pScroll->SetScrollPos( m_iCurPos );
	}
}
//////////////////////////////////////////////////////////////////////////
HeroMatchRankGraphWnd::HeroMatchRankGraphWnd()
{
	m_pGraphRect  = NULL;
	m_pBlueCircle = NULL;
	m_pRedCircle  = NULL;
	m_pBlueTodayCircle = NULL;
	m_pRedTodayCircle  = NULL;
	for(int i = 0;i < MAX_GRAPH_LINE;i++)
		m_pGraphLine[i] = NULL;

	m_iMode = HERO_MATCH;
}

HeroMatchRankGraphWnd::~HeroMatchRankGraphWnd()
{
	SAFEDELETE( m_pGraphRect );
	SAFEDELETE( m_pBlueCircle );
	SAFEDELETE( m_pRedCircle );
	SAFEDELETE( m_pBlueTodayCircle );
	SAFEDELETE( m_pRedTodayCircle );
	for(int i = 0;i < MAX_GRAPH_LINE;i++)
		SAFEDELETE( m_pGraphLine[i] );
}

void HeroMatchRankGraphWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GraphRect" )
	{
		SAFEDELETE( m_pGraphRect );
		m_pGraphRect = pImage;
	}
	else if( szType == "BlueCircle" )
	{
		SAFEDELETE( m_pBlueCircle );
		m_pBlueCircle = pImage;
	}
	else if( szType == "RedCircle" )
	{
		SAFEDELETE( m_pRedCircle );
		m_pRedCircle = pImage;
	}
	else if( szType == "BlueTodayCircle" )
	{
		SAFEDELETE( m_pBlueTodayCircle );
		m_pBlueTodayCircle = pImage;
	}
	else if( szType == "RedTodayCircle" )
	{
		SAFEDELETE( m_pRedTodayCircle );
		m_pRedTodayCircle = pImage;
	}
	else if( szType == "GraphLine0" )
	{
		SAFEDELETE( m_pGraphLine[0] );
		m_pGraphLine[0] = pImage;
	}
	else if( szType == "GraphLine1" )
	{
		SAFEDELETE( m_pGraphLine[1] );
		m_pGraphLine[1] = pImage;
	}
	else if( szType == "GraphLine2" )
	{
		SAFEDELETE( m_pGraphLine[2] );
		m_pGraphLine[2] = pImage;
	}
	else if( szType == "GraphLine3" )
	{
		SAFEDELETE( m_pGraphLine[3] );
		m_pGraphLine[3] = pImage;
	}
	else if( szType == "GraphLine4" )
	{
		SAFEDELETE( m_pGraphLine[4] );
		m_pGraphLine[4] = pImage;
	}
	else if( szType == "GraphLine5" )
	{
		SAFEDELETE( m_pGraphLine[5] );
		m_pGraphLine[5] = pImage;
	}
	else if( szType == "GraphLine6" )
	{
		SAFEDELETE( m_pGraphLine[6] );
		m_pGraphLine[6] = pImage;
	}
	else if( szType == "GraphLine7" )
	{
		SAFEDELETE( m_pGraphLine[7] );
		m_pGraphLine[7] = pImage;
	}
	else if( szType == "GraphLine8" )
	{
		SAFEDELETE( m_pGraphLine[8] );
		m_pGraphLine[8] = pImage;
	}
	else if( szType == "GraphLine9" )
	{
		SAFEDELETE( m_pGraphLine[9] );
		m_pGraphLine[9] = pImage;
	}
	else if( szType == "GraphLine10" )
	{
		SAFEDELETE( m_pGraphLine[10] );
		m_pGraphLine[10] = pImage;
	}
	else if( szType == "GraphLine11" )
	{
		SAFEDELETE( m_pGraphLine[11] );
		m_pGraphLine[11] = pImage;
	}
	else if( szType == "GraphLine12" )
	{
		SAFEDELETE( m_pGraphLine[12] );
		m_pGraphLine[12] = pImage;
	}
	else if( szType == "GraphLine13" )
	{
		SAFEDELETE( m_pGraphLine[13] );
		m_pGraphLine[13] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void HeroMatchRankGraphWnd::SetHeroMatchGraph()
{
	m_GraphData.clear();

	int iDefaultRank = HERO_SEASON_RANK_MAX + 1;
	int i = 0;
	int iSeasonRank[HERO_SEASON_RANK_MAX + 1] = { 0, 0, 0, 0, 0, 0, 0 };   // +1은 투데이 랭킹
	for(i = 0;i < HERO_SEASON_RANK_MAX;i++)
	{	
		iSeasonRank[i] = g_MyInfo.GetHeroSeasonRank( (HERO_SEASON_RANK_MAX-1) - i );		
		iDefaultRank += iSeasonRank[i];
	}
	iSeasonRank[HERO_SEASON_RANK_MAX] = g_MyInfo.GetHeroTodayRank();
	iDefaultRank += iSeasonRank[HERO_SEASON_RANK_MAX];

	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		if( iSeasonRank[i] == 0 )
			iSeasonRank[i] = iDefaultRank;
	}

	int iMinRank = iDefaultRank;
	int iMaxRank = 1;
	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		if( iSeasonRank[i] < iMinRank )
			iMinRank = iSeasonRank[i];
		
		if( iSeasonRank[i] > iMaxRank )
			iMaxRank = iSeasonRank[i];
	}

	const int iMaxGraphLine = MAX_GRAPH_LINE - 1;
	float fGapRank = (float)( iMaxRank - iMinRank ) / iMaxGraphLine;
	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		float fRankPos = 0; 
		if( iSeasonRank[i] == iDefaultRank )
		{	
			fRankPos = iMaxGraphLine;
		}
		else if( fGapRank != 0.0f )
		{
			fRankPos = (float)( iSeasonRank[i] - iMinRank ) / fGapRank;
		}

		GraphData kGraphData;
		kGraphData.m_iRankPos = min( iMaxGraphLine, fRankPos + FLOAT05 );

		if( iSeasonRank[i] == iDefaultRank )
		{
			kGraphData.m_iRanking = 0;
		}
		else
		{
			kGraphData.m_iRanking = iSeasonRank[i];
		}
		if( i == 0 || i == HERO_SEASON_RANK_MAX )
			kGraphData.m_bViewRanking = true;
		
		if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		{
			if( i == HERO_SEASON_RANK_MAX )
				kGraphData.m_pCircle = m_pBlueTodayCircle;
			else
				kGraphData.m_pCircle = m_pBlueCircle;
		}
		else
		{
			if( i == HERO_SEASON_RANK_MAX )
				kGraphData.m_pCircle = m_pRedTodayCircle;
			else
				kGraphData.m_pCircle = m_pRedCircle;
		}

		kGraphData.m_dwAniState = GRAPH_ANI_NONE;
		m_GraphData.push_back( kGraphData );
	}

	// 라인 넣기
	for(i = 0;i < (int)m_GraphData.size();i++)
	{
		if( i + 1 < (int)m_GraphData.size() )
		{
			GraphData &rkData = m_GraphData[i];
			GraphData &rkNextData = m_GraphData[i+1];
			rkData.m_iNextRankPosGap = abs( rkData.m_iRankPos - rkNextData.m_iRankPos );

			if( COMPARE( rkData.m_iNextRankPosGap, 0, MAX_GRAPH_LINE ) )
			{				
				rkData.m_pLine = m_pGraphLine[rkData.m_iNextRankPosGap];
				if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
				{
					if( rkData.m_pLine )
						rkData.m_pLine->SetColor( TCT_DEFAULT_BLUE );
				}
				else
				{
					if( rkData.m_pLine )
						rkData.m_pLine->SetColor( TCT_DEFAULT_RED );
				}
			}

			if( rkData.m_iRankPos < rkNextData.m_iRankPos )
			{
				rkData.m_bLineReverse = true;
			}
		}
	}
}

void HeroMatchRankGraphWnd::SetRankBattleGraph()
{
	m_GraphData.clear();

	int iDefaultRank = HERO_SEASON_RANK_MAX + 1;
	int i = 0;
	int iSeasonRank[HERO_SEASON_RANK_MAX + 1] = { 0, 0, 0, 0, 0, 0, 0 };   // +1은 투데이 랭킹
	for(i = 0;i < HERO_SEASON_RANK_MAX;i++)
	{	
		iSeasonRank[i] = g_RankBattleMgr.GetRankRecord( (HERO_SEASON_RANK_MAX-1) - i ).m_iRank;		
		iDefaultRank += iSeasonRank[i];
	}
	iSeasonRank[HERO_SEASON_RANK_MAX] = g_RankBattleMgr.GetRank();
	iDefaultRank += iSeasonRank[HERO_SEASON_RANK_MAX];

	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		if( iSeasonRank[i] == 0 )
			iSeasonRank[i] = iDefaultRank;
	}

	int iMinRank = iDefaultRank;
	int iMaxRank = 1;
	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		if( iSeasonRank[i] < iMinRank )
			iMinRank = iSeasonRank[i];
		
		if( iSeasonRank[i] > iMaxRank )
			iMaxRank = iSeasonRank[i];
	}

	const int iMaxGraphLine = MAX_GRAPH_LINE - 1;
	float fGapRank = (float)( iMaxRank - iMinRank ) / iMaxGraphLine;
	for(i = 0;i < HERO_SEASON_RANK_MAX+1;i++)
	{
		float fRankPos = 0; 
		if( iSeasonRank[i] == iDefaultRank )
		{	
			fRankPos = iMaxGraphLine;
		}
		else if( fGapRank != 0.0f )
		{
			fRankPos = (float)( iSeasonRank[i] - iMinRank ) / fGapRank;
		}

		GraphData kGraphData;
		kGraphData.m_iRankPos = min( iMaxGraphLine, fRankPos + FLOAT05 );

		if( iSeasonRank[i] == iDefaultRank )
		{
			kGraphData.m_iRanking = 0;
		}
		else
		{
			kGraphData.m_iRanking = iSeasonRank[i];
		}
		if( i == 0 || i == HERO_SEASON_RANK_MAX )
			kGraphData.m_bViewRanking = true;
		
		if( i == HERO_SEASON_RANK_MAX )
			kGraphData.m_pCircle = m_pBlueTodayCircle;
		else
			kGraphData.m_pCircle = m_pBlueCircle;	

		kGraphData.m_dwAniState = GRAPH_ANI_NONE;
		m_GraphData.push_back( kGraphData );
	}

	// 라인 넣기
	for(i = 0;i < (int)m_GraphData.size();i++)
	{
		if( i + 1 < (int)m_GraphData.size() )
		{
			GraphData &rkData = m_GraphData[i];
			GraphData &rkNextData = m_GraphData[i+1];
			rkData.m_iNextRankPosGap = abs( rkData.m_iRankPos - rkNextData.m_iRankPos );

			if( COMPARE( rkData.m_iNextRankPosGap, 0, MAX_GRAPH_LINE ) )
			{				
				rkData.m_pLine = m_pGraphLine[rkData.m_iNextRankPosGap];
				if( rkData.m_pLine )
					rkData.m_pLine->SetColor( TCT_DEFAULT_BLUE );
			}

			if( rkData.m_iRankPos < rkNextData.m_iRankPos )
			{
				rkData.m_bLineReverse = true;
			}
		}
	}
}

void HeroMatchRankGraphWnd::iwm_show()
{
	if( m_iMode == HERO_MATCH )
		SetHeroMatchGraph();
	else
		g_RankBattleMgr.SendRankData();
}

void HeroMatchRankGraphWnd::OnProcess( float fTimePerSec )
{
	ProcessCircleOver();	
	ProcessGraphAni();
}

void HeroMatchRankGraphWnd::ProcessCircleOver()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	ioLSMouse *pMouse = g_App.GetMouse();

	// 동글 + 라인
	for(int i = 0;i < (int)m_GraphData.size();i++)
	{
		if( i == 0 || i == (int)m_GraphData.size() - 1 ) continue;

		GraphData &rkData = m_GraphData[i];
		if( rkData.m_dwAniState <= GRAPH_ANI_CIRCLE ) continue;
		
		RECT rcCircleRect;
		rcCircleRect.left   = iXPos + 36 + i * 41;
		rcCircleRect.top    = iYPos + 31 + rkData.m_iRankPos * 10;
		rcCircleRect.right  = rcCircleRect.left + 12;
		rcCircleRect.bottom = rcCircleRect.top + 12;
		
		if( ::PtInRect( &rcCircleRect, pMouse->GetMousePos() ) )
		{
			rkData.m_bViewRanking = true;
		}
		else
		{
			rkData.m_bViewRanking = false;
		}
	}
}

void HeroMatchRankGraphWnd::ProcessGraphAni()
{
	for(int i = 0;i < (int)m_GraphData.size();i++)
	{
		GraphData &rkData = m_GraphData[i];
		if( rkData.m_dwAniState > GRAPH_ANI_LINE ) continue;
		
		if( rkData.m_dwAniTime == 0 )
		{
			rkData.m_dwAniTime = FRAMEGETTIME();
		}

		DWORD dwGapTime = FRAMEGETTIME() - rkData.m_dwAniTime;
		if( dwGapTime <= GRAPH_ANI_TIME )
		{
			rkData.m_fAniRate = (float)dwGapTime / GRAPH_ANI_TIME;
		}
		else
		{
			rkData.m_dwAniState++;
			if( rkData.m_dwAniState > GRAPH_ANI_LINE )
			{
				rkData.m_fAniRate = FLOAT1;				
			}
			else
			{
				rkData.m_fAniRate = 0.0f;		
				rkData.m_dwAniTime = FRAMEGETTIME();		
			}
		}
		break;
	}
}

void HeroMatchRankGraphWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderRect( iXPos, iYPos );
	OnRenderDay( iXPos, iYPos );
	OnRenderGraph( iXPos, iYPos );
}

void HeroMatchRankGraphWnd::OnRenderRect( int iXPos, int iYPos )
{
	if( !m_pGraphRect ) return;

	int i = 0;	
	enum { MAX_HORZ = 20, MAX_VERT = 9, };
	m_pGraphRect->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pGraphRect->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pGraphRect->SetSize( 330, 2 );
	for(i = 0;i < MAX_HORZ;i++)
	{
		m_pGraphRect->Render( iXPos, iYPos + 5 + (i*10), UI_RENDER_MULTIPLY );
	}

	m_pGraphRect->SetSize( 2, 8 );
	for(i = 0;i < MAX_HORZ - 1;i++)
	{
		for(int j = 0;j < MAX_VERT;j++)
		{
			m_pGraphRect->Render( iXPos + (j * 41), iYPos + 7 + (i*10), UI_RENDER_MULTIPLY );
		}
	}
}

void HeroMatchRankGraphWnd::OnRenderDay( int iXPos, int iYPos )
{	
	if( m_iMode == HERO_MATCH )
	{
		SYSTEMTIME kSTime = { 0, 0, 0, 0, 0, 0, 0, 0 };
		GetLocalTime( &kSTime );

		// 년
		if( COMPARE( g_CampMgr.GetStartSeasonYear(), 1971, 3001 ) )
			kSTime.wYear = g_CampMgr.GetStartSeasonYear();

		// 월
		if( COMPARE( g_CampMgr.GetStartSeasonMonth(), 1, 13 ) )
			kSTime.wMonth = g_CampMgr.GetStartSeasonMonth();

		// 일
		if( COMPARE( g_CampMgr.GetStartSeasonDay(), 1, 32 ) )
			kSTime.wDay = g_CampMgr.GetStartSeasonDay();

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY ); 

		enum { DAY_SEC = 86400 };
		time_t tm_season = DateHelp::ConvertSecondTime( kSTime.wYear, kSTime.wMonth, kSTime.wDay, 6, 0, 0 );
		for(int i = 0; i < HERO_SEASON_RANK_MAX;i++)
		{
			struct tm kDate = *localtime( &tm_season );
			g_FontMgr.PrintText( iXPos + 247 - ( i * 41 ), iYPos + 201, FONT_SIZE_12, STR(1), kDate.tm_mon + 1, kDate.tm_mday );
			tm_season -= ( DAY_SEC * 14 ) ; // 14일씩 뒤로 이동
		}
	}
	else
	{
		for(int i = 0; i < HERO_SEASON_RANK_MAX;i++)
		{
			DWORD dwDate = g_RankBattleMgr.GetRankRecord( (HERO_SEASON_RANK_MAX-1) - i ).m_dwDate;	
			int iMonth = ( dwDate / 100 ) % 100;
			int iDay = dwDate % 100;
			g_FontMgr.PrintText( iXPos + 247 - ( i * 41 ), iYPos + 201, FONT_SIZE_12, STR(1), iMonth, iDay );
		}
	}
	g_FontMgr.PrintText( iXPos + 288, iYPos + 201, FONT_SIZE_12, STR(2) );
}

void HeroMatchRankGraphWnd::OnRenderGraph( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); 

	int i = 0;
	// 동글 + 라인
	for(i = 0;i < (int)m_GraphData.size();i++)
	{
		GraphData &rkData = m_GraphData[i];
		if( rkData.m_dwAniState == GRAPH_ANI_NONE ) continue;

		if( rkData.m_pLine && rkData.m_dwAniState >= GRAPH_ANI_LINE )
		{
			int iMinusYPos = 0;
			if( rkData.m_bLineReverse )
			{
				rkData.m_pLine->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
			}
			else
			{
				rkData.m_pLine->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
				iMinusYPos = rkData.m_iNextRankPosGap * 10;
			}
			rkData.m_pLine->RenderWidthCut( iXPos + 41 + i * 41, ( iYPos + 36 + rkData.m_iRankPos * 10 ) - iMinusYPos,
											0, (float)rkData.m_pLine->GetWidth() * rkData.m_fAniRate, UI_RENDER_NORMAL_ADD_COLOR );
		}

		if( rkData.m_pCircle )
		{
			if( rkData.m_dwAniState == GRAPH_ANI_CIRCLE )
			{
				rkData.m_pCircle->SetColor( TCT_DEFAULT_WHITE );
				rkData.m_pCircle->SetScale( 2.0f - rkData.m_fAniRate );
				rkData.m_pCircle->Render( iXPos + 36 + i * 41, iYPos + 31 + rkData.m_iRankPos * 10, UI_RENDER_NORMAL_ADD_COLOR );
			}
			else
			{
				rkData.m_pCircle->SetScale( FLOAT1 );
				rkData.m_pCircle->Render( iXPos + 36 + i * 41, iYPos + 31 + rkData.m_iRankPos * 10 );
			}
		}
	}

	// 랭킹 표시
	for(i = 0;i < (int)m_GraphData.size();i++)
	{
		GraphData &rkData = m_GraphData[i];
		if( rkData.m_dwAniState <= GRAPH_ANI_CIRCLE ) continue;
		if( rkData.m_bViewRanking )
		{
			if( rkData.m_iRanking == 0 )
				g_FontMgr.PrintText( iXPos + 42 + i * 41, iYPos + 16 + rkData.m_iRankPos * 10, FONT_SIZE_12, STR(1) );
			else
				g_FontMgr.PrintText( iXPos + 42 + i * 41, iYPos + 16 + rkData.m_iRankPos * 10, FONT_SIZE_12, STR(2), rkData.m_iRanking );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
HeroMatchVersusSceneWnd::HeroMatchVersusSceneWnd()
{
	m_pBlueFrame	= NULL;
	m_pRedFrame		= NULL;
	m_pBackFrame    = NULL;

	m_pBlueGra		= NULL;
	m_pRedGra		= NULL;
	m_pDarkBar		= NULL;
	m_pLightBar		= NULL;
	m_pBlackStrip	= NULL;
	m_pWhiteStrip	= NULL;
	m_pHeroRankTitle= NULL;
	m_pNumber		= NULL;
	m_pRankText		= NULL;
	m_pNoneText		= NULL;
	m_pLadderHeroMark = NULL;
	m_pCharShadow	= NULL;
	for(int i = 0;i < MAX_EFFECT_SPR;i++)
		m_pEffectSprite[i] = NULL;

	m_pBlue3DRender = NULL;
	m_pBlueChar		= NULL;
	m_pRed3DRender  = NULL;
	m_pRedChar		= NULL;

	m_bFadeOutStart = false;
	m_pPlayMode		= NULL;
	m_iSelectChar	= 0;
	m_dwSelectTime  = 0;
}

HeroMatchVersusSceneWnd::~HeroMatchVersusSceneWnd()
{
	SAFEDELETE( m_pBlueFrame );
	SAFEDELETE( m_pRedFrame );
	SAFEDELETE( m_pBackFrame );

	SAFEDELETE( m_pBlueGra );
	SAFEDELETE( m_pRedGra );
	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );
	SAFEDELETE( m_pBlackStrip );
	SAFEDELETE( m_pWhiteStrip );
	SAFEDELETE( m_pHeroRankTitle );
	SAFEDELETE( m_pNumber );
	SAFEDELETE( m_pRankText );
	SAFEDELETE( m_pNoneText );
	SAFEDELETE( m_pLadderHeroMark );
	SAFEDELETE( m_pCharShadow );
	for(int i = 0;i < MAX_EFFECT_SPR;i++)
		SAFEDELETE( m_pEffectSprite[i] );

	SAFEDELETE( m_pBlueChar );
	SAFEDELETE( m_pBlue3DRender );
	SAFEDELETE( m_pRedChar );
	SAFEDELETE( m_pRed3DRender );
}

void HeroMatchVersusSceneWnd::iwm_create()
{
	{
		SAFEDELETE( m_pBlue3DRender );
		m_pBlue3DRender = new ioUI3DRender;
		m_pBlue3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
	}

	{
		SAFEDELETE( m_pRed3DRender );
		m_pRed3DRender = new ioUI3DRender;
		m_pRed3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
	}
}

void HeroMatchVersusSceneWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueBar" )
	{
		SAFEDELETE( m_pBlueFrame );
		m_pBlueFrame = pFrame;
	}
	else if( szType == "RedBar" )
	{
		SAFEDELETE( m_pRedFrame );
		m_pRedFrame = pFrame;
	}
	else if( szType == "BackFrame" )
	{
		SAFEDELETE( m_pBackFrame );
		m_pBackFrame = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void HeroMatchVersusSceneWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueGra" )
	{
		SAFEDELETE( m_pBlueGra );
		m_pBlueGra = pImage;
	}
	else if( szType == "RedGra" )
	{
		SAFEDELETE( m_pRedGra );
		m_pRedGra = pImage;
	}
	else if( szType == "DarkBar" )
	{
		SAFEDELETE( m_pDarkBar );
		m_pDarkBar = pImage;
	}
	else if( szType == "LightBar" )
	{
		SAFEDELETE( m_pLightBar );
		m_pLightBar = pImage;
	}
	else if( szType == "BlackStrip" )
	{
		SAFEDELETE( m_pBlackStrip );
		m_pBlackStrip = pImage;
	}
	else if( szType == "WhiteStrip" )
	{
		SAFEDELETE( m_pWhiteStrip );
		m_pWhiteStrip = pImage;
	}
	else if( szType == "HeroRankTitle" )
	{
		SAFEDELETE( m_pHeroRankTitle );
		m_pHeroRankTitle = pImage;
	}
	else if( szType == "Number" )
	{
		SAFEDELETE( m_pNumber );
		m_pNumber = pImage;
	}
	else if( szType == "RankText" )
	{
		SAFEDELETE( m_pRankText );
		m_pRankText = pImage;
	}
	else if( szType == "NoneText" )
	{
		SAFEDELETE( m_pNoneText );
		m_pNoneText = pImage;
	}
	else if( szType == "LadderHeroMark" )
	{
		SAFEDELETE( m_pLadderHeroMark );
		m_pLadderHeroMark = pImage;
	}
	else if( szType == "CharShadow" )
	{
		SAFEDELETE( m_pCharShadow );
		m_pCharShadow = pImage;
	}
	else if( szType == "EffectSprite0" )
	{
		SAFEDELETE( m_pEffectSprite[0] );
		m_pEffectSprite[0] = pImage;
	}
	else if( szType == "EffectSprite1" )
	{
		SAFEDELETE( m_pEffectSprite[1] );
		m_pEffectSprite[1] = pImage;
	}
	else if( szType == "EffectSprite2" )
	{
		SAFEDELETE( m_pEffectSprite[2] );
		m_pEffectSprite[2] = pImage;
	}
	else if( szType == "EffectSprite3" )
	{
		SAFEDELETE( m_pEffectSprite[3] );
		m_pEffectSprite[3] = pImage;
	}
	else if( szType == "EffectSprite4" )
	{
		SAFEDELETE( m_pEffectSprite[4] );
		m_pEffectSprite[4] = pImage;
	}
	else if( szType == "EffectSprite5" )
	{
		SAFEDELETE( m_pEffectSprite[5] );
		m_pEffectSprite[5] = pImage;
	}
	else if( szType == "EffectSprite6" )
	{
		SAFEDELETE( m_pEffectSprite[6] );
		m_pEffectSprite[6] = pImage;
	}
	else if( szType == "EffectSprite7" )
	{
		SAFEDELETE( m_pEffectSprite[7] );
		m_pEffectSprite[7] = pImage;
	}
	else if( szType == "EffectSprite8" )
	{
		SAFEDELETE( m_pEffectSprite[8] );
		m_pEffectSprite[8] = pImage;
	}
	else if( szType == "EffectSprite9" )
	{
		SAFEDELETE( m_pEffectSprite[9] );
		m_pEffectSprite[9] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void HeroMatchVersusSceneWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			SetSize( Setting::Width(), Setting::Height() - 42 );
		}
	}
}

void HeroMatchVersusSceneWnd::StartFadeOut( ioPlayMode *pPlayMode, int now_select_char, DWORD dwSelectTime )
{
	m_bFadeOutStart = true;

	m_pPlayMode		= pPlayMode;
	m_iSelectChar	= now_select_char;
	m_dwSelectTime  = dwSelectTime;
}

void HeroMatchVersusSceneWnd::OtherHeroInfoUpdate()
{
	//적팀이 블루일 때
	if( g_LadderTeamMgr.GetOtherTeamType() == TEAM_RED )
	//if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		// 상대 정보
		m_RedData.m_Data = g_LadderTeamMgr.GetHeroMatchOtherInfo();
		SAFEDELETE( m_pRedChar );
		m_pRedChar = GetCreateCharacter( m_pRed3DRender, m_RedData.m_Data );
	}
	else
	{
		// 상대 정보
		m_BlueData.m_Data = g_LadderTeamMgr.GetHeroMatchOtherInfo();
		SAFEDELETE( m_pBlueChar );
		m_pBlueChar = GetCreateCharacter( m_pBlue3DRender, m_BlueData.m_Data );
	}

	// 순위에 따라서 Y값 변화
	if( m_BlueData.m_Data.m_iHeroTodayRank < m_RedData.m_Data.m_iHeroTodayRank )
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 133;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 113;
	}
	else if( m_BlueData.m_Data.m_iHeroTodayRank > m_RedData.m_Data.m_iHeroTodayRank )
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 113;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 133;
	}
	else
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 123;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 123;
	}
}

void HeroMatchVersusSceneWnd::iwm_show()
{
	SetSize( Setting::Width(), Setting::Height() - 42 );
	/*
	  ANI
	  1단계 - 검은 배경 / 좌(정부군) - 우(반란군) 카드 끝에서 부터 Sin 출력 정중앙까지 이동 : 0 ~ 230ms
	  2단계 - 좌(정부군) - 우(반란군) 카드 하얀색 이펙트(알파 시작) + 좌 - 우측으로 밀림    : 230ms ~ 730ms
	        - 가운데 VS 하얀색 이펙트(알파 시작)										    : 230ms ~ 730ms
			- 가운데 VS 이펙트 01~10까지 진행하고 없어짐								    : 230ms ~ 730ms
			- 화면 전체 하얀색 이펙트(알파 시작)										    : 230ms ~ 730ms	
	  3단계 - 사라짐 진행
	        - 가운데 VS 400%로 확대되면서 투명도 0%로 진행                                  : 0 ~ 100ms
			- 검은 배경 투명도 0%로 진행                                                    : 0 ~ 200ms
			- 좌(정부군) - 우(반란군) 카드 좌/우로 사라짐                                   : 0 ~ 200ms
	*/ 

	int i = 0;
	//적팀 팀이 Red 일때
	if( g_LadderTeamMgr.GetOtherTeamType() == TEAM_RED )
	//if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		// 내 정보
		m_BlueData.m_Data.m_szUserID = g_MyInfo.GetPublicID();
		m_BlueData.m_Data.m_iGradeLevel = g_MyInfo.GetGradeLevel();
		m_BlueData.m_Data.m_iHeroTodayRank = g_MyInfo.GetHeroRank();
		m_BlueData.m_Data.m_iHeroTitle = g_MyInfo.GetHeroTitle();
		m_BlueData.m_Data.m_iHeroWin = g_MyInfo.GetRecordWin( RECORD_HEROMATCH );
		m_BlueData.m_Data.m_iHeroLose= g_MyInfo.GetRecordLose( RECORD_HEROMATCH );

		DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		m_BlueData.m_Data.m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			m_BlueData.m_Data.m_EquipItem[i].m_item_code = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_BlueData.m_Data.m_EquipItem[i].m_item_male_custom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_BlueData.m_Data.m_EquipItem[i].m_item_female_custom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
		}
		SAFEDELETE( m_pBlueChar );
		m_pBlueChar = GetCreateCharacter( m_pBlue3DRender, m_BlueData.m_Data );

		// 상대 정보
		m_RedData.m_Data = g_LadderTeamMgr.GetHeroMatchOtherInfo();
		SAFEDELETE( m_pRedChar );
		m_pRedChar = GetCreateCharacter( m_pRed3DRender, m_RedData.m_Data );
	}
	else
	{
		// 내 정보
		m_RedData.m_Data.m_szUserID = g_MyInfo.GetPublicID();
		m_RedData.m_Data.m_iGradeLevel = g_MyInfo.GetGradeLevel();
		m_RedData.m_Data.m_iHeroTodayRank = g_MyInfo.GetHeroRank();
		m_RedData.m_Data.m_iHeroTitle = g_MyInfo.GetHeroTitle();
		m_RedData.m_Data.m_iHeroWin = g_MyInfo.GetRecordWin( RECORD_HEROMATCH );
		m_RedData.m_Data.m_iHeroLose= g_MyInfo.GetRecordLose( RECORD_HEROMATCH );

		DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		m_RedData.m_Data.m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
		for(i = 0;i < MAX_INVENTORY;i++)
		{
			m_RedData.m_Data.m_EquipItem[i].m_item_code = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_RedData.m_Data.m_EquipItem[i].m_item_male_custom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
			m_RedData.m_Data.m_EquipItem[i].m_item_female_custom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
		}
		SAFEDELETE( m_pRedChar );
		m_pRedChar = GetCreateCharacter( m_pRed3DRender, m_RedData.m_Data );

		// 상대 정보
		m_BlueData.m_Data = g_LadderTeamMgr.GetHeroMatchOtherInfo();
		SAFEDELETE( m_pBlueChar );
		m_pBlueChar = GetCreateCharacter( m_pBlue3DRender, m_BlueData.m_Data );
	}

	// 좌표 설정
	m_BlueData.m_dwAniState = CARD_ANI_START;
	m_BlueData.m_dwMoveTime = 0;  // 시간을 먼저 넣지않은 이유는 Show된 시점이 로딩중일 수 있다
	m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = -147;	

	m_RedData.m_dwAniState = CARD_ANI_START;
	m_RedData.m_dwMoveTime = 0;
	m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = Setting::Width() + 147;

	if( m_BlueData.m_Data.m_iHeroTodayRank < m_RedData.m_Data.m_iHeroTodayRank )
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 133;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 113;
	}
	else if( m_BlueData.m_Data.m_iHeroTodayRank > m_RedData.m_Data.m_iHeroTodayRank )
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 113;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 133;
	}
	else
	{
		m_BlueData.m_iStartYPos = m_BlueData.m_iCurrentYPos = Setting::HalfHeight() - 123;
		m_RedData.m_iStartYPos = m_RedData.m_iCurrentYPos = Setting::HalfHeight() - 123;
	}
}

ioCreateChar *HeroMatchVersusSceneWnd::GetCreateCharacter( ioUI3DRender *p3DRender, LadderHeroUserInfo &rkData )
{
	if( !p3DRender ) return NULL;
	if( rkData.m_CharInfo.m_class_type == 0 ) return NULL;

	ioCreateChar *pChar = p3DRender->CreateChar( rkData.m_CharInfo, "ResultChar" );
	if( !pChar ) return NULL;

	pChar->SetPosition( 0.0f, -FLOAT90, 900.0f );
	pChar->SetYaw( FLOAT10 );
	p3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	p3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	// 코스튬 관련 (??)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = rkData.m_CharInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && rkData.m_szUserID != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( rkData.m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			DWORD dwItemCode = rkData.m_EquipItem[i].m_item_code;
			DWORD dwItemMaleCustom = rkData.m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwItemFemaleCustom = rkData.m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( TempCostume[i] <= 0 )
				{
					dwItemMaleCustom = 0;
					dwItemFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( TempCostume[i] <= 0 )
				{
					dwItemMaleCustom = rkData.m_EquipItem[i].m_item_male_custom;
					dwItemFemaleCustom = rkData.m_EquipItem[i].m_item_female_custom;
				}
				pChar->EquipItem( rkData.m_EquipItem[i].m_item_code, dwItemMaleCustom, dwItemFemaleCustom, TempCostume[i] );
			}
		}
	}

	if( !pChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = p3DRender->GetModelAnimation( 0, pChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			pChar->SetLoopAni( szUIAnimation );
		}
	}
	return pChar;
}

void HeroMatchVersusSceneWnd::iwm_hide()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->StartChangeCharacter( m_iSelectChar, m_dwSelectTime );
	}
}

void HeroMatchVersusSceneWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		return;
	}

	OnProcessChar();
	OnProcessVS();

	if( m_pBlueChar )
		m_pBlueChar->Update( fTimePerSec );
	if( m_pRedChar )
		m_pRedChar->Update( fTimePerSec );
}

void HeroMatchVersusSceneWnd::OnProcessChar()
{
	// 블루 - 가운데 vs 마크와 배경은 블루 기준으로 작동한다.
	switch( m_BlueData.m_dwAniState )
	{
	case CARD_ANI_START:
		{
			if( m_BlueData.m_dwMoveTime == 0 )
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= START_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / START_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos + ( fMoveGap * fRate );
			}
			else
			{
				m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = ( Setting::HalfWidth() - 147.0f );
				m_BlueData.m_fAniRate   = FLOAT1;
				m_BlueData.m_dwAniState = CARD_ANI_BACK;
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();

				// VS Ani Start
				m_VersusAni.m_dwAniState = VS_ANI_START;
				m_VersusAni.m_dwAniTime  = FRAMEGETTIME();
				m_VersusAni.m_fAniRate   = FLOAT1;
				m_VersusAni.m_dwEffectTime =  FRAMEGETTIME();
				m_VersusAni.m_iEffectArray = 0;
			}
		}
		break;
	case CARD_ANI_BACK:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= BACK_MOVE_TIME )
			{
				float fRate = min( FLOAT1, sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / BACK_MOVE_TIME ) );
				float fMoveGap = 17.0f;
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos - ( fMoveGap * fRate );
				m_BlueData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / BACK_MOVE_TIME );
			}
			else
			{
				m_BlueData.m_iStartXPos = m_BlueData.m_iCurrentXPos = ( Setting::HalfWidth() - 164.0f );
				m_BlueData.m_fAniRate   = 0.0f;
				m_BlueData.m_dwAniState = CARD_ANI_DELAY;
				m_BlueData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_DELAY:
		{
			if( m_bFadeOutStart )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
				if( dwGapTime > DELAY_TIME )
				{
					m_BlueData.m_dwAniState = CARD_ANI_FADEOUT;
					m_BlueData.m_dwMoveTime = FRAMEGETTIME();

					// VS Ani FADEOUT
					m_VersusAni.m_dwAniState = VS_ANI_FADEOUT;
					m_VersusAni.m_dwAniTime  = FRAMEGETTIME();
					m_VersusAni.m_fAniRate   = 0.0f;
				}
			}
		}
		break;
	case CARD_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_BlueData.m_dwMoveTime;
			if( dwGapTime <= FADEOUT_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / FADEOUT_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_BlueData.m_iCurrentXPos = m_BlueData.m_iStartXPos - ( fMoveGap * fRate );

				//없어질 때 백그라운드 알파
				m_BlueData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / FADEOUT_MOVE_TIME );
			}
			else
			{				
				HideWnd();
			}
		}
		break;
	}

	// 레드
	switch( m_RedData.m_dwAniState )
	{
	case CARD_ANI_START:
		{
			if( m_RedData.m_dwMoveTime == 0 )
				m_RedData.m_dwMoveTime = FRAMEGETTIME();

			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= START_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / START_MOVE_TIME ) );
				float fMoveGap = 147.0f + Setting::HalfWidth();
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos - ( fMoveGap * fRate );
			}
			else
			{
				m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = Setting::HalfWidth();
				m_RedData.m_fAniRate   = FLOAT1;
				m_RedData.m_dwAniState = CARD_ANI_BACK;
				m_RedData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_BACK:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= BACK_MOVE_TIME )
			{
				float fRate = min( FLOAT1, sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / BACK_MOVE_TIME ) );
				float fMoveGap = 17.0f;
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos + ( fMoveGap * fRate );
				m_RedData.m_fAniRate   = min( FLOAT1, FLOAT1 - (float) dwGapTime / BACK_MOVE_TIME );
			}
			else
			{
				m_RedData.m_iStartXPos = m_RedData.m_iCurrentXPos = ( Setting::HalfWidth() + 17.0f );
				m_RedData.m_fAniRate   = 0.0f;
				m_RedData.m_dwAniState = CARD_ANI_DELAY;
				m_RedData.m_dwMoveTime = FRAMEGETTIME();
			}
		}
		break;
	case CARD_ANI_DELAY:
		{
			if( m_bFadeOutStart )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
				if( dwGapTime > DELAY_TIME )
				{
					m_RedData.m_dwAniState = CARD_ANI_FADEOUT;
					m_RedData.m_dwMoveTime = FRAMEGETTIME();
				}
			}
		}
		break;
	case CARD_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_RedData.m_dwMoveTime;
			if( dwGapTime <= FADEOUT_MOVE_TIME )
			{
				float fRate = min( FLOAT1, FLOAT1 - cosf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / FADEOUT_MOVE_TIME ) );
				float fMoveGap = Setting::HalfWidth();
				m_RedData.m_iCurrentXPos = m_RedData.m_iStartXPos + ( fMoveGap * fRate );
			}
		}
		break;
	}
}

void HeroMatchVersusSceneWnd::OnProcessVS()
{
	switch( m_VersusAni.m_dwAniState )
	{
	case VS_ANI_START:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_VersusAni.m_dwAniTime;
			if( dwGapTime <= VS_START_TIME )
			{
				m_VersusAni.m_fAniRate = min( FLOAT1, FLOAT1 - (float)dwGapTime / VS_START_TIME );

				if( FRAMEGETTIME() - m_VersusAni.m_dwEffectTime > 50 )
				{
					m_VersusAni.m_dwEffectTime =  FRAMEGETTIME();
					m_VersusAni.m_iEffectArray++;
				}
			}
			else
			{
				m_VersusAni.m_fAniRate = 0.0f;
				m_VersusAni.m_dwEffectTime = 0;
				m_VersusAni.m_iEffectArray = MAX_EFFECT_SPR;
			}
		}
		break;
	case VS_ANI_FADEOUT:
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_VersusAni.m_dwAniTime;
			if( dwGapTime <= VS_FADEOUT_TIME )
			{
				m_VersusAni.m_fAniRate = (float)dwGapTime / VS_FADEOUT_TIME;
			}
		}
		break;
	}

}

void HeroMatchVersusSceneWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	{   // 백 그라운드 
		OnRenderBack( iXPos, iYPos );
	}

	{   // 캐릭터 카드
		OnRenderBlueChar( iXPos + m_BlueData.m_iCurrentXPos, iYPos + m_BlueData.m_iCurrentYPos );
		OnRenderRedChar( iXPos + m_RedData.m_iCurrentXPos, iYPos + m_RedData.m_iCurrentYPos );
	}
	
	{   // VERSUS
		OnRenderVersus( iXPos + Setting::HalfWidth(), iYPos + Setting::HalfHeight() - 20 );
	}
}

void HeroMatchVersusSceneWnd::OnRenderBack( int iXPos, int iYPos )
{
	if( m_pBlackStrip )
	{
		m_pBlackStrip->SetSize( Setting::Width(), Setting::Height() );
		m_pBlackStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBlackStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

		if( m_BlueData.m_dwAniState == CARD_ANI_FADEOUT )
		{
			m_pBlackStrip->SetAlpha( (float)MAX_ALPHA_RATE * m_BlueData.m_fAniRate );
		}
		m_pBlackStrip->Render( iXPos, iYPos );
	}
}

void HeroMatchVersusSceneWnd::OnRenderBlueChar( int iXPos, int iYPos )
{
	if( m_pBackFrame )
	{
		m_pBackFrame->SetAlpha( MAX_ALPHA_RATE );
		m_pBackFrame->Render( iXPos, iYPos );
	}

	if( m_pBlueFrame )
		m_pBlueFrame->Render( iXPos, iYPos );

	// 
	if( m_pBlue3DRender && m_pBlueChar )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 3;
		rcD3D.y1 = iYPos + 3;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pBlue3DRender->RenderViewPort( &rcD3D, true );
	}
	else if( m_pCharShadow )
	{
		m_pCharShadow->Render( iXPos, iYPos );
	}

    //
	if( m_pBlueGra )
		m_pBlueGra->Render( iXPos, iYPos );

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	//
	if( m_pHeroRankTitle )
		m_pHeroRankTitle->Render( iXPos + 73, iYPos + 113 );

	int iHeroRank = m_BlueData.m_Data.m_iHeroTodayRank;
	int iNumberCount = GetNumberCount( iHeroRank );
	if( iHeroRank == 0 || iNumberCount >= 6 )
	{
		if( m_pNoneText )
			m_pNoneText->Render( iXPos + 73, iYPos + 132 );
	}
	else
	{
		enum { NUM_W = 27, NUM_GAP = 5, TEXT_W = 27,};
		if( iNumberCount >= 5 )
		{
			// 위 텍스트 찍지 않음
			if( m_pNumber )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pNumber->RenderNum( iXPos + 73, iYPos + 132, iHeroRank, -NUM_GAP );
			}
		}
		else
		{
			int iNumberSize = (iNumberCount*NUM_W) - ((iNumberCount-1)*NUM_GAP);
			int iStartPos   = (iNumberSize + TEXT_W) / 2;
			if( m_pNumber && m_pRankText )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
				m_pNumber->RenderNum( (iXPos + 73) - iStartPos, iYPos + 132, iHeroRank, -NUM_GAP );

				m_pRankText->Render( ((iXPos + 73) - iStartPos)+(iNumberSize-NUM_GAP), iYPos + 132 );
			}
		}
	}

	//
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( TCT_DEFAULT_BLUE );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	if( m_BlueData.m_Data.m_iHeroTitle == 0 )
		g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, "-" );
	else
		g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, STR(1), g_LevelMgr.GetHeroTitleString( m_BlueData.m_Data.m_iHeroTitle ).c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 73, iYPos + 203, FONT_SIZE_13, 240.0f, STR(2), m_BlueData.m_Data.m_szUserID.c_str() );
	
	int iWinCount = m_BlueData.m_Data.m_iHeroWin;
	int iLoseCount= m_BlueData.m_Data.m_iHeroLose;
	int iRecordRate = ( (float)iWinCount / (float)max( 1, iWinCount + iLoseCount ) ) * 100;
	g_FontMgr.PrintText( iXPos + 73, iYPos + 224, FONT_SIZE_13, STR(3), iWinCount, iLoseCount, iRecordRate );

	if( m_BlueData.m_dwAniState == CARD_ANI_BACK && m_pBackFrame && m_BlueData.m_fAniRate > 0.0f )
	{
		m_pBackFrame->SetColor( TCT_DEFAULT_WHITE );
		m_pBackFrame->SetAlpha( (float)MAX_ALPHA_RATE * m_BlueData.m_fAniRate );
		m_pBackFrame->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
}

void HeroMatchVersusSceneWnd::OnRenderRedChar( int iXPos, int iYPos )
{
	if( m_pBackFrame )
	{
		m_pBackFrame->SetAlpha( MAX_ALPHA_RATE );
		m_pBackFrame->Render( iXPos, iYPos );
	}

	if( m_pRedFrame )
		m_pRedFrame->Render( iXPos, iYPos );

	// 
	if( m_pRed3DRender && m_pRedChar )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 3;
		rcD3D.y1 = iYPos + 3;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pRed3DRender->RenderViewPort( &rcD3D, true );
	}
	else if( m_pCharShadow )
	{
		m_pCharShadow->Render( iXPos, iYPos );
	}

	//
	if( m_pRedGra )
		m_pRedGra->Render( iXPos, iYPos );

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	//
	if( m_pHeroRankTitle )
		m_pHeroRankTitle->Render( iXPos + 73, iYPos + 113 );

	int iHeroRank = m_RedData.m_Data.m_iHeroTodayRank;
	int iNumberCount = GetNumberCount( iHeroRank );
	if( iHeroRank == 0 || iNumberCount >= 6 )
	{
		if( m_pNoneText )
			m_pNoneText->Render( iXPos + 73, iYPos + 132 );
	}
	else
	{
		enum { NUM_W = 27, NUM_GAP = 5, TEXT_W = 27,};
		if( iNumberCount >= 5 )
		{
			// 위 텍스트 찍지 않음
			if( m_pNumber )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pNumber->RenderNum( iXPos + 73, iYPos + 132, iHeroRank, -NUM_GAP );
			}
		}
		else
		{
			int iNumberSize = (iNumberCount*NUM_W) - ((iNumberCount-1)*NUM_GAP);
			int iStartPos   = (iNumberSize + TEXT_W) / 2;
			if( m_pNumber && m_pRankText )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
				m_pNumber->RenderNum( (iXPos + 73) - iStartPos, iYPos + 132, iHeroRank, -NUM_GAP );

				m_pRankText->Render( ((iXPos + 73) - iStartPos)+(iNumberSize-NUM_GAP), iYPos + 132 );
			}
		}
	}

	//
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( TCT_DEFAULT_RED );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	if( m_RedData.m_Data.m_iHeroTitle == 0 )
		g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, "-" );
	else
		g_FontMgr.PrintText( iXPos + 73, iYPos + 182, FONT_SIZE_13, STR(1), g_LevelMgr.GetHeroTitleString( m_RedData.m_Data.m_iHeroTitle ).c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 73, iYPos + 203, FONT_SIZE_13, 240.0f, STR(2), m_RedData.m_Data.m_szUserID.c_str() );

	int iWinCount = m_RedData.m_Data.m_iHeroWin;
	int iLoseCount= m_RedData.m_Data.m_iHeroLose;
	int iRecordRate = ( (float)iWinCount / (float)max( 1, iWinCount + iLoseCount ) ) * 100;
	g_FontMgr.PrintText( iXPos + 73, iYPos + 224, FONT_SIZE_13, STR(3), iWinCount, iLoseCount, iRecordRate );

	if( m_RedData.m_dwAniState == CARD_ANI_BACK && m_pBackFrame && m_RedData.m_fAniRate > 0.0f )
	{
		m_pBackFrame->SetColor( TCT_DEFAULT_WHITE );
		m_pBackFrame->SetAlpha( (float)MAX_ALPHA_RATE * m_RedData.m_fAniRate );
		m_pBackFrame->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
	}
}

void HeroMatchVersusSceneWnd::OnRenderVersus( int iXPos, int iYPos )
{
	if( m_VersusAni.m_dwAniState == VS_ANI_NONE ) 
		return;

	if( m_pLadderHeroMark )
	{
		if( m_VersusAni.m_dwAniState == VS_ANI_FADEOUT )
		{
			m_pLadderHeroMark->SetScale( 4.0f * m_VersusAni.m_fAniRate );
			m_pLadderHeroMark->SetAlpha( (float)MAX_ALPHA_RATE * ( FLOAT1 - m_VersusAni.m_fAniRate ) );
			m_pLadderHeroMark->Render( iXPos, iYPos );
		}
		else
		{
			m_pLadderHeroMark->SetAlpha( MAX_ALPHA_RATE );
			m_pLadderHeroMark->Render( iXPos, iYPos );

			if( m_VersusAni.m_dwAniState == VS_ANI_START && m_VersusAni.m_fAniRate > 0.0f )
			{
				m_pLadderHeroMark->SetColor( TCT_DEFAULT_WHITE );
				m_pLadderHeroMark->SetAlpha( (float)MAX_ALPHA_RATE * m_VersusAni.m_fAniRate );
				m_pLadderHeroMark->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
			}
		}
	}

	// 이펙트
	if( m_VersusAni.m_dwAniState == VS_ANI_START )
	{
		if( COMPARE( m_VersusAni.m_iEffectArray, 0, MAX_EFFECT_SPR ) )
		{
			if( m_pEffectSprite[m_VersusAni.m_iEffectArray] )
			{
				m_pEffectSprite[m_VersusAni.m_iEffectArray]->SetScale( 4.0f );
				m_pEffectSprite[m_VersusAni.m_iEffectArray]->Render( iXPos, iYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
	}

	// 화면 전체 허여멀건
	if( m_VersusAni.m_dwAniState == VS_ANI_START )
	{
		if( m_pWhiteStrip && m_VersusAni.m_fAniRate > 0.0f )
		{
			m_pWhiteStrip->SetSize( Setting::Width(), Setting::Height() );
			m_pWhiteStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pWhiteStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pWhiteStrip->SetAlpha( (float)MAX_ALPHA_RATE * ( FLOAT05 * m_VersusAni.m_fAniRate ) );
			m_pWhiteStrip->Render( 0, 0 );
		}
	}	
}