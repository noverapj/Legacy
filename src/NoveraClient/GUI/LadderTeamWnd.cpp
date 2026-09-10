

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"

#include "ioInterfaceBackground.h"
#include "ChatMsgWnd.h"
#include "LadderTeamWnd.h"
#include "ModeMapSelectList.h"
#include "UserInfoToolTip.h"
#include "HeroMainWnd.h"

LadderTeamListBtn::LadderTeamListBtn()
{
	m_pBlueTeamMark	= NULL;
	m_pRedTeamMark	= NULL;

	m_pLockKey	   = NULL;
	m_pLockKeyGray = NULL;
	m_pIconEmpty   = NULL;
}

LadderTeamListBtn::~LadderTeamListBtn()
{
	SAFEDELETE( m_pLockKey );
	SAFEDELETE( m_pLockKeyGray );
	SAFEDELETE( m_pIconEmpty );
}

void LadderTeamListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "LockKey" )
	{
		SAFEDELETE( m_pLockKey );
		m_pLockKey = pImage;
	}
	else if( szType == "LockKeyGray" )
	{
		SAFEDELETE( m_pLockKeyGray );
		m_pLockKeyGray = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void LadderTeamListBtn::SetLadderTeamInfo( LadderTeamInfo &rkInfo )
{
	m_LadderTeamInfo = rkInfo;
	m_szState        = g_ModeSelectInfo.GetLadderTeamModeTitle( m_LadderTeamInfo.m_iSelectModeIndex );
}

void LadderTeamListBtn::SetTeamIcon(  ioUIRenderImage* pBlueIcon, ioUIRenderImage* pRedIcon  )
{
	m_pRedTeamMark	= pRedIcon;
	m_pBlueTeamMark	= pBlueIcon;
}

void LadderTeamListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_LadderTeamInfo.m_dwTeamIndex == 0 )
	{
		if( m_pIconEmpty )
		{
			m_pIconEmpty->SetScale( 0.30f );
			m_pIconEmpty->Render( iXPos + 20, iYPos + 17 , UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 35, iYPos + 8, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.PrintText( iXPos + 373, iYPos + 8, FONT_SIZE_12, "0/0" );
	}
	else
	{
		if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		
		if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
		{
			if( m_pBlueTeamMark )
			{
				m_pBlueTeamMark->SetScale( FLOAT1, FLOAT1 );
				m_pBlueTeamMark->Render( iXPos + 19, iYPos + 16 );
			}
		}
		else
		{
			if( m_pRedTeamMark )
			{
				m_pRedTeamMark->SetScale( FLOAT1, FLOAT1 );
				m_pRedTeamMark->Render( iXPos + 19, iYPos + 16 );
			}
		}		
		
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintTextWidthCut( iXPos + 35, iYPos + 8, FONT_SIZE_12, 137.0f, m_LadderTeamInfo.m_szTeamName.c_str() );
		int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_LadderTeamInfo.m_szTeamName.c_str(), TS_NORMAL, FONT_SIZE_12, 137.0f );
		if( m_LadderTeamInfo.m_dwGuildIndex != 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( m_LadderTeamInfo.m_dwGuildIndex, m_LadderTeamInfo.m_dwGuildMark, iXPos + 35 + iRealNameSize + 4, iYPos + 9 );
			iRealNameSize += 17;
		}

		if( !m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
		{
			if( m_pLockKey && m_pLockKeyGray )
			{
				if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_ACTIVE )
					m_pLockKey->Render( iXPos + 35 + iRealNameSize + 4, iYPos + 9 );
				else
					m_pLockKeyGray->Render( iXPos + 35 + iRealNameSize + 4, iYPos + 9 );
			}
		}
        
		//승무패연승
		if( m_LadderTeamInfo.m_iVictoriesRecord > 1 )
			g_FontMgr.PrintText( iXPos + 219, iYPos + 8, FONT_SIZE_12, STR(2), m_LadderTeamInfo.m_iWinRecord, m_LadderTeamInfo.m_iLoseRecord, m_LadderTeamInfo.m_iVictoriesRecord );
		else
			g_FontMgr.PrintText( iXPos + 219, iYPos + 8, FONT_SIZE_12, STR(3), m_LadderTeamInfo.m_iWinRecord, m_LadderTeamInfo.m_iLoseRecord );

		//인원
		g_FontMgr.SetAlignType( TAT_RIGHT );
		if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 373, iYPos + 8, FONT_SIZE_12, STR(4), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
	}
}
//////////////////////////////////////////////////////////////////////////
LadderTeamCreateBtn::LadderTeamCreateBtn()
{
	m_szTitle.Clear();
	m_pBtnIcon = NULL;
}
LadderTeamCreateBtn::~LadderTeamCreateBtn()
{
	SAFEDELETE( m_pBtnIcon );
}

void LadderTeamCreateBtn::SetText( ioHashString szTitle, CampType eCampType )
{
	m_szTitle = szTitle;
	m_eCampType = eCampType;
}

void LadderTeamCreateBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_2X );
	if( m_eCampType == CAMP_BLUE )
		kPrinter.SetBkColor( 33, 90, 126 );
	else
		kPrinter.SetBkColor( 159, 41, 26 );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_18, "%s 팀 만들기", m_szTitle );		
	kPrinter.PrintFullText( iXPos + 159, iYPos + 10, TAT_CENTER );

	int iWidth = kPrinter.GetFullWidth();
	
	m_pBtnIcon->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pBtnIcon->Render( iXPos + 159 - (iWidth/2) - 10, iYPos + 11 );
}

void LadderTeamCreateBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BtnIcon" )
	{
		SAFEDELETE( m_pBtnIcon );
		m_pBtnIcon = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}	
}
//////////////////////////////////////////////////////////////////////////
LadderTeamListWnd::LadderTeamListWnd()
{
	m_iPageYPos = 226;
	m_iCurPage = m_iMaxPage = 0;
	m_dwMaxTeamList = LOBBY_MAX_TEAM_LIST;
}

LadderTeamListWnd::~LadderTeamListWnd()
{
	m_vLadderTeamList.clear();
}

void LadderTeamListWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_LADDER_TEAM_LIST );
	kPacket << m_iCurPage << m_dwMaxTeamList << false;       // false = 진영팀 , true = 영웅전
	TCPNetwork::SendToServer( kPacket );	

	m_dwSearchTime = FRAMEGETTIME();
}

void LadderTeamListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > TEAM_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void LadderTeamListWnd::iwm_show()
{	
	m_vLadderTeamList.clear();
	m_iCurPage = m_iMaxPage = 0;
	StartSend();

	for( int i = ID_LIST_1; i <= ID_LIST_10; i++ )
	{
		LadderTeamListBtn *pBtn = (LadderTeamListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			LadderTeamInfo kInfo;
			pBtn->SetLadderTeamInfo( kInfo );
			pBtn->AddWndStyle( IWS_EXACTIVE );
			if( i < static_cast<int>( ID_LIST_1 + m_dwMaxTeamList ) )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}	
	
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament )
	{
		ioHashString szCampName = pTournament->GetCampName( g_MyInfo.GetUserCampPos() );

		if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		{			
			LadderTeamCreateBtn* pButton = dynamic_cast<LadderTeamCreateBtn*>( FindChildWnd( ID_BLUE_TEAM_CREATE ) );
			if( pButton )
			{
				pButton->SetText( szCampName, CAMP_BLUE );				
			}
		}
		else if(  g_MyInfo.GetUserCampPos() == CAMP_RED )
		{
			LadderTeamCreateBtn* pButton = dynamic_cast<LadderTeamCreateBtn*>( FindChildWnd( ID_RED_TEAM_CREATE ) );
			if( pButton )
			{
				pButton->SetText( szCampName, CAMP_RED );
			}
		}
	}

	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		HideChildWnd( ID_RED_TEAM_CREATE );
		ShowChildWnd( ID_BLUE_TEAM_CREATE );

		HideChildWnd( ID_RED_LADDER_RANKING );
		ShowChildWnd( ID_BLUE_LADDER_RANKING );			
	}
	else
	{
		ShowChildWnd( ID_RED_TEAM_CREATE );
		HideChildWnd( ID_BLUE_TEAM_CREATE );

		ShowChildWnd( ID_RED_LADDER_RANKING );
		HideChildWnd( ID_BLUE_LADDER_RANKING );
	}

	m_dwSelectIndex = g_TournamentMgr.GetRegularIndex();

	UpdateMainUI();
}

void LadderTeamListWnd::PlazaUIMove()
{
	ioWnd* pLeft = FindChildWnd( ID_LEFT_PAGE );
	if( pLeft )
		pLeft->SetWndPos( pLeft->GetDefaultPosX(), pLeft->GetDefaultPosY() + 172 );

	ioWnd* pRight = FindChildWnd( ID_RIGHT_PAGE );
	if( pRight )
		pRight->SetWndPos( pRight->GetDefaultPosX(), pRight->GetDefaultPosY() + 172 );

	m_iPageYPos = 398;
}

void LadderTeamListWnd::iwm_hide()
{
	m_dwSearchTime = 0;
}

void LadderTeamListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_BLUE_TEAM_CREATE:
	case ID_RED_TEAM_CREATE:
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
				else if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
				else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				}
				else
				{
					g_GUIMgr.ShowWnd( LADDER_TEAM_CREATE_WND );
				}
			}
		}
		break;
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
	case ID_BLUE_LADDER_RANKING:
	case ID_RED_LADDER_RANKING:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( LADDER_RANKING_LIST_WND ) )
				g_GUIMgr.HideWnd( LADDER_RANKING_LIST_WND );
			else
			{
				LadderRankingListWnd *pRankList = dynamic_cast<LadderRankingListWnd*>(g_GUIMgr.FindWnd( LADDER_RANKING_LIST_WND ));
				if( pRankList )
					pRankList->ShowTabWnd( LadderRankingListWnd::ID_TEAM_RANKING );
			}
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
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				LadderTeamListBtn *pBtn = (LadderTeamListBtn*)pWnd;
				if( pBtn->GetLadderTeamIndex() == 0 )
				{
					iwm_command( FindChildWnd( ID_BLUE_TEAM_CREATE ), IOBN_BTNUP, 0 );
					return;
				}
				if( pBtn->GetLadderTeamIndex() == g_LadderTeamMgr.GetLadderTeamIndex() ) return;

				LadderTeamInfoWnd *pInfoWnd = dynamic_cast<LadderTeamInfoWnd *>(g_GUIMgr.FindWnd(LADDER_TEAM_INFO_WND));
				if( pInfoWnd )
				{
					pInfoWnd->SetInfo( pBtn->GetLadderTeamInfo(), true );

					SP2Packet kPacket( CTPK_LADDERTEAM_JOIN_INFO );
					kPacket << pBtn->GetLadderTeamIndex();
					TCPNetwork::SendToServer( kPacket );
				}          
			}
		}
		break;
	}
}

void LadderTeamListWnd::UpdateMainUI()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament == NULL ) return;	
	
	for( int i = ID_LIST_1; i <= ID_LIST_10; i++ )
	{
		LadderTeamListBtn *pButton = (LadderTeamListBtn*)FindChildWnd( i );
		if( pButton && i < ID_LIST_1 + static_cast<int>( m_dwMaxTeamList ) )
		{			
			pButton->SetTeamIcon( pTournament->GetResCampSmallIcon( CAMP_BLUE ),  pTournament->GetResCampSmallIcon( CAMP_RED ) );
		}
	}
}

void LadderTeamListWnd::OnUpdateProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwSelectIndex );
	if( pTournament )
	{
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

void LadderTeamListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
	
	ProcessSearch();
	OnUpdateProcess();
}

void LadderTeamListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 201, iYPos + m_iPageYPos, FONT_SIZE_12, STR(1), m_iCurPage + 1 );;
}

void LadderTeamListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();

	int iListSIze = m_vLadderTeamList.size();
	int iStartPos = 0;
	for( int i = ID_LIST_1; i < ID_LIST_10; i++, iStartPos++ )
	{
		LadderTeamListBtn *pBtn = (LadderTeamListBtn*)FindChildWnd( i );
		if( pBtn && i < ID_LIST_1 + static_cast<int>( m_dwMaxTeamList ) )
		{
			if( iStartPos < iListSIze )
			{
				LadderTeamInfo &kInfo = m_vLadderTeamList[iStartPos];
				pBtn->SetLadderTeamInfo( kInfo );
				pBtn->RemoveWndStyle( IWS_EXACTIVE );
			}
			else
			{
				LadderTeamInfo kInfo;
				pBtn->SetLadderTeamInfo( kInfo );
				pBtn->AddWndStyle( IWS_EXACTIVE );
			}
		}
	}
}

void LadderTeamListWnd::ApplyLadderTeamList( SP2Packet &rkPacket )
{
	m_vLadderTeamList.clear();

	LadderTeamInfoWnd *pInfoWnd = dynamic_cast<LadderTeamInfoWnd *>(g_GUIMgr.FindWnd(LADDER_TEAM_INFO_WND));
	if( !pInfoWnd ) return;

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;			
	for(i = 0;i < iCurList;i++)
	{
		LadderTeamInfo kInfo;
		rkPacket >> kInfo.m_dwTeamIndex >> kInfo.m_iCampType >> kInfo.m_dwGuildIndex >> kInfo.m_dwGuildMark >> kInfo.m_iSelectModeIndex >> kInfo.m_iState >> kInfo.m_szTeamName >> kInfo.m_szTeamPW >> kInfo.m_iCurPlayer >> kInfo.m_iMaxPlayer 
			     >> kInfo.m_iAverageLevel >> kInfo.m_iWinRecord >> kInfo.m_iLoseRecord >> kInfo.m_iVictoriesRecord;
		m_vLadderTeamList.push_back( kInfo );
		pInfoWnd->UpdateTeamState( kInfo.m_dwTeamIndex, kInfo.m_iState );
	}    
	m_iMaxPage = max( 0, iTotalList - 1 ) / m_dwMaxTeamList;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
LadderTeamInfoBtn::LadderTeamInfoBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal = NULL;
	m_pBlueOver = NULL;
	m_pRedOver = NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_eTeam = TEAM_BLUE;
	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = m_iLadderPoint = m_iPingStep = 0;
}

LadderTeamInfoBtn::~LadderTeamInfoBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void LadderTeamInfoBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void LadderTeamInfoBtn::iwm_create()
{
	m_PingStep.Create();
}

void LadderTeamInfoBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, int iLadderPoint, TeamType eTeam )
{
	m_iMaxSlot     = iMaxSlot;
	m_iSlotArray   = iSlotArray;
	m_szName       = rkName;
	m_eTeam        = eTeam;
	m_iGradeLevel  = iGradeLevel;
	m_iPingStep    = iPingStep;
	m_iLadderPoint = iLadderPoint;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void LadderTeamInfoBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case TEAM_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void LadderTeamInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBase( iXPos, iYPos );

	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_eTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 205.0f, m_szName.c_str() );
		int iRealNameSize = min( 205.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_iLadderPoint, szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintText( iXPos + 316, iYPos + 3, FONT_SIZE_12, STR(1), szConvertText );

		OnRenderPing( iXPos + 322, iYPos + 2 );
	}
}

void LadderTeamInfoBtn::OnRenderPing( int iXPos, int iYPos )
{
	int  iPingStep = 0;
	bool bServerRelay = false;
	if( m_szName == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else if( P2PNetwork::IsNetworkTableUser( m_szName ) )
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( m_szName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( m_szName ) )
			bServerRelay = true;
	}
	else
	{
		iPingStep = m_iPingStep;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}

void LadderTeamInfoBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
LadderTeamInfoWnd::LadderTeamInfoWnd()
{
	m_pModeIconBack = NULL;
	m_pBlueCampIcon = NULL;
	m_pRedCampIcon  = NULL;
	m_bLadderOrBattleTeam    = false;
	m_bStateBackup = false;
}

LadderTeamInfoWnd::~LadderTeamInfoWnd()
{
	m_vUserList.clear();

	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pBlueCampIcon );
	SAFEDELETE( m_pRedCampIcon );
}

void LadderTeamInfoWnd::SetInfo( SP2Packet &rkPacket )
{
	m_vUserList.clear();

	rkPacket >> m_LadderTeamInfo.m_dwTeamIndex;

	if( m_LadderTeamInfo.m_dwTeamIndex == 0 )
	{
		HideWnd();
		return;
	}
	int iState;
	rkPacket >> iState;
	rkPacket >> m_LadderTeamInfo.m_iCampType >> m_LadderTeamInfo.m_szTeamName >> m_LadderTeamInfo.m_szTeamPW >> m_LadderTeamInfo.m_iAverageLevel;
	rkPacket >> m_LadderTeamInfo.m_iWinRecord >> m_LadderTeamInfo.m_iLoseRecord
		     >> m_LadderTeamInfo.m_iVictoriesRecord >> m_LadderTeamInfo.m_iMaxPlayer >> m_LadderTeamInfo.m_iCurPlayer;

	if( !m_bStateBackup )
		m_LadderTeamInfo.m_iState = iState;

	if( m_LadderTeamInfo.m_iCurPlayer > 0 )
	{
		for(int i = 0;i < m_LadderTeamInfo.m_iCurPlayer;i++)
		{
			DWORD dwGuildIndex, dwGuildMark;
			LadderTeamUserInfo kUserInfo;
			rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_iLadderPoint 
					 >> dwGuildIndex >> dwGuildMark >> kUserInfo.m_iServerPingStep;
			m_vUserList.push_back( kUserInfo );

			// 길드 정보
			g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, dwGuildIndex, dwGuildMark );

			// 친구 정보 갱신
			g_FriendManager.ClientFriendInfoUpdate( kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, UP_LADDER_TEAM );
		}
	}	

	ioHashString szBlueCampName;
	ioHashString szRedCampName;

	szBlueCampName.Clear();
	szRedCampName.Clear();

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament != NULL )
	{
		szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
		szRedCampName = pTournament->GetCampName( CAMP_RED );

		m_pBlueCampIcon = pTournament->GetCampIcon( CAMP_BLUE );
		m_pRedCampIcon  = pTournament->GetCampIcon( CAMP_RED );
	}

	// UI TITLE	
	char szTitle[MAX_PATH] = "";
	if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
	{
		if( m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d", szBlueCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d(비밀팀)", szBlueCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
	}
	else
	{
		if( m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d", szRedCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d(비밀팀)", szRedCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
	}
	SetTitleText( szTitle );	

	if( !IsShow() )
		ShowWnd();
	else
		iwm_show();
}

const LadderTeamUserInfo &LadderTeamInfoWnd::GetUserInfo( int iArray )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		if( --iArray < 0 )
			return m_vUserList[i];
	}

	static LadderTeamUserInfo kReturnInfo;
	return kReturnInfo;
}

void LadderTeamInfoWnd::UpdateUserList()
{
	TeamType eBtnTeam = TEAM_BLUE;
	if( m_LadderTeamInfo.m_iCampType == CAMP_RED )
		eBtnTeam = TEAM_RED;

	int i = 0;
	for(i = 0;i < MAX_LIST;i++)
	{
		LadderTeamInfoBtn *pTeamBtn = dynamic_cast<LadderTeamInfoBtn*>(FindChildWnd( ID_LIST_1 + i ));
		if( pTeamBtn )
		{
			const LadderTeamUserInfo &rkInfo = GetUserInfo( i );
			pTeamBtn->SetMemberInfo( m_LadderTeamInfo.m_iMaxPlayer, i, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, rkInfo.m_iLadderPoint, eBtnTeam );
		}
	}
}

void LadderTeamInfoWnd::SetInfo( LadderTeamInfo &rkInfo, bool bStateBackup )
{
	m_LadderTeamInfo = rkInfo;
	m_bStateBackup   = bStateBackup;
}

void LadderTeamInfoWnd::UpdateTeamState( DWORD dwTeamIndex, int iState )
{
	if( !IsShow() ) return;
	if( m_LadderTeamInfo.m_dwTeamIndex == dwTeamIndex )
	{
		m_LadderTeamInfo.m_iState = iState;
	}
}

void LadderTeamInfoWnd::SetBtn()
{
	if( m_bLadderOrBattleTeam )
	{
		SetChildWndStyleAdd( ID_ENTER, IWS_EXACTIVE );
	}
	else if( g_MyInfo.GetActiveCharCount() == 0 || 	( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) ||
		     m_LadderTeamInfo.m_iState != LadderTeamInfo::LTS_ACTIVE || g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetLadderTeamLimitGrade() )
	{
		SetChildWndStyleAdd( ID_ENTER, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleRemove( ID_ENTER, IWS_EXACTIVE );
	}
}

void LadderTeamInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void LadderTeamInfoWnd::iwm_show()
{
	UpdateUserList();
	m_bLadderOrBattleTeam = false;
	if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() )
		m_bLadderOrBattleTeam = true;
	SetBtn();

	if( strcmp( g_MyInfo.GetPublicID().c_str(), "개발자B" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자K" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자L" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자Q" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자U" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "매니저X" ) == 0 )  //Except Extracting Hangeul
	{
		ShowChildWnd( ID_DEVELOPER_ENTER );
	}
	else
	{
		HideChildWnd( ID_DEVELOPER_ENTER );
	}
}

void LadderTeamInfoWnd::iwm_hide()
{
	m_bStateBackup = false;
}

void LadderTeamInfoWnd::JoinLadderTeam()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_JOIN_LADDERTEAM );
	kPacket << m_LadderTeamInfo.m_dwTeamIndex << false;       // 팀 인덱스 << 초대 << 비번
	TCPNetwork::SendToServer( kPacket );
}

void LadderTeamInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ENTER:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_LadderTeamMgr.IsKickOutRoom( m_LadderTeamInfo.m_dwTeamIndex ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
			else if( g_LadderTeamMgr.IsLeaveRoomInfo( m_LadderTeamInfo.m_dwTeamIndex ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
			}
			else if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else if( g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetLadderTeamLimitGrade() )
			{
				char szGradeName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_LadderTeamMgr.GetLadderTeamLimitGrade(), szGradeName, sizeof( szGradeName ), false );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13), szGradeName );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_FULL_USER )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_NOT_MIN_LEVEL_MATCH && !g_App.IsMeDeveloper(true) )	// 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_NOT_MAX_LEVEL_MATCH && !g_App.IsMeDeveloper(true) ) // 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(6), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_MATCH_PLAY )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_GUILD_TEAM_JOIN )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
			else if( m_LadderTeamInfo.m_iState == LadderTeamInfo::LTS_CAMP_NOT_MATCH )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
			}			
			else  if( g_CampMgr.IsCloseCampBattle() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12),
												  g_CampMgr.GetPlayPreceedMent().c_str() );
			}
			else if( !m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
			{
				LadderPasswordWnd *pPassword = dynamic_cast<LadderPasswordWnd*>( g_GUIMgr.FindWnd( LADDER_TEAM_PASSWORD_WND ) );
				if( pPassword )
					pPassword->SetLadderTeamInfo( m_LadderTeamInfo );
			}
			else
			{
				JoinLadderTeam();
			}
			HideWnd();
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
		if( cmd == IOBN_BTNUP )
		{
			LadderTeamInfoBtn *pUserBtn = dynamic_cast<LadderTeamInfoBtn*>(pWnd);
			if( pUserBtn )
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_LadderTeamInfo.m_dwTeamIndex != 0 )
				{
					if( !m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
					{
						LadderPasswordWnd *pPassword = dynamic_cast<LadderPasswordWnd*>( g_GUIMgr.FindWnd( LADDER_TEAM_PASSWORD_WND ) );
						if( pPassword )
							pPassword->SetLadderTeamInfo( m_LadderTeamInfo );
					}
					else
					{
						JoinLadderTeam();
					}
				}
				HideWnd();
			}
		}
		break;
	case ID_DEVELOPER_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( m_vUserList.empty() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );	
			}
			else
			{
				g_App.SendRoomStealthEnter( m_vUserList[0].m_szUserID );
			}
		}
		break;
	}
}

void LadderTeamInfoWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE && m_pBlueCampIcon )
		{
			m_pBlueCampIcon->SetScale( 0.60f, 0.60f );
			m_pBlueCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else if( m_pRedCampIcon )
		{
			m_pRedCampIcon->SetScale( 0.60f, 0.60f );
			m_pRedCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, m_LadderTeamInfo.m_szTeamName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_LadderTeamInfo.m_iVictoriesRecord > 1 )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(1), m_LadderTeamInfo.m_iWinRecord,
			m_LadderTeamInfo.m_iLoseRecord, m_LadderTeamInfo.m_iVictoriesRecord );	
	}
	else
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(2), m_LadderTeamInfo.m_iWinRecord,
			m_LadderTeamInfo.m_iLoseRecord );	
	}		
}

void LadderTeamInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );

	bool bTeam = false;
	if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() )
		bTeam = true;
	if( m_bLadderOrBattleTeam != bTeam )
	{
		m_bLadderOrBattleTeam = bTeam;
		SetBtn();
	}
}
//////////////////////////////////////////////////////////////////////////
MyLadderTeamInfoWnd::MyLadderTeamInfoWnd()
{
	m_pModeIconBack = NULL;
	m_pBlueCampIcon = NULL;
	m_pRedCampIcon  = NULL;

	m_pBlueDark  = NULL;
	m_pBlueLight = NULL;
	m_pBlueOver  = NULL;
	m_pRedDark   = NULL;
	m_pRedLight  = NULL;
	m_pRedOver   = NULL;

	m_pCurrentDark  = NULL;
	m_pCurrentLight = NULL;
	m_pCurrentOver  = NULL;
	m_bMouseLBtnDown= false;
}

MyLadderTeamInfoWnd::~MyLadderTeamInfoWnd()
{
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pBlueCampIcon );
	SAFEDELETE( m_pRedCampIcon );

	SAFEDELETE( m_pBlueDark );
	SAFEDELETE( m_pBlueLight );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedDark );
	SAFEDELETE( m_pRedLight );
	SAFEDELETE( m_pRedOver );
}

void MyLadderTeamInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "BlueDark" )
	{
		SAFEDELETE( m_pBlueDark );
		m_pBlueDark = pImage;
	}
	else if( szType == "BlueLight" )
	{
		SAFEDELETE( m_pBlueLight );
		m_pBlueLight = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedDark" )
	{
		SAFEDELETE( m_pRedDark );
		m_pRedDark = pImage;
	}
	else if( szType == "RedLight" )
	{
		SAFEDELETE( m_pRedLight );
		m_pRedLight = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MyLadderTeamInfoWnd::iwm_create()
{
	m_PingStep.Create();
}

void MyLadderTeamInfoWnd::iwm_show()
{
	if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
	{
		SetChildWndStyleRemove( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleAdd( ID_CHANGE_INFO, IWS_EXACTIVE );
	}

	ioHashString szBlueCampName;
	ioHashString szRedCampName;

	szBlueCampName.Clear();
	szRedCampName.Clear();

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament != NULL )
	{
		szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
		szRedCampName = pTournament->GetCampName( CAMP_RED );

		m_pBlueCampIcon = pTournament->GetCampIcon( CAMP_BLUE );
		m_pRedCampIcon  = pTournament->GetCampIcon( CAMP_RED );
	}
	
	// UI TITLE	
	char szTitle[MAX_PATH] = "";
	const LadderTeamInfo &kTeamInfo = g_LadderTeamMgr.GetLadderTeamInfo();
	if( kTeamInfo.m_iCampType == CAMP_BLUE )
	{
		if( kTeamInfo.m_szTeamPW.IsEmpty() )
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d",szBlueCampName.c_str(), kTeamInfo.m_iCurPlayer, kTeamInfo.m_iMaxPlayer );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d(비밀팀)", szBlueCampName.c_str(), kTeamInfo.m_iCurPlayer, kTeamInfo.m_iMaxPlayer );
	}
	else
	{
		if( kTeamInfo.m_szTeamPW.IsEmpty() )
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d", szRedCampName.c_str(), kTeamInfo.m_iCurPlayer, kTeamInfo.m_iMaxPlayer );
		else
			SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d(비밀팀)", szRedCampName.c_str(), kTeamInfo.m_iCurPlayer, kTeamInfo.m_iMaxPlayer );
	}

	//적팀
	const LadderTeamInfo& OtherTeamInfo = g_LadderTeamMgr.GetLadderOtherTeamInfo();
	if ( OtherTeamInfo.m_iTeamType == TEAM_RED )
	{
		m_pCurrentDark  = m_pBlueDark;
		m_pCurrentLight = m_pBlueLight;
		m_pCurrentOver  = m_pBlueOver;
	}
	else
	{
		m_pCurrentDark  = m_pRedDark;
		m_pCurrentLight = m_pRedLight;
		m_pCurrentOver  = m_pRedOver;
	}
	SetTitleText( szTitle );	

	if( g_LadderTeamMgr.IsHeroMatchMode() )
		HideChildWnd( ID_CHANGE_INFO );
	else
		ShowChildWnd( ID_CHANGE_INFO );
}

void MyLadderTeamInfoWnd::iwm_hide()
{
}

void MyLadderTeamInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
		}
		break;
	case ID_CHANGE_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			g_GUIMgr.ShowWnd( LADDER_TEAM_OWNER_WND );
		}
		break;
	}
}


void MyLadderTeamInfoWnd::ProcessSiteClick()
{
	bool bPrevMouseDown = m_bMouseLBtnDown;
	ioHashString szPrevOverName = m_MouseOverName;

	m_bMouseLBtnDown = false;
	m_MouseOverName.Clear();

	if( !g_App.IsVisibleMouse() || g_GUIMgr.GetPreOverWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	ioLSMouse *pMouse = g_App.GetMouse();
	for(int i = 0;i < g_LadderTeamMgr.GetMaxPlayer();i++)
	{
		RECT rcNameRect;
		rcNameRect.left   = iXPos + 17;
		rcNameRect.top    = iYPos + 100 + ( i * 21 );
		rcNameRect.right  = rcNameRect.left + 348;
		rcNameRect.bottom = rcNameRect.top + 21;

		const LadderTeamUserInfo &kUserInfo = g_LadderTeamMgr.GetUserInfo( i );
		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( szPrevOverName != kUserInfo.m_szUserID )
			{
				g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
			}
			m_MouseOverName = kUserInfo.m_szUserID;
			if( pMouse->IsLBtnDown() )
			{
				if( !bPrevMouseDown )
					g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );
				m_bMouseLBtnDown = true;
			}
			else if( pMouse->IsLBtnUp() )
			{
				g_App.OpenMannerWnd( kUserInfo.m_szUserID.c_str(), true );
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_MouseOverName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void MyLadderTeamInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSiteClick();
}

void MyLadderTeamInfoWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE && m_pBlueCampIcon )
		{
			m_pBlueCampIcon->SetScale( 0.60f, 0.60f );
			m_pBlueCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else if( m_pRedCampIcon )
		{
			m_pRedCampIcon->SetScale( 0.60f, 0.60f );
			m_pRedCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}	
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, g_LadderTeamMgr.GetLadderTeamName().c_str() );

	const LadderTeamInfo &kTeamInfo = g_LadderTeamMgr.GetLadderTeamInfo();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( kTeamInfo.m_iVictoriesRecord > 1 )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(1), kTeamInfo.m_iWinRecord,
																										 kTeamInfo.m_iLoseRecord, kTeamInfo.m_iVictoriesRecord );	
	}
	else
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 300.0f, STR(2), kTeamInfo.m_iWinRecord, kTeamInfo.m_iLoseRecord );	
	}

	// 유저 정보
	for(int i = 0;i < MAX_LIST;i++)
	{
		if( i < g_LadderTeamMgr.GetMaxPlayer() )
		{
			const LadderTeamUserInfo &kUserInfo = g_LadderTeamMgr.GetUserInfo( i );
			OnRenderUser( iXPos + 17, iYPos + 100 + (i*21), i, kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, kUserInfo.m_iLadderPoint );
		}
		else
		{
			OnRenderUser( iXPos + 17, iYPos + 100 + (i*21), i, "", 0, 0 );
		}
	}
}

void MyLadderTeamInfoWnd::OnRenderUser( int iXPos, int iYPos, int iSlotArray, const ioHashString szName, int iGradeLevel, int iLadderPoint )
{
	// 바탕
	if( iSlotArray % 2 == 0 )
	{
		if( m_pCurrentDark )
			m_pCurrentDark->Render( iXPos, iYPos );
	}
	else if( m_pCurrentLight )
	{
		m_pCurrentLight->Render( iXPos, iYPos );
	}
	
	if( szName.IsEmpty() )
		return;

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_MouseOverName == szName ) // 오버 슬롯
	{
		if( m_pCurrentOver ) 
			m_pCurrentOver->Render( iXPos, iYPos );

		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
		g_FontMgr.SetTextColor( 255, 255, 255 );		
	}
	else
	{
		g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, iGradeLevel, TEAM_PRIVATE_1 );
		//적팀이
		const LadderTeamInfo& OtherTeamInfo = g_LadderTeamMgr.GetLadderOtherTeamInfo();
		if( OtherTeamInfo.m_iTeamType == TEAM_RED )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );		
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	}
	g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, szName.c_str() );
	int iRealNameSize = g_FontMgr.GetTextWidth( szName.c_str(), TS_NORMAL, FONT_SIZE_12 );
	DWORD dwGuildIndex, dwGuildMark;
	g_UserInfoMgr.GetGuildInfo( szName, dwGuildIndex, dwGuildMark );
	g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	char szConvertText[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iLadderPoint, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 316, iYPos + 3, FONT_SIZE_12, STR(1), szConvertText );
	OnRenderPing( szName, iXPos + 322, iYPos + 2 );
}

void MyLadderTeamInfoWnd::OnRenderPing( const ioHashString &rkName, int iXPos, int iYPos )
{
	int  iPingStep = 0;
	bool bServerRelay = false;
	if( rkName == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( rkName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( rkName ) )
			bServerRelay = true;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}
//////////////////////////////////////////////////////////////////////////
LadderPasswordWnd::LadderPasswordWnd()
{
	m_pPreEdit	= NULL;
}

LadderPasswordWnd::~LadderPasswordWnd()
{

}

void LadderPasswordWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}
}

void LadderPasswordWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
		pEdit->KillKeyFocus();
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void LadderPasswordWnd::SetLadderTeamInfo( LadderTeamInfo &rkLadderTeam )
{
	m_LadderTeamInfo = rkLadderTeam;

	if( m_LadderTeamInfo.m_dwTeamIndex != 0 )
		ShowWnd();
}

void LadderPasswordWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			JoinLadderTeam();
		}
		break;
	case ID_CANCEL:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();			
		}
		break;
	case ID_PASS_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				JoinLadderTeam();
			}						
		}
		break;
	}
}

void LadderPasswordWnd::JoinLadderTeam()
{
	if( g_App.IsMouseBusy() )
		return;
	if( m_LadderTeamInfo.m_dwTeamIndex == 0 )
		return;

	if( g_CampMgr.IsCloseCampBattle() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), g_CampMgr.GetPlayPreceedMent().c_str() );
		return;
	}

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_JOIN_LADDERTEAM );
		kPacket << m_LadderTeamInfo.m_dwTeamIndex << false << pEdit->GetText();     // 팀 인덱스 << 초대 << 비번 
		TCPNetwork::SendToServer( kPacket );
	}
	HideWnd();
}

void LadderPasswordWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 125, iYPos + 55, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
LadderCreateWnd::LadderCreateWnd(): m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
	m_dwJoinGuildIndex = 0;
	m_iModeIndex       = -1;
}

LadderCreateWnd::~LadderCreateWnd()
{

}

void LadderCreateWnd::iwm_show()
{
	if( g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetLadderTeamLimitGrade() )
	{
		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( g_LadderTeamMgr.GetLadderTeamLimitGrade(), szGradeName, sizeof( szGradeName ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szGradeName );
		HideWnd();
		return;
	}

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	//
	m_pPWEdit = FindChildWnd( ID_EDIT_PW );
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}

	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}
	
	SetGuildTeam( false );

	m_iModeIndex       = -1;
	ioWnd *pModeBtn = FindChildWnd( ID_CHANGE_MODE );
	if( pModeBtn )
	{
		pModeBtn->SetTitleText( g_ModeSelectInfo.GetLadderTeamModeTitle( m_iModeIndex ).c_str()  );
	}
}

void LadderCreateWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void LadderCreateWnd::CreateLadderTeam()
{
	if( g_App.IsMouseBusy() )
		return;
	
	if( m_iModeIndex == -2 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		return;
	}

	if( g_MyInfo.GetUserCampPos() == 0 )
	{		
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		HideWnd();
		return;
	}	

	if( g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetLadderTeamLimitGrade() )
	{
		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( g_LadderTeamMgr.GetLadderTeamLimitGrade(), szGradeName, sizeof( szGradeName ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), szGradeName );
		HideWnd();
		return;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		HideWnd();
		return;
	}

	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		HideWnd();
		return;
	}

	if( g_CampMgr.IsCloseCampBattle() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), g_CampMgr.GetPlayPreceedMent().c_str() );
		HideWnd();
		return;
	}

	if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		HideWnd();
		return;
	}

	if( m_dwJoinGuildIndex != 0 )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData )
		{
			SetGuildTeam( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			return;
		}
	}

	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
	if( pEdit && pPWEdit )
	{
		char szName[MAX_PATH] = "";
		if( strlen( pEdit->GetText() ) == 0 )
			sprintf( szName, "%s", g_LadderTeamMgr.GetRandomName().c_str() );
		else
			sprintf( szName, "%s", pEdit->GetText() );
		char szPassword[MAX_PATH] = "";
		sprintf( szPassword, "%s", pPWEdit->GetText() );

		SP2Packet kPacket( CTPK_CREATE_LADDERTEAM );
		kPacket << m_iModeIndex << g_SlangUtil.ConvertString( szName ) << szPassword 
				<< g_ModeSelectInfo.GetLadderTeamDefaultPlayer( m_iModeIndex ) << m_dwJoinGuildIndex
				<< g_ModeSelectInfo.IsLadderTeamHeroMatchMode( m_iModeIndex );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
		HideWnd();
	}			
}

void LadderCreateWnd::SetGuildTeam( bool bGuildTeam )
{
	if( bGuildTeam )
	{
		ioGuildData*pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData )
		{
			m_dwJoinGuildIndex = 0;
			g_GUIMgr.SetMsgBox( MB_OK, this, STR(1) );
			SetGuildTeam( false );
			return;
		}

		m_dwJoinGuildIndex = g_GuildInfo.GetGuildIndex();

		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
		if( pEdit && pPWEdit )
		{
			g_InputBox.SetString("");
			pEdit->KillKeyFocus();
			char szName[MAX_PATH] = "";
			sprintf( szName, "%s", pGuildData->GetGuildName().c_str() );
			pEdit->SetText( szName );

			pPWEdit->ClearString();
			pPWEdit->KillKeyFocus();
		}			
	}
	else
	{
		m_dwJoinGuildIndex = 0;
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
		if( pPWEdit )
		{
			pPWEdit->ClearString();
			pPWEdit->SetKeyFocus();
			g_InputBox.SetString("");
		}
		if( pEdit )
		{
			pEdit->ClearString();
			pEdit->SetKeyFocus();
			g_InputBox.SetString("");
		}
	}

	ioCheckButton *pGuildTeam = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_GUILD_TEAM ) );
	if( pGuildTeam )
		pGuildTeam->SetCheck( bGuildTeam );
}

void LadderCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_GUILD_TEAM:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwJoinGuildIndex == 0 )
				SetGuildTeam( true );
			else
				SetGuildTeam( false );
		}
		break;
	case ID_CREATE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			CreateLadderTeam();
		}
		break;
	case ID_CHANGE_MODE:
		if( cmd == IOBN_BTNUP )
		{			
			CreateModeSelectList *pModeList = dynamic_cast<CreateModeSelectList *>(FindChildWnd( ID_MODE_LIST ));
			if( pModeList )
			{
				pModeList->OpenList( pWnd );
				g_ModeSelectInfo.GetLadderCreateModeList( pModeList );
			}
		}
		else if( cmd == ID_MODE_LIST )
		{			
			m_iModeIndex = param;
			pWnd->SetTitleText( g_ModeSelectInfo.GetLadderTeamModeTitle( m_iModeIndex ).c_str()  );
			SetChildWndStyleRemove( ID_CREATE, IWS_EXACTIVE );
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				CreateLadderTeam();	
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( m_pNameEdit )
			{
				if( m_dwJoinGuildIndex != 0 )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->KillKeyFocus();
					g_GUIMgr.SetMsgBox( MB_OK, this, STR(1) );
					return;
				}
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				CreateLadderTeam();
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( m_pPWEdit )
			{
				if( m_dwJoinGuildIndex != 0 )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->KillKeyFocus();
					g_GUIMgr.SetMsgBox( MB_OK, this, STR(2) );
					return;
				}
			}
		}
		break;
	}
}

void LadderCreateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 130, FONT_SIZE_12, STR(3) );
}

bool LadderCreateWnd::QuestGuide( int iModeIndex, float &rfXPos, float &rfYPos )
{
	// 모드 체크
	if( iModeIndex != m_iModeIndex )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MODE );
		CreateModeSelectList *pModeList = dynamic_cast<CreateModeSelectList *>(FindChildWnd( ID_MODE_LIST ));
		if( pModeList && pModeList->IsShow() )
		{
			if( pModeList->GetOpenBtn() == pSelectBtn )
			{
				if( pModeList->GetIndexToXYPos( iModeIndex, rfXPos, rfYPos ) )
					return true;
			}
		}
		else if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;			
		}
		return false;
	}

	// 생성 버튼
	ioWnd *pCreateBtn = FindChildWnd( ID_CREATE );
	if( pCreateBtn && pCreateBtn->IsShow() )
	{
		rfXPos = pCreateBtn->GetDerivedPosX() + ( pCreateBtn->GetWidth() / 2 );
		rfYPos = pCreateBtn->GetDerivedPosY();
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
LadderOwnerWnd::LadderOwnerWnd() : m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
	m_dwJoinGuildIndex  = 0;
}

LadderOwnerWnd::~LadderOwnerWnd()
{

}

void LadderOwnerWnd::iwm_show()
{
	m_pPreEdit  = ioEdit::m_pKeyFocusEdit;
	m_pPWEdit   = FindChildWnd( ID_EDIT_PW );
	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );

	m_szLadderName		= g_LadderTeamMgr.GetLadderTeamName();
	m_szLadderPW		= g_LadderTeamMgr.GetLadderTeamPW();
	m_dwJoinGuildIndex  = g_LadderTeamMgr.GetJoinGuildIndex();
	if( m_dwJoinGuildIndex == 0 )
		SetGuildTeam( false );
	else
		SetGuildTeam( true );
}

void LadderOwnerWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void LadderOwnerWnd::SetGuildTeam( bool bGuildTeam )
{
	if( bGuildTeam )
	{
		ioGuildData*pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData )
		{
			m_dwJoinGuildIndex = 0;
			g_GUIMgr.SetMsgBox( MB_OK, this, STR(1) );
			SetGuildTeam( false );
			return;
		}

		m_dwJoinGuildIndex = g_GuildInfo.GetGuildIndex();

		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
		if( pEdit && pPWEdit )
		{
			g_InputBox.SetString("");
			pEdit->KillKeyFocus();
			char szName[MAX_PATH] = "";
			sprintf( szName, "%s", pGuildData->GetGuildName().c_str() );
			pEdit->SetText( szName );

			pPWEdit->ClearString();
			pPWEdit->KillKeyFocus();
		}			
	}
	else
	{
		m_dwJoinGuildIndex = 0;
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
		if( pPWEdit )
		{
			pPWEdit->ClearString();
			pPWEdit->SetKeyFocus();
			g_InputBox.SetString( m_szLadderPW.c_str() );
		}
		if( pEdit )
		{
			pEdit->ClearString();
			pEdit->SetKeyFocus();
			g_InputBox.SetString( m_szLadderName.c_str() );
		}
	}

	ioCheckButton *pGuildTeam = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_GUILD_TEAM ) );
	if( pGuildTeam )
		pGuildTeam->SetCheck( bGuildTeam );
}

void LadderOwnerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_GUILD_TEAM:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwJoinGuildIndex == 0 )
				SetGuildTeam( true );
			else
				SetGuildTeam( false );
		}
		break;
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			ChangeLadderTeamInfo();
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				ChangeLadderTeamInfo();
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( m_pNameEdit )
			{
				if( m_dwJoinGuildIndex != 0 )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->KillKeyFocus();
					g_GUIMgr.SetMsgBox( MB_OK, this, STR(1) );
					return;
				}
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				ChangeLadderTeamInfo();
			}
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( m_pPWEdit )
			{
				if( m_dwJoinGuildIndex != 0 )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->KillKeyFocus();
					g_GUIMgr.SetMsgBox( MB_OK, this, STR(2) );
					return;
				}
			}
		}
		break;
	}
}

void LadderOwnerWnd::ChangeLadderTeamInfo()
{
	// 닉네임 변경
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_NAME );
	if( !pEdit ) return;

	ioHashString szName = g_SlangUtil.ConvertString( pEdit->GetText() );
	if( szName != g_LadderTeamMgr.GetLadderTeamName() )
	{
		if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
	}

	if( m_dwJoinGuildIndex != 0 )
	{
		ioGuildData*pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData )
		{
			SetGuildTeam( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}
	}

	// 비번 변경
	pEdit = (ioEdit*)FindChildWnd( ID_EDIT_PW );
	if( !pEdit ) return;

	ioHashString szPW = pEdit->GetText();
	if( m_szLadderName == szName && m_szLadderPW == szPW && m_dwJoinGuildIndex == g_LadderTeamMgr.GetJoinGuildIndex() )
	{
		HideWnd();
		return;
	}

	if( szName.IsEmpty() )
		szName = g_LadderTeamMgr.GetRandomName();

	m_szLadderName = szName;
	m_szLadderPW   = szPW;

	// 정보 변경
	SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
	kPacket << LADDERTEAM_MACRO_CHANGE_INFO << szName << szPW << m_dwJoinGuildIndex;
	TCPNetwork::SendToServer( kPacket );

	if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
	{
		// 플레이중에 팀 이름을 변경하면 상대팀에 알린다.
		if( szName != g_LadderTeamMgr.GetLadderTeamName() )
		{
			const LadderTeamInfo &rkOwnerTeam = g_LadderTeamMgr.GetLadderOtherTeamInfo();
			if( rkOwnerTeam.m_dwTeamIndex != 0 )
			{
				SP2Packet kPacket( CTPK_LADDER_OTHER_NAME_CHANGE );
				kPacket << rkOwnerTeam.m_dwTeamIndex << szName;
				TCPNetwork::SendToServer( kPacket );
			}
		}
	}

	g_LadderTeamMgr.SetLadderTeamName( szName );
	g_LadderTeamMgr.SetLadderTeamPW( szPW );
	g_LadderTeamMgr.SetJoinGuildIndex( m_dwJoinGuildIndex );
	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
	if( pLadderBattleWnd )
		pLadderBattleWnd->UpdateLadderBattle();
	HideWnd();
}

void LadderOwnerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
}
//////////////////////////////////////////////////////////////////////////
LadderBattleMemberList::LadderBattleMemberList()
{
	m_pBlueNor		= NULL;
	m_pBlueOver		= NULL;
	m_pRedNor		= NULL;
	m_pRedOver		= NULL;
	m_pCaptainBack	= NULL;

	m_pBlueNorGra		= NULL;
	m_pBlueOverGra		= NULL;
	m_pRedNorGra		= NULL;
	m_pRedOverGra		= NULL;
	m_pGuildMarkLine	= NULL;
	m_pBlueEmpty		= NULL;
	m_pRedEmpty			= NULL;

	m_pCurrentNor		= NULL;
	m_pCurrentOver		= NULL;
	m_pCurrentNorGra	= NULL;
	m_pCurrentOverGra	= NULL;
	m_pCurrentEmptyX    = NULL;

	m_pLoadCharBack     = NULL;
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		m_pCharLoading[i] = NULL;

	m_dwCharLoadTime = 0;
	m_iCharLoadArray = 0;

	m_iListArray	 = 0;
	m_eTeamType      = TEAM_NONE;

	m_dwCharMotionItem = 0;
	m_pUI3DRender    = NULL;
	m_aMyChar	     = NULL;
}

LadderBattleMemberList::~LadderBattleMemberList()
{
	SAFEDELETE( m_pBlueNor );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedNor );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pCaptainBack );

	SAFEDELETE( m_pBlueNorGra );
	SAFEDELETE( m_pBlueOverGra );
	SAFEDELETE( m_pRedNorGra );
	SAFEDELETE( m_pRedOverGra );
	SAFEDELETE( m_pGuildMarkLine );
	SAFEDELETE( m_pBlueEmpty );
	SAFEDELETE( m_pRedEmpty );

	SAFEDELETE( m_pLoadCharBack );
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		SAFEDELETE( m_pCharLoading[i] );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void LadderBattleMemberList::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );

	g_GUIMgr.AddWnd( "XML/chatbubblewnd.xml", this );

	m_PingStep.Create();
}

void LadderBattleMemberList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNorGra" )
	{
		SAFEDELETE( m_pBlueNorGra );
		m_pBlueNorGra = pImage;
	}
	else if( szType == "BlueOverGra" )
	{
		SAFEDELETE( m_pBlueOverGra );
		m_pBlueOverGra = pImage;
	}
	else if( szType == "RedNorGra" )
	{
		SAFEDELETE( m_pRedNorGra );
		m_pRedNorGra = pImage;
	}
	else if( szType == "RedOverGra" )
	{
		SAFEDELETE( m_pRedOverGra );
		m_pRedOverGra = pImage;
	}
	else if( szType == "GuildMarkLine" )
	{
		SAFEDELETE( m_pGuildMarkLine );
		m_pGuildMarkLine = pImage;
	}
	else if( szType == "BlueEmpty" )
	{
		SAFEDELETE( m_pBlueEmpty );
		m_pBlueEmpty = pImage;
	}
	else if( szType == "RedEmpty" )
	{
		SAFEDELETE( m_pRedEmpty );
		m_pRedEmpty = pImage;
	}
	else if( szType == "LoadCharBack" )
	{
		SAFEDELETE( m_pLoadCharBack );
		m_pLoadCharBack = pImage;
	}
	else if( szType == "CharLoading1" )
	{
		SAFEDELETE( m_pCharLoading[0] );
		m_pCharLoading[0] = pImage;
	}
	else if( szType == "CharLoading2" )
	{
		SAFEDELETE( m_pCharLoading[1] );
		m_pCharLoading[1] = pImage;
	}
	else if( szType == "CharLoading3" )
	{
		SAFEDELETE( m_pCharLoading[2] );
		m_pCharLoading[2] = pImage;
	}
	else if( szType == "CharLoading4" )
	{
		SAFEDELETE( m_pCharLoading[3] );
		m_pCharLoading[3] = pImage;
	}
	else if( szType == "CharLoading5" )
	{
		SAFEDELETE( m_pCharLoading[4] );
		m_pCharLoading[4] = pImage;
	}
	else if( szType == "CharLoading6" )
	{
		SAFEDELETE( m_pCharLoading[5] );
		m_pCharLoading[5] = pImage;
	}
	else if( szType == "CharLoading7" )
	{
		SAFEDELETE( m_pCharLoading[6] );
		m_pCharLoading[6] = pImage;
	}
	else if( szType == "CharLoading8" )
	{
		SAFEDELETE( m_pCharLoading[7] );
		m_pCharLoading[7] = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void LadderBattleMemberList::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueNor" )
	{
		SAFEDELETE( m_pBlueNor );
		m_pBlueNor = pFrame;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pFrame;
	}
	else if( szType == "RedNor" )
	{
		SAFEDELETE( m_pRedNor );
		m_pRedNor = pFrame;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pFrame;
	}
	else if( szType == "CaptainBack" )
	{
		SAFEDELETE( m_pCaptainBack );
		m_pCaptainBack = pFrame;
	}
	else
		ioButton::AddRenderFrame( szType, pFrame );
}

void LadderBattleMemberList::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd )
		pChatBubbleWnd->StartChatBubble( szID, szChat, bWholeChat );
}

bool LadderBattleMemberList::IsChangeChatBubble()
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd && pChatBubbleWnd->IsShow() )
	{
		if( GetListName() != pChatBubbleWnd->GetBubbleName() )
			return true;
	}
	return false;    	
}

void LadderBattleMemberList::ClearChatBubble()
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd && pChatBubbleWnd->IsShow() )
	{
		if( GetListName() != pChatBubbleWnd->GetBubbleName() )
		{	
			pChatBubbleWnd->HideWnd();
		}
	}
}

void LadderBattleMemberList::ChangeChatBubble( LadderBattleMemberList *pMemberList )
{
	if( !pMemberList ) return;

	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd )
	{
		ChatBubbleWnd *pTempChatBubble = dynamic_cast<ChatBubbleWnd*>(pMemberList->FindChildWnd( ID_CHAT_BUBBLE ));
		if( pTempChatBubble )
		{
			if( GetListName() == pTempChatBubble->GetBubbleName() )
				pChatBubbleWnd->ChangeChatBubble( pTempChatBubble );
		}
	}
}

void LadderBattleMemberList::CreateCharacter()
{
	SAFEDELETE( m_aMyChar );

	m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex );
	if( m_CharInfo.m_class_type == 0 )
		return;

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( m_CharInfo, "ResultChar" );
	if( !pChar )	return;

	pChar->SetPosition( 0.0f, -FLOAT90, 900.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (레더방 대기시)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = m_CharInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && m_LadderUserInfo.m_szUserID != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			DWORD dwItemCode = g_GlobalSoldierInfo.GetSoldierItemCode( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex, i );
			DWORD dwCostumeMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex, i );
					dwCostumeFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex, i );
				}
				pChar->EquipItem( dwItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
		}
	}

	m_dwCharMotionItem = m_LadderUserInfo.m_dwCharMotionItem;
	ioHashString kMotionAni;
	ioEtcItemMotion *pMotionItem = dynamic_cast< ioEtcItemMotion * > ( g_EtcItemMgr.FindEtcItem( m_dwCharMotionItem ) );
	if( pMotionItem )
	{
		if( m_aMyChar->IsMale() )
			kMotionAni = pMotionItem->GetHumanManMotion();
		else
			kMotionAni = pMotionItem->GetHumanWomanMotion();
	}

	if( kMotionAni.IsEmpty() )
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
		m_aMyChar->SetMotionItemAni( kMotionAni );
	}
}

void LadderBattleMemberList::MemberInfo( int iListArray, TeamType eTeam, const LadderTeamInfo &rkTeamInfo, const LadderTeamUserInfo &rkUserInfo )
{
	m_iListArray     = iListArray;
	m_eTeamType      = eTeam;
	m_TeamInfo       = rkTeamInfo;
	m_LadderUserInfo = rkUserInfo;	

	if( m_eTeamType == TEAM_BLUE )
	{
		m_pCurrentNor		= m_pBlueNor;
		m_pCurrentOver		= m_pBlueOver;
		m_pCurrentNorGra	= m_pBlueNorGra;
		m_pCurrentOverGra	= m_pBlueOverGra;
		m_pCurrentEmptyX    = m_pBlueEmpty;
	}
	else
	{
		m_pCurrentNor		= m_pRedNor;
		m_pCurrentOver		= m_pRedOver;
		m_pCurrentNorGra	= m_pRedNorGra;
		m_pCurrentOverGra	= m_pRedOverGra;
		m_pCurrentEmptyX    = m_pRedEmpty;
	}

	if( m_CharInfo != g_GlobalSoldierInfo.GetSoldierData( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex ) ||
		m_dwCharMotionItem != m_LadderUserInfo.m_dwCharMotionItem )
	{
		CreateCharacter();
	}
}

void LadderBattleMemberList::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_LadderUserInfo.m_szUserID.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
		{
			// 상태 캐릭터가 보이므로 실시간 동기화
			CHARACTER kChar         = g_GlobalSoldierInfo.GetSoldierData( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex );
			UserInfoNode *pUserInfo = g_UserInfoMgr.GetUserInfoNode( m_LadderUserInfo.m_szUserID );
			if( pUserInfo )
			{
				if( pUserInfo->GetClassType() != kChar.m_class_type )
					pUserInfo->SetForceUDPSync( true );
			}
			pMouse->SetToolTipID( m_LadderUserInfo.m_szUserID, UserInfoToolTip::TT_GLOBAL );
		}
	}	

	if( FRAMEGETTIME() - m_dwCharLoadTime > 100 )
	{
		m_dwCharLoadTime = FRAMEGETTIME();
		m_iCharLoadArray++;
		if( m_iCharLoadArray >= MAX_CHAR_LODING )
			m_iCharLoadArray = 0;
	}

	if( m_CharInfo != g_GlobalSoldierInfo.GetSoldierData( m_LadderUserInfo.m_szUserID, m_LadderUserInfo.m_dwCharIndex ) )
	{
		CreateCharacter();
	}

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void LadderBattleMemberList::OnRenderSoldier( int iXPos, int iYPos )
{
	if( m_LadderUserInfo.m_szUserID.IsEmpty() ) return;
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

void LadderBattleMemberList::OnRenderFrame( int iXPos, int iYPos )
{
	if( !m_pCurrentNor || !m_pCurrentOver )
		return;

	if( m_bOver || m_bClicked || g_MyInfo.GetPublicID() == m_LadderUserInfo.m_szUserID )
	{
		m_pCurrentOver->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pCurrentOverGra->SetSize( 82, 55 );
		m_pCurrentOverGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCurrentOverGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCurrentOverGra->Render( iXPos + 3, iYPos + 188 );
	}	
	else
	{
		m_pCurrentNor->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pCurrentNorGra->SetSize( 82, 55 );
		m_pCurrentNorGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCurrentNorGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCurrentNorGra->Render( iXPos + 3, iYPos + 188 );
	}
}

void LadderBattleMemberList::OnRenderLoadingChar( int iXPos, int iYPos )
{
	if( m_LadderUserInfo.m_szUserID.IsEmpty() ) return;
	if( !COMPARE( m_iCharLoadArray, 0, MAX_CHAR_LODING ) )
		return;

	if( m_pLoadCharBack )
		m_pLoadCharBack->Render( iXPos, iYPos );

	if( m_pCharLoading[m_iCharLoadArray] )
		m_pCharLoading[m_iCharLoadArray]->Render( iXPos + 32, iYPos + 111 );
}

void LadderBattleMemberList::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderFrame( iXPos, iYPos );

	if( m_iListArray > m_TeamInfo.m_iMaxPlayer )
	{
		if( m_pCurrentEmptyX )
			m_pCurrentEmptyX->Render( iXPos + 18, iYPos + 97, UI_RENDER_MULTIPLY );
	}
	else if( m_LadderUserInfo.m_szUserID.IsEmpty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_eTeamType == TEAM_RED )
		{
			g_FontMgr.SetTextColor( 204, 69, 52 );
		}
		else
		{
			g_FontMgr.SetTextColor( 60, 126, 207 );
		}
		g_FontMgr.PrintText( iXPos + 44, iYPos + 212, FONT_SIZE_11, STR(1), m_iListArray );
		g_FontMgr.PrintText( iXPos + 44, iYPos + 227, FONT_SIZE_11, "-" );
	}
	else
	{
		if( m_LadderUserInfo.m_szUserID == m_TeamInfo.m_szOwnerName )
		{				
			if( m_pCaptainBack )
				m_pCaptainBack->Render( iXPos, iYPos );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_11, STR(2) );
		}			

		enum{ GRADE_GAP = 17, GUILD_MARK_GAP = 17, };
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_LadderUserInfo.m_szUserID, dwGuildIndex, dwGuildMark );
		int iGradeLineSize = GRADE_GAP;
		if( dwGuildIndex > 0 )           // 길드가 있으면
		{
			iGradeLineSize += GUILD_MARK_GAP;
		}		

		int iStartXPos = 44 - iGradeLineSize / 2;
		g_LevelMgr.RenderGrade( m_LadderUserInfo.m_szUserID, iXPos + iStartXPos, iYPos + 193, m_LadderUserInfo.m_iGradeLevel, TEAM_PRIVATE_2 );
		iStartXPos += GRADE_GAP;
		if( dwGuildIndex > 0 )   
		{
			if( m_pGuildMarkLine )
				m_pGuildMarkLine->Render( iXPos + iStartXPos, iYPos + 193 );
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartXPos + 2, iYPos + 195 );			
			iStartXPos += GUILD_MARK_GAP;
		}

		//닉네임
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		if( m_eTeamType == TEAM_BLUE )
		{
			if( m_bClicked || m_bOver )
				g_FontMgr.SetBkColor( 50, 118, 176 );
			else
				g_FontMgr.SetBkColor( 27, 71, 128 );
		}
		else
		{
			if( m_bClicked || m_bOver )
				g_FontMgr.SetBkColor( 172, 62, 42 );
			else
				g_FontMgr.SetBkColor( 126, 34, 22 );
		}
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 44, iYPos + 210, FONT_SIZE_11, 80.0f, m_LadderUserInfo.m_szUserID.c_str() );

		char szConvertText[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_LadderUserInfo.m_iLadderPoint, szConvertText, sizeof( szConvertText ) );
		g_FontMgr.PrintTextWidthCut( iXPos + 44, iYPos + 225, FONT_SIZE_11, 80.0f, STR(3), szConvertText );

		OnRenderPing( iXPos + 59, iYPos + 5 );
	}
}

void LadderBattleMemberList::OnRenderPing( int iXPos, int iYPos )
{
	bool bServerRelay = false;
	int  iPingStep = 0;
	if( m_LadderUserInfo.m_szUserID == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( m_LadderUserInfo.m_szUserID ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( m_LadderUserInfo.m_szUserID ) )
			bServerRelay = true;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}
//////////////////////////////////////////////////////////////////////////
LadderModeNMapBtn::LadderModeNMapBtn()
{
}

LadderModeNMapBtn::~LadderModeNMapBtn()
{
}

void LadderModeNMapBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	switch( GetID() )
	{
	case LadderBattleWnd::ID_CHANGE_MODE:
		OnModeRender( iXPos, iYPos );
		break;
	case LadderBattleWnd::ID_CHANGE_MAP:
		OnMapRender( iXPos, iYPos );
		break;
	}

}

void LadderModeNMapBtn::OnModeRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kModeTitle = g_ModeSelectInfo.GetLadderTeamModeTitle( g_LadderTeamMgr.GetSelectModeIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kModeTitle.c_str() );

	ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetLadderTeamModeOrderMarkSmall( g_LadderTeamMgr.GetSelectModeIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kModeTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}
}

void LadderModeNMapBtn::OnMapRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kMapTitle = g_ModeSelectInfo.GetLadderTeamMapTitle( g_LadderTeamMgr.GetSelectModeIndex(), g_LadderTeamMgr.GetSelectMapIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

	ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_LadderTeamMgr.GetSelectModeIndex(), g_LadderTeamMgr.GetSelectMapIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}
}
//////////////////////////////////////////////////////////////////////////
LadderChatWnd::LadderChatWnd()
{
	m_eChatType = CT_LADDER;
}

LadderChatWnd::~LadderChatWnd()
{
}

void LadderChatWnd::SetPlayStage( ioPlayStage *pPlayStage, ChatType eType )
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetPlayStage( pPlayStage );

		m_eChatType = eType;
		pGameChatInput->SetChatTypeToBtn( m_eChatType );
	}
}

void LadderChatWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szLoadInfo = xElement.GetStringAttribute_e( "LoadInfo" );	
}

void LadderChatWnd::iwm_show()
{
	g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );

	g_ChatMgr.LoadChatInfo( m_szLoadInfo );

	ShowChildWnd( ID_CHAT_MSG_WND );
	ShowChildWnd( ID_CHAT_INPUT_WND );

	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetFirstChatTypeToBtn( m_eChatType );
	}

	g_ChatMgr.SetMsgWndFeature( MSF_NORMAL_CHAT | MSF_SYSTEM_CHAT | MSF_WHOLE_CHAT );
}

void LadderChatWnd::iwm_hide()
{
	HideChildWnd( ID_CHAT_MSG_WND );
	HideChildWnd( ID_CHAT_INPUT_WND );
}

bool LadderChatWnd::iwm_esc()
{	
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
		return pGameChatInput->iwm_esc();
	return false;
}

void LadderChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHAT_INPUT_WND:
		if( cmd == IOBN_BTNUP )
		{
			ChatMsgWnd *pChatWnd = dynamic_cast<ChatMsgWnd*>( FindChildWnd( ID_CHAT_MSG_WND ) );
			if( pChatWnd )
			{
				pChatWnd->InitScrollPos();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
LadderUserOptionWnd::LadderUserOptionWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iOverListIndex = -1;
	m_eTeamType = TEAM_BLUE;
	m_iSlotArray= 0;
}

LadderUserOptionWnd::~LadderUserOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void LadderUserOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void LadderUserOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}

}

void LadderUserOptionWnd::CommandOption( DWORD dwCmd )
{
	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szName.c_str(), true );
		}
		break;
	case OPTION_KICK:
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
				return;
			}
			
			if( g_LadderTeamMgr.IsSearching() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
				return;
			}

			if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );			
				return;
			}

			if( g_MyInfo.GetPublicID() == m_szName )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );			
				return;
			}

			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );			
				return;
			}
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(6), m_szName.c_str() );	
		}
		break;
	case OPTION_CLOSE:
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );			
				return;
			}

			if( g_LadderTeamMgr.IsSearching() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );			
				return;
			}

			if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );			
				return;
			}

			int iMaxPlayerTeam = g_LadderTeamMgr.GetMaxPlayer();
			int iMaxPlayer	   = m_iSlotArray - 1;
			if( iMaxPlayer > iMaxPlayerTeam )
				return;

			int iMinimumPlayer = g_ModeSelectInfo.GetLadderTeamMinimumPlayer( g_LadderTeamMgr.GetSelectModeIndex() );
			if( iMaxPlayer < iMinimumPlayer )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), iMinimumPlayer );
				return;
			}

			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_MAX_PLAYER << iMaxPlayer;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	case OPTION_OPEN:
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );			
				return;
			}

			if( g_LadderTeamMgr.IsSearching() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );			
				return;
			}

			if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );			
				return;
			}

			int iMaxPlayerTeam = g_LadderTeamMgr.GetMaxPlayer();
			int iMaxPlayer     = m_iSlotArray;
			if( iMaxPlayer <= iMaxPlayerTeam )
				return;

			int iMinimumPlayer = g_ModeSelectInfo.GetLadderTeamMinimumPlayer( g_LadderTeamMgr.GetSelectModeIndex() );
			if( iMaxPlayer < iMinimumPlayer )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14), iMinimumPlayer );
				return;
			}

			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_MAX_PLAYER << iMaxPlayer;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	}
}

void LadderUserOptionWnd::SetOptionInfo( const ioHashString &rkName, TeamType eTeam, int iSlotArray )
{
	HideWnd();
	/*
	-내가 방장일 경우
	ㄴ 자기클릭 - 자기정보
	ㄴ 사람클릭 - 사람정보 / 강퇴
	ㄴ 빈곳클릭 - 닫기 / 팀변경
	ㄴ 엑스클릭 - 열기 or '해골영웅 1층(쉬움)은 2명까지만 함께 플레이가 가능합니다.' 팝업 띄우기

	-내가 일반일 경우
	ㄴ 자기클릭 - 자기정보
	ㄴ 사람클릭 - 사람정보
	ㄴ 빈곳클릭 - 팀변경
	ㄴ 엑스클릭 - 동작X - 팝업 띄우기

	-명칭
	[누구누구 정보]
	[강제퇴장 시키기]
	[슬롯 닫기]
	[레드(블루)팀으로 변경]
	[슬롯 열기]

	-옵션 명령이 하나밖에 없으면 즉시 실행한다.
	*/
	m_OptionMap.clear();

	m_szName	= rkName;
	m_eTeamType = eTeam;
	m_iSlotArray= iSlotArray;

	// 방장
	if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
	{
		if( rkName.IsEmpty() )
		{
			if( m_iSlotArray > g_LadderTeamMgr. GetMaxPlayer() )
				m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(1) ) );
			else
			{
				// 닫기
				if( m_iSlotArray > 1 )
					m_OptionMap.insert( OptionMap::value_type( OPTION_CLOSE, STR(2) ) );
			}
		}
		else if( rkName == g_MyInfo.GetPublicID() )
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(3) ) );
		}
		else
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4), m_szName.c_str() );
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );

			//강퇴
			if( !g_App.IsAdminID( m_szName.c_str() ) )
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_KICK, STR(5) ) );
			}
		}
	}
	else    //일반 - 즉시 실행되므로 타이틀에 신경쓰지 말자.
	{
		if( rkName.IsEmpty() )
		{	
			m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(6) ) );
		}
		else
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(7) ) );
		}
	}

	if( m_OptionMap.empty() )
		return;

	if( m_OptionMap.size() == 1 )
	{
		//즉시 실행
		OptionMap::iterator iCreate = m_OptionMap.begin();
		if( GetParent() )
		{
			CommandOption( iCreate->first );
		}
	}
	else
	{
		// 윈도우 사이즈 결정
		int iListWidth = 0;
		OptionMap::iterator iter;
		for( iter = m_OptionMap.begin() ; iter != m_OptionMap.end() ; ++iter )
		{
			int iTextWidth = g_FontMgr.GetTextWidth( iter->second.c_str(), TS_NORMAL, FONT_SIZE_12 );
			if( iListWidth < iTextWidth )
				iListWidth = iTextWidth;
		}
		SetSize( iListWidth + m_iAddWidth + 10, (m_OptionMap.size() * m_iHeightGap) + m_iAddHeight );

		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && GetParent() )
		{
			int iOffsetX = GetParent()->GetXPos();
			int iOffsetY = GetParent()->GetYPos();
			if( pMouse->GetMousePos().x + GetWidth() > Setting::Width() )
				SetWndPos( ( pMouse->GetMousePos().x - GetWidth() ) - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
			else
				SetWndPos( pMouse->GetMousePos().x - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
		}
		ShowWnd();
	}
}

void LadderUserOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void LadderUserOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void LadderUserOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( g_MyInfo.GetPublicID() == m_szName )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
					return;
				}

				if( g_App.IsAdminID( m_szName.c_str() ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
					return;
				}		

				if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
					kPacket << LADDERTEAM_MACRO_KICK_OUT << m_szName;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	}
}

void LadderUserOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	OptionMap::iterator iter = m_OptionMap.begin();
	for(int iIndex = 0 ; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( GetWidth() - m_iAddWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex  = iIndex;
			if( pMouse->IsLBtnUp() )
			{
				CommandOption( iter->first );
				HideWnd();
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void LadderUserOptionWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	OptionMap::iterator iter = m_OptionMap.begin();
	for( int iIndex = 0; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		bDarkRender = !bDarkRender;

		if( m_iOverListIndex == iIndex )
		{
			m_pOverLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, iter->second.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
LadderBattleWnd::LadderBattleWnd() : m_pPlayStage( NULL )
{
	m_bOwner		  = false;
	m_dwRankingUpdateTime = 0;
	m_dwCurSearchTime = 0;
	m_dwFullSearchTime= 0;
	m_iCurSearchSec   = 0;

	m_pModeNMapGrayFrm = NULL;

	g_SoundMgr.StopStream();
	
	m_pWarCountCircle = NULL;
	m_pWarCountWait = NULL;
	m_pManualMark   = NULL;

	m_pTextBack = NULL;

	m_dwChangeCountTime = 0;

	m_iDefaultX   = 0;
	m_iDefaultY   = 0;

	m_pModeHelpTooltip = NULL;
	m_bModeTooltipOver = false;

	m_szCampName.Clear();
}

LadderBattleWnd::~LadderBattleWnd()
{
	SAFEDELETE( m_pModeNMapGrayFrm );
	SAFEDELETE( m_pManualMark );

	ClearMapMidImage();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "LadderBattleWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "LadderBattleWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void LadderBattleWnd::ClearMapMidImage()
{
	SAFEDELETE( m_pWarCountCircle );
	SAFEDELETE( m_pWarCountWait );

	SAFEDELETE( m_pTextBack );
}

void LadderBattleWnd::SetPlayStage( ioPlayStage *pPlayStage )
{ 
	LadderChatWnd *pLadderChat = dynamic_cast<LadderChatWnd*>( FindChildWnd( ID_LADDER_CHAT_WND ) );
	if( pLadderChat )
		pLadderChat->SetPlayStage( pPlayStage, CT_LADDER );

	m_pPlayStage = pPlayStage;
}

void LadderBattleWnd::UpdateLadderBattle()
{
	if( !g_LadderTeamMgr.IsLadderTeam() )
	{
		HideWnd();
		return;
	}

	SetChildWndStyleRemove( ID_SEARCH_START, IWS_EXACTIVE );
	SetChildWndStyleRemove( ID_SEARCH_END, IWS_EXACTIVE );
	SetChildWndStyleRemove( ID_LADDER_INVITE, IWS_EXACTIVE );

	m_bOwner = false;
	if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
	{
		m_bOwner = true;
		SetChildWndStyleRemove( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleAdd( ID_CHANGE_INFO, IWS_EXACTIVE );
	}

	const LadderTeamInfo kTeamInfo = g_LadderTeamMgr.GetLadderTeamInfo();
	TeamType eTeamType = TEAM_BLUE;
	if( kTeamInfo.m_iCampType == CAMP_RED )
		eTeamType = TEAM_RED;
	
	int i = 0;
	// 유저 정보 - 영웅전이 아닐때만 표시
	if( !g_LadderTeamMgr.IsHeroMatchMode() )
	{
		vWndList vChangeChatBubble;

		int iStartArray = 0;
		for(i = ID_USER_1;i <= ID_USER_8;i++,iStartArray++)
		{
			LadderBattleMemberList *pUserList = dynamic_cast<LadderBattleMemberList *>( FindChildWnd( i ) );
			if( pUserList )
			{
				if( iStartArray < kTeamInfo.m_iCurPlayer )
				{
					const LadderTeamUserInfo &kUserInfo = g_LadderTeamMgr.GetUserInfo( iStartArray );
					pUserList->MemberInfo( iStartArray + 1, eTeamType, kTeamInfo, kUserInfo );

					if( pUserList->IsChangeChatBubble() )
						vChangeChatBubble.push_back( pUserList );
				}
				else
				{
					LadderTeamUserInfo kDummyUserInfo;
					pUserList->MemberInfo( iStartArray + 1, eTeamType, kTeamInfo, kDummyUserInfo );

					if( pUserList->IsChangeChatBubble() )
						vChangeChatBubble.push_back( pUserList );
				}
			}
		}

		{	// Chat Bubble Change		
			for(i = 0;i < (int)vChangeChatBubble.size();i++)
			{
				LadderBattleMemberList *pUserList = dynamic_cast<LadderBattleMemberList *>(vChangeChatBubble[i]);
				if( !pUserList ) continue;

				for(int j = ID_USER_1;j <= ID_USER_8;j++)
				{
					LadderBattleMemberList *pUserBtn = dynamic_cast<LadderBattleMemberList *>( FindChildWnd( j ) );
					if( pUserBtn && pUserBtn != pUserList )
					{
						pUserBtn->ChangeChatBubble( pUserList );
					}
				}
			}
			vChangeChatBubble.clear();

			for(i = ID_USER_1;i <= ID_USER_8;i++)
			{
				LadderBattleMemberList *pUserBtn = dynamic_cast<LadderBattleMemberList *>( FindChildWnd( i ) );
				if( pUserBtn && pUserBtn->IsChangeChatBubble() )
				{
					pUserBtn->ClearChatBubble();
				}
			}
		}
	}	

	// 옵션
	if( kTeamInfo.m_szOwnerName == g_MyInfo.GetPublicID() )
	{
		ShowChildWnd( ID_SEARCH_START );
		HideChildWnd( ID_SEARCH_END );	
		SetChildWndStyleRemove( ID_SEARCH_START, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_LADDER_INVITE, IWS_EXACTIVE );

		if( g_LadderTeamMgr.IsSearching() )
		{
			HideChildWnd( ID_SEARCH_START );
			ShowChildWnd( ID_SEARCH_END );	
			SetChildWndStyleAdd( ID_LADDER_INVITE, IWS_EXACTIVE );
			g_GUIMgr.HideWnd( LADDER_TEAM_INVITE_LIST_WND );
		}
		else if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
		{
			SetChildWndStyleAdd( ID_SEARCH_START, IWS_EXACTIVE );
			SetChildWndStyleAdd( ID_LADDER_INVITE, IWS_EXACTIVE );
		}
		
		if( g_LadderTeamMgr.IsHeroMatchMode() )
		{
			HideChildWnd( ID_CHANGE_MODE );
			HideChildWnd( ID_CHANGE_MAP );
		}
		else
		{
			ShowChildWnd( ID_CHANGE_MODE );
			ShowChildWnd( ID_CHANGE_MAP );		
		}
	}
	else
	{
		ShowChildWnd( ID_SEARCH_START );
		HideChildWnd( ID_SEARCH_END );	
		SetChildWndStyleAdd( ID_SEARCH_START, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_LADDER_INVITE, IWS_EXACTIVE );

		if( g_LadderTeamMgr.IsSearching() )
		{
			HideChildWnd( ID_SEARCH_START );
			ShowChildWnd( ID_SEARCH_END );	
			SetChildWndStyleAdd( ID_LADDER_INVITE, IWS_EXACTIVE );
			g_GUIMgr.HideWnd( LADDER_TEAM_INVITE_LIST_WND );
		}
		else if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
		{
			ShowChildWnd( ID_SEARCH_START );
			HideChildWnd( ID_SEARCH_END );	
		}

		HideChildWnd( ID_CHANGE_MODE );
		HideChildWnd( ID_CHANGE_MAP );
	}

	// 
	ioCheckButton *pCheckBtn = (ioCheckButton*)FindChildWnd( ID_LEVEL_MATCH_SEARCH );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_LadderTeamMgr.IsSearchLevelMatch() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_SAME_USER_SEARCH );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_LadderTeamMgr.IsSearchSameUser() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_BAD_PING_KICK );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_LadderTeamMgr.IsBadPingKick() );
    
	if( g_LadderTeamMgr.IsLadderBattleModePlaying() )
	{
		HideChildWnd( ID_CHANGE_MODE );
		HideChildWnd( ID_CHANGE_MAP );

		SetChildWndStyleAdd( ID_SEARCH_START, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_SEARCH_END, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_LADDER_INVITE, IWS_EXACTIVE );
	}

	if( g_LadderTeamMgr.IsHeroMatchMode() )
	{
		SetChildWndStyleAdd( ID_LADDER_INVITE, IWS_EXACTIVE );
	}
}

void LadderBattleWnd::StartSearch( DWORD dwFullTime )
{
	if( dwFullTime == 0 ) return;

	m_dwCurSearchTime = FRAMEGETTIME();
	m_dwFullSearchTime= dwFullTime;
	m_iCurSearchSec   = (int)dwFullTime / 1000;

	m_dwChangeCountTime = FRAMEGETTIME();
}

void LadderBattleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szBackOgg = xElement.GetStringAttribute_e( "BackOGG" );
}

void LadderBattleWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ModeNMapGray" )
	{
		SAFEDELETE( m_pModeNMapGrayFrm );
		m_pModeNMapGrayFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void LadderBattleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "count_circle" )
	{
		SAFEDELETE( m_pWarCountCircle );
		m_pWarCountCircle = pImage;
	}
	else if( szType == "count_wait" )
	{
		SAFEDELETE( m_pWarCountWait );
		m_pWarCountWait = pImage;
	}
	else if( szType == "text_back" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void LadderBattleWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/ladderchatwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ladderbattleherownd.xml", this );

	int iXPos = g_GUIMgr.GUIPosLoadInt( "LadderBattleWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "LadderBattleWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}
	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void LadderBattleWnd::iwm_show()
{
	m_dwChangeCountTime = 0;
	g_GUIMgr.HideWnd( LADDER_TEAM_INVITED_WND );

	ShowChildWnd( ID_LADDER_CHAT_WND );
	HideChildWnd( ID_MODE_MAP_LIST );
	SetChildActive( ID_CHANGE_MODE );
	SetChildActive( ID_CHANGE_MAP );

	if( g_LadderTeamMgr.IsHeroMatchMode() )
	{
		for(int i = ID_USER_1;i <=ID_USER_8;i++)
			HideChildWnd( i );
		ShowChildWnd( ID_HERO_WND );
		HideChildWnd( ID_CHANGE_INFO );
		ShowChildWnd( ID_LEVEL_MATCH_SEARCH );
		HideChildWnd( ID_SAME_USER_SEARCH );
		ShowChildWnd( ID_OTHER_CHAT_HIDE );

		ioCheckButton *pCheckBtn = (ioCheckButton*)FindChildWnd( ID_OTHER_CHAT_HIDE );;
		if( pCheckBtn )
			pCheckBtn->SetCheck( g_LadderTeamMgr.IsOtherChatHide() );

		pCheckBtn = (ioCheckButton*)FindChildWnd( ID_LEVEL_MATCH_SEARCH );
		if( pCheckBtn )
			pCheckBtn->SetCheck( g_LadderTeamMgr.IsSearchLevelMatch() );

		ioWnd *pRankingBtn = FindChildWnd( ID_LADDER_RANKING );
		if( pRankingBtn )
			pRankingBtn->SetTitleText( STR(1) );
	}
	else
	{
		for(int i = ID_USER_1;i <=ID_USER_8;i++)
			ShowChildWnd( i );
		HideChildWnd( ID_HERO_WND );
		ShowChildWnd( ID_CHANGE_INFO );

		ShowChildWnd( ID_LEVEL_MATCH_SEARCH );
		ShowChildWnd( ID_SAME_USER_SEARCH );
		HideChildWnd( ID_OTHER_CHAT_HIDE );

		ioWnd *pRankingBtn = FindChildWnd( ID_LADDER_RANKING );
		if( pRankingBtn )
			pRankingBtn->SetTitleText( STR(2) );
	}

	UpdateLadderBattle();

	g_BackgroundMgr.SetBackground( BGT_LADDER );

	if( !m_szBackOgg.IsEmpty() )
	{
		g_SoundMgr.StopStream();

		char szName[MAX_PATH];
		sprintf( szName, "Resource/wave/%s", m_szBackOgg.c_str() );
		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament )
	{
		if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
		{
			m_szCampName = pTournament->GetCampName( CAMP_BLUE );
		}
		else
		{
			m_szCampName = pTournament->GetCampName( CAMP_RED );
		}
	}
}

void LadderBattleWnd::iwm_hide()
{
	HideChildWnd( ID_LADDER_CHAT_WND );
	g_GUIMgr.HideWnd( LADDER_TEAM_INVITE_LIST_WND );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}
}

bool LadderBattleWnd::iwm_esc()
{
	LadderChatWnd *pLadderChat = dynamic_cast<LadderChatWnd*>( FindChildWnd( ID_LADDER_CHAT_WND ) );
	if( pLadderChat && pLadderChat->IsShow() )
		return pLadderChat->iwm_esc();
	return false;
}

void LadderBattleWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
	}
}

void LadderBattleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_USER_1:
	case ID_USER_2:
	case ID_USER_3:
	case ID_USER_4:
	case ID_USER_5:
	case ID_USER_6:
	case ID_USER_7:
	case ID_USER_8:
		if( cmd == IOBN_BTNUP )
		{
			LadderBattleMemberList *pSlotWnd = dynamic_cast<LadderBattleMemberList*>(pWnd);
			if( pSlotWnd )
			{
				LadderUserOptionWnd *pUserOption = dynamic_cast<LadderUserOptionWnd*>( FindChildWnd( ID_USER_SLOT_OPTION ) );
				if( pUserOption )
					pUserOption->SetOptionInfo( pSlotWnd->GetListName(), pSlotWnd->GetTeam(), pSlotWnd->GetListArray() );
			}
		}
		break;
	case ID_CHANGE_MODE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.IsLadderBattleModePlaying() ) return;
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() ) return;		

			ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
			if( pModeList )
			{
				pModeList->OpenList( pWnd, true, g_LadderTeamMgr.GetSelectModeIndex(), false, pWnd->GetXPos(), pWnd->GetYPos() - 2 );
				g_ModeSelectInfo.GetLadderTeamModeUIList( pModeList, g_LadderTeamMgr.IsHeroMatchMode() );
			}
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			if( g_LadderTeamMgr.IsLadderBattleModePlaying() ) return;
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() ) return;		
			if( g_LadderTeamMgr.IsSearching() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );			
				return;
			}

			//모드가 변경되면 맵은 항상 랜덤이 시작이다.
			int iModeIndex = g_ModeSelectInfo.CheckLadderTeamModeIndex( (int)HIWORD( param ) );
			if( iModeIndex == g_LadderTeamMgr.GetSelectModeIndex() ) 
				return;

			g_LadderTeamMgr.SetSelectModeNMap( iModeIndex, -1 );

			SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_MODE_SEL << g_LadderTeamMgr.GetSelectModeIndex() << g_LadderTeamMgr.GetSelectMapIndex();
			TCPNetwork::SendToServer( kPacket );
			UpdateLadderBattle();
		}
		break;
	case ID_CHANGE_MAP:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.IsLadderBattleModePlaying() ) return;
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() ) return;

			//모드가 랜덤이면 맵은 변경 불가
			if( g_LadderTeamMgr.GetSelectModeIndex() == -1 )
				return;             

			ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
			if( pMapList )
			{
				pMapList->OpenList( pWnd, false, g_LadderTeamMgr.GetSelectMapIndex(), false, pWnd->GetXPos(), pWnd->GetYPos() - 2 );
				g_ModeSelectInfo.GetLadderTeamMapUIList( pMapList, g_LadderTeamMgr.GetSelectModeIndex() );
			}
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			if( g_LadderTeamMgr.IsLadderBattleModePlaying() ) return;
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() ) return;		
			if( g_LadderTeamMgr.IsSearching() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );			
				return;
			}

			//모드가 랜덤이면 맵은 변경 불가
			if( g_LadderTeamMgr.GetSelectModeIndex() == -1 )
				return;             

			int iMapIndex = (int)HIWORD( param );
			if( iMapIndex != -1 )
				iMapIndex = g_ModeSelectInfo.CheckLadderTeamMapIndex( g_LadderTeamMgr.GetSelectModeIndex(), iMapIndex );
			if( iMapIndex == g_LadderTeamMgr.GetSelectMapIndex() )
				return;

			g_LadderTeamMgr.SetSelectModeNMap( g_LadderTeamMgr.GetSelectModeIndex(), iMapIndex );

			SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_MODE_SEL << g_LadderTeamMgr.GetSelectModeIndex() << g_LadderTeamMgr.GetSelectMapIndex();
			TCPNetwork::SendToServer( kPacket );
			UpdateLadderBattle();
		}
		break;
	case ID_LADDER_INVITE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_LadderTeamMgr.IsHeroMatchMode() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );		
			else if( g_LadderTeamMgr.IsSearching() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );					
			else if( g_GUIMgr.IsShow( LADDER_TEAM_INVITE_LIST_WND ) )
				g_GUIMgr.HideWnd( LADDER_TEAM_INVITE_LIST_WND );
			else
				g_GUIMgr.ShowWnd( LADDER_TEAM_INVITE_LIST_WND );
		}
		break;
	case ID_CHANGE_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
				return;
			}
			g_GUIMgr.ShowWnd( LADDER_TEAM_OWNER_WND );
		}
		break;
	case ID_BATTLE_EXIT:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			g_App.AltF4ExitProcess( false );
		}
		break;
	case ID_LEVEL_MATCH_SEARCH:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
			{
				if( g_LadderTeamMgr.IsSearching() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );	
					UpdateLadderBattle();		
					return;
				}

				g_LadderTeamMgr.SetSearchLevelMatch( !g_LadderTeamMgr.IsSearchLevelMatch() );
				
                SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
				kPacket << LADDERTEAM_MACRO_CHANGE_OPTION << g_LadderTeamMgr.IsSearchLevelMatch() << g_LadderTeamMgr.IsSearchSameUser() << g_LadderTeamMgr.IsBadPingKick();
				TCPNetwork::SendToServer( kPacket );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}			
			UpdateLadderBattle();
		}
		break;
	case ID_SAME_USER_SEARCH:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
			{
				if( g_LadderTeamMgr.IsSearching() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );	
					UpdateLadderBattle();		
					return;
				}

				g_LadderTeamMgr.SetSearchSameUser( !g_LadderTeamMgr.IsSearchSameUser() );

				SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
				kPacket << LADDERTEAM_MACRO_CHANGE_OPTION << g_LadderTeamMgr.IsSearchLevelMatch() << g_LadderTeamMgr.IsSearchSameUser() << g_LadderTeamMgr.IsBadPingKick();
				TCPNetwork::SendToServer( kPacket );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}			
			UpdateLadderBattle();
		}
		break;
	case ID_OTHER_CHAT_HIDE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
			{
				g_LadderTeamMgr.SetOtherChatHide( !g_LadderTeamMgr.IsOtherChatHide() );

				ioCheckButton *pCheckBtn = (ioCheckButton*)(pWnd);
				if( pCheckBtn )
					pCheckBtn->SetCheck( g_LadderTeamMgr.IsOtherChatHide() );
			}
		}
		break;
	case ID_BAD_PING_KICK:
		if( cmd == IOBN_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
			{
				if( g_LadderTeamMgr.IsSearching() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );	
					UpdateLadderBattle();		
					return;
				}

				if( g_LadderTeamMgr.IsHeroMatchMode() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );	
				}
				else
				{
					g_LadderTeamMgr.SetBadPingKick( !g_LadderTeamMgr.IsBadPingKick() );

					SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
					kPacket << LADDERTEAM_MACRO_CHANGE_OPTION << g_LadderTeamMgr.IsSearchLevelMatch() << g_LadderTeamMgr.IsSearchSameUser() << g_LadderTeamMgr.IsBadPingKick();
					TCPNetwork::SendToServer( kPacket );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}			
			UpdateLadderBattle();
		}
		break;
	case ID_SEARCH_START:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
			{
				int iMinimumPlayer = g_ModeSelectInfo.GetLadderTeamMinimumPlayer( g_LadderTeamMgr.GetSelectModeIndex() );
				if( g_CampMgr.IsCloseCampBattle() )
				{
					g_CampMgr.TodayBattleEndAlarm( true );
				}
				else if( g_LadderTeamMgr.GetUserCount() < iMinimumPlayer )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11), iMinimumPlayer );
				}
				else if( g_LadderTeamMgr.GetUserCount() == iMinimumPlayer )
				{
					// 강퇴중인 유저가 있는지 확인
					if( g_LadderTeamMgr.IsHaveUser( g_LadderTeamMgr.GetKickOutUser() ) ) 
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11), iMinimumPlayer );
					else
					{
						SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
						kPacket << LADDERTEAM_MACRO_TEAM_STATE << (DWORD)ioLadderTeamMgr::TMS_SEARCH_RESERVE;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
				}
				else
				{
					SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
					kPacket << LADDERTEAM_MACRO_TEAM_STATE << (DWORD)ioLadderTeamMgr::TMS_SEARCH_RESERVE;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
			}		
			UpdateLadderBattle();
		}
		break;
	case ID_SEARCH_END:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_TEAM_STATE << (DWORD)ioLadderTeamMgr::TMS_READY;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );

			UpdateLadderBattle();
		}
		break;
	case ID_LADDER_RANKING:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( LADDER_RANKING_LIST_WND ) )
				g_GUIMgr.HideWnd( LADDER_RANKING_LIST_WND );
			else
			{
				LadderRankingListWnd *pRankList = dynamic_cast<LadderRankingListWnd*>(g_GUIMgr.FindWnd( LADDER_RANKING_LIST_WND ));
				if( pRankList )
				{
					if( g_LadderTeamMgr.IsHeroMatchMode() )
						pRankList->ShowTabWnd( LadderRankingListWnd::ID_HERO_RANKING );
					else
						pRankList->ShowTabWnd( LadderRankingListWnd::ID_TEAM_RANKING );
				}
			}	
		}
		break;
	}
}

void LadderBattleWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_dwRankingUpdateTime == 0 ||
		FRAMEGETTIME() - m_dwRankingUpdateTime >= RANKING_UPDATE_TIME )
	{
		m_dwRankingUpdateTime = FRAMEGETTIME();

		SP2Packet kPacket( CTPK_LADDER_TEAM_RANKING );
		kPacket << g_LadderTeamMgr.GetLadderTeamIndex();
		TCPNetwork::SendToServer( kPacket );
	}

	if( g_LadderTeamMgr.IsSearching() && !g_LadderTeamMgr.IsLadderBattleModePlaying() )
	{
		int iFullSec = m_dwFullSearchTime / 1000;
		int iCurSec  = (FRAMEGETTIME() - m_dwCurSearchTime) / 1000;
		int iNextSec = max( 0, iFullSec - iCurSec );
		if( iNextSec != m_iCurSearchSec )
		{
			m_iCurSearchSec = iNextSec;
			m_dwChangeCountTime = FRAMEGETTIME();
		}		
	}

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessModeHelpTooltip();
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ) )
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos();
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ) )
				m_iDefaultY = iYPos;
		}
	}

	g_BackgroundMgr.ProcessBackground();
}

void LadderBattleWnd::ProcessModeHelpTooltip()
{	
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	m_bModeTooltipOver = false;
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		if( g_App.IsMouseRender() && !m_bOwner && !g_LadderTeamMgr.IsHeroMatchMode() )
		{
			if( g_GUIMgr.GetPreOverWnd() == this ||
				g_GUIMgr.GetPreClickWnd() == this )
			{
				ioLSMouse *pMouse = g_App.GetMouse();
				enum { STARTX = 553, STARTY = 390, WIDTH = 21, HEIGHT = 21, };
				RECT rcIconRect;
				rcIconRect.left   = GetDerivedPosX() + STARTX;
				rcIconRect.top    = GetDerivedPosY() + STARTY;
				rcIconRect.right  = rcIconRect.left + WIDTH;
				rcIconRect.bottom = rcIconRect.top + HEIGHT;
				if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
				{
					m_bModeTooltipOver = true;	

					if( pMouse->IsLBtnUp() )
					{
						// 공략 창
						int iManualIndex = g_ModeSelectInfo.GetLadderTeamModeManualIndex( g_LadderTeamMgr.GetSelectModeIndex() );
						if( iManualIndex != -1 )
						{
							ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
							if( pManualWnd )
								pManualWnd->ShowWndByCurManual( iManualIndex );
						}				
					}
				}
			}
		}    

		if( m_bModeTooltipOver )
		{
			pModeHelpTooltip->SetModeHelpTooltip( this, g_ModeSelectInfo.GetLadderTeamModeTitle( g_LadderTeamMgr.GetSelectModeIndex() ),
												  g_ModeSelectInfo.GetLadderTeamModeHelpIndex( g_LadderTeamMgr.GetSelectModeIndex() ) );
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}

void LadderBattleWnd::OnRender()
{
	g_BackgroundMgr.RenderBackground();

	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	//Title
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );
	kPrinter.SetTextColor( 255, 255, 255 );
	kPrinter.AddTextPiece( FONT_SIZE_17, m_szCampName.c_str() );

	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, " | " );
	kPrinter.SetTextColor( 255, 255, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(3), g_LadderTeamMgr.GetLadderTeamName().c_str() );
	if( !g_LadderTeamMgr.GetLadderTeamPW().IsEmpty() )
	{
		if( m_bOwner )
			kPrinter.AddTextPiece( FONT_SIZE_17, STR(4), g_LadderTeamMgr.GetLadderTeamPW().c_str() );
		else
			kPrinter.AddTextPiece( FONT_SIZE_17, STR(5) );
	}
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	if( g_LadderTeamMgr.IsHeroMatchMode() )
	{
		OnHeroRender( iXPos, iYPos );
	}
	else
	{
		OnTeamRender( iXPos, iYPos );
	}	
	OnModeNMapRender( iXPos + 416, iYPos + 385 );
}

void LadderBattleWnd::OnTeamRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	//길드 마크
	g_GuildMarkMgr.RenderMarkSize( 31, 31, g_LadderTeamMgr.GetGuildIndex(), g_LadderTeamMgr.GetGuildMark(), iXPos + 27, iYPos + 62 );

	kPrinter.SetTextStyle( TS_NORMAL );
	if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), g_LadderTeamMgr.GetLadderTeamName().c_str(), g_LadderTeamMgr.GetTeamRanking() );
	kPrinter.PrintFullText( iXPos + 64, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	const LadderTeamInfo &rkLadderTeam = g_LadderTeamMgr.GetLadderTeamInfo();	
	if( rkLadderTeam.m_iVictoriesRecord > 1 )
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), rkLadderTeam.m_iWinRecord, rkLadderTeam.m_iLoseRecord, rkLadderTeam.m_iVictoriesRecord );
	else
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), rkLadderTeam.m_iWinRecord, rkLadderTeam.m_iLoseRecord );
	kPrinter.PrintFullText( iXPos + 64, iYPos + 79, TAT_LEFT );
	kPrinter.ClearList();
}

void LadderBattleWnd::OnHeroRender( int iXPos, int iYPos )
{
	char szConvertText[MAX_PATH] = "";
	ioComplexStringPrinter kPrinter;
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
		g_GuildMarkMgr.RenderMarkSize( 31, 31, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 27, iYPos + 62 );
	else
		g_GuildMarkMgr.RenderMarkSize( 31, 31, 0, 0, iXPos + 27, iYPos + 62 );

	kPrinter.SetTextStyle( TS_NORMAL );
	if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

	if( g_MyInfo.GetHeroTodayRank() > 0 )
	{
		Help::ConvertNumToStrComma( g_MyInfo.GetHeroTodayRank(), szConvertText, sizeof( szConvertText ) );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), szConvertText );
	}

	if( g_MyInfo.GetHeroTitle() > 0 )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), g_LevelMgr.GetHeroTitleString( g_MyInfo.GetHeroTitle() ).c_str() ); 
	}
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), g_MyInfo.GetPublicID().c_str() ); 
	kPrinter.PrintFullText( iXPos + 64, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	Help::ConvertNumToStrComma( g_MyInfo.GetLadderPoint(), szConvertText, sizeof( szConvertText ) );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4), szConvertText );
	kPrinter.PrintFullText( iXPos + 64, iYPos + 79, TAT_LEFT );
	kPrinter.ClearList();
}

void LadderBattleWnd::OnRenderAfterChild()
{
	OnSearchRender();
}

void LadderBattleWnd::OnSearchRender()
{
	if( !g_LadderTeamMgr.IsSearching() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwWaitScaleTime = 150;
	DWORD dwCircleTime = 500;
	DWORD dwStartCircleTime1 = 150;
	DWORD dwStartCircleTime2 = 210;

	float fWaitScaleRate = 0.1f;
	float fStartCircleRate = FLOAT05;
	float fCircleRate = 1.5f;

	float fWaitCurScale = FLOAT1;
	float fCircleRate1 = FLOAT05;
	float fCircleRate2 = FLOAT05;
	float fCircleAlpha1 = FLOAT1;
	float fCircleAlpha2 = FLOAT1;

	bool bRenderCircle1 = false;
	bool bRenderCircle2 = false;

	DWORD dwGapTime = dwCurTime - m_dwChangeCountTime;
	if( dwGapTime < dwWaitScaleTime )
	{
		float fTimeRate = (float)dwGapTime / (float)dwWaitScaleTime;
		fWaitCurScale = FLOAT1 + fWaitScaleRate * sinf( D3DX_PI * fTimeRate );
	}
	else
	{
		bRenderCircle1 = true;
		fCircleRate1 = 2.0f;
		DWORD dwCircleGap1 = dwGapTime - dwStartCircleTime1;
		if( dwCircleGap1 < dwCircleTime )
		{
			float fTimeRate = (float)dwCircleGap1 / (float)dwCircleTime;
			fCircleRate1 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
			fCircleAlpha1 = cosf( (D3DX_PI/2) * fTimeRate );
		}
		else
		{
			bRenderCircle1 = false;
		}

		if( dwGapTime > dwStartCircleTime2 )
		{
			bRenderCircle2 = true;

			fCircleRate2 = 2.0f;
			DWORD dwCircleGap2 = dwGapTime - dwStartCircleTime2;
			if( dwCircleGap2 < dwCircleTime )
			{
				float fTimeRate = (float)dwCircleGap2 / (float)dwCircleTime;
				fCircleRate2 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
				fCircleAlpha2 = cosf( (D3DX_PI/2) * fTimeRate );
			}
			else
			{
				bRenderCircle2 = false;
			}
		}
	}
	
	enum
	{
		CIRCLE_X		= 386,
		CIRCLE_Y		= 266,
		
		TEXT_BACK_X		= 386,
		TEXT_BACK_Y		= 232,

		WAIT_X			= 386,
		WAIT_Y			= 266,

		BLUE_TEXT1_X	= 317,
		BLUE_TEXT1_Y	= 296,
		BLUE_TEXT2_X	= 317,
		BLUE_TEXT2_Y	= 318,

		RED_TEXT1_X		= 455,
		RED_TEXT1_Y		= 296,
		RED_TEXT2_X		= 455,
		RED_TEXT2_Y		= 318,

		MAIN_TEXT_X		= 336,
		MAIN_TEXT_Y		= 223,
	};


	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	// Circle
	if( m_pWarCountCircle )
	{
		if( bRenderCircle1 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha1));
			m_pWarCountCircle->SetScale( fCircleRate1 );
			m_pWarCountCircle->Render( iXPos+CIRCLE_X, iYPos+CIRCLE_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( bRenderCircle2 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha2));
			m_pWarCountCircle->SetScale( fCircleRate2 );
			m_pWarCountCircle->Render( iXPos+CIRCLE_X, iYPos+CIRCLE_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	
	// TextBack
	if( m_pTextBack )
	{
		m_pTextBack->SetSize( 162, 35 );
		m_pTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

		int iOffSetX = m_pTextBack->GetOffsetX();
		int iOffSetY = m_pTextBack->GetOffsetY();

		m_pTextBack->Render( iXPos+TEXT_BACK_X-iOffSetX, iYPos+TEXT_BACK_Y+iOffSetY-17 );
	}

	// Wait
	if( m_pWarCountWait )
	{
		m_pWarCountWait->SetScale( fWaitCurScale );
		m_pWarCountWait->Render( iXPos+WAIT_X, iYPos+WAIT_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Text
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	if( g_LadderTeamMgr.IsHeroMatchMode() )
		g_FontMgr.PrintText( iXPos + MAIN_TEXT_X, iYPos+MAIN_TEXT_Y, FONT_SIZE_15, STR(5), m_iCurSearchSec );
	else
		g_FontMgr.PrintText( iXPos + MAIN_TEXT_X, iYPos+MAIN_TEXT_Y, FONT_SIZE_15, STR(6), m_iCurSearchSec );
}

void LadderBattleWnd::OnModeNMapRender( int iXPos, int iYPos )
{
	if( m_bOwner && !g_LadderTeamMgr.IsHeroMatchMode() ) return;
	if( !m_pModeNMapGrayFrm ) return;

	// 모드
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kModeTitle = g_ModeSelectInfo.GetLadderTeamModeTitle( g_LadderTeamMgr.GetSelectModeIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kModeTitle.c_str() );

	ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetLadderTeamModeOrderMarkSmall( g_LadderTeamMgr.GetSelectModeIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kModeTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}	

	if( m_pManualMark && !g_LadderTeamMgr.IsHeroMatchMode() )
	{
		if( m_bModeTooltipOver )
			m_pManualMark->SetColor( 206, 78, 0 );
		else
			m_pManualMark->SetColor( 110, 110, 110 );
		m_pManualMark->Render( iXPos + 140, iYPos + 8, UI_RENDER_NORMAL_ADD_COLOR );
	}

	// 맵
	iXPos += 165;
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(2) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kMapTitle = g_ModeSelectInfo.GetLadderTeamMapTitle( g_LadderTeamMgr.GetSelectModeIndex(), g_LadderTeamMgr.GetSelectMapIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

	pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_LadderTeamMgr.GetSelectModeIndex(), g_LadderTeamMgr.GetSelectMapIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}
}

void LadderBattleWnd::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	int i = 0;
	for(i = ID_USER_1;i <= ID_USER_8;i++)
	{
		LadderBattleMemberList *pUserBtn = dynamic_cast<LadderBattleMemberList *>( FindChildWnd( i ) );
		if( !pUserBtn ) continue;

		if( pUserBtn->GetListName() == szID )
			pUserBtn->StartChatBubble( szID, szChat, bWholeChat );
	}
}
//////////////////////////////////////////////////////////////////////////
LadderBattleHeroRankBtn::LadderBattleHeroRankBtn()
{
	m_pUI3DRender		= NULL;
	m_aMyChar			= NULL;

	m_pBlueBar			= NULL;
	m_pRedBar			= NULL;
	m_pGrayBar			= NULL;

	m_pBlueOverBar		= NULL;
	m_pRedOverBar		= NULL;
	m_pGrayOverBar		= NULL;

	m_pBlueGra			= NULL;
	m_pRedGra			= NULL;
	m_pGrayGra			= NULL;

	m_pBlueOverGra		= NULL;
	m_pRedOverGra		= NULL;
	m_pGrayOverGra		= NULL;

	m_pDarkBar			= NULL;
	m_pLightBar			= NULL;

	m_pHeroRankTitle	= NULL;
	m_pIconBack			= NULL; 
	m_pNumber			= NULL;
	m_pRankText			= NULL;
	m_pNoneText			= NULL;
	m_pNoneIcon			= NULL;

	m_pCurrentBar		= NULL;
	m_pCurrentOverBar	= NULL;
	m_pCurrentGra		= NULL;
	m_pCurrentOverGra	= NULL;	
}	

LadderBattleHeroRankBtn::~LadderBattleHeroRankBtn()
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

	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );
	SAFEDELETE( m_pHeroRankTitle );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNumber );
	SAFEDELETE( m_pRankText );
	SAFEDELETE( m_pNoneText );
	SAFEDELETE( m_pNoneIcon );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void LadderBattleHeroRankBtn::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void LadderBattleHeroRankBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void LadderBattleHeroRankBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "GrayGra" )
	{
		SAFEDELETE( m_pGrayGra );
		m_pGrayGra = pImage;
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
	else if( szType == "HeroRankTitle" )
	{
		SAFEDELETE( m_pHeroRankTitle );
		m_pHeroRankTitle = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
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

void LadderBattleHeroRankBtn::CreateButton()
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

	ioHashString kMotionAni = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_8, m_aMyChar->IsMale(), "" );
	if( kMotionAni.IsEmpty() )
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
		m_aMyChar->SetMotionItemAni( kMotionAni );
	}
}

void LadderBattleHeroRankBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void LadderBattleHeroRankBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCurrentOverBar && ( IsOver() || IsClicked() ) )
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

	if( m_pCurrentOverGra && ( IsOver() || IsClicked() ) )
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

	//
	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 3, iYPos + 180 );
		m_pLightBar->Render( iXPos + 3, iYPos + 201 );
		m_pDarkBar->Render( iXPos + 3, iYPos + 222 );
	}

	int iWinCount = g_MyInfo.GetRecordWin( RECORD_HEROMATCH );
	int iLoseCount= g_MyInfo.GetRecordLose( RECORD_HEROMATCH );
	int iRecordRate = g_MyInfo.GetRecordWinLoseRate( RECORD_HEROMATCH ) * 100;

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	if( /*g_LadderTeamMgr.GetLadderCampType()*/g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		g_FontMgr.SetBkColor( 25, 70, 125 );
	else if( g_MyInfo.GetUserCampPos() == CAMP_RED )
		g_FontMgr.SetBkColor( 120, 31, 19 );
	else
		g_FontMgr.SetBkColor( 110, 110, 110 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 182, FONT_SIZE_13, STR(1), iWinCount );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, STR(2), iLoseCount );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 224, FONT_SIZE_13, STR(3), iRecordRate );
}

void LadderBattleHeroRankBtn::OnRenderSoldier( int iXPos, int iYPos )
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
LadderBattleHeroWnd::LadderBattleHeroWnd()
{
	m_pBlueBar	= NULL;
	m_pRedBar	= NULL;
	m_pBlueGra	= NULL;
	m_pRedGra	= NULL;
	m_pDarkBar	= NULL;
	m_pLightBar	= NULL;
	m_pHeroRankTitle    = NULL;
	m_pSeasonHeroTitle	= NULL;
	m_pSeasonRecordTitle= NULL;
	m_pIconBack		= NULL;
	m_pRecordIcon	= NULL;
	m_pNumber		= NULL;
	m_pRankText		= NULL;
	m_pPercentText	= NULL;
	m_pNoneText     = NULL;
	m_pNoneIcon     = NULL;

	m_pCurrentBar = NULL;
	m_pCurrentGra = NULL;

	m_dwTabID = ID_TAB_RANK_TOP100;
}

LadderBattleHeroWnd::~LadderBattleHeroWnd()
{
	SAFEDELETE( m_pBlueBar );
	SAFEDELETE( m_pRedBar );
	SAFEDELETE( m_pBlueGra );
	SAFEDELETE( m_pRedGra );
	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );
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
}

void LadderBattleHeroWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/heromatcrankgraphwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatctop100wnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatchhistorywnd.xml", this );

	HeroMatchRankGraphWnd *pGraphWnd = dynamic_cast<HeroMatchRankGraphWnd*>( FindChildWnd( ID_RANK_GRAPH_WND ) );
	if( pGraphWnd )
		pGraphWnd->SetMode( HERO_MATCH );

	HeroMatchTop100Wnd *p100Wnd = dynamic_cast<HeroMatchTop100Wnd*>( FindChildWnd( ID_RANK_TOP100_WND ) );
	if( p100Wnd )
		p100Wnd->SetMode( HERO_MATCH );

	HeroMatchHistoryWnd *pRecordWnd = dynamic_cast<HeroMatchHistoryWnd*>( FindChildWnd( ID_BATTLE_RECORD_WND ) );
	if( pRecordWnd )
		pRecordWnd->SetMode( HERO_MATCH );
}

void LadderBattleHeroWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void LadderBattleHeroWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void LadderBattleHeroWnd::CheckTabWnd( DWORD dwID )
{
	m_dwTabID = dwID;
	CheckRadioButton( ID_TAB_RANK_GRAPH, ID_TAB_BATTLE_RECORD, m_dwTabID );

	HideChildWnd( ID_RANK_GRAPH_WND );
	HideChildWnd( ID_RANK_TOP100_WND );
	HideChildWnd( ID_BATTLE_RECORD_WND );
	
	switch( m_dwTabID )
	{
	case ID_TAB_RANK_GRAPH:
		{
			ShowChildWnd( ID_RANK_GRAPH_WND );
		}
		break;
	case ID_TAB_RANK_TOP100:
		{
			ShowChildWnd( ID_RANK_TOP100_WND );
		}
		break;
	case ID_TAB_BATTLE_RECORD:
		{
			ShowChildWnd( ID_BATTLE_RECORD_WND );
		}
		break;
	}
}

void LadderBattleHeroWnd::iwm_show()
{	
	if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
	{
		m_pCurrentBar = m_pBlueBar;
		m_pCurrentGra = m_pBlueGra;
	}
	else
	{
		m_pCurrentBar = m_pRedBar;
		m_pCurrentGra = m_pRedGra;
	}

	LadderBattleHeroRankBtn *pBattleRankBtn = dynamic_cast<LadderBattleHeroRankBtn *>(FindChildWnd( ID_MY_RANK_BTN ));
	if( pBattleRankBtn )
		pBattleRankBtn->CreateButton();

	//
	CheckTabWnd( ID_TAB_RANK_GRAPH );
}

void LadderBattleHeroWnd::iwm_hide()
{
}

void LadderBattleHeroWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_TAB_RANK_GRAPH:
	case ID_TAB_RANK_TOP100:
	case ID_TAB_BATTLE_RECORD:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckTabWnd( dwID );
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

void LadderBattleHeroWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void LadderBattleHeroWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderHeroTitle( iXPos + 128, iYPos );
	OnRenderSeasonRecord( iXPos + 256, iYPos );
}

void LadderBattleHeroWnd::OnRenderHeroTitle( int iXPos, int iYPos )
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
	if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
		g_FontMgr.SetBkColor( 25, 70, 125 );
	else
		g_FontMgr.SetBkColor( 120, 31, 19 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 182, FONT_SIZE_13, "-" );
	if( g_MyInfo.GetHeroTitle() == 0 )
		g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, "-" );
	else
	{
		int iBonus = (float)( g_LevelMgr.GetHeroTitleBonus( g_MyInfo.GetHeroTitle() ) * FLOAT100 ) + 0.001f;
		g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, STR(1), iBonus );
	}
	g_FontMgr.PrintText( iXPos + 63, iYPos + 224, FONT_SIZE_13, "-" );
}

void LadderBattleHeroWnd::OnRenderSeasonRecord( int iXPos, int iYPos )
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
	if( g_LadderTeamMgr.GetLadderCampType() == CAMP_BLUE )
		g_FontMgr.SetBkColor( 25, 70, 125 );
	else
		g_FontMgr.SetBkColor( 120, 31, 19 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 182, FONT_SIZE_13, STR(1), g_MyInfo.GetHeroSeasonWin() );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 203, FONT_SIZE_13, STR(2), g_MyInfo.GetHeroSeasonLose() );
	g_FontMgr.PrintText( iXPos + 63, iYPos + 224, FONT_SIZE_13, "-" );
}
//////////////////////////////////////////////////////////////////////////
LadderTeamInviteList::LadderTeamInviteList()
{
	m_iGradeLevel = 0;

	m_pGuildMark	= NULL;
	m_pFriendMark	= NULL;
}

LadderTeamInviteList::~LadderTeamInviteList()
{
	SAFEDELETE( m_pGuildMark );
	SAFEDELETE( m_pFriendMark );
}

void LadderTeamInviteList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GuildMark" )
	{
		SAFEDELETE( m_pGuildMark );
		m_pGuildMark = pImage;
	}
	else if( szType == "FriendMark" )
	{
		SAFEDELETE( m_pFriendMark );
		m_pFriendMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void LadderTeamInviteList::InviteInfo( int iGradeLevel, ioHashString szName )
{
	m_iGradeLevel = iGradeLevel;
	m_szName      = szName;

	HideChildWnd( ID_INVITE_BTN );
	if( !szName.IsEmpty() )
	{
		ShowChildWnd( ID_INVITE_BTN );
		SetActive();
	}
	else
	{
		SetInActive();
	}
}

void LadderTeamInviteList::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INVITE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			
			if( g_LadderTeamMgr.IsLadderTeamFull() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			//초대
			SP2Packet kPacket( CTPK_LADDERTEAM_INVITE );
			kPacket << 1 << m_szName;
			TCPNetwork::SendToServer( kPacket );

			if( GetParent() )
			{
				LadderTeamInviteListWnd *pParentWnd = dynamic_cast<LadderTeamInviteListWnd*>(GetParent());
				pParentWnd->StartInivteWait( GetLevel(), GetName() );
			}
		}
		break;
	}
}

void LadderTeamInviteList::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}

void LadderTeamInviteList::OnRender()
{
	ioButton::OnRender();

	if( !m_szName.IsEmpty() )
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 133.0f, m_szName.c_str() );
		int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, 133.0f );
		
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_UserInfoMgr.GetGuildIndex( m_szName ) );
		if( pGuildData )
		{
			int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_szName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_13, 94.0f );
			g_GuildMarkMgr.RenderSmallMark( pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
			iRealNameSize += 16;
		}

		// 길드 - > 친구 - > 일반
		if( g_GuildInfo.IsMyGuildUser( m_szName ) && m_pGuildMark )
			m_pGuildMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
		else if( g_FriendManager.IsFriend( m_szName ) && m_pFriendMark )
			m_pFriendMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
	}		
}
//////////////////////////////////////////////////////////////////////////
LadderTeamInviteListWnd::LadderTeamInviteListWnd() : m_pPreEdit( NULL )
{
	m_iCurPage = m_iPrevCurPage = m_iMaxPage = 0;
	m_dwSearchTime  = 0;
}

LadderTeamInviteListWnd::~LadderTeamInviteListWnd()
{
	m_vUserList.clear();
}

void LadderTeamInviteListWnd::StartSend( bool bProcess )
{
	SP2Packet kPacket( CTPK_LADDERTEAM_INVITE_LIST );
	kPacket << m_iCurPage << MAX_PAGE_LIST;
	TCPNetwork::SendToServer( kPacket );
	m_dwSearchTime = FRAMEGETTIME();
}

void LadderTeamInviteListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > INVITE_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void LadderTeamInviteListWnd::iwm_show()
{	
	g_GUIMgr.HideWnd( INVITE_PROGRESS_WND );

	m_iCurPage = m_iPrevCurPage;
	m_iPrevCurPage = 0;
	m_vUserList.clear();

	StartSend();

	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++)
	{
		LadderTeamInviteList *pItem = dynamic_cast<LadderTeamInviteList*> ( FindChildWnd( i ) );
		if( pItem )
		{
			pItem->InviteInfo( 0, "" );
		}
	}	
    
	// 아이디 입력
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
	
	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );	
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void LadderTeamInviteListWnd::iwm_hide()
{	
	m_vUserList.clear();
	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->KillKeyFocus();		
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void LadderTeamInviteListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = min( m_iMaxPage, iCurPage );

	if( m_iCurPage != iPrevPage )
		StartSend();

	int iListSIze = m_vUserList.size();
	int iStartPos = 0;
	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++,iStartPos++)
	{
		LadderTeamInviteList *pItem = (LadderTeamInviteList*)FindChildWnd( i );
		if( pItem )
		{
			if( iStartPos < iListSIze )
			{
				UserInviteInfo &kInfo = m_vUserList[iStartPos];
				pItem->InviteInfo( kInfo.m_iLevel, kInfo.m_szName );
			}
			else
			{
				pItem->InviteInfo( 0, "" );
			}
		}
	}	
}

void LadderTeamInviteListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ALL_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			AllInvite();
		}
		break;
	case ID_NAME_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			NameInvite();
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_NAME_INVITE ), IOBN_BTNUP, 0 );
			}			
		}
		break;
	case ID_USER_LIST_1:
	case ID_USER_LIST_2:
	case ID_USER_LIST_3:
	case ID_USER_LIST_4:
	case ID_USER_LIST_5:
	case ID_USER_LIST_6:
	case ID_USER_LIST_7:
	case ID_USER_LIST_8:
	case ID_USER_LIST_9:
	case ID_USER_LIST_10:
		if( cmd == IOBN_BTNUP )
		{
			LadderTeamInviteList *pListBtn = dynamic_cast<LadderTeamInviteList*>(pWnd);
			if( !pListBtn->GetName().IsEmpty() )
				g_App.OpenMannerWnd( pListBtn->GetName().c_str(), true );
		}
		break;
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
	}

}

void LadderTeamInviteListWnd::AllInvite()
{
	if( g_LadderTeamMgr.IsLadderTeamFull() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	int i;
	vUserInviteInfo vSendList;
	for(i = 0;i < (int)m_vUserList.size();i++)
	{
		UserInviteInfo &kInfo = m_vUserList[i];
		if( kInfo.m_szName.IsEmpty() ) continue;
		if( g_App.IsAdminID( kInfo.m_szName.c_str() ) ) continue;

		vSendList.push_back( kInfo );
	}

	if( vSendList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	m_iPrevCurPage = m_iCurPage;
	int iSize = vSendList.size();
	SP2Packet kPacket( CTPK_LADDERTEAM_INVITE );
	kPacket << iSize;
	for(i = 0;i < iSize;i++)
	{
		UserInviteInfo &kInfo = vSendList[i];
		kPacket << kInfo.m_szName;
	}		
	TCPNetwork::SendToServer( kPacket );	
	
	if( iSize == 1 )
		StartInivteWait( vSendList[0].m_iLevel, vSendList[0].m_szName );
	else
		StartInivteWait( iSize, "" );	
	vSendList.clear();
}

void LadderTeamInviteListWnd::NameInvite()
{
	if( g_LadderTeamMgr.IsLadderTeamFull() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_App.IsMouseBusy() )
		return;

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( !pNameEdit )
		return;

	char szName[MAX_PATH] = "";
	sprintf( szName, "%s", pNameEdit->GetText() );

	pNameEdit->ClearString();
	pNameEdit->SetKeyFocus();
	g_InputBox.SetString( "" );

	if( !g_App.IsRightID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	ioHashString szIsID = szName;
	if( g_MyInfo.GetPublicID() == szIsID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_App.IsAdminID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}		

	m_iPrevCurPage = m_iCurPage;
	SP2Packet kPacket( CTPK_LADDERTEAM_INVITE );
	kPacket << 1 << szName;
	TCPNetwork::SendToServer( kPacket );	
	StartInivteWait( -1, szName );		
}

void LadderTeamInviteListWnd::StartInivteWait( int iValue, const ioHashString &szName )
{
	if( !IsShow() )
		return;

	HideWnd();
	// 초대 현황 UI SHOW
	InviteProgressWnd *pProgressWnd = dynamic_cast<InviteProgressWnd*>( g_GUIMgr.FindWnd( INVITE_PROGRESS_WND ) );
	if( pProgressWnd )
	{
		pProgressWnd->StartInivteWait( this, iValue, szName );
	}
}

void LadderTeamInviteListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSearch();	

	ioMovingWnd::OnProcess( fTimePerSec );
}

void LadderTeamInviteListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 273, TAT_CENTER );

	if( m_vUserList.empty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 151, iYPos + 120, FONT_SIZE_12, STR(3) );
	}
}

void LadderTeamInviteListWnd::SetUserList( SP2Packet &rkPacket )
{	
	m_vUserList.clear();

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;
	for(i = 0;i < iCurList;i++)
	{
		DWORD dwGuildIndex, dwGuildMark;
		UserInviteInfo kList;
		rkPacket >> kList.m_szName >> kList.m_iLevel >> dwGuildIndex >> dwGuildMark;
		g_GuildInfo.SetGuildMark( kList.m_szName, dwGuildIndex, dwGuildMark );
		m_vUserList.push_back( kList );
	}    	
	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
LadderTeamInvitedBtn::LadderTeamInvitedBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal = NULL;
	m_pBlueOver = NULL;
	m_pRedOver = NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_eTeam = TEAM_BLUE;
	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = m_iPingStep = 0;
}

LadderTeamInvitedBtn::~LadderTeamInvitedBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void LadderTeamInvitedBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void LadderTeamInvitedBtn::iwm_create()
{
	m_PingStep.Create();
}

void LadderTeamInvitedBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, TeamType eTeam )
{
	m_iMaxSlot     = iMaxSlot;
	m_iSlotArray   = iSlotArray;
	m_szName       = rkName;
	m_eTeam        = eTeam;
	m_iPingStep    = iPingStep;
	m_iGradeLevel  = iGradeLevel;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void LadderTeamInvitedBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case TEAM_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void LadderTeamInvitedBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBase( iXPos, iYPos );

	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_eTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 128.0f, m_szName.c_str() );
		int iRealNameSize = min( 128.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
		
		m_PingStep.RenderPing( iXPos + 197, iYPos + 2, m_iPingStep, false );
	}
}
//////////////////////////////////////////////////////////////////////////
bool LadderTeamInvitedWnd::sg_bAutoRefuse = false;
LadderTeamInvitedWnd::LadderTeamInvitedWnd() : m_dwStartTime( 0 )
{
	m_pModeIconBack = NULL;
	m_pBlueCampIcon = NULL;
	m_pRedCampIcon  = NULL;

	m_iCurPage = 0;
	m_iMaxPage = 1;
}

LadderTeamInvitedWnd::~LadderTeamInvitedWnd()
{
	m_vUserList.clear();

	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pBlueCampIcon );
	SAFEDELETE( m_pRedCampIcon );
}

void LadderTeamInvitedWnd::SetInfo( SP2Packet &rkPacket )
{
	m_vUserList.clear();

	rkPacket >> m_LadderTeamInfo.m_iCampType >> m_LadderTeamInfo.m_dwTeamIndex >> m_LadderTeamInfo.m_szTeamName >> m_LadderTeamInfo.m_szTeamPW >> m_LadderTeamInfo.m_szOwnerName
			 >> m_LadderTeamInfo.m_iMaxPlayer >> m_LadderTeamInfo.m_iAverageLevel >> m_LadderTeamInfo.m_iWinRecord
			 >> m_LadderTeamInfo.m_iLoseRecord >> m_LadderTeamInfo.m_iVictoriesRecord >> m_LadderTeamInfo.m_iCurPlayer;

	if( m_LadderTeamInfo.m_iCampType != g_MyInfo.GetUserCampPos() )
	{
		return;
	}

	bool bFriendInParty = false;
	for(int i = 0;i < m_LadderTeamInfo.m_iCurPlayer;i++)
	{
		DWORD dwGuildIndex, dwGuildMark;
		LadderTeamUserInfo kUserInfo;
		rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_iLadderPoint 
				 >> dwGuildIndex >> dwGuildMark >> kUserInfo.m_iServerPingStep;
		m_vUserList.push_back( kUserInfo );

		// 길드 정보
		g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, dwGuildIndex, dwGuildMark );

		if( !bFriendInParty )
		{
			if( g_FriendManager.IsFriend(kUserInfo.m_szUserID) || g_GuildInfo.IsMyGuildUser( kUserInfo.m_szUserID )  )
			{
				bFriendInParty = true;		
			}
		}
		// 친구 정보 갱신
		g_FriendManager.ClientFriendInfoUpdate( kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, UP_LADDER_TEAM );
	}	

	if( !bFriendInParty )
	{
		// 거절한 전투방에서 1분이내에 초대가 올 경우 초대 무시 단, 친구의 초대는 예외
		if( Setting::AutoRejectCamp() || sg_bAutoRefuse || g_LadderTeamMgr.IsInvitedRefuseRoom( m_LadderTeamInfo.m_dwTeamIndex ) )	 
		{
			return;
		}
	}

	ioHashString szBlueCampName;
	ioHashString szRedCampName;

	szBlueCampName.Clear();
	szRedCampName.Clear();

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament != NULL )
	{
		szBlueCampName = pTournament->GetCampName( CAMP_BLUE );
		szRedCampName = pTournament->GetCampName( CAMP_RED );

		m_pBlueCampIcon = pTournament->GetCampIcon( CAMP_BLUE );
		m_pRedCampIcon = pTournament->GetCampIcon( CAMP_RED );
	}

	// UI TITLE	
	char szTitle[MAX_PATH] = "";
	if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
		SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d", szBlueCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
	else
		SafeSprintf( szTitle, sizeof( szTitle ), "%s %d/%d", szRedCampName.c_str(), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
	SetTitleText( szTitle );	

	g_SoundMgr.PlaySound( "interface/team_invite.wav" );
	m_dwStartTime = FRAMEGETTIME();

	if( IsShow() )
		HideWnd();

	ShowWnd();
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
}

const LadderTeamUserInfo &LadderTeamInvitedWnd::GetUserInfo( int iArray )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		if( --iArray < 0 )
			return m_vUserList[i];
	}

	static LadderTeamUserInfo kReturnInfo;
	return kReturnInfo;
}

void LadderTeamInvitedWnd::UpdatePage( int iCurPage )
{
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );
	const int iStartArray = m_iCurPage * MAX_PAGE_LIST;
	TeamType eBtnTeam = TEAM_BLUE;
	if( m_LadderTeamInfo.m_iCampType == CAMP_RED )
		eBtnTeam = TEAM_RED;

	int i = 0;
	int iSlotArray = 0;		
	for(i = 0,iSlotArray = iStartArray;i < MAX_PAGE_LIST;i++,iSlotArray++)
	{
		LadderTeamInvitedBtn *pTeamBtn = dynamic_cast<LadderTeamInvitedBtn*>(FindChildWnd( ID_LIST_1 + i ));
		if( pTeamBtn )
		{
			const LadderTeamUserInfo &rkInfo = GetUserInfo( iSlotArray );
			pTeamBtn->SetMemberInfo( m_LadderTeamInfo.m_iMaxPlayer, iSlotArray, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, eBtnTeam );
		}
	}
}

void LadderTeamInvitedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void LadderTeamInvitedWnd::iwm_show()
{
	ioButton *pBtn = (ioButton*)FindChildWnd( ID_ENTER );
	if( pBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), INVITE_DELAY_SEC );
		pBtn->SetTitleText( szTitle );
	}

	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_REFUSE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoRefuse );

	UpdatePage( 0 );
}

void LadderTeamInvitedWnd::JoinLadderTeam()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_JOIN_LADDERTEAM );
	kPacket << m_LadderTeamInfo.m_dwTeamIndex << true << m_LadderTeamInfo.m_szTeamPW;      // 팀 인덱스 << 초대 << 비번 
	TCPNetwork::SendToServer( kPacket );
}

bool LadderTeamInvitedWnd::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		g_LadderTeamMgr.SetInvitedRefuse( m_LadderTeamInfo.m_dwTeamIndex );
		return true;
	}
	return false;
}

void LadderTeamInvitedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
			g_LadderTeamMgr.SetInvitedRefuse( m_LadderTeamInfo.m_dwTeamIndex );
		}
		break;
	case ID_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( m_LadderTeamInfo.m_iCurPlayer >= m_LadderTeamInfo.m_iMaxPlayer )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}			
			else if( g_CampMgr.IsCloseCampBattle() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), g_CampMgr.GetPlayPreceedMent().c_str() );
			}
			else
			{
				JoinLadderTeam();
			}
			HideWnd();
		}
		break;
	case ID_AUTO_REFUSE:
		if( cmd == IOBN_BTNUP )
		{			
			sg_bAutoRefuse = !sg_bAutoRefuse;
			if( sg_bAutoRefuse )
			{
				HideWnd();
				g_LadderTeamMgr.SetInvitedRefuse( m_LadderTeamInfo.m_dwTeamIndex );
			}
		}
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdatePage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{	
			UpdatePage( m_iCurPage + 1 );
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
		if( cmd == IOBN_BTNUP )
		{
			LadderTeamInvitedBtn *pUserBtn = dynamic_cast<LadderTeamInvitedBtn*>(pWnd);
			if( pUserBtn )
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
		}
		break;
	}
}

void LadderTeamInvitedWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE && m_pBlueCampIcon )
		{
			m_pBlueCampIcon->SetScale( 0.60f, 0.60f );
			m_pBlueCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else if( m_pRedCampIcon )
		{
			m_pRedCampIcon->SetScale( 0.60f, 0.60f );
			m_pRedCampIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}	
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 175.0f, m_LadderTeamInfo.m_szTeamName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_LadderTeamInfo.m_iVictoriesRecord > 1 )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 175.0f, STR(1), m_LadderTeamInfo.m_iWinRecord,
									 m_LadderTeamInfo.m_iLoseRecord, m_LadderTeamInfo.m_iVictoriesRecord );	
	}
	else
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 73, FONT_SIZE_11, 175.0f, STR(2), m_LadderTeamInfo.m_iWinRecord,
									 m_LadderTeamInfo.m_iLoseRecord );	
	}		

	////
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_iCurPage + 1 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4), m_iMaxPage + 1 );
		kPrinter.PrintFullText( iXPos + 128, iYPos + 193, TAT_CENTER );
	}
}

void LadderTeamInvitedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;
	if( g_LadderTeamMgr.IsLadderTeam() )
	{
		HideWnd();
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
	
	if(m_dwStartTime == 0)
		return;

	if( FRAMEGETTIME() - m_dwStartTime >= INVITE_DELAY_SEC * 1000 )
	{
		HideWnd();	
		m_dwStartTime = 0;
	}
	else
	{
		ioButton *pBtn = (ioButton*)FindChildWnd( ID_ENTER );
		if( pBtn )
		{
			DWORD dwSec = ((float)(FRAMEGETTIME() - m_dwStartTime) / 1000);
			int   iSec  = max( 0, INVITE_DELAY_SEC - dwSec );	
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iSec );
			pBtn->SetTitleText( szTitle );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
LadderRankingListWnd::LadderRankingListWnd()
{
	m_dwTabID = ID_TEAM_RANKING;
}

LadderRankingListWnd::~LadderRankingListWnd()
{

}

void LadderRankingListWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/ladderteamrankwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ladderherorankwnd.xml", this );
}

void LadderRankingListWnd::iwm_show()
{	
	HideChildWnd( ID_TEAM_RANK_WND );
	HideChildWnd( ID_HERO_RANK_WND );

	CheckRankingButton( m_dwTabID );
}

void LadderRankingListWnd::iwm_hide()
{
	HideChildWnd( ID_TEAM_RANK_WND );
	HideChildWnd( ID_HERO_RANK_WND );
}

void LadderRankingListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_TEAM_RANKING:
	case ID_HERO_RANKING:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckRankingButton( dwID );
		}
		break;
	}
}

void LadderRankingListWnd::CheckRankingButton( DWORD dwID )
{
	m_dwTabID = dwID;
	CheckRadioButton( ID_TEAM_RANKING, ID_HERO_RANKING, m_dwTabID );

	HideChildWnd( ID_TEAM_RANK_WND );
	HideChildWnd( ID_HERO_RANK_WND );

	switch( m_dwTabID )
	{
	case ID_TEAM_RANKING:
		{
			LadderTeamRankListWnd *pRankListWnd = dynamic_cast<LadderTeamRankListWnd*>( FindChildWnd( ID_TEAM_RANK_WND ) );
			if( pRankListWnd )
			{
				pRankListWnd->SetMainWndTabID( m_dwTabID );
				pRankListWnd->ShowWnd();
			}
		}
		break;
	case ID_HERO_RANKING:
		{
			LadderHeroRankListWnd *pRankListWnd = dynamic_cast<LadderHeroRankListWnd*>( FindChildWnd( ID_HERO_RANK_WND ) );
			if( pRankListWnd )
			{
				pRankListWnd->SetMainWndTabID( m_dwTabID );
				pRankListWnd->ShowWnd();
			}
		}
		break;
	}
}

void LadderRankingListWnd::ShowTabWnd( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	ShowWnd();
}

void LadderRankingListWnd::ApplyRankingList( SP2Packet &rkPacket )
{
	bool bHeroMatch;
	rkPacket >> bHeroMatch;
	if( bHeroMatch )
	{
		LadderHeroRankListWnd *pLadderHeroRank = dynamic_cast<LadderHeroRankListWnd*>(FindChildWnd( ID_HERO_RANK_WND ));
		if( !pLadderHeroRank || !pLadderHeroRank->IsShow() )
			return;

		pLadderHeroRank->ApplyLadderHeroRank( rkPacket );
	}
	else
	{
		LadderTeamRankListWnd *pLadderTeamRank = dynamic_cast<LadderTeamRankListWnd*>(FindChildWnd( ID_TEAM_RANK_WND ));
		if( !pLadderTeamRank || !pLadderTeamRank->IsShow() )
			return;

		pLadderTeamRank->ApplyLadderTeamRank( rkPacket );
	}
}

void LadderRankingListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

}

void LadderRankingListWnd::OnRender()
{
	ioWnd::OnRender();
}

LadderRankListBtn::LadderRankListBtn()
{
	m_pInActiveFrm = NULL;
	m_pBlueNormal  = NULL;
	m_pRedNormal   = NULL;
	m_pBluePush    = NULL;
	m_pRedPush     = NULL;	
	m_pKey		   = NULL;
	m_pArrow	   = NULL;

	m_pBlueTeamIcon = NULL;
	m_pRedTeamIcon	= NULL;

	m_bHeroMatch   = false;

	ArrowAni kAni;
	kAni.m_dwCheckTime = 200;
	kAni.m_iOffsetX    = 234;
	m_ArrowAni.push_back( kAni );
	kAni.m_dwCheckTime = 100;
	kAni.m_iOffsetX    = 235;
	m_ArrowAni.push_back( kAni );
	kAni.m_dwCheckTime = 100;
	kAni.m_iOffsetX    = 236;
	m_ArrowAni.push_back( kAni );
	kAni.m_dwCheckTime = 100;
	kAni.m_iOffsetX    = 235;
	m_ArrowAni.push_back( kAni );
	m_AniArray  = 0;
	m_dwAniTime = 0;
}

LadderRankListBtn::~LadderRankListBtn()
{
	SAFEDELETE( m_pInActiveFrm );
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pBluePush );
	SAFEDELETE( m_pRedPush );
	SAFEDELETE( m_pKey );
	SAFEDELETE( m_pArrow );
}

void LadderRankListBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "InActive" )
	{
		SAFEDELETE( m_pInActiveFrm );
		m_pInActiveFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void LadderRankListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}	
	else if( szType == "BluePush" )
	{
		SAFEDELETE( m_pBluePush );
		m_pBluePush = pImage;
	}
	else if( szType == "RedPush" )
	{
		SAFEDELETE( m_pRedPush );
		m_pRedPush = pImage;
	}
	else if( strcmp(szType.c_str(), "Key") == 0 )
	{
		SAFEDELETE( m_pKey );
		m_pKey = pImage;
	}
	else if( szType == "Arrow" )
	{
		SAFEDELETE( m_pArrow );
		m_pArrow = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void LadderRankListBtn::SetLadderTeamInfo( LadderTeamInfo &rkInfo, bool bHeroMatch )
{
	m_LadderTeamInfo = rkInfo;
	m_bHeroMatch     = bHeroMatch;

	if( m_LadderTeamInfo.m_dwTeamIndex == 0 )
		SetInActive();
	else
	{
		SetActive();
	}
}

void LadderRankListBtn::SetTeamIcon(  ioUIRenderImage* pBlueIcon, ioUIRenderImage* pRedIcon )
{
	m_pBlueTeamIcon	= pBlueIcon;
	m_pRedTeamIcon	= pRedIcon;
}


void LadderRankListBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
	{
		if( m_pBlueNormal )
			m_pBlueNormal->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pRedNormal )
			m_pRedNormal->Render( iXPos, iYPos );
	}
}

void LadderRankListBtn::OnDrawOveredAdd( int iXPos, int iYPos )
{
	if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
	{
		if( m_pBlueNormal )
			m_pBlueNormal->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pRedNormal )
			m_pRedNormal->Render( iXPos, iYPos );
	}

	ioButton::OnDrawOveredAdd( iXPos, iYPos );
}

void LadderRankListBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	
	if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
	{
		if( m_pBluePush )
			m_pBluePush->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pRedPush )
			m_pRedPush->Render( iXPos, iYPos );
	}
}

void LadderRankListBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( COMPARE( m_AniArray, 0, (int)m_ArrowAni.size() ) )
	{
		if( FRAMEGETTIME() - m_dwAniTime > m_ArrowAni[m_AniArray].m_dwCheckTime )
		{
			m_dwAniTime = FRAMEGETTIME();
			m_AniArray++;
			if( !COMPARE( m_AniArray, 0, (int)m_ArrowAni.size() ) )
				m_AniArray = 0;
		}
	}
}

void LadderRankListBtn::OnRender()
{
	if( IsDisabled() )
	{
		if( m_pInActiveFrm )
			m_pInActiveFrm->Render( GetDerivedPosX(), GetDerivedPosY() );
	}
	else
	{
		if( !m_pBlueTeamIcon || !m_pRedTeamIcon || !m_pKey || !m_pArrow )
			return;

		ioButton::OnRender();

		int iXPos, iYPos;
		iXPos = GetDerivedPosX();
		iYPos = GetDerivedPosY();

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );	
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		if( m_LadderTeamInfo.m_iCampType == CAMP_BLUE )
		{			
			m_pBlueTeamIcon->Render( iXPos + 61, iYPos + 15 );
			g_FontMgr.SetBkColor( 33, 90, 162 );
		}
		else
		{			
			m_pRedTeamIcon->Render( iXPos + 61, iYPos + 15 );
			g_FontMgr.SetBkColor( 159, 41, 26 );		
		}



		g_FontMgr.PrintText( iXPos + 23, iYPos + 7, FONT_SIZE_12, STR(1), m_LadderTeamInfo.m_iTeamRanking );

		float fNameLimit = 118.0f;
		if( m_bHeroMatch )
			fNameLimit = 115.0f;

		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 78, iYPos + 8, FONT_SIZE_12, fNameLimit, m_LadderTeamInfo.m_szTeamName.c_str() );
		int iRealXPos = 78 + g_FontMgr.GetTextWidthCutSize( m_LadderTeamInfo.m_szTeamName.c_str(), TS_NORMAL, FONT_SIZE_12, fNameLimit );
		if( m_LadderTeamInfo.m_dwGuildIndex > 0 )
		{
			g_GuildMarkMgr.RenderSmallMark( m_LadderTeamInfo.m_dwGuildIndex, m_LadderTeamInfo.m_dwGuildMark, iXPos + iRealXPos + 4, iYPos + 9 );
			iRealXPos += 17;
		}

		if( !m_LadderTeamInfo.m_szTeamPW.IsEmpty() )
		{
			m_pKey->Render( iXPos + iRealXPos + 4, iYPos + 10 );
			iRealXPos += m_pKey->GetWidth() + 4;
		}

		int iMinusXPos = 0;
		if( m_bHeroMatch )
			iMinusXPos = 20;

		if( m_LadderTeamInfo.m_iVictoriesRecord > 1 )
			g_FontMgr.PrintText( iXPos + 260 - iMinusXPos, iYPos + 8, FONT_SIZE_12, STR(2), m_LadderTeamInfo.m_iWinRecord, m_LadderTeamInfo.m_iLoseRecord, m_LadderTeamInfo.m_iVictoriesRecord );
		else
			g_FontMgr.PrintText( iXPos + 260 - iMinusXPos, iYPos + 8, FONT_SIZE_12, STR(3), m_LadderTeamInfo.m_iWinRecord, m_LadderTeamInfo.m_iLoseRecord );
        
		if( m_bHeroMatch )
		{
			int iWinLoseRate = ( (float)m_LadderTeamInfo.m_iWinRecord / (float)max( 1, m_LadderTeamInfo.m_iWinRecord + m_LadderTeamInfo.m_iLoseRecord ) ) * 100;
			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.PrintText( iXPos + 415, iYPos + 8, FONT_SIZE_12, STR(4), iWinLoseRate );
		}
		else
		{
			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.PrintText( iXPos + 415, iYPos + 8, FONT_SIZE_12, STR(5), m_LadderTeamInfo.m_iCurPlayer, m_LadderTeamInfo.m_iMaxPlayer );
		}

		if( g_LadderTeamMgr.IsLadderTeam() && COMPARE( m_AniArray, 0, (int)m_ArrowAni.size() ) )
		{
			if( g_LadderTeamMgr.GetLadderTeamIndex() == m_LadderTeamInfo.m_dwTeamIndex )
			{
				m_pArrow->Render( ( iXPos - iMinusXPos ) + m_ArrowAni[m_AniArray].m_iOffsetX, iYPos + 5 );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
LadderTeamRankListWnd::LadderTeamRankListWnd()
{
	m_dwMainWndTabID        = 0;
	m_iCurPage = m_iMaxPage = 0;
	m_dwSearchTime			= 0;
	m_bHeroMatch			= false;

	m_pBlueCampMark = NULL;
	m_pRedCampMark = NULL;
}

LadderTeamRankListWnd::~LadderTeamRankListWnd()
{
	SAFEDELETE( m_pBlueCampMark );
	SAFEDELETE( m_pRedCampMark );
	m_vLadderTeamList.clear();
}

void LadderTeamRankListWnd::StartSend( DWORD dwTeamIndex )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_LADDER_TEAM_RANK_LIST );
	kPacket << dwTeamIndex << m_iCurPage << MAX_RANK_LIST << m_bHeroMatch;
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

void LadderTeamRankListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > RANK_SEARCH_TIME )
	{
		StartSend( 0 );
	}
}

void LadderTeamRankListWnd::iwm_show()
{	
	m_iCurPage = m_iMaxPage = 0;
	m_vLadderTeamList.clear();
	
	StartSend( g_LadderTeamMgr.GetLadderTeamIndex() );
	DWORD dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwSelectIndex );

	if( pTournament )
	{
		m_pBlueCampMark = pTournament->GetCampSmallIcon( CAMP_BLUE );
		m_pRedCampMark = pTournament->GetCampSmallIcon( CAMP_RED );
	}

	for(int i = ID_LIST_1;i < ID_LIST_8 + 1;i++)
	{
		LadderRankListBtn *pBtn = (LadderRankListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			LadderTeamInfo kInfo;
			pBtn->SetLadderTeamInfo( kInfo, m_bHeroMatch );
			pBtn->SetTeamIcon( m_pBlueCampMark, m_pRedCampMark );			
		}
	}	
}

void LadderTeamRankListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
		if( cmd == IOBN_BTNUP )
		{
			LadderRankListBtn *pBtn = (LadderRankListBtn*)pWnd;
			if( pBtn->GetLadderTeamIndex() == 0 ) return;

			LadderTeamInfoWnd *pInfoWnd = dynamic_cast<LadderTeamInfoWnd *>(g_GUIMgr.FindWnd(LADDER_TEAM_INFO_WND));
			if( pInfoWnd )
			{
				pInfoWnd->SetInfo( pBtn->GetLadderTeamInfo(), false );

				SP2Packet kPacket( CTPK_LADDERTEAM_JOIN_INFO );
				kPacket << pBtn->GetLadderTeamIndex();
				TCPNetwork::SendToServer( kPacket );
			}            
		}
		break;
	}
}

void LadderTeamRankListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSearch();		
}

void LadderTeamRankListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 212, iYPos + 264, TAT_CENTER );
}

void LadderTeamRankListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend( 0 );

	int iListSIze = m_vLadderTeamList.size();
	int iStartPos = 0;
	for(int i = ID_LIST_1;i < ID_LIST_8 + 1;i++,iStartPos++)
	{
		LadderRankListBtn *pBtn = (LadderRankListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetLadderTeamInfo( m_vLadderTeamList[iStartPos], m_bHeroMatch );
			}
			else
			{
				LadderTeamInfo kInfo;
				pBtn->SetLadderTeamInfo( kInfo, m_bHeroMatch );
			}
		}
	}	
}

void LadderTeamRankListWnd::ApplyLadderTeamRank( SP2Packet &rkPacket )
{	
	m_vLadderTeamList.clear();

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> m_iCurPage >> iTotalList >> iCurList;		
	for(i = 0;i < iCurList;i++)
	{
		LadderTeamInfo kInfo;
		rkPacket >> kInfo.m_dwTeamIndex >> kInfo.m_iCampType >> kInfo.m_iTeamRanking >> kInfo.m_iAverageLevel >> kInfo.m_szTeamName >> kInfo.m_dwGuildIndex >> kInfo.m_dwGuildMark
			     >> kInfo.m_szTeamPW >> kInfo.m_iWinRecord >> kInfo.m_iLoseRecord >> kInfo.m_iVictoriesRecord >> kInfo.m_iCurPlayer >> kInfo.m_iMaxPlayer;
		m_vLadderTeamList.push_back( kInfo );
	}

	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_RANK_LIST;
	UpdateSetPage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
LadderHeroRankListWnd::LadderHeroRankListWnd()
{
	m_dwMainWndTabID        = 0;
	m_iCurPage = m_iMaxPage = 0;
	m_dwSearchTime			= 0;
	m_bHeroMatch			= true;

	m_pBlueCampMark = NULL;
	m_pRedCampMark = NULL;

}

LadderHeroRankListWnd::~LadderHeroRankListWnd()
{
	SAFEDELETE( m_pBlueCampMark );
	SAFEDELETE( m_pRedCampMark );

	m_vLadderHeroList.clear();
}

void LadderHeroRankListWnd::StartSend( DWORD dwTeamIndex )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_LADDER_TEAM_RANK_LIST );
	kPacket << dwTeamIndex << m_iCurPage << MAX_RANK_LIST << m_bHeroMatch;
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

void LadderHeroRankListWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > RANK_SEARCH_TIME )
	{
		StartSend( 0 );
	}
}

void LadderHeroRankListWnd::iwm_show()
{	
	m_iCurPage = m_iMaxPage = 0;
	m_vLadderHeroList.clear();

	StartSend( 0 );
	DWORD dwSelectIndex = g_TournamentMgr.GetRegularIndex();
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwSelectIndex );
	if( pTournament )
	{
		m_pBlueCampMark = pTournament->GetCampSmallIcon( CAMP_BLUE );
		m_pRedCampMark = pTournament->GetCampSmallIcon( CAMP_RED );
	}

	for(int i = ID_LIST_1;i < ID_LIST_8 + 1;i++)
	{
		LadderRankListBtn *pBtn = (LadderRankListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			LadderTeamInfo kInfo;
			pBtn->SetLadderTeamInfo( kInfo, m_bHeroMatch );
			pBtn->SetTeamIcon( m_pBlueCampMark, m_pRedCampMark );			
		}
	}
}

void LadderHeroRankListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
		if( cmd == IOBN_BTNUP )
		{
			LadderRankListBtn *pBtn = (LadderRankListBtn*)pWnd;
			if( pBtn->GetLadderTeamIndex() == 0 ) return;
			
			g_App.OpenMannerWnd( pBtn->GetTeamName().c_str(), true );
		}
		break;
	}
}


void LadderHeroRankListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSearch();		
}

void LadderHeroRankListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 212, iYPos + 264, TAT_CENTER );
}

void LadderHeroRankListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend( 0 );

	int iListSIze = m_vLadderHeroList.size();
	int iStartPos = 0;
	for(int i = ID_LIST_1;i < ID_LIST_8 + 1;i++,iStartPos++)
	{
		LadderRankListBtn *pBtn = (LadderRankListBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetLadderTeamInfo( m_vLadderHeroList[iStartPos], m_bHeroMatch );
			}
			else
			{
				LadderTeamInfo kInfo;
				pBtn->SetLadderTeamInfo( kInfo, m_bHeroMatch );
			}
		}
	}	
}

void LadderHeroRankListWnd::ApplyLadderHeroRank( SP2Packet &rkPacket )
{	
	m_vLadderHeroList.clear();

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> m_iCurPage >> iTotalList >> iCurList;		
	for(i = 0;i < iCurList;i++)
	{
		LadderTeamInfo kInfo;
		rkPacket >> kInfo.m_dwTeamIndex >> kInfo.m_iCampType >> kInfo.m_iTeamRanking >> kInfo.m_iAverageLevel >> kInfo.m_szTeamName >> kInfo.m_dwGuildIndex >> kInfo.m_dwGuildMark
			>> kInfo.m_szTeamPW >> kInfo.m_iWinRecord >> kInfo.m_iLoseRecord >> kInfo.m_iVictoriesRecord >> kInfo.m_iCurPlayer >> kInfo.m_iMaxPlayer;
		m_vLadderHeroList.push_back( kInfo );
	}

	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_RANK_LIST;
	UpdateSetPage( m_iCurPage );
}
