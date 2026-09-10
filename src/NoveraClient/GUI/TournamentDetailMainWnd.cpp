#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/HelpFunc.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/safesprintf.h"

#include "../WndID.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioCreateChar.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"
#include "../ioTournamentRewardManager.h"
#include "../ioDateHelp.h"

#include "ioUI3DRender.h"
#include "ioSP2GUIManager.h"

#include "TournamentDetailMainWnd.h"
#include "TournamentMainWnd.h"
#include "CustomTournamentCreateWnd.h"
#include "CustomTournamentWnd.h"

#include "PartyMenuWnd.h"

//////////////////////////////////////////////////////////////////////////
TournamentTeamCharWnd::TournamentTeamCharWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;

	m_pBubbleGra	= NULL;
	m_pBubbleTail	= NULL;
	m_pNumberText	= NULL;
	m_pRoundText	= NULL;
	m_pFinalText	= NULL;

	m_TourPos			= 0;
	m_Position			= 0;
	m_dwMaxRound		= 0;
	m_dwMaxStartTeam	= 0;
	m_dwNextRoundDate	= 0;

	m_dwStartTime = 0;
}

TournamentTeamCharWnd::~TournamentTeamCharWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	SAFEDELETE( m_pBubbleGra );
	SAFEDELETE( m_pBubbleTail );
	SAFEDELETE( m_pNumberText );
	SAFEDELETE( m_pRoundText );
	SAFEDELETE( m_pFinalText );
}

void TournamentTeamCharWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void TournamentTeamCharWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BubbleGra" )
	{
		SAFEDELETE( m_pBubbleGra );
		m_pBubbleGra = pImage;

	}
	else if( szType == "BubbleTail" )
	{
		SAFEDELETE( m_pBubbleTail );
		m_pBubbleTail = pImage;

	}
	else if( szType == "NumberText" )
	{
		SAFEDELETE( m_pNumberText );
		m_pNumberText = pImage;

	}
	else if( szType == "RoundText" )
	{
		SAFEDELETE( m_pRoundText );
		m_pRoundText = pImage;

	}
	else if( szType == "FinalText" )
	{
		SAFEDELETE( m_pFinalText );
		m_pFinalText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentTeamCharWnd::SetTournamentTeamData( DWORD dwMyTeamIndex )
{
	// 활성화 조건 : 다음 경기가 정해졌을 때!!
	m_TourPos			= 0;
	m_Position			= 0;
	m_dwMaxRound		= 0;
	m_dwMaxStartTeam	= 0;
	m_dwNextRoundDate	= 0;
	TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( dwMyTeamIndex );
	if( pTeamData )
	{
		m_TourPos	= pTeamData->GetTourPos();
		m_Position	= pTeamData->GetPosition();
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( pTeamData->GetTourIndex() );
		if( pTournament )
		{
			m_dwMaxRound		= pTournament->GetTournamentMaxRound();
			m_dwMaxStartTeam	= pTournament->GetTournamentStartTeamCount();
			m_dwNextRoundDate	= pTournament->GetStateEndDate();

			if( pTournament->GetState() == TournamentNode::STATE_WAITING )
			{
				m_TourPos = m_Position = 0;         // 대회 종료됨.
			}
			else if( pTournament->GetState() >= TournamentNode::STATE_TOURNAMENT && 
				     m_TourPos < ( pTournament->GetState() - TournamentNode::STATE_TOURNAMENT ) + 1 )
			{
				m_TourPos = m_Position = 0;			// 탈락
			}
			else if( m_TourPos > 0 )
			{
				// 진행중. 다음 초대 발송 시간 표시
				m_dwNextRoundDate = pTournament->GetStateEndDate( TournamentNode::STATE_TOURNAMENT + ( m_TourPos - 1 ) );
			}
		}
	}
	
	if( m_TourPos == 0 || m_Position == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

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
			DWORD dwCostumeMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
			else
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
					dwCostumeFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
				}
				pChar->EquipItem( dwItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
		}
	}

	if( !m_aMyChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			m_aMyChar->SetLoopAni( szUIAnimation );
		}
	}
}

void TournamentTeamCharWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void TournamentTeamCharWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderSoldier( iXPos + 3, iYPos + 3 );
	
	if( m_pBubbleGra )
	{
		m_pBubbleGra->Render( iXPos, iYPos );
	}

	if( m_pBubbleTail )
	{
		m_pBubbleTail->Render( iXPos + 43, iYPos + 172 );
	}

	int iNextTeamCount = Help::TournamentCurrentRoundWithTeam( m_dwMaxStartTeam, m_TourPos - 1 );
	if( iNextTeamCount <= 2 )
	{
		// 결승
		if( m_pFinalText )
			m_pFinalText->Render( iXPos +( GetWidth() / 2 ) - ( m_pFinalText->GetWidth() / 2 ), iYPos + 6 );
	}
	else if( m_pNumberText && m_pRoundText )
	{
		int iStartX     = 51;
		int iRoundSize  = 21;    //갭  - 4
		int iNumberCount = GetNumberCount( iNextTeamCount ); 
		int iNumberSize   = iNumberCount * 18;		// 숫자 1개가 22픽셀에 -4갭
		iStartX -= (float)(iRoundSize + iNumberSize) / 2;
		m_pNumberText->RenderNum( iXPos + iStartX, iYPos + 6, iNextTeamCount, -4 );
		m_pRoundText->Render( iXPos + iStartX + iNumberSize, iYPos + 6 );
	}

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 88, 88, 88 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	if( m_dwNextRoundDate == 0 )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 152, FONT_SIZE_12, 94.0f, "주최자 수동진행" );
	}
	else
	{
		char szNextRound[MAX_PATH] = "";
		sprintf_s_e( szNextRound, "%d/%d %02d:%02d", DateHelp::GetDateMonth( m_dwNextRoundDate ), DateHelp::GetDateDay( m_dwNextRoundDate ),
			DateHelp::GetDateHour( m_dwNextRoundDate ), DateHelp::GetDateMinute( m_dwNextRoundDate ) );
		g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 152, FONT_SIZE_12, 94.0f, "%s", szNextRound );
	}
}

void TournamentTeamCharWnd::OnRenderSoldier( int iXPos, int iYPos )
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
TournamentDetailMainTabBtn::TournamentDetailMainTabBtn()
{
	m_pActiveIcon = NULL;

	m_bClicked = false;
}

TournamentDetailMainTabBtn::~TournamentDetailMainTabBtn()
{
	SAFEDELETE( m_pActiveIcon );	
}

void TournamentDetailMainTabBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pActiveIcon && m_bClicked )
	{
		m_pActiveIcon->Render( iXPos, iYPos );
	}
}

void TournamentDetailMainTabBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ActiveMark" )
	{
		SAFEDELETE( m_pActiveIcon );
		m_pActiveIcon = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailMainTabBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_bClicked = xElement.GetBoolAttribute_e( "SetClicked" );
}


//////////////////////////////////////////////////////////////////////////

TournamentDetailMainWnd::TournamentDetailMainWnd()
{
	m_dwCurChildActiveListIdx = 0;
	m_dwTournamentIndex = 0;
	m_dwTournamentOwnerIndex = 0;
}

TournamentDetailMainWnd::~TournamentDetailMainWnd()
{
}

void TournamentDetailMainWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_dwTournamentOwnerIndex = pTournament->GetOwnerIndex();
		SetTournamentTtitle( pTournament );
		ShowWnd();
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 정보를 불러올 수 없습니다." );
		return;
	}

	for( int i = ID_TOURNAMENT_INFO_WND; i <= ID_AWARD_INFO_WND; ++i )
	{
		TournamentDetailFrameWnd* pWnd = dynamic_cast<TournamentDetailFrameWnd*>( FindChildWnd( i ) );
		if( pWnd )
		{
			pWnd->SetTournamentData( m_dwTournamentIndex );
		}
	}

	UpdateTournamentData( pTournament );

	if( g_App.IsMeDeveloper() || m_dwTournamentOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		ShowChildWnd( ID_BATTLE_ROOM_LIST );
	}
	else
	{
		HideChildWnd( ID_BATTLE_ROOM_LIST );
	}
}

void TournamentDetailMainWnd::SetAllocated( DWORD dwTourIdx )
{
	TournamentDetailFrameInfoWnd* pWnd = dynamic_cast<TournamentDetailFrameInfoWnd*>( FindChildWnd( ID_TOURNAMENT_INFO_WND ) );
	if( pWnd )
		pWnd->SetAllocated( dwTourIdx );
}

void  TournamentDetailMainWnd::SetTournamentTtitle( TournamentNode *pTournament )
{
	if( pTournament == NULL ) return;

	char szTitle[MAX_PATH] = "";
	if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
	{
		HideChildWnd( ID_ALLOCATE_INFO_BTN );
		ShowChildWnd( ID_PREV_TOURNAMENT_BTN );
		ShowChildWnd( ID_VICTORY_CHEER_BTN );
		HideChildWnd( ID_INVITE_PLAYER );
		sprintf_s_e( szTitle, "%s 정규 대회", pTournament->GetTournamentTitle().c_str() );
	}
	else
	{
		ShowChildWnd( ID_ALLOCATE_INFO_BTN );
		HideChildWnd( ID_VICTORY_CHEER_BTN );
		HideChildWnd( ID_PREV_TOURNAMENT_BTN );
		sprintf_s_e( szTitle, "%s 대회", pTournament->GetTournamentTitle().c_str() );

		if( pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() && pTournament->GetState() == TournamentNode::STATE_TEAM_APP )
		{
			HideChildWnd( ID_ENTER_PLAYER );
			ShowChildWnd( ID_INVITE_PLAYER );
		}
		else
		{
			ShowChildWnd( ID_ENTER_PLAYER );
			HideChildWnd( ID_INVITE_PLAYER );
		}
	}

	SetTitleText( szTitle );
}

void TournamentDetailMainWnd::UpdateTournamentData( TournamentNode *pTournament )
{
	if( pTournament == NULL ) return;
	SetTournamentTtitle( pTournament );

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		DWORD dwTeamIndex = pUserTournament->GetTeamIndex( m_dwTournamentIndex );
		TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIndex );
		if( pTeamData )
		{
			if( pTeamData->GetPosition() == 0 || 
				pTournament->GetState() == TournamentNode::STATE_WAITING || 
				( pTournament->GetState() >= TournamentNode::STATE_TOURNAMENT && pTeamData->GetTourPos() < ( pTournament->GetState() - TournamentNode::STATE_TOURNAMENT ) + 1 ) )
			{
				SetChildWndStyleAdd( ID_ENTER_PLAYER, IWS_EXACTIVE );
			}
			else
			{
				SetChildWndStyleRemove( ID_ENTER_PLAYER, IWS_EXACTIVE );
			}
		}
		else
		{
			SetChildWndStyleAdd( ID_ENTER_PLAYER, IWS_EXACTIVE );
		}

		TournamentTeamCharWnd *pCharWnd = dynamic_cast< TournamentTeamCharWnd * >( FindChildWnd( ID_CHARACTER_WND ) );
		if( pCharWnd )
		{
			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
			if( pUserTournament )
			{
				pCharWnd->SetTournamentTeamData( dwTeamIndex );
			}
		}
	}
}

void TournamentDetailMainWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwTournamentHelpID = xElement.GetFloatAttribute_e( "TournamentHelpID" );
}

void TournamentDetailMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/tournamentdetailInfownd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentdetailcheerwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentdetailschedulewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentdetailawardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentdetailallocatewnd.xml", this );
}

void TournamentDetailMainWnd::iwm_show()
{
	ResetChildActvieWnd();
}

void TournamentDetailMainWnd::iwm_hide()
{
	HideChildWnd( ID_TOURNAMENT_INFO_WND );
	HideChildWnd( ID_VICTORY_CHEER_WND );
	HideChildWnd( ID_ALLOCATE_INFO_WND );
	HideChildWnd( ID_TOURNAMENT_SCHEDULE_WND );
	HideChildWnd( ID_AWARD_INFO_WND );
}

void TournamentDetailMainWnd::ResetChildActvieWnd()
{
	CheckExInActvieWnd();
	m_dwCurChildActiveListIdx = 0;

	int iWndID = ID_TOURNAMENT_INFO_WND;
	for( int i = ID_TOURNAMENT_INFO_BTN; i <= ID_AWARD_INFO_BTN; ++i, ++iWndID  )
	{
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( i ) );
		if( pButton )
		{
			if( i == ID_TOURNAMENT_INFO_BTN )
			{
				ShowChildWnd( iWndID );
				pButton->SetClick( true );
			}
			else
			{
				HideChildWnd( iWndID );
				pButton->SetClick( false );
			}
		}
	}
}

void TournamentDetailMainWnd::CheckExInActvieWnd()
{
	m_vChildActiveList.clear();
	m_vChildInActiveList.clear();

	int iOffset = 33;
	int iWndID = ID_TOURNAMENT_INFO_WND;
	int i = 0;
	for( int iID = ID_TOURNAMENT_INFO_BTN; iID <= ID_AWARD_INFO_BTN; ++iID, ++iWndID  )
	{
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( iID ) );
		if( pButton )
		{
			if( pButton->HasWndStyle( IWS_EXACTIVE ) || !pButton->IsShow() )
			{
				ActiveWnd eWnd;
				eWnd.m_eBtn = (ChildType)iID;
				eWnd.m_eWnd = (ChildType)iWndID;
				m_vChildInActiveList.push_back( eWnd );

				if( pButton->IsShow() )				
					pButton->SetWndPos( 17, 59 + iOffset * i++ );				
			}
			else
			{
				ActiveWnd eWnd;
				eWnd.m_eBtn = (ChildType)iID;
				eWnd.m_eWnd = (ChildType)iWndID;
				m_vChildActiveList.push_back( eWnd );

				pButton->SetWndPos( 17, 59 + iOffset * i++ );
			}
		}
	}
}

void TournamentDetailMainWnd::NextActvieWnd()
{	
	DWORD dwNext = m_dwCurChildActiveListIdx + 1;
	if( m_vChildActiveList.size() <= dwNext  )
		return;
		
	for( UINT i = 0; i < m_vChildActiveList.size(); ++i  )
	{
		ActiveWnd eWnd = m_vChildActiveList[i];
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( eWnd.m_eBtn ) );
		TournamentDetailFrameWnd* pWnd = dynamic_cast<TournamentDetailFrameWnd*>( FindChildWnd( eWnd.m_eWnd ) );
		if( pButton )
		{
			if( i == dwNext )
			{
				m_dwCurChildActiveListIdx++;
				ShowChildWnd( eWnd.m_eWnd );
				pButton->SetClick( true );
				pWnd->iwm_nextmenu();
			}
			else
			{
				HideChildWnd( eWnd.m_eWnd );
				pButton->SetClick( false );
			}
		}	
	}
}

void TournamentDetailMainWnd::PrevActvieWnd()
{
	DWORD dwPrev = m_dwCurChildActiveListIdx - 1;
	if(  dwPrev < 0  )
		return;

	for( UINT i = 0; i < m_vChildActiveList.size(); ++i )
	{
		ActiveWnd eWnd = m_vChildActiveList[i];
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( eWnd.m_eBtn ) );
		TournamentDetailFrameWnd* pWnd = dynamic_cast<TournamentDetailFrameWnd*>( FindChildWnd( eWnd.m_eWnd ) );
		if( pButton && pWnd )
		{
			if( i == dwPrev )
			{
				m_dwCurChildActiveListIdx--;
				ShowChildWnd( eWnd.m_eWnd );
				pButton->SetClick( true );
				pWnd->iwm_prevmenu();
			}
			else
			{
				HideChildWnd( eWnd.m_eWnd );
				pButton->SetClick( false );
			}
		}	
	}
}

void TournamentDetailMainWnd::SetChildActvieWnd( DWORD dwBtnID )
{
	if( dwBtnID < ID_TOURNAMENT_INFO_BTN || ID_AWARD_INFO_BTN < dwBtnID )
		return;
	
	int iActiveIdx = 0;
	ChildTypeList::iterator iter;
	for( iter = m_vChildActiveList.begin(); iter != m_vChildActiveList.end(); ++iter, ++iActiveIdx )
	{
		ActiveWnd eWnd = (*iter);
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( eWnd.m_eBtn ) );
		if( pButton )
		{
			if( pButton->GetID() == dwBtnID )
			{
				m_dwCurChildActiveListIdx = iActiveIdx;
				pButton->SetClick( true );
				ShowChildWnd( eWnd.m_eWnd );
			}
			else
			{
				pButton->SetClick( false );
				HideChildWnd( eWnd.m_eWnd );
			}
		}
	}

	for( iter = m_vChildInActiveList.begin(); iter != m_vChildInActiveList.end(); ++iter )
	{
		ActiveWnd eWnd = (*iter);
		TournamentDetailMainTabBtn* pButton = dynamic_cast<TournamentDetailMainTabBtn*>( FindChildWnd( eWnd.m_eBtn ) );
		if( pButton )
		{
			HideChildWnd( eWnd.m_eWnd );
			pButton->SetClick( false );
		}
	}
}

void TournamentDetailMainWnd::EnterButtonExPopup()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		if( pTournament->GetState() == TournamentNode::STATE_TEAM_APP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "현재는 팀 모집 기간입니다." );
		}
		else if( pTournament->GetState() == TournamentNode::STATE_TEAM_DELAY )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "현재는 다음 경기의 대기 기간입니다." );
		}
		else if( pTournament->GetState() == TournamentNode::STATE_WAITING )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회가 종료되었습니다." );
		}
		else
		{
			ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
			if( pUserTournament )
			{
				DWORD dwTeamIndex = pUserTournament->GetTeamIndex( m_dwTournamentIndex );
				TeamGlobalData *pTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIndex );
				if( pTeamData )
				{
					int iCurrentRound = pTournament->GetState() - TournamentNode::STATE_TOURNAMENT;
					if( iCurrentRound == 0 && pTeamData->GetTourPos() == 0 )
					{
						// 예비엔트리
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "경기시간으로부터 5분 후 미참가팀이 발생한#경기장이 있다면, 대회로부터의#입장초대 메세지가 발송될 수 있습니다." );
					}
					else if( pTeamData->GetTourPos() < iCurrentRound + 1 )
					{
						// 다음 경기 없음
						if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "참가할 다음 경기가 없습니다.#대회일정의 팀이름을 클릭하여 승패예측 및 관전에 참여해보세요." );
						else
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "참가할 다음 경기가 없습니다." );
					}
					else
					{
						// 다음 경기 안내
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당시간에 대회로부터의 입장초대 메세지가 발송됩니다.#꼭 접속하고 있다가 참여하시기 바랍니다." );
					}
				}
				else
				{
					if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "모집기간이 지나 참가가 불가능합니다.#대회일정의 팀이름을 클릭하여 승패예측 및 관전에#참여해보세요." );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "모집기간이 지나 참가가 불가능합니다." );
				}
			}
		}
	}
}

void TournamentDetailMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_PREV_TOURNAMENT_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "지난 대회 정보는 준비중입니다." );
			}
		}
		break;
	case ID_TOURNAMENT_DEFINE:
		{
			if( cmd == IOBN_BTNUP )
			{
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( m_dwTournamentHelpID );
			}
		}
		break;
	case ID_BATTLE_ROOM_LIST:
		if( cmd == IOBN_BTNUP )
		{
			TournamentRoomListWnd *pTournamentRoomListWnd = dynamic_cast< TournamentRoomListWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_ROOM_LIST_WND ) );
			if( pTournamentRoomListWnd )
			{
				pTournamentRoomListWnd->SetTournament( m_dwTournamentIndex );
			}
		}
		break;
	case ID_WND_CLOSE:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	case ID_VICTORY_CHEER_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				SetChildActvieWnd( dwID );
			}
			else if( cmd == IOEX_BTNUP )
			{	
				if( dwID == ID_VICTORY_CHEER_BTN )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "우승 응원은 차후 업데이트 될 예정입니다." );
			}
		}
		break;
	case ID_TOURNAMENT_INFO_BTN:
	case ID_ALLOCATE_INFO_BTN:
	case ID_TOURNAMENT_SCHEDULE_BTN:
	case ID_AWARD_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetChildActvieWnd( dwID );
		}
		break;
	case ID_ENTER_PLAYER:
		if( cmd == IOBN_BTNUP )
		{
			TournamentTeamCharWnd *pTeamCharWnd = dynamic_cast< TournamentTeamCharWnd * >( FindChildWnd( ID_CHARACTER_WND ) );
			if( pTeamCharWnd )
			{
				DWORD dwStateEndDate = pTeamCharWnd->GetNextRoundDate();
				char szEndDate[MAX_PATH] = "";			
				if( dwStateEndDate == 0 )
					sprintf_s_e( szEndDate, "%s", "주최자 수동진행" );
				else
					sprintf_s_e( szEndDate, "%d/%d(%s) %02d:%02d", DateHelp::GetDateMonth( dwStateEndDate ), DateHelp::GetDateDay( dwStateEndDate ), DateHelp::GetWeekDayString( dwStateEndDate ),
																 DateHelp::GetDateHour( dwStateEndDate ), DateHelp::GetDateMinute( dwStateEndDate ) );

				char szCurrentRound[MAX_PATH] = "";
				int iCurrentTeam = Help::TournamentCurrentRoundWithTeam( pTeamCharWnd->GetMaxStartTeam(), pTeamCharWnd->GetTourPos() - 1 );
				if( iCurrentTeam <= 2 )
					sprintf_s_e( szCurrentRound, "결승전" );
				else
					sprintf_s_e( szCurrentRound, "%d강", iCurrentTeam );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s# %s 초대가 발송될 예정입니다.", szEndDate, szCurrentRound );
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			EnterButtonExPopup();
		}
		break;
	case ID_INVITE_PLAYER:
		{
			if( cmd == IOBN_BTNUP )				
			{
				TournamentInvitationWnd* pWnd = dynamic_cast<TournamentInvitationWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_INVITATION_WND ) );
				if( pWnd )
					pWnd->SetTournamentInvitation( m_dwTournamentIndex, m_dwTournamentOwnerIndex );
			}
		}
		break;
	}
}

void TournamentDetailMainWnd::ApplyDecision()
{
	TournamentDetailFrameCheerWnd* pWnd = dynamic_cast<TournamentDetailFrameCheerWnd*>( FindChildWnd( ID_VICTORY_CHEER_WND ) );
	if( pWnd )
		pWnd->ApplyDecision();
}

void TournamentDetailMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament )
	{
		if( pTournament->IsUpdateDetailUI() )
			UpdateTournamentData( pTournament );
	}
}
void TournamentDetailMainWnd::OnRender()
{
	if( !IsShow() ) return;

	ioMovingWnd::OnRender();
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailFrameWnd::TournamentDetailFrameWnd()
{
	m_pScheduleProgressActiveLeft	= NULL;
	m_pScheduleProgressActiveCenter	= NULL;
	m_pScheduleProgressActiveRight	= NULL;	

	m_pActiveAcceptCenterClose		= NULL;
	m_pActiveAcceptRightClose		= NULL;
	m_pActiveCheerCenterClose		= NULL;
	m_pActiveCheerRightClose		= NULL;
	m_pActiveTournamentCenterClose	= NULL;
	m_pActiveTournamentRightClose	= NULL;
	m_pActiveAwardCenterClose		= NULL;
	m_pActiveAwardRightClose		= NULL;
	
	m_pScheduleProgressInactiveLeft		= NULL;
	m_pScheduleProgressInactiveCenter	= NULL;
	m_pScheduleProgressInactiveRight	= NULL;
	m_pScheduleProgressInactiveClose	= NULL;

	m_pDevideDot = NULL;

	m_TournamentState = 0;
	m_TournamentType  = 0;
}

TournamentDetailFrameWnd::~TournamentDetailFrameWnd()
{
	SAFEDELETE( m_pScheduleProgressActiveLeft );
	SAFEDELETE( m_pScheduleProgressActiveCenter );
	SAFEDELETE( m_pScheduleProgressActiveRight );

	SAFEDELETE( m_pActiveAcceptCenterClose );
	SAFEDELETE( m_pActiveAcceptRightClose );
	SAFEDELETE( m_pActiveCheerCenterClose );
	SAFEDELETE( m_pActiveCheerRightClose );
	SAFEDELETE( m_pActiveTournamentCenterClose );
	SAFEDELETE( m_pActiveTournamentRightClose );
	SAFEDELETE( m_pActiveAwardCenterClose );
	SAFEDELETE( m_pActiveAwardRightClose );
	
	SAFEDELETE( m_pScheduleProgressInactiveLeft );
	SAFEDELETE( m_pScheduleProgressInactiveCenter );
	SAFEDELETE( m_pScheduleProgressInactiveRight );
	SAFEDELETE( m_pScheduleProgressInactiveClose );	

	SAFEDELETE( m_pDevideDot );
}

void TournamentDetailFrameWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ScheduleProgressBarActiveLeft" )
	{
		SAFEDELETE( m_pScheduleProgressActiveLeft );
		m_pScheduleProgressActiveLeft = pImage;
	}
	else if( szType == "ScheduleProgressBarActiveCenter" )
	{
		SAFEDELETE( m_pScheduleProgressActiveCenter );
		m_pScheduleProgressActiveCenter = pImage;
	}
	else if( szType == "ScheduleProgressBarActiveRight" )
	{
		SAFEDELETE( m_pScheduleProgressActiveRight );
		m_pScheduleProgressActiveRight = pImage;
	}
	else if( szType == "ActiveAcceptCenterClose" )
	{		
		SAFEDELETE( m_pActiveAcceptCenterClose );
		m_pActiveAcceptCenterClose = pImage;
	}
	else if( szType == "ActiveAcceptRightClose" )
	{		
		SAFEDELETE( m_pActiveAcceptRightClose );
		m_pActiveAcceptRightClose = pImage;
	}

	else if( szType == "ActiveCheerCenterClose" )
	{		
		SAFEDELETE( m_pActiveCheerCenterClose );
		m_pActiveCheerCenterClose = pImage;
	}
	else if( szType == "ActiveCheerRightClose" )
	{		
		SAFEDELETE( m_pActiveCheerRightClose );
		m_pActiveCheerRightClose = pImage;
	}
	else if( szType == "ActiveTournamentCenterClose" )
	{		
		SAFEDELETE( m_pActiveTournamentCenterClose );
		m_pActiveTournamentCenterClose = pImage;
	}	
	else if( szType == "ActiveTournamentRightClose" )
	{		
		SAFEDELETE( m_pActiveTournamentRightClose );
		m_pActiveTournamentRightClose = pImage;
	}
	else if( szType == "ActiveAwardCenterClose" )
	{		
		SAFEDELETE( m_pActiveAwardCenterClose );
		m_pActiveAwardCenterClose = pImage;
	}
	else if( szType == "ActiveAwardRightClose" )
	{		
		SAFEDELETE( m_pActiveAwardRightClose );
		m_pActiveAwardRightClose = pImage;
	}
	else if( szType == "ScheduleProgressBarInactiveLeft" )
	{
		SAFEDELETE( m_pScheduleProgressInactiveLeft );
		m_pScheduleProgressInactiveLeft = pImage;
	}
	else if( szType == "ScheduleProgressBarInactiveCenter" )
	{
		SAFEDELETE( m_pScheduleProgressInactiveCenter );
		m_pScheduleProgressInactiveCenter = pImage;
	}
	else if( szType == "ScheduleProgressBarInactiveRight" )
	{
		SAFEDELETE( m_pScheduleProgressInactiveRight );
		m_pScheduleProgressInactiveRight = pImage;
	}
	else if( szType == "ScheduleProgressBarInactiveClose" )
	{
		SAFEDELETE( m_pScheduleProgressInactiveClose );
		m_pScheduleProgressInactiveClose = pImage;
	}
	else if( szType == "WndDevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailFrameWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentDetailFrameWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;
}

void TournamentDetailFrameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	TournamentDetailMainWnd* pParent = dynamic_cast< TournamentDetailMainWnd * >( GetParent() );
	if( pParent )
	{
		if( cmd == IOBN_BTNUP )
		{
			switch( dwID )
			{
			case ID_PREV_BTN:
				pParent->PrevActvieWnd();
				break;
			case ID_NEXT_BTN:
				pParent->NextActvieWnd();
				break;
			}
		}
	}
}

void TournamentDetailFrameWnd::iwm_prevmenu()
{
	//메뉴 이동이 발생할떄 호출되는 함수
}
void TournamentDetailFrameWnd::iwm_nextmenu()
{
	//메뉴 이동이 발생할떄 호출되는 함수
}

void TournamentDetailFrameWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_TournamentState = pTournament->GetState();
		m_TournamentType  = pTournament->GetType();

		if( pTournament->IsUpdateSubUI() )
		{
			UpdateMainUI();
		}
	}

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( pUserTournament->IsUpdateMainUI() )
		{
			UpdateMainUI();
		}
	}
}

void TournamentDetailFrameWnd::UpdateMainUI()
{

}

void TournamentDetailFrameWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	
	ioWnd::OnProcess( fTimePerSec );

	OnUpdateProcess();
}

void TournamentDetailFrameWnd::OnInactiveNotCloseRender( int iXPos, int iYPos )
{
	if(	!m_pScheduleProgressInactiveLeft	||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveRight )
		return;
	
	m_pScheduleProgressInactiveLeft->Render( iXPos, iYPos );
	m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
	m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
}

void TournamentDetailFrameWnd::OnDevideWndDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;
	
	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
		{
			
			m_pDevideDot->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDevideDot->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDevideDot->SetSize( 2, 3 );
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );	
		}
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailFrameInfoWnd::TournamentDetailFrameInfoWnd()
{
}

TournamentDetailFrameInfoWnd::~TournamentDetailFrameInfoWnd()
{
}


void TournamentDetailFrameInfoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/detailinnerdefinewnd.xml" , this );
	g_GUIMgr.AddWnd( "XML/detailinnerschedulewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/detailinnerawardwnd.xml", this );
}

void TournamentDetailFrameInfoWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;
	
	TournamentDetailInnerPageNoticeWnd *pInfoWnd = dynamic_cast< TournamentDetailInnerPageNoticeWnd * >( FindChildWnd( ID_INNER_TOURNAMENT_INFO ) );
	if( pInfoWnd )
		pInfoWnd->SetTournamentData( m_dwTournamentIndex );

	TournamentDetailInnerPageScheduleWnd *pScheduleWnd = dynamic_cast< TournamentDetailInnerPageScheduleWnd * >( FindChildWnd( ID_INNER_TOURNAMENT_SCHEDULE ) );
	if( pScheduleWnd )
		pScheduleWnd->SetTournamentData( m_dwTournamentIndex );

	TournamentDetailInnerPageRewardWnd *RewardWnd = dynamic_cast< TournamentDetailInnerPageRewardWnd * >( FindChildWnd( ID_INNER_TOURNAMENT_REWARD ) );
	if( RewardWnd )
		RewardWnd->SetTournamentData( m_dwTournamentIndex );			
}

void TournamentDetailFrameInfoWnd::SetAllocated( DWORD dwTourIdx )
{
	TournamentDetailInnerPageScheduleWnd *pWnd = dynamic_cast< TournamentDetailInnerPageScheduleWnd * >( FindChildWnd( ID_INNER_TOURNAMENT_SCHEDULE ) ); 
	if( pWnd )
		pWnd->SetAllocated( dwTourIdx );
}

void TournamentDetailFrameInfoWnd::UpdateMainUI()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameInfoWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
		
	OnActiveTypeRender( iXPos, iYPos );
	OnActiveRender( iXPos, iYPos );
	OnInfoRender( iXPos, iYPos );
	OnDevideWndDotRender( iXPos, iYPos );
}

void TournamentDetailFrameInfoWnd::OnActiveTypeRender( int iXPos, int iYPos )
{
	if(	!m_pActiveCheerCenterClose			||
		!m_pActiveCheerRightClose			||
		!m_pActiveTournamentCenterClose		||
		!m_pActiveTournamentRightClose		||
		!m_pActiveAwardCenterClose			||
		!m_pActiveAwardRightClose			||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveClose	||
		!m_pScheduleProgressInactiveRight )
		return;

	//대회응원기간
	if( m_TournamentState == TournamentNode::STATE_TEAM_DELAY )
	{
		if( m_TournamentType == TournamentNode::TYPE_REGULAR )
		{
			m_pActiveCheerCenterClose->Render( iXPos, iYPos );
			m_pActiveCheerRightClose->Render( iXPos, iYPos );
		}
		else
		{
			m_pActiveAcceptCenterClose->Render( iXPos, iYPos );
			m_pActiveAcceptRightClose->Render( iXPos, iYPos );
		}
	}
	//대회기간
	else if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT )
	{
		m_pActiveTournamentCenterClose->Render( iXPos, iYPos );
		m_pActiveTournamentRightClose->Render( iXPos, iYPos );
	}
	//대회포상대기기간 또는 유휴기간
	else if( m_TournamentState == TournamentNode::STATE_WAITING )
	{
		m_pActiveAwardCenterClose->Render( iXPos, iYPos );
		m_pActiveAwardRightClose->Render( iXPos, iYPos );
	}
	else
	{
		m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
	}
}

void TournamentDetailFrameInfoWnd::OnActiveRender( int iXPos, int iYPos )
{
	if( !m_pScheduleProgressActiveLeft	 ||
		!m_pScheduleProgressActiveCenter ||
		!m_pScheduleProgressActiveRight )
		return;

	if( TournamentNode::STATE_TEAM_APP == m_TournamentState )
	{
		m_pScheduleProgressActiveLeft->Render( iXPos, iYPos );
		m_pScheduleProgressActiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressActiveRight->Render( iXPos, iYPos );
	}
	else
	{
		OnInactiveNotCloseRender( iXPos, iYPos );
	}
}

void TournamentDetailFrameInfoWnd::OnInfoRender( int iXPos, int iYPos )
{
	// 상단 모집 기간
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) 
		return;

	char szPeriod[MAX_PATH] = "";
	DWORD dwAppStartDate = pTournament->GetStateStartDate( TournamentNode::STATE_TEAM_APP );
	DWORD dwAppEndDate   = pTournament->GetStateEndDate( TournamentNode::STATE_TEAM_APP );

	char szStartBuff[MAX_PATH] = "";
	char szEndBuff[MAX_PATH] = "";
	sprintf_s_e( szStartBuff, "%s", DateHelp::GetWeekDayString( dwAppStartDate ) );
	sprintf_s_e( szEndBuff, "%s", DateHelp::GetWeekDayString( dwAppEndDate ) );
	if( dwAppStartDate == 0 && dwAppEndDate == 0 )
	{
		sprintf_s_e( szPeriod, "주최자 수동진행");
	}
	else if( dwAppStartDate != 0 && dwAppEndDate == 0 )
	{
		sprintf_s_e( szPeriod, "%d/%d(%s) ~ 주최자 수동진행", 
			DateHelp::GetDateMonth( dwAppStartDate ),
			DateHelp::GetDateDay( dwAppStartDate ),
			szStartBuff );
	}
	else
	{
		sprintf_s_e( szPeriod, "%d/%d(%s) ~ %d/%d(%s)",
			DateHelp::GetDateMonth( dwAppStartDate ),
			DateHelp::GetDateDay( dwAppStartDate ),
			szStartBuff,
			DateHelp::GetDateMonth( dwAppEndDate ),
			DateHelp::GetDateDay( dwAppEndDate ),
			szEndBuff );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	g_FontMgr.PrintText( iXPos + 308, iYPos + 23, FONT_SIZE_11, "%s", szPeriod );
	g_FontMgr.PrintText( iXPos + 308, iYPos + 35, FONT_SIZE_11, "모집기간" );
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerPageNoticeWnd::TournamentDetailInnerPageNoticeWnd()
{
	m_pDevideDot   = NULL;
	m_pHelpIcon    = NULL;
	m_pNoticeIcon  = NULL;
	m_pIconBack	   = NULL;

	m_dwOwnerIndex				= 0;
	m_iCurPage					= 0;
	m_iMaxPage					= 0;
	m_MaxPlayer					= 0;
	m_TournamentMaxRound		= 0;
	m_TournamentStartTeamCount	= 0;
	m_dwTournamentIndex			= 0;
	m_dwOwnerIndex				= 0;
}

TournamentDetailInnerPageNoticeWnd::~TournamentDetailInnerPageNoticeWnd()
{
	SAFEDELETE( m_pDevideDot );
	SAFEDELETE( m_pHelpIcon );
	SAFEDELETE( m_pNoticeIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentDetailInnerPageNoticeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "HelpIcon" )
	{
		SAFEDELETE( m_pHelpIcon );
		m_pHelpIcon = pImage;
	}
	else if( szType == "NoticeIcon" )
	{
		SAFEDELETE( m_pNoticeIcon );
		m_pNoticeIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerPageNoticeWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentDetailInnerPageNoticeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{

	case ID_PREV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_iCurPage = max( 0, m_iCurPage - 1 );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_iCurPage = min( m_iMaxPage, m_iCurPage + 1 );
		}
		break;
	case ID_NOTICE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
			{
				CustomTournamentNoticeWnd* pWnd = dynamic_cast<CustomTournamentNoticeWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_NOTICE_WND ) );
				if( pWnd )
				{
					pWnd->SetNoticeData( m_dwTournamentIndex, m_vAnnounce, m_dwOwnerIndex );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "공지사항은 대회 주최자만 수정 할 수 있습니다." );
			}
		}
		break;
	}
}

void TournamentDetailInnerPageNoticeWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;

	m_TournamentType		   = pTournament->GetType();
	m_szTournamentTitle		   = pTournament->GetTournamentTitle();
	m_MaxPlayer				   = pTournament->GetMaxPlayer();
	m_TournamentMaxRound       = pTournament->GetTournamentMaxRound();
	m_TournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
	m_szPlayModeName		   = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pTournament->GetPlayMode() ).c_str();
	m_dwOwnerIndex			   = pTournament->GetOwnerIndex();
	m_vAnnounce.clear();
	CustomTournamentNoticeWnd::ConvertStringVector( pTournament->GetCustomAnnounce(), m_vAnnounce );
	
	HideChildWnd( ID_NOTICE_BTN );

	//위치 초기화
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
	if( pButton )
	{
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );
	}
	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
	if( pButton )
	{
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );
	}

	if( m_TournamentType == TournamentNode::TYPE_CUSTOM )
	{		
		if( pTournament->GetOwnerIndex() == g_MyInfo.GetUserIndex() )
		{
			//Show 처리
			ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
			if( pButton )
			{
				pButton->SetWndPos( pButton->GetXPos() - 15, pButton->GetYPos() - 33 );
			}
			pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
			if( pButton )
			{
				pButton->SetWndPos( pButton->GetXPos() + 15, pButton->GetYPos() - 33 );
			}

			ShowChildWnd( ID_NOTICE_BTN );			
		}

		m_iMaxPage = ((int)m_vAnnounce.size()-1) / MAX_PAGE_LINE;
	}

	m_iCurPage = 0;
}

void TournamentDetailInnerPageNoticeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentDetailInnerPageNoticeWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );
	OnPageNumberRender( iXPos, iYPos );
	OnIconRender(  iXPos, iYPos );

	if(m_TournamentType == TournamentNode::TYPE_CUSTOM )
		OnNoticeWndRender( iXPos, iYPos );
	else	
		OnHelpWndRender( iXPos, iYPos );
	
}

void TournamentDetailInnerPageNoticeWnd::OnIconRender( int iXPos, int iYPos )
{
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		if( !m_pHelpIcon || !m_pNoticeIcon )
			return;

		iYPos -= 28;
		m_pIconBack->Render( iXPos, iYPos );
		m_pNoticeIcon->Render( iXPos, iYPos );
	}
	else
	{
		if( !m_pHelpIcon || !m_pIconBack )
			return;

		m_pIconBack->Render( iXPos, iYPos );
		m_pHelpIcon->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerPageNoticeWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 28;
	}

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerPageNoticeWnd::OnPageNumberRender( int iXPos, int iYPos )
{
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 33;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 366, TAT_CENTER );
}

void TournamentDetailInnerPageNoticeWnd::OnHelpWndRender( int iXPos, int iYPos )
{
	// 타이틀
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "대회란" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "무엇입니까?" );

	// 
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "양 진영 중 최고의 팀을 뽑는" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "진영전 최후의 전투입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	//
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "각 진영의 대회팀들이" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 221, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_12, "토너먼트 형식으로 대진" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "을 펼치며" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 239, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "마지막엔 양 진영 최고의 두 팀이" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 257, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "우승을 겨루는 시합입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 275, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "경기에 직접 참여하지 않아도" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 293, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_12, "우승응원 및 승리예측" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "하기로" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 311, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "쉽게 참여할 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 329, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerPageNoticeWnd::OnNoticeWndRender( int iXPos, int iYPos )
{
	// 타이틀
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "공지 사항" );
		iYPos -= 28;
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "대회" );	
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "공지 사항" );
	}

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "주최자가 언제나 공지사항 작성 및" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "수정이 가능합니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	//
	if( m_vAnnounce.empty() )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "우리 대회에" );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 221, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "오신것을 환영합니다." );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 239, TAT_CENTER );
		kPrinter.ClearList();
	}
	else
	{
		int iOffset = 0;
		int iStartArray = m_iCurPage * MAX_PAGE_LINE;
		for( int iCount = 0; iStartArray < (int)m_vAnnounce.size(); ++iStartArray, ++iCount )
		{
			if( iCount >= MAX_PAGE_LINE )
				break;

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, "%s", m_vAnnounce[iStartArray].c_str() );
			kPrinter.PrintFullText( iXPos + 93, iYPos + 221 + iOffset, TAT_CENTER );
			kPrinter.ClearList();
			iOffset += 18;

		}
	}
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerPageScheduleWnd::TournamentDetailInnerPageScheduleWnd()
{
	m_pDevideDot = NULL;
	m_pIcon		 = NULL;
	m_pIconBack  = NULL;

	m_iCurPage					= 0;
	m_iMaxPage					= 0;
	m_MaxPlayer					= 0;
	m_TournamentMaxRound		= 0;
	m_TournamentStartTeamCount	= 0;
	m_dwTournamentIndex			= 0;
	m_dwOwnerIndex				= 0;
	m_TournamentState			= 0;
	m_iScheduleGapTime          = 0;
}

TournamentDetailInnerPageScheduleWnd::~TournamentDetailInnerPageScheduleWnd()
{
	SAFEDELETE( m_pDevideDot );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentDetailInnerPageScheduleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerPageScheduleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentDetailInnerPageScheduleWnd::iwm_create()
{
	CreateCustomData();
}

void TournamentDetailInnerPageScheduleWnd::CreateCustomData()
{
	ioINILoader_e kLoader( "config/sp2_custom_tournament.ini" );
	kLoader.SetTitle_e( "scheudle" );
	m_iScheduleGapTime = kLoader.LoadInt_e( "schedule_gap_time", 30 );
}

void TournamentDetailInnerPageScheduleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{

	case ID_PREV_BTN:
		if( cmd == IOBN_BTNUP )		
		{
			m_iCurPage = max( 0, m_iCurPage - 1 );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )		
		{			
			m_iCurPage = min( m_iMaxPage, m_iCurPage + 1 );
		}
		break;
	case ID_OWNER_COMMAND_BTN:
		{
			IwmOwnerCommand( cmd );
		}
		break;
	case MESSAGE_BOX:
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendTournamentStart();
			}
		}
		break;
	}
}

void TournamentDetailInnerPageScheduleWnd::IwmOwnerCommand( DWORD dwCmd )
{
	switch( dwCmd )
	{
	case IOBN_BTNUP:
		{
			IwmOwnerNormalCommand();
		}
	break;
	case IOEX_BTNUP:
		{
			IwmOwnerExCommand();
		}
	break;
	}
}

void TournamentDetailInnerPageScheduleWnd::IwmOwnerNormalCommand()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;
	if( pTournament->GetOwnerIndex() != g_MyInfo.GetUserIndex() ) return;
	if( m_TournamentType != TournamentNode::TYPE_CUSTOM ) return;

	if( pTournament->GetCustomProgressType() == TournamentNode::CPT_OFFLINE )
	{
		if( pTournament->GetState() == TournamentNode::STATE_TEAM_APP )
		{
			SetTournamentStart();
		}
		else if( pTournament->GetState() == TournamentNode::STATE_TEAM_DELAY )
		{
			if( pTournament->IsEntryAllocated() )
			{
				SetTournamentStart();
			}
			else
			{
				CustomTournamentAllocateWnd* pWnd = dynamic_cast<CustomTournamentAllocateWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_ALLOCATE_WND ) );
				if( pWnd )
					pWnd->SetCustomTournamentData( m_dwTournamentIndex );
			}
		}
		else if( pTournament->GetState() >= TournamentNode::STATE_TOURNAMENT )
		{
			SetTournamentStart();
		}
	}
	else
	{
		if( pTournament->GetState() == TournamentNode::STATE_TEAM_DELAY )
		{
			CustomTournamentAllocateWnd* pWnd = dynamic_cast<CustomTournamentAllocateWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_ALLOCATE_WND ) );
			if( pWnd )
				pWnd->SetCustomTournamentData( m_dwTournamentIndex );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대진 배정기간이 아닙니다." );
		}
	}
}

void TournamentDetailInnerPageScheduleWnd::IwmOwnerExCommand()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;

	if( pTournament->GetCustomProgressType() == TournamentNode::CPT_OFFLINE )
	{
		if( pTournament->GetState() >= TournamentNode::STATE_TOURNAMENT )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "경기를 시작 할 수 없습니다 # 경기간 최소 시작 간격은 %d분입니다.", m_iScheduleGapTime );
		}
		else if( pTournament->GetState() == TournamentNode::STATE_WAITING )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "모든 경기가 종료 되어 # 더 이상 대회를 진행 할 수 없습니다." );
		}
	}
	else
	{
		if( pTournament->GetState() == TournamentNode::STATE_TEAM_DELAY && pTournament->IsEntryAllocated() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대진 배정이 완료 되었습니다." );
		}		
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "대진 배정기간이 아닙니다." );
		}
	}
}

void TournamentDetailInnerPageScheduleWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;
	m_iCurPage = 0;
	UpdateScheduleInfo();
}

void TournamentDetailInnerPageScheduleWnd::SetTournamentStart()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;

	if( m_TournamentState == TournamentNode::STATE_TEAM_APP )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, "모집을 마감하시겠습니까?" );
		g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
	}
	else if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT )
	{
		DWORD dwPos = m_TournamentState - TournamentNode::STATE_TOURNAMENT;
		DWORD dwNextRound = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, dwPos );
		int iSize = pTournament->GetCustomRoundTeamCount( dwPos + 1 );

		if( 2 < dwNextRound )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "%d강 시작하시겠습니까?", dwNextRound );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "%d강 시작 가능한팀 : %d팀", dwNextRound, iSize );
			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
		else if( 2 == dwNextRound )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "결승 시작하시겠습니까?", dwNextRound );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "결승 시작 가능한팀 : %d팀", iSize );
			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
	}
}

void TournamentDetailInnerPageScheduleWnd::SetAllocated( DWORD dwTourIdx )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTourIdx );
	if( pTournament == NULL ) return;
	if( pTournament->GetOwnerIndex() != g_MyInfo.GetUserIndex() ) return;
	if( pTournament->GetType() != TournamentNode::TYPE_CUSTOM ) return;
	if( pTournament->GetCustomProgressType() != TournamentNode::CPT_OFFLINE ) return;

	//배정 성공 후 첫 라운드의 데이터를 미리 동기화
	pTournament->SetCustomRoundSync( pTournament->GetState() + 1 );

	SendTournamentStart();
}

void TournamentDetailInnerPageScheduleWnd::SendTournamentStart()
{
	//Send
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_TOURNAMENT_CUSTOM_STATE_START );
	kPacket << m_dwTournamentIndex;
	TCPNetwork::SendToServer( kPacket );
}

void TournamentDetailInnerPageScheduleWnd::ChangeOwnerCommandButton( ioHashString szTitle, bool bActive, DWORD dwColor )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_OWNER_COMMAND_BTN ) );
	if( !pButton ) return;

	if( bActive )
	{
		SetChildWndStyleRemove( ID_OWNER_COMMAND_BTN, IWS_EXACTIVE );		
		ioUITitle* pTtitle = pButton->GetTitle();
		pTtitle->SetText( szTitle.c_str(), "" );
		pTtitle->SetColor( "Normal", dwColor, 0xFF215AA2 );
		pTtitle->SetColor( "Over",   dwColor, 0xFF215AA2 );
		pTtitle->SetColor( "Push",   dwColor, 0xFF215AA2 );
	}
	else
	{
		SetChildWndStyleAdd( ID_OWNER_COMMAND_BTN, IWS_EXACTIVE );
		ioUITitle* pTtitle = pButton->GetTitle();
		pTtitle->SetText( szTitle.c_str(), "" );
	}
}

void TournamentDetailInnerPageScheduleWnd::UpdateOwnerCommand( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	SetChildWndStyleRemove( ID_OWNER_COMMAND_BTN, IWS_EXACTIVE );
	switch( pTournament->GetCustomProgressType() )
	{
	case TournamentNode::CPT_OFFLINE:
		UpdateOwnerCommandOffline( pTournament );
		break;
	default:
		UpdateOwnerCommandOnline( pTournament );
		break;
	}
	ShowChildWnd( ID_OWNER_COMMAND_BTN );
}

void TournamentDetailInnerPageScheduleWnd::UpdateOwnerCommandOnline( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	if( pTournament->GetState() == TournamentNode::STATE_TEAM_DELAY )
	{
		if( !pTournament->IsEntryAllocated() )
			ChangeOwnerCommandButton( "대진 배정하기", true, TCT_DEFAULT_DARKGRAY );
		else
			ChangeOwnerCommandButton( "대진 배정하기", false );
	}
	else
		ChangeOwnerCommandButton( "대진 배정하기", false );
}

void TournamentDetailInnerPageScheduleWnd::UpdateOwnerCommandOffline( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	switch( pTournament->GetState() )
	{
	case TournamentNode::STATE_TEAM_APP:
		{
			DWORD dwDate = pTournament->GetStateEndDate( pTournament->GetState() );
			if( dwDate == 0 )
				ChangeOwnerCommandButton( "모집 마감하기", true, TCT_DEFAULT_RED );
			else
				ChangeOwnerCommandButton( "모집 마감 처리중", false );
		}
		break;
	case TournamentNode::STATE_TEAM_DELAY:
		{
			if( !pTournament->IsEntryAllocated() )
				ChangeOwnerCommandButton( "대진 배정 하기", true, TCT_DEFAULT_RED );
			else
				ChangeOwnerCommandButton( "대진 배정 처리중", false );
		}
		break;
	case TournamentNode::STATE_WAITING:
		{
			ChangeOwnerCommandButton( "대회 종료", false );
		}
		break;
	default:
		{
			UpdateOwnerCommandStateTournament( pTournament );
		}
		break;
	}
}

void TournamentDetailInnerPageScheduleWnd::UpdateOwnerCommandStateTournament( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	DWORD dwPos  = pTournament->GetState() - TournamentNode::STATE_TOURNAMENT;

	ioCalendarSet PrevDate( pTournament->GetStateEndDate( pTournament->GetState() - 1 ), DT_LAST_MIN );
	ioCalendarSet CurrDate( DT_LAST_MIN );
	CurrDate.SetTodayServerDate();

	int iGapTime = CurrDate - PrevDate;

	DWORD dwNextRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), dwPos );
	char szBuff[MAX_PATH];

	if( pTournament->GetState() == TournamentNode::STATE_TOURNAMENT )
	{
		sprintf( szBuff, "%d강 시작하기", dwNextRound );
		ChangeOwnerCommandButton( szBuff, true, TCT_DEFAULT_RED );
	}
	else if( 2 < dwNextRound )
	{
		if( !PrevDate.isEmpty() && iGapTime < m_iScheduleGapTime )
		{
			sprintf( szBuff, "%d강 시작 준비중", dwNextRound );
			ChangeOwnerCommandButton( szBuff, false );
		}
		else
		{
			sprintf( szBuff, "%d강 시작하기", dwNextRound );
			ChangeOwnerCommandButton( szBuff, true, TCT_DEFAULT_RED );
		}
	}
	else if( 2 == dwNextRound )
	{
		if( !PrevDate.isEmpty() && iGapTime < m_iScheduleGapTime )
			ChangeOwnerCommandButton( "결승 시작 준비중", false );
		else
			ChangeOwnerCommandButton( "결승 시작", true, TCT_DEFAULT_RED );
	}
	else
	{
		ChangeOwnerCommandButton( "경기 시작 준비중", false );
	}
}

void TournamentDetailInnerPageScheduleWnd::SetPageButtonDefaultPos()
{
	//위치 초기화
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
	if( pButton )
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );

	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
	if( pButton )
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );
}

void TournamentDetailInnerPageScheduleWnd::SetPageButtonPos()
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
	if( pButton )
		pButton->SetWndPos( pButton->GetXPos() - 15, pButton->GetYPos() - 33 );

	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
	if( pButton )
		pButton->SetWndPos( pButton->GetXPos() + 15, pButton->GetYPos() - 33 );
}

void TournamentDetailInnerPageScheduleWnd::UpdateScheduleInfo()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;

	m_TournamentType		   = pTournament->GetType();
	m_MaxPlayer				   = pTournament->GetMaxPlayer();
	m_TournamentMaxRound       = pTournament->GetTournamentMaxRound();
	m_TournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
	m_szPlayModeName		   = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pTournament->GetPlayMode() ).c_str();
	m_dwOwnerIndex			   = pTournament->GetOwnerIndex();

	SetPageButtonDefaultPos();
	HideChildWnd( ID_OWNER_COMMAND_BTN );
	if( m_TournamentType == TournamentNode::TYPE_CUSTOM )
	{
		UpdateCustomScheduleInfo( pTournament );
		if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
		{
			SetPageButtonPos();
			UpdateOwnerCommand( pTournament );
		}
	}
	else
	{
		UpdateRegularScheduleInfo( pTournament );
	}
	
	m_iMaxPage = ((int)m_TournamentRoundDateList.size()-1) / MAX_PAGE_LINE;
}

void TournamentDetailInnerPageScheduleWnd::UpdateRegularScheduleInfo( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	m_TournamentRoundDateList.clear();
	for(int i = 0; i < (int)m_TournamentMaxRound; i++ )
	{
		m_TournamentRoundDateList.push_back( pTournament->GetStateEndDate( TournamentNode::STATE_TOURNAMENT + i ) );
	}
}

void TournamentDetailInnerPageScheduleWnd::UpdateCustomScheduleInfo( TournamentNode* pTournament )
{
	if( !pTournament ) return;

	m_TournamentRoundDateList.clear();
	m_TournamentRoundDateList.push_back( pTournament->GetStateEndDate( TournamentNode::STATE_TEAM_APP ) );   //모집마감
	m_TournamentRoundDateList.push_back( pTournament->GetStateEndDate( TournamentNode::STATE_TEAM_DELAY ) ); //대진배정

	//경기일정
	for(int i = 0; i < (int)m_TournamentMaxRound; i++ )
	{
		m_TournamentRoundDateList.push_back( pTournament->GetStateEndDate( TournamentNode::STATE_TOURNAMENT + i ) );
	}

	//대회종료
	if( pTournament->GetEndDate() == 0 )
	{
		ioCalendarSet kEndDate;
		kEndDate.SetTournamentDate( pTournament->GetStartDate() );
		kEndDate = kEndDate + TournamentHelp::TOURNAMENT_WEEK * TournamentHelp::TOURNAMENT_DAYTIME * TournamentHelp::TOURNAMENT_MIN;
		m_TournamentRoundDateList.push_back( kEndDate.GetDate() );
	}
	else
	{
		m_TournamentRoundDateList.push_back( pTournament->GetEndDate() );
	}
}

void TournamentDetailInnerPageScheduleWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_TournamentState = pTournament->GetState();
		UpdateScheduleInfo();
	}	
}

void TournamentDetailInnerPageScheduleWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	OnUpdateProcess();
}

void TournamentDetailInnerPageScheduleWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );
	OnIconRender( iXPos, iYPos );
	OnPageNumberRender( iXPos, iYPos );
	OnScheduleMentRender( iXPos, iYPos );
	switch( m_TournamentType )
	{
	case TournamentNode::TYPE_REGULAR:
		OnScheduleRegularRender( iXPos, iYPos );
		break;
	default:
		OnScheduleCustomRender( iXPos, iYPos );
		break;
	}
}

void TournamentDetailInnerPageScheduleWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 28;
	}
	
	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerPageScheduleWnd::OnIconRender( int iXPos, int iYPos )
{
	if( !m_pIcon || !m_pIconBack )
		return;

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 28;
	}

	m_pIconBack->Render( iXPos, iYPos );
	m_pIcon->Render( iXPos, iYPos );
}

void TournamentDetailInnerPageScheduleWnd::OnPageNumberRender( int iXPos, int iYPos )
{
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 33;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 366, TAT_CENTER );
}

void TournamentDetailInnerPageScheduleWnd::OnScheduleMentRender( int iXPos, int iYPos )
{
	// 타이틀
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "전체 일정표" );
		iYPos -= 28;
	}
	else
	{
		g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "대회" );
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "전체 일정표" );
	}

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s %d인으로", m_szPlayModeName.c_str(), (int)m_MaxPlayer );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "총 %d회전이 치뤄집니다.", m_TournamentMaxRound );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerPageScheduleWnd::OnScheduleRegularRender( int iXPos, int iYPos )
{
	char szSrc[MAX_PATH] = "";
	char szDst[MAX_PATH] = "";
	int iStartArray = m_iCurPage * MAX_PAGE_LINE;
	int iSize = (int)m_TournamentRoundDateList.size();
	for(int iCount = 0; iStartArray < iSize; iStartArray++, iCount++)
	{
		if( iCount >= MAX_PAGE_LINE )
			break;

		DWORD dwRoundDate = m_TournamentRoundDateList[iStartArray];

		// 왼쪽
		int iRoundTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, iStartArray );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		
		if( iRoundTeam <= 2 )
			g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "결승" );
		else
			g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%d강", iRoundTeam );

		// 오른쪽
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		sprintf_s_e( szSrc, "%d/%d(%s)", DateHelp::GetDateMonth( dwRoundDate ), DateHelp::GetDateDay( dwRoundDate ), DateHelp::GetWeekDayString( dwRoundDate ) );
		if( strcmp( szSrc, szDst ) == 0 )
		{
			g_FontMgr.PrintText( iXPos + 173, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%02d:%02d", DateHelp::GetDateHour( dwRoundDate ), DateHelp::GetDateMinute( dwRoundDate ) );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 173, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%s %02d:%02d", szSrc, DateHelp::GetDateHour( dwRoundDate ), DateHelp::GetDateMinute( dwRoundDate ) );
			strcpy( szDst, szSrc );
		}
	}
}

void TournamentDetailInnerPageScheduleWnd::OnScheduleCustomRender( int iXPos, int iYPos )
{
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )		
		iYPos -= 28;

	char szSrc[MAX_PATH] = "";
	char szDst[MAX_PATH] = "";
	int iStartArray = m_iCurPage * MAX_PAGE_LINE;	
	int iSize = (int)m_TournamentRoundDateList.size();
	for(int iCount = 0; iStartArray < iSize; iStartArray++, iCount++)
	{
		if( iCount >= MAX_PAGE_LINE )
			break;

		DWORD dwRoundDate = m_TournamentRoundDateList[iStartArray];

		// 왼쪽
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( iStartArray == iSize-1 )
		{
			g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "대회종료" );
		}
		else if( iStartArray == 0 )
		{
			g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "모집마감" );
		}
		else if( iStartArray == 1 )
		{
			g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "대진배정" );
		}
		else
		{
			int iRoundTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, iStartArray-2 );
			if( iRoundTeam == 2 )
				g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "결승" );
			else
				g_FontMgr.PrintText( iXPos + 12, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%d강", iRoundTeam );
		}

		// 오른쪽
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		if( dwRoundDate == 0 )
		{
			g_FontMgr.PrintText( iXPos + 173, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "주최자 수동진행" );
		}
		else
		{
			sprintf_s_e( szSrc, "%d/%d(%s)", DateHelp::GetDateMonth( dwRoundDate ), DateHelp::GetDateDay( dwRoundDate ), DateHelp::GetWeekDayString( dwRoundDate ) );
			if( strcmp( szSrc, szDst ) == 0 )
			{
				g_FontMgr.PrintText( iXPos + 173, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%02d:%02d", DateHelp::GetDateHour( dwRoundDate ), DateHelp::GetDateMinute( dwRoundDate ) );
			}
			else
			{
				g_FontMgr.PrintText( iXPos + 173, iYPos + 221 + ( iCount * 18 ), FONT_SIZE_12, "%s %02d:%02d", szSrc, DateHelp::GetDateHour( dwRoundDate ), DateHelp::GetDateMinute( dwRoundDate ) );
				strcpy( szDst, szSrc );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerPageRewardWnd::TournamentDetailInnerPageRewardWnd()
{
	m_pDevideDot = NULL;
	m_pIcon		 = NULL;
	m_pIconBack  = NULL;

	m_iCurPage					= 0;
	m_iMaxPage					= 0;
	m_MaxPlayer					= 0;
	m_TournamentMaxRound		= 0;
	m_TournamentStartTeamCount	= 0;
	m_dwTournamentIndex			= 0;
	m_dwOwnerIndex				= 0;
}

TournamentDetailInnerPageRewardWnd::~TournamentDetailInnerPageRewardWnd()
{
	SAFEDELETE( m_pDevideDot );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void TournamentDetailInnerPageRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerPageRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentDetailInnerPageRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{

	case ID_PREV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_iCurPage = max( 0, m_iCurPage - 1 );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )		
		{
			m_iCurPage = min( m_iMaxPage, m_iCurPage + 1 );
		}
		break;
	case ID_REWARDADD_BTN:
		if( cmd == IOBN_BTNUP )		
		{
			if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
			{
				TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
				if( pTournament == NULL ) return;

				int iPos = m_iMaxPage - m_iCurPage + 1;
				DWORDVec vCustomReward;
				pTournament->GetCustomRewardData( m_dwTournamentIndex, iPos, vCustomReward );

				CustomTournamentRewardSelectWnd* pWnd = dynamic_cast<CustomTournamentRewardSelectWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_REWARD_SELECT_WND ) );
				if( pWnd )
				{
					if( (int)vCustomReward.size() < 4 )
						pWnd->SetCustomTournamentData( m_dwTournamentIndex, iPos, 4 - (int)vCustomReward.size() );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "더이상 보상을 추가 할 수 없습니다." );
				}				
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "보상은 모집기간에만 추가 할 수 있습니다." );
		}
		break;
	}
}

void TournamentDetailInnerPageRewardWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;

	m_TournamentType		   = pTournament->GetType();
	m_MaxPlayer				   = pTournament->GetMaxPlayer();
	m_TournamentMaxRound       = pTournament->GetTournamentMaxRound();
	m_TournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
	m_szPlayModeName		   = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pTournament->GetPlayMode() ).c_str();
	m_dwOwnerIndex			   = pTournament->GetOwnerIndex();
	
	m_iCurPage = 0;
	m_iMaxPage = m_TournamentMaxRound;

	HideChildWnd( ID_REWARDADD_BTN );

	//위치 초기화
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
	if( pButton )
	{
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );
	}
	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
	if( pButton )
	{
		pButton->SetPositionToDefault( GetWidth(), GetHeight(), true );
	}
		
	if( m_TournamentType == TournamentNode::TYPE_CUSTOM )
	{
		if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
		{			
			ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PREV_BTN ) );
			if( pButton )
			{
				pButton->SetWndPos( pButton->GetXPos() - 15, pButton->GetYPos() - 33 );
			}
			pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT_BTN ) );
			if( pButton )
			{
				pButton->SetWndPos( pButton->GetXPos() + 15, pButton->GetYPos() - 33 );
			}

			if( pTournament->GetState() == TournamentNode::STATE_TEAM_APP )
				SetChildWndStyleRemove( ID_REWARDADD_BTN, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_REWARDADD_BTN, IWS_EXACTIVE );

			ShowChildWnd( ID_REWARDADD_BTN );
		}
	}
}

void TournamentDetailInnerPageRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentDetailInnerPageRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );
	OnPageNumberRender( iXPos, iYPos );
	OnIconRender( iXPos, iYPos );
	OnRewardWndRender( iXPos, iYPos );
}

void TournamentDetailInnerPageRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 28;
	}

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerPageRewardWnd::OnPageNumberRender( int iXPos, int iYPos )
{
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 33;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 366, TAT_CENTER );
}

void TournamentDetailInnerPageRewardWnd::OnIconRender( int iXPos, int iYPos )
{
	if( !m_pIcon || !m_pIconBack )
		return;

	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		iYPos -= 28;
	}

	m_pIconBack->Render( iXPos, iYPos );
	m_pIcon->Render( iXPos, iYPos );
}

void TournamentDetailInnerPageRewardWnd::OnRewardWndRender( int iXPos, int iYPos )
{
	// 타이틀
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	int iRoundTeam = Help::TournamentCurrentRoundWithTeam( m_TournamentStartTeamCount, m_iMaxPage - m_iCurPage );
	if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
	{
		if( iRoundTeam <= 1 )
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "우승 보상" );
		else if( iRoundTeam <= 2 )
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "준우승 보상" );
		else
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%d강 보상", iRoundTeam );

		iYPos -= 28;
	}
	else
	{
		if( iRoundTeam <= 1 )
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "우승" );
		else if( iRoundTeam <= 2 )
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "준우승" );
		else
			g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%d강", iRoundTeam );

		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "대회성적 보상" );
	}

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회팀 소속의 각 개인당 " );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) return;

	if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
	{
		// 상품 표시
		ioHashStringVec kRewardList = pTournament->GetRewardTitle( m_iCurPage + 1 );
		if( kRewardList.empty() ) return;

		for( int i = 0; i < (int)kRewardList.size(); ++i )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 93, iYPos + 221 + ( i * 18 ), FONT_SIZE_12, "%s", kRewardList[i].c_str() );
		}
	}
	else if( pTournament->GetType() == TournamentNode::TYPE_CUSTOM )
	{
		// 상품 표시
		DWORDVec kRewardVec;
		int iPos = m_iMaxPage - m_iCurPage + 1;
		pTournament->GetCustomRewardData( m_dwTournamentIndex, iPos, kRewardVec );
		if( kRewardVec.empty() ) return;

		for( int i = 0;i < (int)kRewardVec.size(); ++i )
		{
			ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( kRewardVec[i] );
			if( pItem )
			{
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
				g_FontMgr.PrintText( iXPos + 93, iYPos + 221 + ( i * 18 ), FONT_SIZE_12, "%s", pItem->GetName() );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailFrameCheerWnd::TournamentDetailFrameCheerWnd()
{
}

TournamentDetailFrameCheerWnd::~TournamentDetailFrameCheerWnd()
{
}


void TournamentDetailFrameCheerWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/detailinnercheerlistwnd.xml", this );	
}


void TournamentDetailFrameCheerWnd::iwm_show()
{
	ShowChildWnd( ID_INNER_CHEER );
}

void TournamentDetailFrameCheerWnd::iwm_hide()
{
	HideChildWnd( ID_INNER_CHEER );
}

void TournamentDetailFrameCheerWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;
		
	TournamentDetailInnerCheerListWnd *pWnd = dynamic_cast< TournamentDetailInnerCheerListWnd * >( FindChildWnd( ID_INNER_CHEER ) );
	if( pWnd )
		pWnd->SetCheerData( dwTourIndex );
}

void TournamentDetailFrameCheerWnd::ApplyDecision()
{
	TournamentDetailInnerCheerListWnd *pWnd = dynamic_cast< TournamentDetailInnerCheerListWnd * >( FindChildWnd( ID_INNER_CHEER ) );
	if( pWnd )
		pWnd->ApplyDecision();
}

void TournamentDetailFrameCheerWnd::UpdateMainUI()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameCheerWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnActiveTypeRender( iXPos, iYPos );
	OnActiveRender( iXPos, iYPos );
	OnDevideWndDotRender( iXPos, iYPos );
	OnInfoRender( iXPos, iYPos );
}
void TournamentDetailFrameCheerWnd::OnActiveTypeRender( int iXPos, int iYPos )
{
	if(	!m_pActiveAwardCenterClose			||
		!m_pActiveAwardRightClose			||
		!m_pActiveTournamentCenterClose		||
		!m_pActiveTournamentRightClose		||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveClose	||
		!m_pScheduleProgressInactiveRight )
		return;

	//대회기간
	if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT )
	{
		m_pActiveTournamentCenterClose->Render( iXPos, iYPos );
		m_pActiveTournamentRightClose->Render( iXPos, iYPos );
	}
	//대회포상대기기간 또는 유휴기간
	else if( m_TournamentState == TournamentNode::STATE_WAITING )
	{
		m_pActiveAwardCenterClose->Render( iXPos, iYPos );
		m_pActiveAwardRightClose->Render( iXPos, iYPos );
	}
	else
	{
		m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
	}
}

void TournamentDetailFrameCheerWnd::OnActiveRender( int iXPos, int iYPos )
{
	if( !m_pScheduleProgressActiveLeft	 ||
		!m_pScheduleProgressActiveCenter ||
		!m_pScheduleProgressActiveRight )
		return;

	if(	TournamentNode::STATE_TEAM_DELAY == m_TournamentState )
	{
		m_pScheduleProgressActiveLeft->Render( iXPos, iYPos );
		m_pScheduleProgressActiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressActiveRight->Render( iXPos, iYPos );
	}
	else
	{
		OnInactiveNotCloseRender( iXPos, iYPos );
	}
}

void TournamentDetailFrameCheerWnd::OnInfoRender( int iXPos, int iYPos )
{
	// 상단 모집 기간
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) 
		return;

	char szPeriod[MAX_PATH] = "";
	DWORD dwStartDate = pTournament->GetStateStartDate( TournamentNode::STATE_TEAM_DELAY );
	DWORD dwEndDate   = pTournament->GetStateEndDate( TournamentNode::STATE_TEAM_DELAY );

	char szStartBuff[MAX_PATH] = "";
	char szEndBuff[MAX_PATH] = "";
	sprintf_s_e( szStartBuff, "%s", DateHelp::GetWeekDayString( dwStartDate ) );
	sprintf_s_e( szEndBuff, "%s", DateHelp::GetWeekDayString( dwEndDate ) );
	if( dwStartDate == 0 && dwEndDate == 0 )
	{
		sprintf_s_e( szPeriod, "주최자 수동진행");
	}
	else if( dwStartDate != 0 && dwEndDate == 0 )
	{
		sprintf_s_e( szPeriod, "%d/%d(%s) ~ 주최자 수동진행", 
			DateHelp::GetDateMonth( dwStartDate ),
			DateHelp::GetDateDay( dwStartDate ),
			szStartBuff );
	}
	else
	{
		sprintf_s_e( szPeriod, "%d/%d(%s) ~ %d/%d(%s)",
			DateHelp::GetDateMonth( dwStartDate ),
			DateHelp::GetDateDay( dwStartDate ),
			szStartBuff,
			DateHelp::GetDateMonth( dwEndDate ),
			DateHelp::GetDateDay( dwEndDate ),
			szEndBuff );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	g_FontMgr.PrintText( iXPos + 308, iYPos + 23, FONT_SIZE_11, "%s", szPeriod );
	g_FontMgr.PrintText( iXPos + 308, iYPos + 35, FONT_SIZE_11, "응원기간" );
}

//////////////////////////////////////////////////////////////////////////

CheerListRadioBtn::CheerListRadioBtn()
{
	m_pCheckMark	 = NULL;
	m_pCheckMarkBack = NULL;
	m_pCheckDisable  = NULL;

	m_dwTeamIdx		= 0;
	m_iCampPos		= CAMP_NONE;
	m_iCheerPoint	= 0;
}

CheerListRadioBtn::~CheerListRadioBtn()
{
	SAFEDELETE( m_pCheckMark );	
	SAFEDELETE( m_pCheckMarkBack );
	SAFEDELETE( m_pCheckDisable );
}

void CheerListRadioBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CheckMark" )
	{
		SAFEDELETE( m_pCheckMark );
		m_pCheckMark = pImage;
	}
	else if( szType == "CheckMarkBack" )
	{
		SAFEDELETE( m_pCheckMarkBack );
		m_pCheckMarkBack = pImage;
	}
	else if( szType == "CheckDisable" )
	{
		SAFEDELETE( m_pCheckDisable );
		m_pCheckDisable = pImage;
	}
	else
	{
		ioRadioButton::AddRenderImage( szType, pImage );
	}
}

void CheerListRadioBtn::SetCheerData( DWORD dwTeamIdx, const ioHashString& szTeamName, int iCampPos, int iCheerPoint )
{
	m_szTeamName	= szTeamName;	
	m_iCampPos		= iCampPos;
	m_iCheerPoint	= iCheerPoint;
	m_dwTeamIdx		= dwTeamIdx;
	
	SetTitleText( szTeamName.c_str() );

	if( m_iCampPos == 0 && m_szTeamName.IsEmpty() && m_iCheerPoint == 0 )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();
	}
}

void CheerListRadioBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_pCheckMark || !m_pCheckMarkBack || !m_pCheckDisable )
		return;

	if( !IsDisabled() )
	{
		m_pCheckMarkBack->Render( iXPos, iYPos );
		if( m_pCheckMark && m_bClicked )
			m_pCheckMark->Render( iXPos, iYPos );
	}
	else
	{
		m_pCheckDisable->Render( iXPos, iYPos );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_iCampPos == CAMP_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

	g_FontMgr.PrintTextWidthCut( iXPos + 24, iYPos + 5, FONT_SIZE_12, 155.0f, "%s", m_szTeamName.c_str() );	
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 259, iYPos + 5, FONT_SIZE_12, 155.0f, "%d 응원", m_iCheerPoint );
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerCheerListWnd::TournamentDetailInnerCheerListWnd()
{
	m_pDarkLine	  = NULL;
	m_pLightLine  = NULL;

	m_pBackCircle = NULL;
	m_pBackHub	  = NULL;
	m_pCheerIcon  = NULL;

	m_iCurPage = 0;
	m_iMaxPage = 0;

	m_dwTournamentIndex	  = 0;
	m_dwAllocateSyncTimer = 0;
	m_dwCheerTeamCnt	  = 0;

	m_CheerState = CS_NONE;
	m_iCampPos   = CAMP_NONE;

	m_dwMyTeamIdx = 0;
}

TournamentDetailInnerCheerListWnd::~TournamentDetailInnerCheerListWnd()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pCheerIcon );
}

void TournamentDetailInnerCheerListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{		
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CheerIcon" )
	{
		SAFEDELETE( m_pCheerIcon );
		m_pCheerIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}	
}

void TournamentDetailInnerCheerListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentDetailInnerCheerListWnd::iwm_create()
{
}

void TournamentDetailInnerCheerListWnd::SetCheerData( DWORD dwTourIndex )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTourIndex );
	if( pTournament == NULL ) return;

	m_dwTournamentIndex = dwTourIndex;
}

void TournamentDetailInnerCheerListWnd::SetDataSync()
{
	m_dwAllocateSyncTimer = FRAMEGETTIME();
}

void TournamentDetailInnerCheerListWnd::iwm_show()
{
	SetPageParsingData();
	SetDataSync();
	CheckMyTeamPage();
}

void TournamentDetailInnerCheerListWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )
		PrevPage();
	else if( zDelta == -WHEEL_DELTA )
		NextPage();
}

void TournamentDetailInnerCheerListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	if( dwID == ID_VERT_SCROLL )
	{
		if( m_iCurPage != curPos )
		{
			int iPrevPage = m_iCurPage;
			if( iPrevPage < curPos )
			{
				m_iCurPage = curPos;
				m_iCurPage = min( m_iCurPage, m_iMaxPage );
			}
			else if( iPrevPage > curPos )
			{
				m_iCurPage = curPos;
				m_iCurPage = max( 0, m_iCurPage );
			}

			if( iPrevPage != m_iCurPage )
			{
				SetDataSync();
				SetPageParsingData();
			}
		}
	}
}

void TournamentDetailInnerCheerListWnd::CheckMyTeamPage()
{
	//Show될때만 자신의 응원팀을 구해서 SendAllocatePageSync()를 통해서 전달 후 자신의 응원팀이 몇페이지에 있는지 구함
	ioUserTournament *pTournament = g_MyInfo.GetTournament();
	if( pTournament && pTournament->IsCheerTournament( m_dwTournamentIndex ) )
	{
		const CheerTeamInfo& rkTeamInfo = pTournament->GetCheerTeam( m_dwTournamentIndex );
		m_dwMyTeamIdx = rkTeamInfo.m_dwTeamIndex;
	}
}

void TournamentDetailInnerCheerListWnd::PrevPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iPrevPage = m_iCurPage;
		m_iCurPage--;
		m_iCurPage = max( 0, m_iCurPage );

		if( m_iCurPage != iPrevPage )
		{
			pScroll->SetScrollPos( m_iCurPage );
			SetDataSync();
			SetPageParsingData();
		}
	}
}

void TournamentDetailInnerCheerListWnd::NextPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iPrevPage = m_iCurPage;
		m_iCurPage++;
		m_iCurPage = min( m_iCurPage, m_iMaxPage );

		if( m_iCurPage != iPrevPage )
		{
			pScroll->SetScrollPos( m_iCurPage );
			SetDataSync();
			SetPageParsingData();
		}
	}
}

void TournamentDetailInnerCheerListWnd::SetMaxPage()
{
	int iTotalCount = 0;
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pNode )
	{
		iTotalCount = pNode->GetUnAllocateTotalCount();
	}
	m_iMaxPage = max( 0, max( 0, iTotalCount-1 ) / TournamentHelp::PAGE_TEAM_MAX );

	//스크롤 설정
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, m_iMaxPage + 1 );
	}
}

void TournamentDetailInnerCheerListWnd::SendCheerDecision()
{
	DWORD dwTeamIdx = 0;
	CheerListRadioBtn* pRadio = GetSelectRadio();
	if( pRadio )	
		dwTeamIdx = pRadio->GetTeamIdx();
	
	SP2Packet kPacket( CTPK_TOURNAMENT_CHEER_DECISION );
	kPacket << m_dwTournamentIndex << dwTeamIdx;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void TournamentDetailInnerCheerListWnd::ApplyDecision()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
		pTournament->SendAllocatePageSync( m_iCurPage );
}

void TournamentDetailInnerCheerListWnd::SetPageParsingData()
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );

	short Pos = 0;
	for( int ID = ID_RADIO_BTN1; ID <= ID_RADIO_BTN16; ++ID, ++Pos )
	{
		CheerListRadioBtn* pWnd = dynamic_cast<CheerListRadioBtn*>( FindChildWnd( ID ) );
		if( pWnd )
		{
			if( pNode )
			{
				TournamentNode::AllocateTeamData& kData = pNode->GetUnAllocateTeamData( m_iCurPage * TournamentHelp::PAGE_TEAM_MAX + Pos );
				if( kData.m_dwTeamIndex != 0 )
				{
					TeamGlobalData* pTeam = g_TournamentTeamMgr.GetTeamData( kData.m_dwTeamIndex, false );
					if( pTeam )
					{
						pWnd->SetCheerData( kData.m_dwTeamIndex, pTeam->GetTeamName(), pTeam->GetCampPos(), pTeam->GetCheerPoint() );
						continue;
					}
				}
			}
			pWnd->SetCheerData( 0, "", 0, 0 );
			
		}
	}
	SetMaxPage();
}

void TournamentDetailInnerCheerListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_RADIO_BTN1:
	case ID_RADIO_BTN2:
	case ID_RADIO_BTN3:
	case ID_RADIO_BTN4:
	case ID_RADIO_BTN5:
	case ID_RADIO_BTN6:
	case ID_RADIO_BTN7:
	case ID_RADIO_BTN8:
	case ID_RADIO_BTN9:
	case ID_RADIO_BTN10:
	case ID_RADIO_BTN11:
	case ID_RADIO_BTN12:
	case ID_RADIO_BTN13:
	case ID_RADIO_BTN14:
	case ID_RADIO_BTN15:
	case ID_RADIO_BTN16:
		{
			if( cmd == IOBN_BTNDOWN )		
			{
				SetChildActvieWnd( dwID );
				CalcCheerPoint();
			}
		}
		break;

	case ID_CHEER_DECISION_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				int iCampPos = CAMP_NONE;
				ioHashString szTeamName;
				CheerListRadioBtn* pSelect = GetSelectRadio();
				if( pSelect )
				{
					szTeamName = pSelect->GetTeamName();
					iCampPos   = pSelect->GetCampPos();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "응원할 팀을 선택해주세요" );
					return;
				}

				TournamentCheerPopUpWnd* pWnd = dynamic_cast<TournamentCheerPopUpWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_CHEER_DECISION_WND ) );
				if( pWnd )				
					pWnd->SetCheerDecision( szTeamName, iCampPos, this );
				
			}
		}
		break;
	case TOURNAMENT_CHEER_DECISION_WND:
		{			
			SendCheerDecision();
		}
		break;
	}
}

void TournamentDetailInnerCheerListWnd::SetChildActvieWnd( DWORD dwBtnID )
{
	if( dwBtnID < ID_RADIO_BTN1 || ID_RADIO_BTN16 < dwBtnID )
		return;

	CheckRadioButton( ID_RADIO_BTN1, ID_RADIO_BTN16, dwBtnID );
}

void TournamentDetailInnerCheerListWnd::SetRadioAllActive()
{
	for (int i = ID_RADIO_BTN1; i <= ID_RADIO_BTN16; ++i )
	{
		CheerListRadioBtn* pRadio = dynamic_cast<CheerListRadioBtn*>( FindChildWnd( i ) );
		if( pRadio )		
			pRadio->SetActive();		
	}

	CalcCheerPoint();
}

void TournamentDetailInnerCheerListWnd::SetRadioSelectActive( DWORD dwTeamIdx )
{
	DWORD dwID = 0;
	for (int i = ID_RADIO_BTN1; i <= ID_RADIO_BTN16; ++i )
	{
		CheerListRadioBtn* pRadio = dynamic_cast<CheerListRadioBtn*>( FindChildWnd( i ) );

		if(pRadio)
		{
			if( pRadio->GetTeamIdx() == dwTeamIdx )
			{
				dwID = i;
				pRadio->SetActive();
			}
			else
			{
				pRadio->SetInActive();
			}
		}
	}

	CheckRadioButton( ID_RADIO_BTN1, ID_RADIO_BTN16, dwID );
	CalcCheerPoint();
}

CheerListRadioBtn* TournamentDetailInnerCheerListWnd::GetSelectRadio()
{	
	for (int i = ID_RADIO_BTN1; i <= ID_RADIO_BTN16; ++i )
	{
		CheerListRadioBtn* pRadio = dynamic_cast<CheerListRadioBtn*>( FindChildWnd( i ) );
		if( pRadio && pRadio->IsPushed() )
			return pRadio;
	}

	return NULL;
}

void TournamentDetailInnerCheerListWnd::OnTeamDataSyncProcess()
{
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( !pTournament )
		return;

	if( m_dwAllocateSyncTimer + 1000 < FRAMEGETTIME() )
	{
		ProcessMyTeamPage();

		if( !pTournament->IsUnAllocatePageSync( m_iCurPage ) )	
		{
			pTournament->SendAllocatePageSync( m_iCurPage, m_dwMyTeamIdx );
		}

		if( pTournament->IsUnAllocatePageUpdate( m_iCurPage ) )
		{			
			SetPageParsingData();
		}
	}	
}

void TournamentDetailInnerCheerListWnd::ProcessMyTeamPage()
{
	//자신의 응원팀이 몇페이지에 있는지 구했다면 해당페이지로 이동
	if( 0 < m_dwMyTeamIdx )
	{
		ioUserTournament *pTournament = g_MyInfo.GetTournament();
		if( pTournament && pTournament->IsCheerTournament( m_dwTournamentIndex ) )
		{
			const CheerTeamInfo& rkTeamInfo = pTournament->GetCheerTeam( m_dwTournamentIndex );
			if( 0 <= rkTeamInfo.m_iMyCheerTeamPage )
			{
				m_iCurPage = rkTeamInfo.m_iMyCheerTeamPage;
				m_dwMyTeamIdx = 0;
				
				SetMaxPage();
				ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
				if( pScroll )
					pScroll->SetScrollPos( m_iCurPage );

				TournamentNode* pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
				if( pTournament && !pTournament->IsUnAllocatePageSync( m_iCurPage ) )
				{
					//동기화 패킷을 보냈다면 pTournament->IsUnAllocatePageUpdate()에 의해 SetPageParsingData() 호출
					pTournament->SendAllocatePageSync( m_iCurPage );
				}
				else
				{					
					SetPageParsingData();
				}
			}
		}
	}
}

void TournamentDetailInnerCheerListWnd::CalcCheerPoint()
{
	DWORD dwTeamIdx = 0;
	if( m_CheerState == CS_DECISION )
	{
		ioUserTournament *pTournament = g_MyInfo.GetTournament();
		if( pTournament && pTournament->IsCheerTournament( m_dwTournamentIndex ) )
		{
			const CheerTeamInfo& rkTeamInfo = pTournament->GetCheerTeam( m_dwTournamentIndex );
			dwTeamIdx = rkTeamInfo.m_dwTeamIndex;
		}
	}
	else
	{
		CheerListRadioBtn* pRadio = GetSelectRadio();
		if( pRadio )
		{
			dwTeamIdx = pRadio->GetTeamIdx();
		}
	}

	DWORD dwState = 0;
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament )
	{	
		dwState = pTournament->GetState();

		TeamGlobalData* pTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIdx, false );		
		if( pTeamData )
		{
			int iCheerPoint			= pTeamData->GetCheerPoint();
			DWORD dwTotalCheerCount = pTournament->GetTotalCheerCount();
			DWORD dwStartTeamCount  = pTournament->GetTournamentStartTeamCount();

			//응원중인 팀이 결정이 안됬으면 예상페소를 계산
			if( m_CheerState != CS_DECISION )
			{
				iCheerPoint += 1;
				dwTotalCheerCount += 1;
			}

			char szPeso[MAX_PATH];
			char szBuff[MAX_PATH];
			DWORD dwPoint = 1000;
			if( 0 < dwTotalCheerCount && 0 < dwStartTeamCount && 0 < iCheerPoint )
			{
				float fPoint  = ((float)dwTotalCheerCount / (float)dwStartTeamCount) / (float)iCheerPoint;
				dwPoint = max( 1000, fPoint * pTournament->GetAdjustCheerPeso());
				dwPoint = min( 100000, dwPoint );
			}			

			Help::ConvertNumToStrComma( dwPoint, szBuff, MAX_PATH );
			sprintf( szPeso, "%s 페소", szBuff );
			m_szTourCheerPoint = szPeso;
			return;
		}
	}

	switch( dwState )
	{
	case TournamentNode::STATE_TEAM_APP:
	case TournamentNode::STATE_TEAM_DELAY:
		m_szTourCheerPoint = "????? 페소";
		break;
	default:
		m_szTourCheerPoint = "0 페소";
		break;
	}	
}

void TournamentDetailInnerCheerListWnd::OnCheckCheerState()
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( !pNode )
		return;

	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CHEER_DECISION_BTN ) );
	if( !pButton )
		return;
	
	SetChildActive( ID_CHEER_DECISION_BTN );
	pButton->SetTitleText( "우승응원 결정" );
	m_iCampPos = CAMP_NONE;

	TeamGlobalData* pTeamData = NULL;
	ioUserTournament *pTournament = g_MyInfo.GetTournament();
	if( pTournament && pTournament->IsCheerTournament( m_dwTournamentIndex ) )
	{
		const CheerTeamInfo& rkTeamInfo = pTournament->GetCheerTeam( m_dwTournamentIndex );
		pTeamData = g_TournamentTeamMgr.GetTeamData( rkTeamInfo.m_dwTeamIndex, false );
	}
	
	SetRadioAllActive();
	
	m_szTourCheerTeamName = "";

	switch( pNode->GetState() )
	{
	case TournamentNode::STATE_TEAM_APP:
		{
			m_CheerState = CS_NONE;
			SetChildInActive( ID_CHEER_DECISION_BTN );
			pButton->SetTitleText( "응원기간이 아닙니다." );
		}
		break;
	case TournamentNode::STATE_TEAM_DELAY:
		{
			if( pTeamData )
			{	
				m_CheerState = CS_DECISION;
				m_szTourCheerTeamName = pTeamData->GetTeamName();
				SetChildInActive( ID_CHEER_DECISION_BTN );
				pButton->SetTitleText( "결정되었습니다" );
				SetRadioSelectActive( pTeamData->GetTeamIndex() );
				m_iCampPos = pTeamData->GetCampPos();
			}
			else
			{
				m_CheerState = CS_NONE;
			}
		}
		break;
	default:
		{
			if( pTeamData )
			{
				m_CheerState = CS_DECISION;
				m_szTourCheerTeamName = pTeamData->GetTeamName();
				SetChildInActive( ID_CHEER_DECISION_BTN );
				pButton->SetTitleText( "결정되었습니다" );
				SetRadioSelectActive( pTeamData->GetTeamIndex() );
				m_iCampPos = pTeamData->GetCampPos();
			}
			else
			{
				m_CheerState = CS_NO_DECISION;
				SetChildInActive( ID_CHEER_DECISION_BTN );
				pButton->SetTitleText( "응원기간이 종료 되었습니다" );
				SetRadioSelectActive( 0 );
			}
		}
		break;
	}	
}

void TournamentDetailInnerCheerListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	OnTeamDataSyncProcess();
	OnCheckCheerState();
}

void TournamentDetailInnerCheerListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	OnListBackRender( iXPos, iYPos );
	OnCheerIconRender(  iXPos, iYPos );
	OnMentRender( iXPos, iYPos );
	OnCheerRender( iXPos, iYPos );
	OnCheerPointRender( iXPos, iYPos );
}

void TournamentDetailInnerCheerListWnd::OnListBackRender( int iXPos, int iYPos )
{
	if( !m_pDarkLine || !m_pLightLine )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
	{
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
		{
			if( j % 2 == 0 )
			{				
				m_pDarkLine->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j, UI_RENDER_MULTIPLY );
			}
			else
			{				
				m_pLightLine->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j, UI_RENDER_MULTIPLY );
			}
		}
	}
}

void TournamentDetailInnerCheerListWnd::OnCheerIconRender( int iXPos, int iYPos )
{
	if( !m_pBackCircle || !m_pBackHub || !m_pCheerIcon )
		return;

	//응원 선택전
	if( m_CheerState == CS_DECISION )
		m_pBackHub->Render( iXPos, iYPos );
	else
		m_pBackCircle->Render( iXPos, iYPos );

	m_pCheerIcon->Render( iXPos, iYPos );
}

void TournamentDetailInnerCheerListWnd::OnMentRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	switch( m_CheerState )
	{
	case CS_NO_DECISION:
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintTextWidthCut( iXPos + 425, iYPos + 22, FONT_SIZE_26, 266, "우승응원에" );
			g_FontMgr.PrintText( iXPos + 425, iYPos + 52, FONT_SIZE_26, "참가하지 않음" );
		}
		break;
	default:
		{
			if( m_iCampPos == CAMP_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( m_iCampPos == CAMP_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			if( m_szTourCheerTeamName.IsEmpty() )
				g_FontMgr.PrintTextWidthCut( iXPos + 425, iYPos + 22, FONT_SIZE_26, 266, "?????" );
			else
				g_FontMgr.PrintTextWidthCut( iXPos + 425, iYPos + 22, FONT_SIZE_26, 266, m_szTourCheerTeamName.c_str() );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 425, iYPos + 52, FONT_SIZE_26, "우승응원하기" );
		}
		break;
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 425, iYPos + 97,  FONT_SIZE_13, "우승할 것 같은 팀에게 응원을 해보세요." );
	g_FontMgr.PrintText( iXPos + 425, iYPos + 117, FONT_SIZE_13, "응원한 팀이 우승하면 응원 보상을 드립니다." );
	g_FontMgr.PrintText( iXPos + 425, iYPos + 137, FONT_SIZE_13, "응원이 적은 팀일수록 응원 보상이 커지며" );
	g_FontMgr.PrintText( iXPos + 425, iYPos + 157, FONT_SIZE_13, "응원기간 동안 보상은 바뀔 수 있습니다." );
}

void TournamentDetailInnerCheerListWnd::OnCheerPointRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 425, iYPos + 293, FONT_SIZE_13, "응원자 보상" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos + 425, iYPos + 311, FONT_SIZE_13, m_szTourCheerPoint.c_str() );
}

void TournamentDetailInnerCheerListWnd::OnCheerRender( int iXPos, int iYPos )
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( !pNode )
		return;
	
	if( pNode->GetState() == TournamentNode::STATE_TEAM_APP )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

		g_FontMgr.PrintText( iXPos + 132, iYPos + 173,  FONT_SIZE_12, "아직 응원기간이 아닙니다." );
		g_FontMgr.PrintText( iXPos + 132, iYPos + 197,  FONT_SIZE_12, "응원 기간에 오시면 참여 하실 수 있습니다." );
	}
	else if( pNode->GetUnAllocateTotalCount() == 0 )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

		g_FontMgr.PrintText( iXPos + 132, iYPos + 173,  FONT_SIZE_12, "대회에 참여 한 팀이 없습니다." );
	}	
}

//////////////////////////////////////////////////////////////////////////

TournamentCheerPopUpWnd::TournamentCheerPopUpWnd()
{
	m_pCallWnd = NULL;
}

TournamentCheerPopUpWnd::~TournamentCheerPopUpWnd()
{
}

void TournamentCheerPopUpWnd::SetCheerDecision( const ioHashString& szTeamName, BYTE CampPos, ioWnd* pCallWnd )
{
	m_szTeamName = szTeamName;
	m_CampPos	 = CampPos;
	m_pCallWnd	 = pCallWnd;

	ShowWnd();
}


void TournamentCheerPopUpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pCallWnd )
				m_pCallWnd->iwm_command( this, 0, 0 );

			HideWnd();
		}
		break;
	}
}

bool TournamentCheerPopUpWnd::iwm_spacebar()
{
	if( m_pCallWnd )
		m_pCallWnd->iwm_command( this, 0, 0 );
	
	HideWnd();
	return true;
}

void TournamentCheerPopUpWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_CampPos == CAMP_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

	if( m_szTeamName.IsEmpty() )
		g_FontMgr.PrintTextWidthCut( iXPos + 151, iYPos + 144,FONT_SIZE_13, 268.0f, "?????" );
	else
		g_FontMgr.PrintTextWidthCut( iXPos + 151, iYPos + 144,FONT_SIZE_13, 268.0f, "%s", m_szTeamName.c_str() );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "응원 결정 후 취소는 불가능합니다." );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "응원 팀이 만약 첫경기에 참가하지 않으면" );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 204, FONT_SIZE_13, "대진표에서 제외될 수도 있습니다." );
}

//////////////////////////////////////////////////////////////////////////

TournamentPredictPopUpWnd::TournamentPredictPopUpWnd()
{
	m_pCallWnd = NULL;
}

TournamentPredictPopUpWnd::~TournamentPredictPopUpWnd()
{
}

void TournamentPredictPopUpWnd::SetPredictDecision( const ioHashString& szTeamName, BYTE CampPos, ioWnd* pCallWnd )
{
	m_szTeamName = szTeamName;
	m_CampPos	 = CampPos;
	m_pCallWnd	 = pCallWnd;

	ShowWnd();
}

void TournamentPredictPopUpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pCallWnd )
				m_pCallWnd->iwm_command( this, 0, 0 );

			HideWnd();
		}
		break;
	}
}

bool TournamentPredictPopUpWnd::iwm_spacebar()
{
	if( m_pCallWnd )
		m_pCallWnd->iwm_command( this, 0, 0 );

	HideWnd();
	return true;
}

void TournamentPredictPopUpWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	if( m_CampPos == CAMP_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

	ioHashString szTeamName;
	Help::TextCut( m_szTeamName.c_str(), 268, TS_NORMAL, FONT_SIZE_13, szTeamName );

	kPrinter.AddTextPiece( FONT_SIZE_12, "%s", szTeamName.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "의" );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164,FONT_SIZE_13, "승리를 예측하시겠습니까?" );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 184, FONT_SIZE_13, "승리예측게임은 일정마다 한번씩만 가능하며" );	
	g_FontMgr.PrintText( iXPos + 23, iYPos + 204, FONT_SIZE_13, "결승에 가까워질 수록 보상이 높습니다." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 234, FONT_SIZE_13, "예측한 팀 승리시 보상:" );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 278, iYPos + 234, FONT_SIZE_13, "%d 페소", 0 );	
		
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 254, FONT_SIZE_13, "예측한 팀 패배시 보상:" );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 278, iYPos + 254, FONT_SIZE_13, "%d 페소", 0 );	
}

//////////////////////////////////////////////////////////////////////////

MatchBtn::MatchBtn()
{
	m_dwOrgTextColor = 0;
	
	m_pMatchLine		= NULL;
	m_pMatchActiveLine	= NULL;

	m_pCurrentNor  = NULL;
	m_pCurrentPush = NULL;
	m_pCurrentOver = NULL;

	m_pBlueNor	   = NULL;
	m_pBlueOver	   = NULL;
	m_pBluePush    = NULL;

	m_pRedNor	   = NULL;
	m_pRedOver	   = NULL;
	m_pRedPush	   = NULL;

	m_pGreenNor	   = NULL;
	m_pGreenOver   = NULL;
	m_pGreenPush   = NULL;

	m_LineState = LS_NONE;
	m_bNextRound = false;
	m_bCheerTeam = false;

	m_iUpHeight = 0;
	m_iUpYPosition = 0;
	
	m_pMyCheerTeam = NULL;
}

MatchBtn::~MatchBtn()
{
	SAFEDELETE(m_pBlueNor);
	SAFEDELETE(m_pBlueOver);
	SAFEDELETE(m_pBluePush);
	SAFEDELETE(m_pRedNor);
	SAFEDELETE(m_pRedOver);
	SAFEDELETE(m_pRedPush);	
	SAFEDELETE(m_pGreenNor);
	SAFEDELETE(m_pGreenOver);
	SAFEDELETE(m_pGreenPush);	
	SAFEDELETE(m_pMyCheerTeam);
}

void MatchBtn::iwm_create()
{
	m_dwOrgTextColor = m_pUITitle->GetNormalTextColor();
}

void MatchBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MyCheerTeam" )
	{
		SAFEDELETE( m_pMyCheerTeam );
		m_pMyCheerTeam = pImage;

	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MatchBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNor );
		m_pBlueNor = pFrame;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pFrame;
	}
	else if( szType == "BluePush" )
	{
		SAFEDELETE( m_pBluePush );
		m_pBluePush = pFrame;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNor );
		m_pRedNor = pFrame;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pFrame;
	}
	else if( szType == "RedPush" )
	{
		SAFEDELETE( m_pRedPush );
		m_pRedPush = pFrame;
	}
	else if( szType == "GreenNor" )
	{
		SAFEDELETE( m_pGreenNor );
		m_pGreenNor = pFrame;
	}
	else if( szType == "GreenOver" )
	{
		SAFEDELETE( m_pGreenOver );
		m_pGreenOver = pFrame;
	}
	else if( szType == "GreenPush" )
	{
		SAFEDELETE( m_pGreenPush );
		m_pGreenPush = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void MatchBtn::SetMatchLine( ioUIRenderImage* pMatchLine, ioUIRenderImage* pMatchAciveLine, int iUpHeight, int iUpYPosition )
{
	m_pMatchLine		= pMatchLine;
	m_pMatchActiveLine	= pMatchAciveLine;
	m_iUpHeight			= iUpHeight;
	m_iUpYPosition		= iUpYPosition;
}

void MatchBtn::SetMatchData( DWORD dwTourIndex, int iTournamentCurHorzPage, DWORD dwMyPageRound, SHORT Position )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( !pTournament ) return;

	ShowWnd();

	m_dwMyPageRound			  = dwMyPageRound;
	m_iTournamentCurHorzPage  = iTournamentCurHorzPage;
	m_iTournamentMaxRound	  = pTournament->GetTournamentMaxRound();
	m_TournamentState		  = pTournament->GetState();

	DWORD dwCurRound = m_iTournamentCurHorzPage + m_dwMyPageRound;
	m_RoundTeamData = pTournament->GetRoundTeamData( dwCurRound, Position );

	ChangeStyle( dwTourIndex, dwCurRound, m_RoundTeamData, Position );
	CheckNextRoundGo( dwTourIndex, dwCurRound, m_RoundTeamData, Position );
	CheckNextPageGo( dwTourIndex, dwCurRound, m_RoundTeamData, Position );
}

void MatchBtn::CheckNextRoundGo( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData, SHORT Position )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTournamentIndex, false );
	if( !pTournament ) return;

	//이전 라운드에서 현재 라운드로 진출했는지 여부를 판단해서 라인 그려주기(현재 -> 이전 참조)
	RoundTeamData &rkPrevData = pTournament->GetRoundTeamData( dwCurRound - 1, Position * 2 );
	if( rkPrevData.m_dwTeamIndex != 0 && rkTeamData.m_dwTeamIndex != 0 )
	{
		if( rkPrevData.m_dwTeamIndex == rkTeamData.m_dwTeamIndex )
			m_LineState = LS_DOWN;
		else
			m_LineState = LS_UP;
	}
	else if( rkTeamData.m_dwTeamIndex != 0 )
	{
		m_LineState = LS_UP;
	}
	else
	{
		m_LineState = LS_NONE;
	}
}

void MatchBtn::CheckNextPageGo( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData, SHORT Position )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTournamentIndex, false );
	if( !pTournament ) return;

	m_bNextRound = false;
	RoundTeamData& rkNextData = pTournament->GetRoundTeamData( dwCurRound + 1, (Position / 2) + (Position % 2) );
	if( rkNextData.m_dwTeamIndex != 0 && rkTeamData.m_dwTeamIndex != 0 )
	{
		//페이지 내에서 제일 마지막 버튼일때 다음 라운드(다음페이지) 데이터를 가져와서 진출했는지 여부를 판단
		//(현재 -> 다음 참조)
		if( TournamentHelp::PAGE_ROUND_MAX == m_dwMyPageRound && rkNextData.m_dwTeamIndex == rkTeamData.m_dwTeamIndex )
		{
			m_bNextRound = true;
		}
	}
}

void MatchBtn::ChangeStyle( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData, SHORT Position )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTournamentIndex, false );
	if( !pTournament ) return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( !pUserTournament ) return;

	m_bCheerTeam = false;

	if( rkTeamData.m_dwTeamIndex != 0 )
	{
		//자신의 팀일때
		if( pUserTournament->IsAlreadyTeam( rkTeamData.m_dwTeamIndex ) )
		{
			ChangeMyTeamStyle( pTournament->GetType(), rkTeamData );
		}
		else
		{
			ChangeOtherTeamStyle( pTournament->GetType(), rkTeamData );
		}

		//응원 여부
		const CheerTeamInfo& pInfo = pUserTournament->GetCheerTeam( dwTournamentIndex );
		TeamGlobalData* pTeamData = g_TournamentTeamMgr.GetTeamData( pInfo.m_dwTeamIndex, false );
		if( pTeamData && pInfo.m_dwTeamIndex == rkTeamData.m_dwTeamIndex && pTeamData->GetTourPos() == dwCurRound )
		{
			m_bCheerTeam = true;
		}		
	}
	else
	{
		ChangeEmptyTeamStyle( dwCurRound, Position );
	}
}

void MatchBtn::ChangeMyTeamStyle( BYTE TourType, const RoundTeamData& rkTeamData )
{
	if( TourType == TournamentNode::TYPE_REGULAR )
	{
		if( rkTeamData.m_CampPos == CAMP_BLUE )
		{
			SetActiveStyle( m_pBlueNor, m_pBluePush, m_pBlueOver, rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_WHITE, 0xFF215AA2 );
		}
		else
		{
			SetActiveStyle( m_pRedNor, m_pRedPush, m_pRedOver, rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_WHITE, 0xFF9F291A );
		}
	}
	else
	{
		SetActiveStyle( m_pGreenNor, m_pGreenPush, m_pGreenOver, rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_WHITE, 0xFF117222 );
	}
}

void MatchBtn::ChangeOtherTeamStyle( BYTE TourType, const RoundTeamData& rkTeamData )
{
	m_pCurrentNor	= NULL;
	m_pCurrentPush	= NULL;
	m_pCurrentOver	= NULL;

	if( TourType == TournamentNode::TYPE_REGULAR )
	{
		if( rkTeamData.m_CampPos == CAMP_BLUE )
		{
			if( rkTeamData.m_bDropTeam )
				SetTitle( rkTeamData.m_szTeamName.c_str(), 0xFF8F9AB2, 0, TS_NORMAL, 90 );
			else
				SetTitle( rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_BLUE, 0, TS_NORMAL, 90 );

			RemoveWndStyle( IWS_EXACTIVE );
			SetActive();
		}
		else
		{
			if( rkTeamData.m_bDropTeam )
				SetTitle( rkTeamData.m_szTeamName.c_str(), 0xFFBA9995, 0, TS_NORMAL, 90 );
			else
				SetTitle( rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_RED, 0, TS_NORMAL, 90 );

			RemoveWndStyle( IWS_EXACTIVE );
			SetActive();
		}
	}
	else
	{
		if( rkTeamData.m_bDropTeam )
			SetTitle( rkTeamData.m_szTeamName.c_str(), 0xFF8FB295, 0, TS_NORMAL, 90 );
		else
			SetTitle( rkTeamData.m_szTeamName.c_str(), TCT_DEFAULT_GREEN, 0, TS_NORMAL, 90 );

		RemoveWndStyle( IWS_EXACTIVE );
		SetActive();
	}
}

void MatchBtn::ChangeEmptyTeamStyle( DWORD dwCurRound, SHORT Position )
{
	if( 1 == dwCurRound )
	{
		char szBuff[MAX_PATH];
		sprintf( szBuff, "%d번 대회팀", (int)Position );
		SetTitle( szBuff, m_dwOrgTextColor, 0xFF000000, TS_NORMAL );
		SetActive();
		AddWndStyle( IWS_EXACTIVE );
	}
	else if( 1 < dwCurRound )
	{
		SetTitleText( "" );
		SetInActive();
		RemoveWndStyle( IWS_EXACTIVE );
	}
}

void MatchBtn::SetActiveStyle( ioUIRenderFrame* pNor, ioUIRenderFrame* pPush, ioUIRenderFrame* pOver, ioHashString szTtitle, DWORD dwTextColor, DWORD dwBkColor )
{
	m_pCurrentNor	= pNor;
	m_pCurrentPush	= pPush;
	m_pCurrentOver	= pOver;
	SetTitle( szTtitle, dwTextColor, dwBkColor, TS_BOLD_OUTLINE_FULL_2X, 90, 50, 3 );
	RemoveWndStyle( IWS_EXACTIVE );
	SetActive();
}

void MatchBtn::SetTitle( ioHashString szTtitle, DWORD dwTextColor, DWORD dwBkColor, TextStyle eStyle, int iWidthCut, int iOffsetX, int iOffsetY )
{
	if( !szTtitle.IsEmpty() )
		SetTitleText( szTtitle.c_str() );
	
	if( 0 < iWidthCut )
	{
		m_pUITitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
		m_pUITitle->SetWidthCutSize( iWidthCut );
	}
	else
	{
		m_pUITitle->SetPrintTextStyle( ioUITitle::PTS_NORMAL );
	}

	if( iOffsetX != 0 || iOffsetY != 0 )
		m_pUITitle->SetOffset( iOffsetX, iOffsetY );

	m_pUITitle->SetStyle( eStyle );
	m_pUITitle->SetColor( "Normal", dwTextColor, dwBkColor );
	m_pUITitle->SetColor( "Over",	dwTextColor, dwBkColor );
	m_pUITitle->SetColor( "Push",	dwTextColor, dwBkColor );
}

void MatchBtn::OnRender()
{
	if( !IsShow() ) return;

	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	//현재 라운드가 페이지의 시작 라운드이면서 이전 페이지가 존재 할때
	if( TournamentHelp::PAGE_ROUND_MIN == m_dwMyPageRound && 0 < m_iTournamentCurHorzPage )
	{
		//진출 데이터가 존재하고 스케쥴표에서 다음라운드로 진행됬을때 활성화
		if( m_LineState != LS_NONE )
		{
			onMatchPrevPageActiveLineRender( iXPos, iYPos );
		}
		else
		{
			onMatchPrevPageLineRender( iXPos, iYPos );
		}
		
	}
	else if( TournamentHelp::PAGE_ROUND_MIN < m_dwMyPageRound )
	{
		if( TournamentHelp::PAGE_ROUND_MAX == m_dwMyPageRound )
		{
			//다음 페이지 진출 라인
			if( m_bNextRound )
			{
				onMatchNextPageActiveLineRender( iXPos, iYPos );
			}
			else
			{
				onMatchNextPageLineRender( iXPos, iYPos );
			}
		}
	
		//기본 Base 라인(회색)
		onMatchLineRender( iXPos, iYPos );

		switch( m_LineState )
		{
		case LS_UP:
			onMatchActiveUpLineRender(  iXPos, iYPos );
			break;
		case LS_DOWN:
			onMatchActiveDownLineRender(  iXPos, iYPos );
			break;
		}
	}

	if( m_pMyCheerTeam && m_bCheerTeam )
		m_pMyCheerTeam->Render( iXPos, iYPos );
}

void MatchBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	if( m_pCurrentNor )
		m_pCurrentNor->Render( iXPos, iYPos );
}

void MatchBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	if( m_pCurrentOver )	
	{
		m_pCurrentOver->Render( iXPos, iYPos );
		OnDrawOveredAdd( iXPos, iYPos );
	}	
}

void MatchBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	if( m_pCurrentPush )
		m_pCurrentPush->Render( iXPos, iYPos );

}

void MatchBtn::onMatchLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchLine )
		return;
	
	//Center
	m_pMatchLine->SetPosition( 0, 0 );	
	m_pMatchLine->SetSize( 4, 2 );	
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos - 4, iYPos + 11 );
	
	//Up
	m_pMatchLine->SetPosition( 0, 0 );
 	m_pMatchLine->SetSize( 2, m_iUpHeight );
 	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
 	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
 	m_pMatchLine->Render( iXPos - 6, iYPos + m_iUpYPosition );

	m_pMatchLine->SetPosition( 0, 0 );
	m_pMatchLine->SetSize( 4, 2 );
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos - 10, iYPos + m_iUpYPosition );
	
	//Down
	m_pMatchLine->SetPosition( 0, 0 );
	m_pMatchLine->SetSize( 2, m_iUpHeight - 2 );
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos - 6, iYPos + 13 );

	m_pMatchLine->SetPosition( 0, 0 );
	m_pMatchLine->SetSize( 4, 2 );
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos - 10, iYPos + 9 + m_iUpHeight );
}

void MatchBtn::onMatchActiveUpLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchActiveLine )
		return;

	//Center
	m_pMatchActiveLine->SetPosition( 0, 0 );	
	m_pMatchActiveLine->SetSize( 4, 2 );	
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 4, iYPos + 11 );

	//Up
	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 2, m_iUpHeight );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 6, iYPos + m_iUpYPosition );

	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 4, 2 );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 10, iYPos + m_iUpYPosition );
}

void MatchBtn::onMatchActiveDownLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchActiveLine )
		return;

	//Center
	m_pMatchActiveLine->SetPosition( 0, 0 );	
	m_pMatchActiveLine->SetSize( 4, 2 );	
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 4, iYPos + 11 );

	//Down
	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 2, m_iUpHeight );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 6, iYPos + 11 );

	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 4, 2 );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 10, iYPos + 9 + m_iUpHeight );
}

void MatchBtn::onMatchPrevPageLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchLine )
		return;

	m_pMatchLine->SetPosition( 0, 0 );
	m_pMatchLine->SetSize( 4, 2 );
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos - 4, iYPos + 11 );
}

void MatchBtn::onMatchPrevPageActiveLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchActiveLine )
		return;

	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 4, 2 );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos - 4, iYPos + 11 );
}

void MatchBtn::onMatchNextPageLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchLine )
		return;

	m_pMatchLine->SetPosition( 0, 0 );
	m_pMatchLine->SetSize( 4, 2 );
	m_pMatchLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchLine->Render( iXPos + 100, iYPos + 11 );
}

void MatchBtn::onMatchNextPageActiveLineRender( int iXPos, int iYPos )
{
	if( !m_pMatchLine )
		return;

	m_pMatchActiveLine->SetPosition( 0, 0 );
	m_pMatchActiveLine->SetSize( 4, 2 );
	m_pMatchActiveLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	m_pMatchActiveLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pMatchActiveLine->Render( iXPos + 100, iYPos + 11 );
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailScheduleFrameWnd::TournamentDetailScheduleFrameWnd()
{
	m_pFirstPartActiveLeft		= NULL;
	m_pFirstPartActiveCenter	= NULL;
	m_pFirstPartActiveRight		= NULL;
		
	m_pRestPartActiveCenter		= NULL;
	m_pRestPartActiveRight		= NULL;

	m_pFirstPartInctiveLeft		= NULL;
	m_pFirstPartInctiveCenter	= NULL;
	m_pFirstPartInctiveRight	= NULL;

	m_pRestPartInctiveCenter	= NULL;
	m_pRestPartInctiveRight		= NULL;

	m_pMatchLine		= NULL;
	m_pMatchLineActive	= NULL;

	m_pChampion		= NULL;
	m_pChampionBack	= NULL;

	m_dwRoundTeamDataSyncTimer = 0;

	m_iTournamentMaxRound		 = 0;
	m_iTournamentStartTeamCount = 0;
	
	m_iTournamentMaxVertPage = 0;
	m_iTournamentCurVertPage = 0;
}

TournamentDetailScheduleFrameWnd::~TournamentDetailScheduleFrameWnd()
{
	SAFEDELETE( m_pFirstPartActiveLeft );
	SAFEDELETE( m_pFirstPartActiveCenter );
	SAFEDELETE( m_pFirstPartActiveRight );
		
	SAFEDELETE( m_pRestPartActiveCenter );
	SAFEDELETE( m_pRestPartActiveRight );

	SAFEDELETE( m_pFirstPartInctiveLeft );
	SAFEDELETE( m_pFirstPartInctiveCenter );
	SAFEDELETE( m_pFirstPartInctiveRight );

	SAFEDELETE( m_pRestPartInctiveCenter );
	SAFEDELETE( m_pRestPartInctiveRight );

	SAFEDELETE( m_pMatchLine );
	SAFEDELETE( m_pMatchLineActive );
	
	SAFEDELETE( m_pChampion );
	SAFEDELETE( m_pChampionBack );
}

void TournamentDetailScheduleFrameWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "FirstPartActiveLeft" )
	{
		SAFEDELETE( m_pFirstPartActiveLeft );
		m_pFirstPartActiveLeft = pImage;
	}
	else if( szType == "FirstPartActiveCenter" )
	{
		SAFEDELETE( m_pFirstPartActiveCenter );
		m_pFirstPartActiveCenter = pImage;
	}
	else if( szType == "FirstPartActiveRight" )
	{
		SAFEDELETE( m_pFirstPartActiveRight );
		m_pFirstPartActiveRight = pImage;
	}
	else if( szType == "RestPartActiveCenter" )
	{
		SAFEDELETE( m_pRestPartActiveCenter );
		m_pRestPartActiveCenter = pImage;
	}
	else if( szType == "RestPartActiveRight" )
	{
		SAFEDELETE( m_pRestPartActiveRight );
		m_pRestPartActiveRight = pImage;
	}
	else if( szType == "FirstPartInctiveLeft" )
	{
		SAFEDELETE( m_pFirstPartInctiveLeft );
		m_pFirstPartInctiveLeft = pImage;
	}
	else if( szType == "FirstPartInctiveCenter" )
	{
		SAFEDELETE( m_pFirstPartInctiveCenter );
		m_pFirstPartInctiveCenter = pImage;
	}
	else if( szType == "FirstPartInctiveRight" )
	{
		SAFEDELETE( m_pFirstPartInctiveRight );
		m_pFirstPartInctiveRight = pImage;
	}
	else if( szType == "RestPartInctiveCenter" )
	{
		SAFEDELETE( m_pRestPartInctiveCenter );
		m_pRestPartInctiveCenter = pImage;
	}
	else if( szType == "RestPartInctiveRight" )
	{
		SAFEDELETE( m_pRestPartInctiveRight );
		m_pRestPartInctiveRight = pImage;
	}
	else if( szType == "MatchLine" )
	{
		SAFEDELETE( m_pMatchLine );
		m_pMatchLine = pImage;
	}
	else if( szType == "MatchLineActive" )
	{
		SAFEDELETE( m_pMatchLineActive );
		m_pMatchLineActive = pImage;
	}
	else if( szType == "Champion" )
	{
		SAFEDELETE( m_pChampion );
		m_pChampion = pImage;
	}
	else if( szType == "ChampionBack" )
	{
		SAFEDELETE( m_pChampionBack );
		m_pChampionBack = pImage;
	}
	else
	{
		TournamentDetailFrameWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailScheduleFrameWnd::ParseExtraInfo( ioXMLElement &xElement )
{		
	TournamentDetailFrameWnd::ParseExtraInfo( xElement );
}

void TournamentDetailScheduleFrameWnd::iwm_create()
{
	for( int i = ID_CHILD_BTN000001; i <=ID_CHILD_BTN000016; ++i )
	{
		MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd(i) );
		if( pButton )
		{
			SetChildWndStyleAdd( i, IWS_EXACTIVE );
			pButton->SetMatchLine( m_pMatchLine, m_pMatchLineActive, 13, 0 );
		}
	}
	
	for( int i = ID_CHILD_BTN00001; i <=ID_CHILD_BTN00008; ++i )
	{
		MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd(i) );
		if( pButton )
		{
			SetChildWndStyleAdd( i, IWS_EXACTIVE );
			pButton->SetMatchLine( m_pMatchLine, m_pMatchLineActive, 13, 0 );
		}
	}
	
	for( int i = ID_CHILD_BTN0001; i <=ID_CHILD_BTN0004; ++i )
	{
		MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd(i) );
		if( pButton )
		{
			SetChildWndStyleAdd( i, IWS_EXACTIVE );
			pButton->SetMatchLine( m_pMatchLine, m_pMatchLineActive, 25, -12 );
		}
	}
	
	for( int i = ID_CHILD_BTN001; i <=ID_CHILD_BTN002; ++i )
	{
		MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd(i) );
		if( pButton )
		{
			SetChildWndStyleAdd( i, IWS_EXACTIVE );
			pButton->SetMatchLine( m_pMatchLine, m_pMatchLineActive, 49, -36 );
		}
	}

	MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd(ID_CHILD_BTN01) );
	if( pButton )
	{
		SetChildWndStyleAdd( ID_CHILD_BTN01, IWS_EXACTIVE );
		pButton->SetMatchLine( m_pMatchLine, m_pMatchLineActive, 97, -84 );
	}
}

void TournamentDetailScheduleFrameWnd::iwm_show()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament )
	{
		m_iTournamentMaxRound		= pTournament->GetTournamentMaxRound();
		m_iTournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
	}
	
	m_iTournamentMaxHorzPage = max( 0, m_iTournamentMaxRound - 4 );

	//토너먼트 현재 페이지
	m_iTournamentCurHorzPage = 0;
	
	//횡페이지 구하기
	m_iTournamentCurVertPage = 0;
		
	//횡페이지 최대값 구하기
	SetVertMaxPage();
	SetParsingUserList();
}

void TournamentDetailScheduleFrameWnd::SetVertMaxPage()
{
	//횡페이지는 종페이지 기준으로 계산
	m_iTournamentMaxVertPage = Help::TournamentCurrentRoundWithTeam( m_iTournamentStartTeamCount, m_iTournamentCurHorzPage ) / TournamentHelp::PAGE_TEAM_MAX;
	m_iTournamentMaxVertPage = max( 1, m_iTournamentMaxVertPage );

	//스크롤 설정
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( m_iTournamentCurVertPage, m_iTournamentMaxVertPage );
		pScroll->SetScrollPos( m_iTournamentCurVertPage );
	}
}

void TournamentDetailScheduleFrameWnd::SetParsingUserList()
{
	SetRound();
	m_dwRoundTeamDataSyncTimer = FRAMEGETTIME();     // 1초뒤에 동기화 신호 전송
}

void TournamentDetailScheduleFrameWnd::SetRound()
{
	DWORD dwTeamCnt = TournamentHelp::PAGE_TEAM_MAX;

	//Button ID 계산
	DWORD dwMinID = 100000;
	DWORD dwMaxID = dwMinID + dwTeamCnt;
	DWORD dwValidID = (DWORD)pow( 10.0, (double)min( 4, m_iTournamentMaxRound ) ) * 10;
	DWORD dwRound = TournamentHelp::PAGE_ROUND_MIN;

	for( int iRound = TournamentHelp::PAGE_ROUND_MIN; iRound <= TournamentHelp::PAGE_ROUND_MAX; ++iRound )
	{
		if( GetNumberCount( dwMinID ) <= GetNumberCount( dwValidID ) )
		{
			int i = 1;
			for( DWORD ID = dwMinID; ID <= dwMaxID; ++ID, ++i )
			{
					
				DWORD dwPosition = m_iTournamentCurVertPage * dwTeamCnt + i;
				MatchBtn* pButton = dynamic_cast<MatchBtn*>( FindChildWnd(ID) );
				if( pButton )
					pButton->SetMatchData( m_dwTournamentIndex, m_iTournamentCurHorzPage, dwRound, dwPosition );
			}
			dwRound++;
		}
		else
		{
			for( DWORD ID = dwMinID; ID <= dwMaxID; ++ID )
			{
				MatchBtn* pButton = dynamic_cast<MatchBtn*>( FindChildWnd(ID) );
				if( pButton )
					pButton->HideWnd();
			}
		}
		
		//팀 갯수 계산
		dwTeamCnt = dwTeamCnt/2;

		//버튼 ID 계산
		dwMinID = dwMinID / 10;
		dwMaxID = dwMinID + dwTeamCnt;
	}	
}

void TournamentDetailScheduleFrameWnd::PrevSlide()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		m_iTournamentCurVertPage--;
		m_iTournamentCurVertPage = max( 0, m_iTournamentCurVertPage );

		pScroll->SetScrollPos( m_iTournamentCurVertPage );
		SetParsingUserList();
	}
}

void TournamentDetailScheduleFrameWnd::NextSlide()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{				
		m_iTournamentCurVertPage++;
		m_iTournamentCurVertPage = min( m_iTournamentCurVertPage, m_iTournamentMaxVertPage );

		pScroll->SetScrollPos( m_iTournamentCurVertPage );
		SetParsingUserList();
	}
}

void TournamentDetailScheduleFrameWnd::iwm_prevmenu()
{
	//다음 탭 메뉴에서 현재 페이지로 올때 호출되는 함수

	m_iTournamentCurHorzPage = m_iTournamentMaxHorzPage;
	m_iTournamentCurVertPage = 0;

	//횡페이지 최대값 구하기
	SetVertMaxPage();
	SetParsingUserList();
}

void TournamentDetailScheduleFrameWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )		
		PrevSlide();		
	else if( zDelta == -WHEEL_DELTA )
		NextSlide();
}

void TournamentDetailScheduleFrameWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	if( dwID == ID_VERT_SCROLL )
	{
		if( m_iTournamentCurVertPage != curPos )
		{
			m_iTournamentCurVertPage = curPos;
			SetParsingUserList();
		}		
	}
}

void TournamentDetailScheduleFrameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	
	bool bTabChange = false;

	switch( dwID )
	{
	case ID_PREV_BTN:
		if( cmd == IOBN_BTNUP )
		{			
			if( m_iTournamentCurHorzPage <= 0 )
			{
				bTabChange = true;
			}
			else
			{
				m_iTournamentCurHorzPage--;
				m_iTournamentCurVertPage = 0; //종 이동하면 횡이동 초기화

				SetVertMaxPage();
				SetParsingUserList();
			}
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			//결승전이 보이면 다른탭으로 이동
			if( m_iTournamentMaxRound - TournamentHelp::PAGE_ROUND_MAX < m_iTournamentCurHorzPage )
			{
				bTabChange = true;
			}
			else
			{
				m_iTournamentCurHorzPage++;
				m_iTournamentCurVertPage = 0; //종 이동하면 횡이동 초기화
				
				SetVertMaxPage();
				SetParsingUserList();
			}		
		}
		break;
	default:
		{
			if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
			{
				round_command( pWnd, cmd, param );
			}
			break;
		}
		break;
	}
	

	if( bTabChange )
	{
		if( cmd == IOBN_BTNUP )
		{
			TournamentDetailFrameWnd::iwm_command( pWnd, cmd, param );
		}
	}
}

void TournamentDetailScheduleFrameWnd::round_command( ioWnd *pWnd, int cmd, DWORD param )
{
	MatchBtn* pButton = dynamic_cast< MatchBtn * >( FindChildWnd( pWnd->GetID() ) );
	if( !pButton )
		return;

	DWORD dwID = pButton->GetID();
	
	if( cmd == IOEX_BTNUP )
	{	
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "존재 하지 않는 팀입니다." );
	}
	else
	{
		DWORD dwTeamIdx = pButton->GetTeamIndex();
		if( dwTeamIdx != 0 )
		{
			TournamentTeamInfoWnd *pTournamentTeam = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
			if( pTournamentTeam )
			{
				pTournamentTeam->SetTournamentTeam( m_dwTournamentIndex, dwTeamIdx );
			}
		}
	}
}

void TournamentDetailScheduleFrameWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	
	TournamentDetailFrameWnd::OnProcess( fTimePerSec );

	OnRoundDataSyncProcess();
}

void TournamentDetailScheduleFrameWnd::OnRoundDataSyncProcess()
{
	if( FRAMEGETTIME() - m_dwRoundTeamDataSyncTimer > 1000 )
	{
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
		if( pTournament )
		{
			int iStartRound = m_iTournamentCurHorzPage + 1;
			int iStartRoundTeam = min( TournamentHelp::PAGE_TEAM_MAX, m_iTournamentStartTeamCount );
			int iStartRoundTeamPage = m_iTournamentCurVertPage;
			
			if( !pTournament->IsRoundTeamDataSyncCheck( iStartRound, TournamentHelp::PAGE_ROUND_MAX, iStartRoundTeam, iStartRoundTeamPage ) )
			{
				// 동기화가 안되었다.
				pTournament->SendRoundTeamDataSync( iStartRound, TournamentHelp::PAGE_ROUND_MAX, iStartRoundTeam, iStartRoundTeamPage );
			}

			// 동기화되었으면
			if( pTournament->IsUpdateRoundTeamData( iStartRound, TournamentHelp::PAGE_ROUND_MAX, iStartRoundTeam, iStartRoundTeamPage ) )
			{
				SetParsingUserList();
			}
		}
	}
}

void TournamentDetailScheduleFrameWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnActiveTypeRender( iXPos, iYPos );
	OnPartyRender( iXPos, iYPos );
	OnChampionRender( iXPos, iYPos );
	OnRoundTextRender( iXPos, iYPos );

}

void TournamentDetailScheduleFrameWnd::OnActiveTypeRender( int iXPos, int iYPos )
{
	if(	!m_pActiveTournamentCenterClose		||
		!m_pActiveTournamentRightClose		||
		!m_pActiveAwardCenterClose			||
		!m_pActiveAwardRightClose			||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveClose	||
		!m_pScheduleProgressInactiveRight )
		return;
	
	//현재 페이지가 결승전이 보이는 페이지가 아닐때
	if( m_iTournamentCurHorzPage < m_iTournamentMaxRound - TournamentHelp::PAGE_ROUND_MAX )
	{
		if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT && 
			max( 0 , m_iTournamentCurHorzPage + TournamentHelp::PAGE_ROUND_MAX - 1) < m_TournamentState - TournamentNode::STATE_TOURNAMENT )
		{
			m_pActiveTournamentCenterClose->Render( iXPos, iYPos );
			m_pActiveTournamentRightClose->Render( iXPos, iYPos );
		}
		else
		{
			m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
			m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
			m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
		}
	}
	else
	{
		if( m_TournamentState == TournamentNode::STATE_WAITING )
		{
			m_pActiveAwardCenterClose->Render( iXPos, iYPos );
			m_pActiveAwardRightClose->Render( iXPos, iYPos );
		}
		else
		{
			m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
			m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
			m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
		}
	}
}

void TournamentDetailScheduleFrameWnd::OnPartyRender( int iXPos, int iYPos )
{
	if(	!m_pFirstPartInctiveLeft	||
		!m_pFirstPartInctiveCenter	||
		!m_pFirstPartInctiveRight	||
		!m_pRestPartInctiveCenter	||
		!m_pRestPartInctiveRight	||
		!m_pFirstPartActiveLeft		||
		!m_pFirstPartActiveCenter	||
		!m_pFirstPartActiveRight	||
		!m_pRestPartActiveCenter	||
		!m_pRestPartActiveRight )
		return;
		
	int iStartPos = max( 0, (TournamentHelp::PAGE_ROUND_MAX-1) - m_iTournamentMaxRound );
	int iTourPos  = m_TournamentState - TournamentNode::STATE_TOURNAMENT + 1 + iStartPos;
	
	int iOffset = 330;
	for( int i = TournamentHelp::PAGE_ROUND_MAX; 0 < i; --i )
	{
		if( i == TournamentHelp::PAGE_ROUND_MIN )
		{
			if( m_iTournamentCurHorzPage + i == iTourPos && m_TournamentState >= TournamentNode::STATE_TOURNAMENT )
			{
				m_pFirstPartActiveLeft->Render( iXPos, iYPos );
				m_pFirstPartActiveCenter->Render( iXPos, iYPos );
				m_pFirstPartActiveRight->Render( iXPos, iYPos );
			}
			else
			{
				m_pFirstPartInctiveLeft->Render( iXPos, iYPos );
				m_pFirstPartInctiveCenter->Render( iXPos, iYPos );
				m_pFirstPartInctiveRight->Render( iXPos, iYPos );
			}
		}
		else
		{
			if( m_iTournamentCurHorzPage + i == iTourPos && m_TournamentState >= TournamentNode::STATE_TOURNAMENT)
			{
				m_pRestPartActiveCenter->Render( iXPos + iOffset, iYPos );
				m_pRestPartActiveRight->Render( iXPos + iOffset, iYPos );
			}
			else
			{
				m_pRestPartInctiveCenter->Render( iXPos + iOffset, iYPos );
				m_pRestPartInctiveRight->Render( iXPos + iOffset, iYPos );
			}
		}
		iOffset -= 110;
	}
}

void TournamentDetailScheduleFrameWnd::OnRoundTextRender( int iXPos, int iYPos )
{
	int iTourPos = m_iTournamentCurHorzPage;	
	int xOffset = 0;
	char szSrc[MAX_PATH] = "";
	char szDst[MAX_PATH] = "";
	
	int iStart = 0;
	for( int i = 0; i < TournamentHelp::PAGE_ROUND_MAX; ++i )
	{		
		if( max(0, (TournamentHelp::PAGE_ROUND_MAX-1) - m_iTournamentMaxRound ) <= i )
		{
			int iRound = Help::TournamentCurrentRoundWithTeam( m_iTournamentStartTeamCount, iTourPos );			
			DWORD dwEndDate = 0;
			char szEndDate[MAX_PATH] = "";

			TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
			if( pTournament )
			{
				dwEndDate = pTournament->GetStateEndDate( TournamentNode::STATE_TOURNAMENT + m_iTournamentCurHorzPage + iStart );
				sprintf_s_e( szSrc, "%d/%d(%s)", DateHelp::GetDateMonth( dwEndDate ), DateHelp::GetDateDay( dwEndDate ), DateHelp::GetWeekDayString( dwEndDate ) );

				if( strcmp( szSrc, szDst ) == 0 )
				{

					sprintf_s_e( szEndDate, "%02d:%02d", DateHelp::GetDateHour( dwEndDate ), DateHelp::GetDateMinute( dwEndDate ) );
				}
				else
				{

					sprintf_s_e( szEndDate, "%s %02d:%02d", szSrc, DateHelp::GetDateHour( dwEndDate ), DateHelp::GetDateMinute( dwEndDate ) );
					sprintf_s( szDst, szSrc );
				}
				
				iStart++;
			}

			ioComplexStringPrinter kPrinter1, kPrinter2;
			kPrinter1.SetTextStyle( TS_NORMAL );
			kPrinter1.SetBkColor( 0, 0, 0 );
			kPrinter1.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter2.SetTextStyle( TS_NORMAL );
			kPrinter2.SetBkColor( 0, 0, 0 );
			kPrinter2.SetTextColor( TCT_DEFAULT_WHITE );

			if( iRound == 2 )
			{
				if( dwEndDate == 0 )
				{
					kPrinter1.AddTextPiece( FONT_SIZE_11, "주최자 수동진행" );
					kPrinter2.AddTextPiece( FONT_SIZE_11, "결승", iRound );
				}
				else
				{
					kPrinter1.AddTextPiece( FONT_SIZE_11, szEndDate );
					kPrinter2.AddTextPiece( FONT_SIZE_11, "결승", iRound );
				}
			}
			else if( iRound == 1 )
			{
				kPrinter2.AddTextPiece( FONT_SIZE_11, "대회 우승", iRound );
			}
			else
			{
				if( dwEndDate == 0 )
				{
					kPrinter1.AddTextPiece( FONT_SIZE_11, "주최자 수동진행" );
					kPrinter2.AddTextPiece( FONT_SIZE_11, "%d강", iRound );
				}
				else
				{
					kPrinter1.AddTextPiece( FONT_SIZE_11, szEndDate );
					kPrinter2.AddTextPiece( FONT_SIZE_11, "%d강", iRound );
				}
			}

			if( kPrinter1.IsEmpty() )
			{
				kPrinter2.PrintFullText( iXPos + 79 + xOffset, iYPos + 20 + 10, TAT_CENTER );
			}
			else
			{
				kPrinter1.PrintFullText( iXPos + 79 + xOffset, iYPos + 13 + 10, TAT_CENTER );
				kPrinter2.PrintFullText( iXPos + 79 + xOffset, iYPos + 25 + 10, TAT_CENTER );
			}
			
			iTourPos++;
		}		
		xOffset += 110;
	}
}

void TournamentDetailScheduleFrameWnd::OnChampionRender( int iXPos, int iYPos )
{
	if( !m_pChampion || !m_pChampionBack )
	   return;

		
	if( m_iTournamentMaxRound - TournamentHelp::PAGE_ROUND_MAX < m_iTournamentCurHorzPage )
	{
		m_pChampionBack->Render( iXPos, iYPos );
		m_pChampion->Render( iXPos, iYPos );
	}	
}

//////////////////////////////////////////////////////////////////////////
TournamentDetailFrameAwardWnd::TournamentDetailFrameAwardWnd()
{
	m_TournamentType = true;
}

TournamentDetailFrameAwardWnd::~TournamentDetailFrameAwardWnd()
{
}

void TournamentDetailFrameAwardWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/detailinnerrewardwnd.xml" , this );
	g_GUIMgr.AddWnd( "XML/detailinnercamprewardwnd.xml" , this );
	g_GUIMgr.AddWnd( "XML/detailinnercheerreward.xml" , this );
	g_GUIMgr.AddWnd( "XML/detailinnercustomreward.xml" , this );
}

void TournamentDetailFrameAwardWnd::iwm_show()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameAwardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	TournamentDetailFrameWnd::OnProcess( fTimePerSec );
}

void TournamentDetailFrameAwardWnd::SetTournamentData( DWORD dwTourIndex )
{
	TournamentDetailFrameWnd::SetTournamentData( dwTourIndex );

	HideChildWnd( ID_INNER_TOURNAMENT_REAWARD );
	HideChildWnd( ID_INNER_CAMP_REAWARD );
	HideChildWnd( ID_INNER_CHEER_REAWARD );
	HideChildWnd( ID_INNER_CUSTOM_REAWARD );
	
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( dwTourIndex );
	m_TournamentType = pTournament->GetType();

	if( pTournament && pTournament->GetType() == TournamentNode::TYPE_CUSTOM )
	{
		ShowChildWnd( ID_INNER_CUSTOM_REAWARD );		

		TournamentDetailInnerCustomWnd *pInnerWnd = dynamic_cast< TournamentDetailInnerCustomWnd * >( FindChildWnd( ID_INNER_CUSTOM_REAWARD ) ); 
		if( pInnerWnd )
		{
			pInnerWnd->SetTournamentData( dwTourIndex );
		}
	}
	else
	{		
		ShowChildWnd( ID_INNER_TOURNAMENT_REAWARD );
		ShowChildWnd( ID_INNER_CAMP_REAWARD );
		ShowChildWnd( ID_INNER_CHEER_REAWARD );

		TournamentDetailInnerRewardWnd *pRewardWnd = dynamic_cast< TournamentDetailInnerRewardWnd * >( FindChildWnd( ID_INNER_TOURNAMENT_REAWARD ) ); 
		if( pRewardWnd )
		{
			pRewardWnd->SetTournamentData( dwTourIndex );
		}
		TournamentDetailInnerCampRewardWnd *pCampWnd = dynamic_cast< TournamentDetailInnerCampRewardWnd * >( FindChildWnd( ID_INNER_CAMP_REAWARD ) ); 
		if( pCampWnd )
		{
			pCampWnd->SetTournamentData( dwTourIndex );
		}
		TournamentDetailInnerCheerWnd *pInnerWnd = dynamic_cast< TournamentDetailInnerCheerWnd * >( FindChildWnd( ID_INNER_CHEER_REAWARD ) ); 
		if( pInnerWnd )
		{
			pInnerWnd->SetCheerData( dwTourIndex );
		}
	}
}

void TournamentDetailFrameAwardWnd::UpdateMainUI()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameAwardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnActiveTypeRender( iXPos, iYPos );
	OnActiveRender( iXPos, iYPos );	
	OnRewardRender( iXPos, iYPos );

	switch( m_TournamentType )
	{
	case TournamentNode::TYPE_REGULAR:
		{
			OnDevideWndDotRender( iXPos, iYPos );
			OnRegularMentRender( iXPos, iYPos );
		}
		break;
	default:
		OnCusotmMentRender( iXPos, iYPos );
		break;
	}
}

void TournamentDetailFrameAwardWnd::OnActiveTypeRender( int iXPos, int iYPos )
{
	if( !m_pActiveAwardCenterClose			||
		!m_pActiveAwardRightClose			||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveClose	||
		!m_pScheduleProgressInactiveRight ) 
		return;
	
	//대회포상대기기간 또는 유휴기간
	if( m_TournamentState == TournamentNode::STATE_WAITING )
	{
		m_pActiveAwardCenterClose->Render( iXPos, iYPos );
		m_pActiveAwardRightClose->Render( iXPos, iYPos );
	}
	else
	{
		m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
	}	
}

void TournamentDetailFrameAwardWnd::OnActiveRender( int iXPos, int iYPos )
{
	if( !m_pScheduleProgressActiveLeft	 ||
		!m_pScheduleProgressActiveCenter ||
		!m_pScheduleProgressActiveRight )
		return;
	
	if(	TournamentNode::STATE_WAITING == m_TournamentState )
	{
		m_pScheduleProgressActiveLeft->Render( iXPos, iYPos );
		m_pScheduleProgressActiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressActiveRight->Render( iXPos, iYPos );
	}
	else
	{
		OnInactiveNotCloseRender( iXPos, iYPos );
	}
}

void TournamentDetailFrameAwardWnd::OnRegularMentRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "정규대회 포상은 " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "포상기간에 접속" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "하시면 획득하실 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 308, iYPos + 416, TAT_CENTER );
}

void TournamentDetailFrameAwardWnd::OnCusotmMentRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "대회포상은 " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "포상기간에 접속" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "하시면 획득하실 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 308, iYPos + 416, TAT_CENTER );
}

void TournamentDetailFrameAwardWnd::OnRewardRender( int iXPos, int iYPos )
{
	//포상 기간
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) 
		return;

	char szPeriod[MAX_PATH] = "";

	DWORD dwStartYear = DateHelp::GetDateYear( pTournament->GetEndDate() );	
	DWORD dwStartMonth = DateHelp::GetDateMonth( pTournament->GetEndDate() );
	DWORD dwStartDay = DateHelp::GetDateDay( pTournament->GetEndDate() );	
	ioHashString szStartDayOfWeek = DateHelp::GetWeekDayString( pTournament->GetEndDate() );

	time_t tStart = DateHelp::ConvertSecondTime( dwStartYear, dwStartMonth, dwStartDay, 0, 0, 0 );
	enum { DAY_SEC = 86400 };

	time_t tEnd = tStart + ( DAY_SEC * 14 );
	struct tm kDate = *localtime( &tEnd );

	sprintf_s_e( szPeriod, "%d/%d(%s) ~ %d/%d(%s)", dwStartMonth,
		dwStartDay,
		szStartDayOfWeek.c_str(),
		kDate.tm_mon+1,
		kDate.tm_mday,
		DateHelp::GetWeekDayString( kDate.tm_wday ) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	g_FontMgr.PrintText( iXPos + 308, iYPos + 23, FONT_SIZE_11, "%s", szPeriod );
	g_FontMgr.PrintText( iXPos + 308, iYPos + 35, FONT_SIZE_11, "포상기간" );	
}

//////////////////////////////////////////////////////////////////////////
TournamentDetailFrameAllocateWnd::TournamentDetailFrameAllocateWnd()
{
	m_TournamentType = true;
}

TournamentDetailFrameAllocateWnd::~TournamentDetailFrameAllocateWnd()
{
}

void TournamentDetailFrameAllocateWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/detailinnerallocatereward.xml" , this );
}

void TournamentDetailFrameAllocateWnd::iwm_show()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameAllocateWnd::iwm_hide()
{
	TournamentDetailInnerAllocateWnd *pWnd = dynamic_cast< TournamentDetailInnerAllocateWnd * >( FindChildWnd( ID_INNER_WND ) );
	if( pWnd )
		pWnd->HideWnd();
}

void TournamentDetailFrameAllocateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	TournamentDetailFrameWnd::OnProcess( fTimePerSec );
}

void TournamentDetailFrameAllocateWnd::SetTournamentData( DWORD dwTourIndex )
{
	TournamentDetailFrameWnd::SetTournamentData( dwTourIndex );

	TournamentNode* pTournament = g_TournamentMgr.GetTournament( dwTourIndex );
	if( !pTournament ) return;

	m_TournamentType = pTournament->GetType();
 	TournamentDetailInnerAllocateWnd *pWnd = dynamic_cast< TournamentDetailInnerAllocateWnd * >( FindChildWnd( ID_INNER_WND ) );
 	if( pWnd )
 		pWnd->SetTournamentData( dwTourIndex );
}

void TournamentDetailFrameAllocateWnd::UpdateMainUI()
{
	SetTournamentData( m_dwTournamentIndex );
}

void TournamentDetailFrameAllocateWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnActiveTypeRender( iXPos, iYPos );
	OnActiveRender( iXPos, iYPos );	
	OnAllocateRender( iXPos, iYPos );
	OnMentRender( iXPos, iYPos );
}

void TournamentDetailFrameAllocateWnd::OnActiveTypeRender( int iXPos, int iYPos )
{
	if(	!m_pActiveTournamentCenterClose		||
		!m_pActiveTournamentRightClose		||
		!m_pActiveAwardCenterClose			||
		!m_pActiveAwardRightClose			||
		!m_pScheduleProgressInactiveCenter	||
		!m_pScheduleProgressInactiveClose	||
		!m_pScheduleProgressInactiveRight )
		return;

	//대회기간
	if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT )
	{
		m_pActiveTournamentCenterClose->Render( iXPos, iYPos );
		m_pActiveTournamentRightClose->Render( iXPos, iYPos );
	}
	//대회포상대기기간 또는 유휴기간
	else if( m_TournamentState == TournamentNode::STATE_WAITING )
	{
		m_pActiveAwardCenterClose->Render( iXPos, iYPos );
		m_pActiveAwardRightClose->Render( iXPos, iYPos );
	}
	else
	{
		m_pScheduleProgressInactiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveClose->Render( iXPos, iYPos );
		m_pScheduleProgressInactiveRight->Render( iXPos, iYPos );
	}
}

void TournamentDetailFrameAllocateWnd::OnActiveRender( int iXPos, int iYPos )
{
	if( !m_pScheduleProgressActiveLeft	 ||
		!m_pScheduleProgressActiveCenter ||
		!m_pScheduleProgressActiveRight )
		return;

	if( TournamentNode::STATE_TEAM_DELAY == m_TournamentState )
	{
		m_pScheduleProgressActiveLeft->Render( iXPos, iYPos );
		m_pScheduleProgressActiveCenter->Render( iXPos, iYPos );
		m_pScheduleProgressActiveRight->Render( iXPos, iYPos );
	}
	else
	{
		OnInactiveNotCloseRender( iXPos, iYPos );
	}
}

void TournamentDetailFrameAllocateWnd::OnMentRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "대진배정이 완료되면 " );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "대회일정" );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "에 팀이 배치됩니다" );
	kPrinter.PrintFullText( iXPos + 308, iYPos + 416, TAT_CENTER );
}

void TournamentDetailFrameAllocateWnd::OnAllocateRender( int iXPos, int iYPos )
{
	//대진배정
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) 
		return;

	char szPeriod[MAX_PATH] = "";

	DWORD dwStartYear				= DateHelp::GetDateYear( pTournament->GetEndDate() );	
	DWORD dwStartMonth				= DateHelp::GetDateMonth( pTournament->GetEndDate() );
	DWORD dwStartDay				= DateHelp::GetDateDay( pTournament->GetEndDate() );
	ioHashString szStartDayOfWeek	= DateHelp::GetWeekDayString( pTournament->GetEndDate() );

	time_t tStart = DateHelp::ConvertSecondTime( dwStartYear, dwStartMonth, dwStartDay, 0, 0, 0 );
	enum { DAY_SEC = 86400 };

	time_t tEnd = tStart + ( DAY_SEC * 14 );
	struct tm kDate = *localtime( &tEnd );

	sprintf_s_e( szPeriod, "%d/%d(%s) ~ %d/%d(%s)", dwStartMonth,
		dwStartDay,
		szStartDayOfWeek.c_str(),
		kDate.tm_mon+1,
		kDate.tm_mday,
		DateHelp::GetWeekDayString( kDate.tm_wday ) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	g_FontMgr.PrintText( iXPos + 308, iYPos + 23, FONT_SIZE_11, "%s", szPeriod );
	g_FontMgr.PrintText( iXPos + 308, iYPos + 35, FONT_SIZE_11, "대진배정기간" );	
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerRewardWnd::TournamentDetailInnerRewardWnd()
{
	m_pDevideDot = NULL;

	m_pPresentBack = NULL;
	m_pPresentNone = NULL;

	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;

	m_pFinishWin	= NULL;

	m_dwCampPos = CAMP_NONE;

	m_TournamentTeamPos = 0;
	m_TournamentPos     = 0;
}

TournamentDetailInnerRewardWnd::~TournamentDetailInnerRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );	
	SAFEDELETE( m_pFinishWin );
}

void TournamentDetailInnerRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "FinishWin" )
	{
		SAFEDELETE( m_pFinishWin );
		m_pFinishWin = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );

}

void TournamentDetailInnerRewardWnd::SetTournamentData( DWORD dwTourIndex )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTourIndex );
	if( pTournament == NULL ) return;
	
	m_TournamentState = pTournament->GetState();
	m_dwTournamentMaxRound = pTournament->GetTournamentMaxRound();
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();

	if( pUserTournament )
	{
		DWORD dwTeamIndex = pUserTournament->GetTeamIndex( dwTourIndex );
		TeamGlobalData *pGlobalTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIndex );
		if( pGlobalTeamData )
		{
			m_TournamentPos		= pGlobalTeamData->GetTourPos();
			m_dwCampPos			= pGlobalTeamData->GetCampPos();
			m_TournamentTeamPos = pGlobalTeamData->GetPosition();

		}
		else
		{
			m_TournamentPos = 0;
			m_dwCampPos = CAMP_NONE;
		}
		
	}
	else
	{
		m_TournamentPos = 0;
		m_dwCampPos = CAMP_NONE;
	}
		
	m_szResultTtitle = pTournament->GetResultRewardTitle( m_TournamentPos );
	m_vReulstTextList = pTournament->GetResultRewardTextList( m_TournamentPos );
	m_vIconList = pTournament->GetResultRewardIconList( m_TournamentPos );
}

void TournamentDetailInnerRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentDetailInnerRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentDetailInnerRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	OnDevideDotRender( iXPos, iYPos );

	//대회기간이지만 탈락 했을떄
	if( m_TournamentState >= TournamentNode::STATE_TOURNAMENT && 0 < m_TournamentPos &&
		m_TournamentPos < (DWORD)(( m_TournamentState - TournamentNode::STATE_TOURNAMENT ) + 1))
	{
		OnBackRender( iXPos, iYPos );
		OnRewardIconRender( iXPos, iYPos );
		OnTournamentRewardResultRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos );
	}
	//포상기간일때
	else if( TournamentNode::STATE_WAITING == m_TournamentState )
	{
		if( 0 < m_TournamentPos )
		{
			//대회에 참가한 경우
			OnBackRender( iXPos, iYPos );
			OnRewardIconRender( iXPos, iYPos );
			OnTournamentRewardResultRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos );
		}
		else
		{
			//참가하지 않은 경우
			OnBackRender( iXPos, iYPos, false );
			OnRewardIconRender( iXPos, iYPos );
			OnTournamentRewardNoneRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos, false );
		}		
	}
	else
	{
		OnBackRender( iXPos, iYPos, false );
		OnRewardIconRender( iXPos, iYPos );
		OnTournamentNoneRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
	}
}

void TournamentDetailInnerRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;
		
	for( DWORD i =0; i < m_dwPresentBackCnt; ++i )
	{
		ioUIRenderImage* pImage = NULL;
		if( bReward && i < m_vIconList.size() )
			pImage = m_vIconList[i];
		
		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			
			pImage->SetPosition( 30, 320 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentDetailInnerRewardWnd::OnRewardIconRender( int iXPos, int iYPos )
{
	if( !m_pFinishWin )
		return;

	m_pFinishWin->Render( iXPos, iYPos );
}

void TournamentDetailInnerRewardWnd::OnTournamentRewardResultRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
		
	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}
	
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%s", m_szResultTtitle.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "대회성적 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
	
	int iOffset = 0;
	ioHashStringVec::iterator iter = m_vReulstTextList.begin();
	for( ; iter != m_vReulstTextList.end(); ++iter  )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s", (*iter).c_str() );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 221 + iOffset, TAT_CENTER );
		kPrinter.ClearList();
		iOffset += 18;
	}	
}

void TournamentDetailInnerRewardWnd::OnTournamentRewardNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "대회에" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "참가하지 않음" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 팀 소속의 각 개인당" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerRewardWnd::OnTournamentNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "?????" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "대회성적 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerCampRewardWnd::TournamentDetailInnerCampRewardWnd()
{
	m_pDevideDot = NULL;

	m_pPresentBack = NULL;
	m_pPresentNone = NULL;

	m_pCampNoneIcon = NULL;
	m_pCampIcon		= NULL;
	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;
	
	m_dwCampPos = CAMP_NONE;
	m_dwCampResult = CAMP_DRAW;
}

TournamentDetailInnerCampRewardWnd::~TournamentDetailInnerCampRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );

	SAFEDELETE( m_pCampNoneIcon );
	SAFEDELETE( m_pCampIcon );
}

void TournamentDetailInnerCampRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CampNoneIcon" )
	{
		SAFEDELETE( m_pCampNoneIcon );
		m_pCampNoneIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerCampRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );

}

void TournamentDetailInnerCampRewardWnd::SetTournamentData( DWORD dwTourIndex )
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( pTournament == NULL ) return;

	m_TournamentState = pTournament->GetState();

	enum
	{
		BLUE_CAMP_WIN  = 1,
		BLUE_CAMP_LOSE = 2,
		RED_CAMP_WIN   = 3,
		RED_CAMP_LOSE  = 4,
		CAMP_DRAW      = 5,
	};
	
	m_dwChampCampPos = pTournament->GetWinTeamCamp();
	m_dwCampPos      = g_MyInfo.GetUserCampPos();		

	m_dwCampResult = CAMP_DRAW;
	if( m_dwCampPos == CAMP_BLUE )
	{
		if( m_dwChampCampPos == CAMP_BLUE )
		{
			m_dwCampResult = BLUE_CAMP_WIN;
		}
		else if( m_dwChampCampPos == CAMP_RED )
		{
			m_dwCampResult = BLUE_CAMP_LOSE;			
		}
	}
	else if( m_dwCampPos == CAMP_RED )
	{
		if( m_dwChampCampPos == CAMP_RED )
		{
			m_dwCampResult = RED_CAMP_WIN;
		}
		else if( m_dwChampCampPos == CAMP_BLUE )
		{
			m_dwCampResult = RED_CAMP_LOSE;			
		}
	}
	else
		return;

	m_pCampIcon		 = pTournament->GetCampIcon( m_dwCampPos );	
	m_szResultTtitle = pTournament->GetResultCampTitle( m_dwCampResult );
	m_vReulstTextList = pTournament->GetResultCampTextList( m_dwCampResult );
	m_vIconList = pTournament->GetResultCampIconList( m_dwCampResult );
}

void TournamentDetailInnerCampRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void TournamentDetailInnerCampRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );

	//포상기간일때
	if(	TournamentNode::STATE_WAITING == m_TournamentState )
	{
		if( m_dwCampPos != CAMP_NONE )
		{
			//진영전에 참가한 경우
			OnBackRender( iXPos, iYPos );
			OnCampIconRender( iXPos, iYPos );
			OnCampRewardResultRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos );
		}
		else
		{
			//참가하지 않은 경우
			OnBackRender( iXPos, iYPos, false );
			OnCampIconRender( iXPos, iYPos );
			OnCampRewardNoneRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos, false );
		}		
	}
	else
	{
		OnBackRender( iXPos, iYPos, false );
		OnCampIconRender( iXPos, iYPos );
		OnCampNoneRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
	}
}

void TournamentDetailInnerCampRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerCampRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerCampRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;

	for( DWORD i =0; i < m_dwPresentBackCnt; ++i )
	{
		ioUIRenderImage* pImage = NULL;
		if( bReward && i < m_vIconList.size() )
			pImage = m_vIconList[i];

		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			pImage->SetPosition( 30, 320 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentDetailInnerCampRewardWnd::OnCampIconRender( int iXPos, int iYPos )
{	
	if( m_dwCampPos != CAMP_NONE )
	{
		if( m_pCampIcon )
		{
			m_pCampIcon->SetScale( FLOAT1 );
			m_pCampIcon->Render( iXPos + 93, iYPos + 167 );
		}
	}
	else
	{
		if( m_pCampNoneIcon )
			m_pCampNoneIcon->Render( iXPos, iYPos );
	}	
}

void TournamentDetailInnerCampRewardWnd::OnCampRewardResultRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}

	g_FontMgr.PrintTextWidthCut( iXPos + 93, iYPos + 15, FONT_SIZE_24, 178.0f, "%s", m_szResultTtitle.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( m_dwCampResult == BLUE_CAMP_WIN || m_dwCampResult == RED_CAMP_WIN )
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "승리 보상" );
	else
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "패배 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "보유한 대회포인트에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	int iOffset = 0;
	ioHashStringVec::iterator iter = m_vReulstTextList.begin();
	for( ; iter != m_vReulstTextList.end(); ++iter  )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s", (*iter).c_str() );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 221 + iOffset, TAT_CENTER );
		kPrinter.ClearList();
		iOffset += 18;
	}
}

void TournamentDetailInnerCampRewardWnd::OnCampRewardNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "진영전에" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "참가하지 않음" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "보유한 대회포인트에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerCampRewardWnd::OnCampNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "?????" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "진영 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "보유한 대회포인트에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerCheerWnd::TournamentDetailInnerCheerWnd()
{
	m_pDevideDot   = NULL;
	m_pPresentBack = NULL;
	m_pPresentNone = NULL;
	m_pPresentPeso = NULL;

	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;
	m_pCheerReward	= NULL;

	m_dwCampPos = CAMP_NONE;

	m_dwCheerPeso	= 0;
	m_bChampionTeam = false;
	m_bCheer		= false;
}

TournamentDetailInnerCheerWnd::~TournamentDetailInnerCheerWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );
	SAFEDELETE( m_pPresentPeso );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pCheerReward );
}

void TournamentDetailInnerCheerWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "PresentPeso" )
	{
		SAFEDELETE( m_pPresentPeso );
		m_pPresentPeso = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CheerReward" )
	{
		SAFEDELETE( m_pCheerReward );
		m_pCheerReward = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerCheerWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );
}

void TournamentDetailInnerCheerWnd::SetCheerData( DWORD dwTourIndex )
{
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament == NULL )
		return;

	m_dwTournamentIndex	= dwTourIndex;
	const CheerTeamInfo& CheerInfo = pUserTournament->GetCheerTeam( dwTourIndex );

	TeamGlobalData* pTeamData = g_TournamentTeamMgr.GetTeamData( CheerInfo.m_dwTeamIndex, false );
	if( !pTeamData )
	{		
		g_TournamentTeamMgr.NewTeamGlobalData( m_dwTournamentIndex, CheerInfo.m_dwTeamIndex );
	}	
}

void TournamentDetailInnerCheerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentDetailInnerCheerWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	CalcCheerPoint();
}

void TournamentDetailInnerCheerWnd::CalcCheerPoint()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) 
		return;

	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament == NULL )
		return;

	m_TournamentState = pTournament->GetState();
	m_dwCheerPeso = 0;
	m_szTeamName = "";
	m_bChampionTeam = false;
	m_bCheer		= false;

	if( TournamentNode::STATE_WAITING != m_TournamentState )  
		return;

	const CheerTeamInfo& CheerInfo = pUserTournament->GetCheerTeam( m_dwTournamentIndex );
	TeamGlobalData* pTeamData = g_TournamentTeamMgr.GetTeamData( CheerInfo.m_dwTeamIndex, false );
	
	if( pTeamData )
	{
		int iCheerPoint			= pTeamData->GetCheerPoint();
		DWORD dwTotalCheerCount = pTournament->GetTotalCheerCount();
		DWORD dwStartTeamCount  = pTournament->GetTournamentStartTeamCount();
		m_bCheer = true;

		if( pTeamData->GetTourPos() == pTournament->GetTournamentMaxRound() + 1 )
		{
			m_bChampionTeam = true;
		}

		m_szTeamName  = pTeamData->GetTeamName();

		if( m_bChampionTeam && 0 < dwTotalCheerCount && 0 < dwStartTeamCount && 0 < iCheerPoint )
		{
			
			float fPeso   = ((float)dwTotalCheerCount / (float)dwStartTeamCount) / (float)iCheerPoint;
			m_dwCheerPeso = fPeso * pTournament->GetAdjustCheerPeso();
			m_dwCheerPeso = max( 1000, m_dwCheerPeso);
			m_dwCheerPeso = min( 100000, m_dwCheerPeso );
		}
	}
}

void TournamentDetailInnerCheerWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );

	//포상기간일때
	if(	TournamentNode::STATE_WAITING == m_TournamentState )
	{
		if( m_bCheer )
		{
			//대회에 참가한 경우
			OnBackRender( iXPos, iYPos );
			OnCheerIconRender( iXPos, iYPos );
			OnCheerRewardResultRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos );
		}
		else
		{
			//참가하지 않은 경우
			OnBackRender( iXPos, iYPos, false );
			OnCheerIconRender( iXPos, iYPos );
			OnCheerRewardNoneRender( iXPos, iYPos );
			OnPresentRender( iXPos, iYPos, false );
		}		
	}
	else
	{
		OnBackRender( iXPos, iYPos, false );
		OnCheerIconRender( iXPos, iYPos );
		OnCheerNoneRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
	}
}

void TournamentDetailInnerCheerWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerCheerWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerCheerWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone || !m_pPresentPeso )
		return;
	
	for( DWORD i = 0; i < m_dwPresentBackCnt; ++i )
	{
		if( bReward && i == 0 )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			m_pPresentPeso->SetScale( 0.55f, 0.55f );
			m_pPresentPeso->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );			
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentDetailInnerCheerWnd::OnCheerIconRender( int iXPos, int iYPos )
{
	if( !m_pCheerReward )
		return;

	m_pCheerReward->Render( iXPos, iYPos );
}

void TournamentDetailInnerCheerWnd::OnCheerRewardResultRender( int iXPos, int iYPos )
{

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}

	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%s", m_szTeamName.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "우승응원 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "응원한 대회팀이 우승할 경우" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d 페소", m_dwCheerPeso );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 221, TAT_CENTER );
	kPrinter.ClearList();

}

void TournamentDetailInnerCheerWnd::OnCheerRewardNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "우승응원에" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "참가하지 않음" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "응원한 대회팀이 우승할 경우" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerCheerWnd::OnCheerNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "?????" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "우승응원 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "응원한 대회팀이 우승할 경우" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TournamentAlarmMainRewardWnd::TournamentAlarmMainRewardWnd()
{
	m_pDevideDot = NULL;

	m_AttainTourPos		= 0;
	m_dwStartDate		= 0;
	m_iMyCampPos		= 0;
	m_iWinCampPos		= 0;
	m_iLadderBonusPeso  = 0;
	m_iMyLadderRank		= 0;
	m_iMyLadderPoint	= 0;
	m_dwCheerPeso		= 0;
}

TournamentAlarmMainRewardWnd::~TournamentAlarmMainRewardWnd()
{
	SAFEDELETE( m_pDevideDot );
}

void TournamentAlarmMainRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "WndDevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentAlarmMainRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );
}

void TournamentAlarmMainRewardWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/tournamentalarminnerrewardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentalarminnercamprewardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/tournamentalarminnercheerrewardwnd.xml", this );
}

void TournamentAlarmMainRewardWnd::iwm_show()
{
}

void TournamentAlarmMainRewardWnd::SetRewardInfo( BYTE AttainTourPos, 
												  DWORD dwStartDate,
												  int iMyCampPos,
												  int iWinCampPos,
												  int iLadderBonusPeso,
												  int iMyLadderRank,
												  int iMyLadderPoint,
												  DWORD dwCheerPeso,
												  const ioHashString& szTeamName )
{
	m_AttainTourPos		= AttainTourPos;
	m_dwStartDate		= dwStartDate;
	m_iMyCampPos		= iMyCampPos;
	m_iWinCampPos		= iWinCampPos;
	m_iLadderBonusPeso	= iLadderBonusPeso;
	m_iMyLadderRank		= iMyLadderRank;
	m_iMyLadderPoint	= iMyLadderPoint;
	m_dwCheerPeso		= dwCheerPeso;
	m_szTeamName		= szTeamName;
	
	DWORD dwTourIndex = g_TournamentMgr.GetRegularIndex();

	TournamentAlarmRewardWnd *pRewardWnd = dynamic_cast< TournamentAlarmRewardWnd * >( FindChildWnd( ID_TOURNAMENT_REAWARD ) ); 
	if( pRewardWnd )
	{
		pRewardWnd->SetTournamentData( m_dwStartDate, m_AttainTourPos, m_iMyCampPos );
	}

	TournamentAlarmCampRewardWnd *pCampWnd = dynamic_cast< TournamentAlarmCampRewardWnd * >( FindChildWnd( ID_CAMP_REAWARD ) ); 
	if( pCampWnd )
	{
		pCampWnd->SetTournamentData( dwTourIndex, m_dwStartDate, m_iMyCampPos, m_iWinCampPos, m_iLadderBonusPeso );
	}

	TournamentAlarmCheerRewardWnd *pCheerWnd = dynamic_cast< TournamentAlarmCheerRewardWnd * >( FindChildWnd( ID_CHEER_REAWARD ) ); 
	if( pCheerWnd )
	{
		pCheerWnd->SetCheerReward( m_AttainTourPos, m_iWinCampPos, m_dwCheerPeso, m_szTeamName );
	}	
}

void TournamentAlarmMainRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	
	switch( dwID )
	{
	case ID_CLOSE :
	case ID_CONFIRM :
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	}
}

void TournamentAlarmMainRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideWndDotRender( iXPos, iYPos );
}

void TournamentAlarmMainRewardWnd::OnDevideWndDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
		{

			m_pDevideDot->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDevideDot->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDevideDot->SetSize( 2, 3 );
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );	
		}
}

//////////////////////////////////////////////////////////////////////////

TournamentAlarmRewardWnd::TournamentAlarmRewardWnd()
{
	m_pDevideDot = NULL;

	m_pPresentBack = NULL;
	m_pPresentNone = NULL;

	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;

	m_pFinishWin	= NULL;
}

TournamentAlarmRewardWnd::~TournamentAlarmRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );	
	SAFEDELETE( m_pFinishWin );

	ResultDataMap::iterator iCreator1 = m_ResultDataMap.begin();
	for(;iCreator1 != m_ResultDataMap.end(); ++iCreator1 )
	{		
		ResultData Result = iCreator1->second;

		ResultIconList::iterator iCreator2 = Result.m_vIconList.begin();
		for(;iCreator2 != Result.m_vIconList.end(); ++iCreator2 )
		{
			ioUIRenderImage* pImage = (*iCreator2);
			SAFEDELETE( pImage );
		}
	}
	m_ResultDataMap.clear();
}

void TournamentAlarmRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "FinishWin" )
	{
		SAFEDELETE( m_pFinishWin );
		m_pFinishWin = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentAlarmRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );

}

void TournamentAlarmRewardWnd::CreateRewardData( DWORD iReguralDateType )
{
	char szTitle[MAX_PATH] = "";
	sprintf_s( szTitle, sizeof( szTitle ), "%d", iReguralDateType );

	ioINILoader_e kLoader( "config/sp2_tournament_detail_reward.ini" );
	kLoader.SetTitle( szTitle );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//대회 보상
	int iMaxReward = kLoader.LoadInt_e( "result_reward_max", 0 );
	for( int i = 0; i < iMaxReward; ++i )
	{
		sprintf_s_e( szKey, "result_reward%d_text_count", i + 1 );
		int iMax = kLoader.LoadInt( szKey, 0 );

		ResultData Result;
		sprintf_s_e( szKey, "result_reward%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Result.m_szTtitle = szBuf;

		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_reward%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Result.m_szResultList.push_back( szBuf );			
		}

		sprintf_s_e( szKey, "result_reward%d_icon_count", i + 1 );
		iMax = kLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_reward%d_icon%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );

			Result.m_vIconList.push_back( pImg );
		}

		m_ResultDataMap.insert( ResultDataMap::value_type( i + 1 , Result ) );
	}
}

ioHashString& TournamentAlarmRewardWnd::GetResultTitle( int iRound )
{
	static ioHashString kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szTtitle;
}

ioHashStringVec& TournamentAlarmRewardWnd::GetResultTextList( int iRound )
{
	static ioHashStringVec kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szResultList;
}

ResultIconList& TournamentAlarmRewardWnd::GetResultIconList( int iRound )
{
	static ResultIconList kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_vIconList;
}

void TournamentAlarmRewardWnd::SetTournamentData( DWORD dwStartDate, BYTE AttainTourPos, int iMyCampPos )
{
	m_dwStartDate = dwStartDate;
	m_dwCampPos = iMyCampPos;
	m_AttainTourPos = AttainTourPos;

	CreateRewardData( m_dwStartDate );
}

void TournamentAlarmRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentAlarmRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentAlarmRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );
	
	//대회에 참가한 경우
	if( 0 < m_AttainTourPos )
	{
		OnBackRender( iXPos, iYPos );
		OnRewardIconRender( iXPos, iYPos );
		OnResultRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos );
	}
	else
	{
		OnBackRender( iXPos, iYPos, false );
		OnRewardIconRender( iXPos, iYPos );
		OnResultNoneRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
	}

}

void TournamentAlarmRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentAlarmRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{
		m_pBackHub->Render( iXPos, iYPos );	
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}	
}

void TournamentAlarmRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;

	ResultIconList vIconList = GetResultIconList( m_AttainTourPos );
	for( DWORD i =0; i < m_dwPresentBackCnt; ++i )
	{
		ioUIRenderImage* pImage = NULL;
		if( bReward && i < vIconList.size() )
			pImage = vIconList[i];

		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			pImage->SetPosition( 30, 320 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentAlarmRewardWnd::OnRewardIconRender( int iXPos, int iYPos )
{
	if( !m_pFinishWin )
		return;

	m_pFinishWin->Render( iXPos, iYPos );
}

void TournamentAlarmRewardWnd::OnResultRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}

	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%s", GetResultTitle( m_AttainTourPos ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "대회성적 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	int iOffset = 0;
	ioHashStringVec ResultList = GetResultTextList( m_AttainTourPos );
	ioHashStringVec::iterator iter = ResultList.begin();
	for( ; iter != ResultList.end(); ++iter  )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s", (*iter).c_str() );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 221 + iOffset, TAT_CENTER );
		kPrinter.ClearList();
		iOffset += 18;
	}
}

void TournamentAlarmRewardWnd::OnResultNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "대회에" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "참가하지 않음" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 팀 소속의 각 개인당" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TournamentAlarmCampRewardWnd::TournamentAlarmCampRewardWnd()
{
	m_pDevideDot	= NULL;
	m_pPresentBack  = NULL;
	m_pPresentNone  = NULL;

	m_pCampRedIcon  = NULL;   // 대회에서 아이콘 참조
	m_pCampBlueIcon = NULL;
	m_pCampNoneIcon = NULL;

	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;

	m_dwCampResult  = CAMP_DRAW;
	m_dwCampPos		= CAMP_NONE;
}

TournamentAlarmCampRewardWnd::~TournamentAlarmCampRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );

	SAFEDELETE( m_pCampNoneIcon );
	SAFEDELETE( m_pCampBlueIcon );
	SAFEDELETE( m_pCampRedIcon );
	
	ResultDataMap::iterator iCreator1 = m_ResultDataMap.begin();
	for(;iCreator1 != m_ResultDataMap.end(); ++iCreator1 )
	{		
		ResultData Result = iCreator1->second;

		ResultIconList::iterator iCreator2 = Result.m_vIconList.begin();
		for(;iCreator2 != Result.m_vIconList.end(); ++iCreator2 )
		{
			ioUIRenderImage* pImage = (*iCreator2);
			SAFEDELETE( pImage );
		}
	}
	m_ResultDataMap.clear();
}

void TournamentAlarmCampRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CampNoneIcon" )
	{
		SAFEDELETE( m_pCampNoneIcon );
		m_pCampNoneIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentAlarmCampRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX	= xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY	= xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt	= xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt	= xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt  = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX   = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );
}

void TournamentAlarmCampRewardWnd::CreateRewardData( DWORD iReguralDateType )
{
	char szTitle[MAX_PATH] = "";
	sprintf_s( szTitle, sizeof( szTitle ), "%d", iReguralDateType );

	ioINILoader_e kLoader( "config/sp2_tournament_detail_reward.ini" );
	kLoader.SetTitle( szTitle );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//진영전 보상
	int iMaxCamp = kLoader.LoadInt_e( "result_camp_max", 0 );

	kLoader.LoadString_e( "result_camp_blue_icon", "", szBuf, MAX_PATH );
	m_pCampBlueIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	kLoader.LoadString_e( "result_camp_Red_icon", "", szBuf, MAX_PATH );
	m_pCampRedIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );

	for( int i = 0; i < iMaxCamp; ++i )
	{
		sprintf_s_e( szKey, "result_camp%d_text_count", i + 1 );
		int iMax = kLoader.LoadInt( szKey, 0 );

		ResultData Result;
		sprintf_s_e( szKey, "result_camp%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Result.m_szTtitle = szBuf;

		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_camp%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Result.m_szResultList.push_back( szBuf );
		}

		sprintf_s_e( szKey, "result_camp%d_icon_count", i + 1 );
		iMax = kLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_camp%d_icon%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			Result.m_vIconList.push_back( pImg );
		}

		m_ResultDataMap.insert( ResultDataMap::value_type( i + 1 , Result ) );
	}
}

ioHashString& TournamentAlarmCampRewardWnd::GetResultTitle( int iRound )
{
	static ioHashString kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szTtitle;
}

ioHashStringVec& TournamentAlarmCampRewardWnd::GetResultTextList( int iRound )
{
	static ioHashStringVec kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szResultList;
}

ResultIconList& TournamentAlarmCampRewardWnd::GetResultIconList( int iRound )
{
	static ResultIconList kNone;
	ResultDataMap::iterator iter = m_ResultDataMap.find( iRound );
	if( iter == m_ResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_vIconList;
}

void TournamentAlarmCampRewardWnd::SetTournamentData( DWORD dwTourIndex, DWORD dwStartDate, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso )
{
	m_dwStartDate	   = dwStartDate;
	m_dwCampPos		   = iMyCampPos;
	m_iLadderBonusPeso = iLadderBonusPeso;

	CreateRewardData( m_dwStartDate );

	m_dwCampResult = CAMP_DRAW;
	if( iMyCampPos == CAMP_BLUE )
	{
		if( iWinCampPos == CAMP_BLUE )
		{
			m_dwCampResult = BLUE_CAMP_WIN;
		}
		else if( iWinCampPos == CAMP_RED )
		{
			m_dwCampResult = BLUE_CAMP_LOSE;
		}
	}
	else if( iMyCampPos == CAMP_RED )
	{
		if( iWinCampPos == CAMP_RED )
		{
			m_dwCampResult = RED_CAMP_WIN;
		}
		else if( iWinCampPos == CAMP_BLUE )
		{
			m_dwCampResult = RED_CAMP_LOSE;			
		}
	}
}

void TournamentAlarmCampRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentAlarmCampRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentAlarmCampRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );	

	if( m_dwCampPos == CAMP_NONE )
	{
		OnBackRender( iXPos, iYPos, false );
		OnCampIconRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
		OnResultNoneRender( iXPos, iYPos );
	}
	else
	{
		OnBackRender( iXPos, iYPos );
		OnCampIconRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos );
		OnResultRender( iXPos, iYPos );		
	}	
}

void TournamentAlarmCampRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}


void TournamentAlarmCampRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{
		m_pBackHub->Render( iXPos, iYPos );	
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}	
}

void TournamentAlarmCampRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward  )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;

	ResultIconList vIconList = GetResultIconList( m_dwCampResult );
	for( DWORD i =0; i < m_dwPresentBackCnt; ++i )
	{
		ioUIRenderImage* pImage = NULL;
		if( bReward && i < vIconList.size() )
			pImage = vIconList[i];

		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			pImage->SetPosition( 30, 320 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentAlarmCampRewardWnd::OnCampIconRender( int iXPos, int iYPos )
{
	//진영이 레드팀일떄
	if( m_dwCampPos == CAMP_BLUE )
	{
		if( m_pCampRedIcon )
		{
			m_pCampBlueIcon->SetScale( FLOAT1 );
			m_pCampBlueIcon->Render( iXPos + 93, iYPos + 167 );
		}
	}	
	else if( m_dwCampPos == CAMP_RED )
	{
		if( m_pCampBlueIcon )	
		{
			m_pCampRedIcon->SetScale( FLOAT1 );
			m_pCampRedIcon->Render( iXPos + 93, iYPos + 167 );
		}
	}	
	else
	{
		if( m_pCampNoneIcon )
		{
			m_pCampNoneIcon->SetScale( FLOAT1 );
			m_pCampNoneIcon->Render( iXPos + 93, iYPos + 167 );
		}
	}	
}

void TournamentAlarmCampRewardWnd::OnResultRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}

	ioHashString szTtitle = GetResultTitle( m_dwCampResult );
	g_FontMgr.PrintTextWidthCut( iXPos + 93, iYPos + 15, FONT_SIZE_24, 178.0f, "%s", szTtitle.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( m_dwCampResult == BLUE_CAMP_WIN || m_dwCampResult == RED_CAMP_WIN )
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "승리 보상" );
	else
		g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "패배 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "보유한 대회포인트에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );

	char szNum[MAX_PATH];
	Help::ConvertNumToStrComma( m_iLadderBonusPeso, szNum, sizeof(szNum) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%s 페소", szNum );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 221, TAT_CENTER );
	kPrinter.ClearList();

	int iOffset = 18;
	ioHashStringVec ResultList = GetResultTextList( m_dwCampResult );
	ioHashStringVec::iterator iter = ResultList.begin();
	if( ResultList.empty() )
		return;

	//첫줄은 보너스 페소로 대신 찍기 떄문에 패스
	iter++;

	for( ; iter != ResultList.end(); ++iter  )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "%s", (*iter).c_str() );
		kPrinter.PrintFullText( iXPos + 93, iYPos + 221 + iOffset, TAT_CENTER );
		kPrinter.ClearList();
		iOffset += 18;
	}
}

void TournamentAlarmCampRewardWnd::OnResultNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "진영전에" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "참가하지 않음" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "보유한 대회포인트에 따라" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}


//////////////////////////////////////////////////////////////////////////

TournamentAlarmCheerRewardWnd::TournamentAlarmCheerRewardWnd()
{
	m_pDevideDot	= NULL;
	m_pPresentBack	= NULL;
	m_pPresentNone	= NULL;
	m_pPresentPeso  = NULL;

	m_pBackCircle	= NULL;
	m_pBackHub		= NULL;
	m_pCheerReward	= NULL;

	m_dwCheerPeso   = 0;
	m_dwCampPos		= 0;
}

TournamentAlarmCheerRewardWnd::~TournamentAlarmCheerRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );
	SAFEDELETE( m_pPresentPeso );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pCheerReward );
}

void TournamentAlarmCheerRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "PresentPeso" )
	{
		SAFEDELETE( m_pPresentPeso );
		m_pPresentPeso = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CheerReward" )
	{
		SAFEDELETE( m_pCheerReward );
		m_pCheerReward = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentAlarmCheerRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );
}

void TournamentAlarmCheerRewardWnd::SetCheerReward( BYTE AttainTourPos, DWORD dwCampPos, DWORD dwCheerPeso, const ioHashString& szWinnerTeamName )
{
	m_AttainTourPos	   = AttainTourPos;
	m_dwCampPos		   = dwCampPos;
	m_dwCheerPeso	   = dwCheerPeso;
	m_szWinnerTeamName = szWinnerTeamName;
}

void TournamentAlarmCheerRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentAlarmCheerRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void TournamentAlarmCheerRewardWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );

	if( 0 < m_dwCheerPeso )
	{
		OnBackRender( iXPos, iYPos );
		OnCheerIconRender( iXPos, iYPos );
		OnResultRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos );
	}
	else
	{
		OnBackRender( iXPos, iYPos, false );
		OnCheerIconRender( iXPos, iYPos );
		OnResultNoneRender( iXPos, iYPos );
		OnPresentRender( iXPos, iYPos, false );
	}	
}

void TournamentAlarmCheerRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentAlarmCheerRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	//포상결/참여/진영승리/응원성공시
	if( bReward )
	{
		m_pBackHub->Render( iXPos, iYPos );	
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}	
}

void TournamentAlarmCheerRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;
		
	for( DWORD i = 0; i < m_dwPresentBackCnt; ++i )
	{
		if( i == 0 && bReward )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			m_pPresentPeso->SetScale( 0.55f, 0.55f );
			m_pPresentPeso->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentAlarmCheerRewardWnd::OnCheerIconRender( int iXPos, int iYPos )
{
	if( !m_pCheerReward )
		return;

	m_pCheerReward->Render( iXPos, iYPos );
}

void TournamentAlarmCheerRewardWnd::OnResultRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_dwCampPos == CAMP_BLUE )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}

	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "%s", m_szWinnerTeamName.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "우승응원 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "응원한 대회팀이 우승할 경우" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
	
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d 페소", m_dwCheerPeso );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 221, TAT_CENTER );
	kPrinter.ClearList();

	
}

void TournamentAlarmCheerRewardWnd::OnResultNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 15, FONT_SIZE_24, "우승응원 보상" );
	g_FontMgr.PrintText( iXPos + 93, iYPos + 43, FONT_SIZE_24, "획득 실패" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "응원한 대회팀이 우승할 경우" );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 93, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////
TournamentRoomListBtn::TournamentRoomListBtn()
{
	m_dwBattleIndex = 0;

	m_BlueCamp = 0;
	m_RedCamp  = 0;
}

TournamentRoomListBtn::~TournamentRoomListBtn()
{

}

void TournamentRoomListBtn::SetRoomValue( DWORD dwBattleIndex, const ioHashString &rkBlueName, BYTE BlueCamp, const ioHashString &rkRedName, BYTE RedCamp )
{
	m_dwBattleIndex = dwBattleIndex;
	m_BlueName      = rkBlueName;
	m_BlueCamp      = BlueCamp;
	m_RedName       = rkRedName;
	m_RedCamp       = RedCamp;

	if( m_dwBattleIndex == 0 )
	{
		SetInActive();
	}
	else
	{
		SetActive();
	}
}

void TournamentRoomListBtn::OnRender()
{
	ioButton::OnRender();

	if( m_dwBattleIndex == 0 )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_BlueCamp == CAMP_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( m_BlueCamp == CAMP_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 76, iYPos + 4, FONT_SIZE_12, 136.0f, m_BlueName.c_str() );

	if( m_RedCamp == CAMP_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( m_RedCamp == CAMP_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintTextWidthCut( iXPos + 230, iYPos + 4, FONT_SIZE_12, 136.0f, m_RedName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 153, iYPos + 4, FONT_SIZE_12, "VS" );
}

TournamentRoomListWnd::TournamentRoomListWnd()
{
	m_dwSearchTime = 0;

	m_iCurPage = 0;
	m_iMaxPage = 0;

	m_dwTourIndex = 0;
}

TournamentRoomListWnd::~TournamentRoomListWnd()
{
}

void TournamentRoomListWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;
	if( m_dwTourIndex == 0 )
		return;

	SP2Packet kPacket( CTPK_TOURNAMENT_ROOM_LIST );
	kPacket << m_dwTourIndex << m_iCurPage << MAX_ROOM_LIST;
	TCPNetwork::SendToServer( kPacket );
	m_dwSearchTime = FRAMEGETTIME();
}

void TournamentRoomListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > ROOM_SEARCH_TIME )
	{
		StartSend( true );
	}
}
void TournamentRoomListWnd::UpdateSetPage( int iCurPage )
{
	if( !COMPARE( iCurPage, 0, m_iMaxPage + 1 ) )
		return;

	int iPrevPage = m_iCurPage;
	m_iCurPage    = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();
}

void TournamentRoomListWnd::SetRoomList( SP2Packet &rkPacket )
{	
	int iCurrentRound, iMaxList;
	rkPacket >> m_iCurPage >> m_iMaxPage >> iCurrentRound >> iMaxList;
	for(int i = 0;i < MAX_ROOM_LIST;i++)
	{
		TournamentRoomListBtn *pRoomButton = dynamic_cast< TournamentRoomListBtn * >( FindChildWnd( ID_ROOM_1 + i ) );
		if( pRoomButton == NULL ) continue;

		BYTE BlueCamp, RedCamp;
		DWORD dwBattleIndex = 0;
		DWORD dwBlueIndex, dwRedIndex;
		ioHashString kBlueName, kRedName;

		if( i < iMaxList )
		{
			rkPacket >> dwBattleIndex >> dwBlueIndex >> kBlueName >> BlueCamp >> dwRedIndex >> kRedName >> RedCamp;

			TeamData kTeamData;
			// Blue Team
			kTeamData.m_dwTourIndex = m_dwTourIndex;
			kTeamData.m_dwTeamIndex = dwBlueIndex;
			kTeamData.m_szTeamName  = kBlueName;
			g_TournamentTeamMgr.SetDefaultData( kTeamData, true );

			// Red Team
			kTeamData.m_dwTourIndex = m_dwTourIndex;
			kTeamData.m_dwTeamIndex = dwRedIndex;
			kTeamData.m_szTeamName  = kRedName;
			g_TournamentTeamMgr.SetDefaultData( kTeamData, true );
		}
		pRoomButton->SetRoomValue( dwBattleIndex, kBlueName, BlueCamp, kRedName, RedCamp );
	}

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTourIndex );
	if( pTournament )
	{
		char szTitle[MAX_PATH] = "";
		if( iMaxList == 0 )
		{
			sprintf_s_e( szTitle, "관전입장 (대기중)" );
		}
		else
		{
			int iTeamCount = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), iCurrentRound );		
			if( iTeamCount <= 2 )
				sprintf_s_e( szTitle, "관전입장 (결승 진행중)" );
			else
				sprintf_s_e( szTitle, "관전입장 (%d강 진행중)", iTeamCount );
		}
		SetTitleText( szTitle );
	}
}

void TournamentRoomListWnd::SetTournament( DWORD dwTourIndex )
{
	m_dwTourIndex = dwTourIndex;

	ShowWnd();
}

void TournamentRoomListWnd::iwm_show()
{
	m_iCurPage = 0;
	m_iMaxPage = 0;

	StartSend();
}

void TournamentRoomListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( m_iCurPage + 1 );
		}
		break;
	case ID_ROOM_1:
	case ID_ROOM_2:
	case ID_ROOM_3:
	case ID_ROOM_4:
	case ID_ROOM_5:
	case ID_ROOM_6:
	case ID_ROOM_7:
	case ID_ROOM_8:
	case ID_ROOM_9:
	case ID_ROOM_10:
	case ID_ROOM_11:
	case ID_ROOM_12:
	case ID_ROOM_13:
	case ID_ROOM_14:
	case ID_ROOM_15:
	case ID_ROOM_16:
		if( cmd == IOBN_BTNUP )
		{
			TournamentRoomListBtn *pRoomButton = dynamic_cast< TournamentRoomListBtn * >( pWnd );
			if( pRoomButton )
			{
				if( pRoomButton->GetBattleIndex() != 0 )
				{
					PartyInfoWnd *pPartyInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
					if( pPartyInfoWnd )
					{
						pPartyInfoWnd->HideWnd();

						SP2Packet kPacket( CTPK_BATTLEROOM_JOIN_INFO );
						kPacket << pRoomButton->GetBattleIndex();
						TCPNetwork::SendToServer( kPacket );					
					}
				}
			}
		}
		break;
	}
}

void TournamentRoomListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSearch();		
	ioMovingWnd::OnProcess( fTimePerSec );
}

void TournamentRoomListWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "/%d", m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 170, iYPos + 445, TAT_CENTER );
}