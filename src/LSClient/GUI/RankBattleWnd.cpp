

#include "StdAfx.h"

#include "RankBattleWnd.h"
#include "LadderTeamWnd.h"
#include "HeroMainWnd.h"
#include "ioInterfaceBackground.h"

#include "../ioGuildInfo.h"
#include "../ioGuildMarkMgr.h"
#include "../ioRankBattleManager.h"

RankBattleWnd::RankBattleWnd() : m_pPlayStage( NULL )
{
	m_pUI3DRender = NULL;
	m_aMyChar = NULL;
	m_pModeNMapGrayFrm = NULL;
	m_pRankTitle = NULL;
	m_pGradeTitle = NULL;
	m_pSeasonTitle = NULL;
	m_pIconBack	= NULL;
	m_pRecordIcon	= NULL;
	m_pNumber = NULL;
	m_pRankText	= NULL;
	m_pPercentText	= NULL;
	m_pNoneText	= NULL;
	m_pNoneIcon	= NULL;
	m_pDarkBar = NULL;
	m_pLightBar = NULL;
	for( int i = 0; i < RANK_SIZE; i++ )
	{
		m_pFrameBG[i] = NULL;
		m_pImageBG[i] = NULL;
		m_pGradation[i] = NULL;
		m_pRankContext[i] = NULL;
		m_pRankIcon[i] = NULL;
	}
	m_pWarCountCircle = NULL;
	m_pWarCountWait = NULL;
	m_pTextBack = NULL;

	g_SoundMgr.StopStream();

	m_dwChangeCountTime = 0;
	m_dwCurSearchTime = 0;
}

RankBattleWnd::~RankBattleWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_pModeNMapGrayFrm );
	SAFEDELETE( m_pRankTitle );
	SAFEDELETE( m_pGradeTitle );
	SAFEDELETE( m_pSeasonTitle );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pRecordIcon );
	SAFEDELETE( m_pNumber );
	SAFEDELETE( m_pRankText );
	SAFEDELETE( m_pPercentText );
	SAFEDELETE( m_pNoneText );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pDarkBar );
	SAFEDELETE( m_pLightBar );
	for( int i = 0; i < RANK_SIZE; i++ )
	{
		SAFEDELETE( m_pFrameBG[i] );
		SAFEDELETE( m_pImageBG[i] );
		SAFEDELETE( m_pGradation[i] );
		SAFEDELETE( m_pRankContext[i] );
		SAFEDELETE( m_pRankIcon[i] );
	}
	SAFEDELETE( m_pWarCountCircle );
	SAFEDELETE( m_pWarCountWait );
	SAFEDELETE( m_pTextBack );

	g_RankBattleMgr.SendCancel();
}

void RankBattleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szBackOgg = xElement.GetStringAttribute_e( "BackOGG" );
	m_szRemainTime = xElement.GetStringAttribute_e( "RemainTime" );
	m_szAlarmText = xElement.GetStringAttribute_e( "AlarmText" );

	char szName[MAX_PATH]="";
	for (int i = 0; i < 4 ; i++)
	{
		StringCbPrintf( szName, sizeof( szName ), "OnModeNMapRender%d", i+1 );
		m_szOnModeNMapRender[i] = xElement.GetStringAttribute( szName );
	}
	for (int i = 0; i < 6 ; i++)
	{
		StringCbPrintf( szName, sizeof( szName ), "DescString%d", i+1 );
		m_szDescString[i] = xElement.GetStringAttribute( szName );
	}
}

void RankBattleWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ModeNMapGray" )
	{
		SAFEDELETE( m_pModeNMapGrayFrm );
		m_pModeNMapGrayFrm = pFrame;
	}
	else if( szType == "FrameBG1" )
	{
		SAFEDELETE( m_pFrameBG[0] );
		m_pFrameBG[0] = pFrame;
	}
	else if( szType == "FrameBG2" )
	{
		SAFEDELETE( m_pFrameBG[1] );
		m_pFrameBG[1] = pFrame;
	}
	else if( szType == "FrameBG3" )
	{
		SAFEDELETE( m_pFrameBG[2] );
		m_pFrameBG[2] = pFrame;
	}
	else if( szType == "FrameBG4" )
	{
		SAFEDELETE( m_pFrameBG[3] );
		m_pFrameBG[3] = pFrame;
	}
	else if( szType == "FrameBG5" )
	{
		SAFEDELETE( m_pFrameBG[4] );
		m_pFrameBG[4] = pFrame;
	}
	else if( szType == "FrameBG6" )
	{
		SAFEDELETE( m_pFrameBG[5] );
		m_pFrameBG[5] = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void RankBattleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RankTitle" )
	{
		SAFEDELETE( m_pRankTitle );
		m_pRankTitle = pImage;
	}
	else if( szType == "GradeTitle" )
	{
		SAFEDELETE( m_pGradeTitle );
		m_pGradeTitle = pImage;
	}
	else if( szType == "SeasonTitle" )
	{
		SAFEDELETE( m_pSeasonTitle );
		m_pSeasonTitle = pImage;
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
	else if( szType == "ImageBG1" )
	{
		SAFEDELETE( m_pImageBG[0] );
		m_pImageBG[0] = pImage;
	}
	else if( szType == "ImageBG2" )
	{
		SAFEDELETE( m_pImageBG[1] );
		m_pImageBG[1] = pImage;
	}
	else if( szType == "ImageBG3" )
	{
		SAFEDELETE( m_pImageBG[2] );
		m_pImageBG[2] = pImage;
	}
	else if( szType == "ImageBG4" )
	{
		SAFEDELETE( m_pImageBG[3] );
		m_pImageBG[3] = pImage;
	}
	else if( szType == "ImageBG5" )
	{
		SAFEDELETE( m_pImageBG[4] );
		m_pImageBG[4] = pImage;
	}
	else if( szType == "ImageBG6" )
	{
		SAFEDELETE( m_pImageBG[5] );
		m_pImageBG[5] = pImage;
	}
	else if( szType == "Gradation1" )
	{
		SAFEDELETE( m_pGradation[0] );
		m_pGradation[0] = pImage;
	}
	else if( szType == "Gradation2" )
	{
		SAFEDELETE( m_pGradation[1] );
		m_pGradation[1] = pImage;
	}
	else if( szType == "Gradation3" )
	{
		SAFEDELETE( m_pGradation[2] );
		m_pGradation[2] = pImage;
	}
	else if( szType == "Gradation4" )
	{
		SAFEDELETE( m_pGradation[3] );
		m_pGradation[3] = pImage;
	}
	else if( szType == "Gradation5" )
	{
		SAFEDELETE( m_pGradation[4] );
		m_pGradation[4] = pImage;
	}
	else if( szType == "Gradation6" )
	{
		SAFEDELETE( m_pGradation[5] );
		m_pGradation[5] = pImage;
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
	else if( szType == "RankContext1" )
	{
		SAFEDELETE( m_pRankContext[0] );
		m_pRankContext[0] = pImage;
	}
	else if( szType == "RankContext2" )
	{
		SAFEDELETE( m_pRankContext[1] );
		m_pRankContext[1] = pImage;
	}
	else if( szType == "RankContext3" )
	{
		SAFEDELETE( m_pRankContext[2] );
		m_pRankContext[2] = pImage;
	}
	else if( szType == "RankContext4" )
	{
		SAFEDELETE( m_pRankContext[3] );
		m_pRankContext[3] = pImage;
	}
	else if( szType == "RankContext5" )
	{
		SAFEDELETE( m_pRankContext[4] );
		m_pRankContext[4] = pImage;
	}
	else if( szType == "RankContext6" )
	{
		SAFEDELETE( m_pRankContext[5] );
		m_pRankContext[5] = pImage;
	}
	else if( szType == "RankIcon1" )
	{
		SAFEDELETE( m_pRankIcon[0] );
		m_pRankIcon[0] = pImage;
	}
	else if( szType == "RankIcon2" )
	{
		SAFEDELETE( m_pRankIcon[1] );
		m_pRankIcon[1] = pImage;
	}
	else if( szType == "RankIcon3" )
	{
		SAFEDELETE( m_pRankIcon[2] );
		m_pRankIcon[2] = pImage;
	}
	else if( szType == "RankIcon4" )
	{
		SAFEDELETE( m_pRankIcon[3] );
		m_pRankIcon[3] = pImage;
	}
	else if( szType == "RankIcon5" )
	{
		SAFEDELETE( m_pRankIcon[4] );
		m_pRankIcon[4] = pImage;
	}
	else if( szType == "RankIcon6" )
	{
		SAFEDELETE( m_pRankIcon[5] );
		m_pRankIcon[5] = pImage;
	}
	else if( szType == "count_circle" )
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
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RankBattleWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	LadderChatWnd *pLadderChat = dynamic_cast<LadderChatWnd*>( FindChildWnd( ID_CHAT_WND ) );
	if( pLadderChat )
		pLadderChat->SetPlayStage( pPlayStage, CT_SERVER_LOBBY );

	m_pPlayStage = pPlayStage;
}

void RankBattleWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/ladderchatwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatcrankgraphwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatctop100wnd.xml", this );
	g_GUIMgr.AddWnd( "XML/heromatchhistorywnd.xml", this );

	HeroMatchRankGraphWnd *pGraphWnd = dynamic_cast<HeroMatchRankGraphWnd*>( FindChildWnd( ID_RANK_GRAPH_WND ) );
	if( pGraphWnd )
	{
		pGraphWnd->SetMode( RANK_BATTLE );
		pGraphWnd->SetWndPos( 414, 127 );
	}

	HeroMatchTop100Wnd *p100Wnd = dynamic_cast<HeroMatchTop100Wnd*>( FindChildWnd( ID_RANK_TOP100_WND ) );
	if( p100Wnd )
	{
		p100Wnd->SetMode( RANK_BATTLE );
		p100Wnd->SetWndPos( 414, 127 );
	}

	HeroMatchHistoryWnd *pRecordWnd = dynamic_cast<HeroMatchHistoryWnd*>( FindChildWnd( ID_BATTLE_RECORD_WND ) );
	if( pRecordWnd )
	{
		pRecordWnd->SetMode( RANK_BATTLE );
		pRecordWnd->SetWndPos( 414, 127 );
	}

	SAFEDELETE( m_pUI3DRender );
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void RankBattleWnd::iwm_show()
{
	ShowChildWnd( ID_CHAT_WND );

	if( !m_szBackOgg.IsEmpty() )
	{
		g_SoundMgr.StopStream();

		char szName[MAX_PATH];
		sprintf( szName, "Resource/wave/%s", m_szBackOgg.c_str() );
		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}
	m_dwChangeCountTime = 0;
	m_dwCurSearchTime = 0;

	ResetButtons();	
	CreateCharacter();
	CheckTabWnd( ID_TAB_RANK_GRAPH );

	int iRankGrade = g_RankBattleMgr.GetRankGrade();
	int iPrevGrade = g_RankBattleMgr.GetPrevRankGrade();
	if( iRankGrade != iPrevGrade )
	{
		g_RankBattleMgr.SetPrevRankGrade( iRankGrade );
		ioRankBattleUpWnd *pWnd = dynamic_cast<ioRankBattleUpWnd*>( g_GUIMgr.FindWnd( RANKBATTLE_UP_WND ) );
		if( pWnd )
			pWnd->SetPrintMessage( iRankGrade, iRankGrade > iPrevGrade );
	}
	if( g_RankBattleMgr.IsActive() )
		SetChildActive( ID_SEARCH_START );
	else
		SetChildInActive( ID_SEARCH_START );

	g_GUIMgr.ShowReserveMsgBox();
}

void RankBattleWnd::iwm_hide()
{
	HideChildWnd( ID_CHAT_WND );
}

bool RankBattleWnd::iwm_esc()
{
	LadderChatWnd *pLadderChat = dynamic_cast<LadderChatWnd*>( FindChildWnd( ID_CHAT_WND ) );
	if( pLadderChat && pLadderChat->IsShow() )
		return pLadderChat->iwm_esc();

	return false;
}

void RankBattleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_MY_RANK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenMannerWnd( g_MyInfo.GetPublicID().c_str(), true );
		}
		break;
	case ID_TAB_RANK_GRAPH:
	case ID_TAB_RANK_TOP100:
	case ID_TAB_BATTLE_RECORD:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckTabWnd( dwID );
		}
		break;
	case ID_SEARCH_START:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
				return;
			}
			g_RankBattleMgr.SendRequest();
			m_dwChangeCountTime = FRAMEGETTIME();
			m_dwCurSearchTime = FRAMEGETTIME();
		}
		break;
	case ID_SEARCH_END:
		if( cmd == IOBN_BTNUP )
		{
			g_RankBattleMgr.SendCancel();
		}
		break;
	case ID_EXIT:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			g_App.AltF4ExitProcess( false );
		}
		break;
	case ID_TOOLTIP:
		{
			if( cmd == IOBN_BTNUP )
			{
				g_pToolTipMgr->CloseToolTip();

				int iHelpTipIdx = g_RankBattleMgr.GetHelpTipIdx();
				if( iHelpTipIdx == 0 )
					return;

				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( iHelpTipIdx );
			}
			else if( cmd == IOWN_OVERED )
			{
				g_pToolTipMgr->OpenToolTip( IoToolTipManager::eToolTip_Base, NULL, NULL, NULL, g_RankBattleMgr.GetInfoTooltip().c_str() );
			}
			else if( cmd == IOWN_LEAVED )
			{
				g_pToolTipMgr->CloseToolTip();
			}
		}
		break;
	}
}

void RankBattleWnd::OnProcess( float fTimePerSec )
{
	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );

	g_BackgroundMgr.ProcessBackground();

	if( g_RankBattleMgr.IsSendRequest() )
	{
		int iCurSec  = (FRAMEGETTIME() - m_dwChangeCountTime) / 1000;
		if( iCurSec > 0 )
			m_dwChangeCountTime = FRAMEGETTIME();
	}
}

void RankBattleWnd::OnRender()
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
	kPrinter.AddTextPiece( FONT_SIZE_17, "·©Å·Àü" );

	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, " | " );
	kPrinter.SetTextColor( 255, 255, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_17, g_MyInfo.GetPublicID().c_str() );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	//±æµå ¸¶Å©
	DWORD dwGuildIndex = g_GuildInfo.GetGuildIndex();
	DWORD dwGuildMark = 0;
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( dwGuildIndex, true );
	if( pGuildData )
		dwGuildMark = pGuildData->GetGuildMark();

	g_GuildMarkMgr.RenderMarkSize( 31, 31, dwGuildIndex, dwGuildMark, iXPos + 27, iYPos + 62 );
	
	char szBuf[MAX_PATH] = "";
	int iMyRank = g_RankBattleMgr.GetRank();
	if( iMyRank != 0 )
		wsprintf( szBuf, m_szDescString[0].c_str(), iMyRank, g_RankBattleMgr.GetRankGradeString().c_str(), g_MyInfo.GetPublicID().c_str() );
	else
		wsprintf( szBuf, m_szDescString[1].c_str(), g_RankBattleMgr.GetRankGradeString().c_str(), g_MyInfo.GetPublicID().c_str() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, szBuf );
	kPrinter.PrintFullText( iXPos + 64, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	wsprintf( szBuf, m_szDescString[2].c_str(), g_RankBattleMgr.GetRankPoint() );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, szBuf );
	kPrinter.PrintFullText( iXPos + 64, iYPos + 79, TAT_LEFT );
	kPrinter.ClearList();

	OnModeNMapRender( iXPos + 416, iYPos + 385 );
	OnRenderInfo( iXPos, iYPos );
}

void RankBattleWnd::OnRenderAfterChild()
{
	if( !g_RankBattleMgr.IsSendRequest() )
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
		m_pWarCountWait->Render( iXPos+CIRCLE_X, iYPos+CIRCLE_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Text
	int iCurSec = (FRAMEGETTIME() - m_dwCurSearchTime) / 1000;
	int iRamainSec = max( 0, SEARCH_TIME - iCurSec );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + TEXT_BACK_X, iYPos + TEXT_BACK_Y - 8, FONT_SIZE_15, m_szRemainTime.c_str(), iRamainSec );

	if( ( iRamainSec <= 90 && iRamainSec > 85 ) ||
		( iRamainSec <= 60 && iRamainSec > 55 ) )
	{
		g_FontMgr.SetBkColor( 58, 58, 58 );
		g_FontMgr.SetTextColor( 167, 167, 167 );
		g_FontMgr.PrintText( iXPos + TEXT_BACK_X, iYPos + CIRCLE_Y + 30, FONT_SIZE_14, m_szAlarmText.c_str() );
	}
}

void RankBattleWnd::OnRenderInfo( int iXPos, int iYPos )
{
	int iRankGrade = g_RankBattleMgr.GetRankGrade();
	int iRecodeRate = g_RankBattleMgr.GetWinRate();
	if( !COMPARE( iRankGrade, RANK_BRONZE, RANK_SIZE ) )
		return;

	if( m_pFrameBG[iRankGrade] )
	{
		m_pFrameBG[iRankGrade]->Render( iXPos + 27, iYPos + 98 );
		m_pFrameBG[iRankGrade]->Render( iXPos + 283, iYPos + 98 );
	}
	if( m_pImageBG[iRankGrade] )
		m_pImageBG[iRankGrade]->Render( iXPos + 155, iYPos + 98 );
	if( m_aMyChar )
		OnRenderSoldier( iXPos + 30, iYPos + 101 );
	else if( m_pNoneIcon && m_pIconBack )
	{
		if( m_pIconBack )
			m_pIconBack->Render( iXPos + 90, iYPos + 159, UI_RENDER_MULTIPLY );
		if( m_pNoneIcon )
			m_pNoneIcon->Render( iXPos + 90, iYPos + 159 );
	}
	if( m_pGradation[iRankGrade] )
		m_pGradation[iRankGrade]->Render( iXPos + 30, iYPos + 286 );

	if( m_pRankTitle )
		m_pRankTitle->Render( iXPos + 90, iYPos + 212 );	

	int iRank = g_RankBattleMgr.GetRank();
	int iNumberCount = GetNumberCount( iRank );
	if( iRank == 0 || iNumberCount >= 6 )
	{
		if( m_pNoneText )
			m_pNoneText->Render( iXPos + 90, iYPos + 230 );
	}
	else
	{
		if( iNumberCount >= 5 )
		{
			if( m_pNumber )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pNumber->RenderNum( iXPos + 90, iYPos + 230, iRank, -NUM_GAP );
			}
		}
		else
		{
			int iNumberSize = (iNumberCount * NUM_W) - ((iNumberCount - 1) * NUM_GAP);
			int iStartPos   = (iNumberSize + TEXT_RANK_W) / 2;
			if( m_pNumber && m_pRankText )
			{
				m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
				m_pNumber->RenderNum( (iXPos + 90) - iStartPos, iYPos + 230, iRank, -NUM_GAP );

				m_pRankText->Render( ((iXPos + 90) - iStartPos) + (iNumberSize - NUM_GAP), iYPos + 230 );
			}
		}
	}

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 218, iYPos + 159, UI_RENDER_MULTIPLY );
		m_pIconBack->Render( iXPos + 346, iYPos + 159, UI_RENDER_MULTIPLY );
	}
	if( m_pGradeTitle )
		m_pGradeTitle->Render( iXPos + 218, iYPos + 212 );
	if( m_pRankIcon[iRankGrade] )
		m_pRankIcon[iRankGrade]->Render( iXPos + 218, iYPos + 159 );
	if( m_pRankContext[iRankGrade] )
		m_pRankContext[iRankGrade]->Render( iXPos + 218, iYPos + 230 );

	if( m_pSeasonTitle )
		m_pSeasonTitle->Render( iXPos + 346, iYPos + 212 );
	if( m_pRecordIcon )
		m_pRecordIcon->Render( iXPos + 346, iYPos + 159 );

	iNumberCount = GetNumberCount( iRecodeRate );
	int iNumberSize = (iNumberCount * NUM_W) - ((iNumberCount - 1) * NUM_GAP);
	int iStartPos   = (iNumberSize + TEXT_SEASON_W) / 2;
	if( m_pNumber && m_pPercentText )
	{
		m_pNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pNumber->RenderNum( (iXPos + 346) - iStartPos, iYPos + 230, iRecodeRate, -NUM_GAP );

		m_pPercentText->Render( ((iXPos + 346) - iStartPos) + (iNumberSize - NUM_GAP), iYPos + 230 );
	}

	if( m_pDarkBar && m_pLightBar )
	{
		m_pDarkBar->Render( iXPos + 286, iYPos + 299 );
		m_pLightBar->Render( iXPos + 286, iYPos + 320 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	switch( iRankGrade )
	{
	case RANK_BRONZE:		g_FontMgr.SetBkColor( 75, 57, 35 );		break;
	case RANK_SILVER:		g_FontMgr.SetBkColor( 58, 58, 58 );		break;
	case RANK_GOLD:			g_FontMgr.SetBkColor( 141, 87, 4 );		break;
	case RANK_PLATINUM:		g_FontMgr.SetBkColor( 10, 95, 85 );		break;
	case RANK_DIA:			g_FontMgr.SetBkColor( 30, 65, 75 );		break;
	case RANK_MASTER:		g_FontMgr.SetBkColor( 115, 20, 5 );		break;
	}
	char szBuf[MAX_PATH] = "";
	wsprintf( szBuf, m_szDescString[3].c_str(), g_RankBattleMgr.GetRankPoint() );
	g_FontMgr.PrintText( iXPos + 218, iYPos + 301, FONT_SIZE_13, szBuf );
	wsprintf( szBuf, m_szDescString[4].c_str(), g_RankBattleMgr.GetTotalWin() );
	g_FontMgr.PrintText( iXPos + 346, iYPos + 301, FONT_SIZE_13, szBuf );
	wsprintf( szBuf, m_szDescString[5].c_str(), g_RankBattleMgr.GetTotalLose() );
	g_FontMgr.PrintText( iXPos + 346, iYPos + 322, FONT_SIZE_13, szBuf );
}

void RankBattleWnd::OnModeNMapRender( int iXPos, int iYPos )
{
	if( !m_pModeNMapGrayFrm ) return;

	// ¸ðµå
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, m_szOnModeNMapRender[0].c_str() );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 54, iYPos + 8, FONT_SIZE_12, m_szOnModeNMapRender[1].c_str() );

	// ¸Ê
	iXPos += 165;
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, m_szOnModeNMapRender[2].c_str() );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 54, iYPos + 8, FONT_SIZE_12, m_szOnModeNMapRender[3].c_str() );
}

void RankBattleWnd::OnRenderSoldier( int iXPos, int iYPos )
{
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

void RankBattleWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	HeroMatchRankGraphWnd *pGraphWnd = dynamic_cast<HeroMatchRankGraphWnd*>( FindChildWnd( ID_RANK_GRAPH_WND ) );
	if( pGraphWnd )
	{
		pGraphWnd->SetMode( RANK_BATTLE );
		pGraphWnd->SetWndPos( 414, 127 );
	}

	HeroMatchTop100Wnd *p100Wnd = dynamic_cast<HeroMatchTop100Wnd*>( FindChildWnd( ID_RANK_TOP100_WND ) );
	if( p100Wnd )
	{
		p100Wnd->SetMode( RANK_BATTLE );
		p100Wnd->SetWndPos( 414, 127 );
	}

	HeroMatchHistoryWnd *pRecordWnd = dynamic_cast<HeroMatchHistoryWnd*>( FindChildWnd( ID_BATTLE_RECORD_WND ) );
	if( pRecordWnd )
	{
		pRecordWnd->SetMode( RANK_BATTLE );
		pRecordWnd->SetWndPos( 414, 127 );
	}
}

void RankBattleWnd::CreateCharacter()
{
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

	// ÄÚ½ºÆ¬ °ü·Ã (??)
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

void RankBattleWnd::CheckTabWnd( DWORD dwID )
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

void RankBattleWnd::SetRankGraph()
{
	HeroMatchRankGraphWnd *pGraphWnd = dynamic_cast<HeroMatchRankGraphWnd*>( FindChildWnd( ID_RANK_GRAPH_WND ) );
	if( pGraphWnd )
		pGraphWnd->SetRankBattleGraph();
}

void RankBattleWnd::ResetButtons()
{
	if( g_RankBattleMgr.IsSendRequest() )
	{
		HideChildWnd( ID_SEARCH_START );
		ShowChildWnd( ID_SEARCH_END );
	}
	else
	{
		ShowChildWnd( ID_SEARCH_START );
		HideChildWnd( ID_SEARCH_END );
	}
}
//////////////////////////////////////////////////////////////////////////
ioRankBattleUpWnd::ioRankBattleUpWnd()
{
	m_pIconBackUp = NULL;
	m_pIconBackDown = NULL;
	for( int i = 0; i < RANK_SIZE; i++ )
		m_pRankIcon[i] = NULL;

	m_iRankGrade = 0;
}

ioRankBattleUpWnd::~ioRankBattleUpWnd()
{
	SAFEDELETE( m_pIconBackUp );
	SAFEDELETE( m_pIconBackDown );
	for( int i = 0; i < RANK_SIZE; i++ )
		SAFEDELETE( m_pRankIcon[i] );
}

void ioRankBattleUpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBackUp" )
	{
		SAFEDELETE( m_pIconBackUp );
		m_pIconBackUp = pImage;
	}
	else if( szType == "IconBackDown" )
	{
		SAFEDELETE( m_pIconBackDown );
		m_pIconBackDown = pImage;
	}
	else if( szType == "RankIcon1" )
	{
		SAFEDELETE( m_pRankIcon[0] );
		m_pRankIcon[0] = pImage;
	}
	else if( szType == "RankIcon2" )
	{
		SAFEDELETE( m_pRankIcon[1] );
		m_pRankIcon[1] = pImage;
	}
	else if( szType == "RankIcon3" )
	{
		SAFEDELETE( m_pRankIcon[2] );
		m_pRankIcon[2] = pImage;
	}
	else if( szType == "RankIcon4" )
	{
		SAFEDELETE( m_pRankIcon[3] );
		m_pRankIcon[3] = pImage;
	}
	else if( szType == "RankIcon5" )
	{
		SAFEDELETE( m_pRankIcon[4] );
		m_pRankIcon[4] = pImage;
	}
	else if( szType == "RankIcon6" )
	{
		SAFEDELETE( m_pRankIcon[5] );
		m_pRankIcon[5] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioRankBattleUpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szUpSound = xElement.GetStringAttribute_e( "Level_Up_Sound" );
	m_szUpTitle = xElement.GetStringAttribute_e( "TitleUp" );
	m_szUpDesc = xElement.GetStringAttribute_e( "DescUp" );
	m_szDownTitle = xElement.GetStringAttribute_e( "TitleDown" );
	m_szDownDesc = xElement.GetStringAttribute_e( "DescDown" );
	m_szRankUpText[0] = xElement.GetStringAttribute_e( "RankUpText1" );
	m_szRankUpText[1] = xElement.GetStringAttribute_e( "RankUpText2" );
	m_szRankDownText = xElement.GetStringAttribute_e( "RankDownText" );
}

void ioRankBattleUpWnd::SetPrintMessage( int iRankGrade, bool bUp )
{
	m_iRankGrade = iRankGrade;
	m_bUp = bUp;
	if( m_bUp )
	{
		SetTitleText( m_szUpTitle.c_str() );
		if( !m_szUpSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szUpSound );
	}
	else
	{
		SetTitleText( m_szDownTitle.c_str() );
	}
	
	if( !IsShow() )
		ShowWnd();
}

void ioRankBattleUpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioRankBattleUpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
		
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_bUp )
	{
		if( m_pIconBackUp )
			m_pIconBackUp->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_17, m_szRankUpText[0].c_str() );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_17, g_RankBattleMgr.GetRankGradeString().c_str() );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_17, m_szRankUpText[1].c_str() );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, m_szUpDesc.c_str() );
	}
	else
	{
		if( m_pIconBackDown )
			m_pIconBackDown->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_17, m_szRankDownText.c_str() );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, m_szDownDesc.c_str() );
	}
	kPrinter.PrintFullText( iXPos + 125, iYPos + 146, TAT_CENTER );

	if( m_pRankIcon[m_iRankGrade] )
		m_pRankIcon[m_iRankGrade]->Render( iXPos + 125, iYPos + 98 );
}

void ioRankBattleUpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void ioRankBattleUpWnd::iwm_show()
{
	ShowChildWnd( ID_ALPHA_WND );
}

void ioRankBattleUpWnd::iwm_hide()
{
	HideChildWnd( ID_ALPHA_WND );

	if( !m_szUpSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szUpSound, 0 );
}

void RankBattleWnd::ResizeScroll()
{
	HeroMatchTop100Wnd *p100Wnd = dynamic_cast<HeroMatchTop100Wnd*>( FindChildWnd( ID_RANK_TOP100_WND ) );
	if( p100Wnd )
		p100Wnd->ResizeScroll();
}