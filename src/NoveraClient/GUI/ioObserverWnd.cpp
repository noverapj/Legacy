

#include "StdAfx.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/iostringmanager.h"

#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../ioBaseChar.h"
#include "../ioPlayStage.h"
#include "../EtcHelpFunc.h"
#include "../ioApplication.h"
#include "../ioModeSelectInfo.h"
#include "../ioGuildMarkMgr.h"
#include "../TextColorTable.h"

#include "TimeScoreWnd.h"
#include "ioUserInfoMgr.h"
#include "ioSP2GUIManager.h"
#include "ioObserverWnd.h"

bool ioObserverRedWnd::m_bPosToDefualt = false;

ioObserverRedWnd::ioObserverRedWnd()
{
	m_pRedBack = NULL;
	m_pRedGauge = NULL;
	m_pBlueBack = NULL;
	m_pBlueGauge = NULL;
	m_pRedDead = NULL;
	m_pAddGauge = NULL;
	m_pGuildBack = NULL;

	m_pTargetMark = NULL;

	for( int i=0; i < MAX_TEAM_PLAYER; ++i )
	{
		m_pRedKeyText[i] = NULL;
		m_pBlueKeyText[i] = NULL;
	}

	m_pPlayStage = NULL;

	m_iDefaultX = 0;
	m_iDefaultY = 0;

	m_bCheckMark = false;
	m_dwCheckTime = 0;

	m_RoundInfoList.clear();
}

ioObserverRedWnd::~ioObserverRedWnd()
{
	m_RoundInfoList.clear();

	SAFEDELETE( m_pRedBack );
	SAFEDELETE( m_pRedGauge );
	SAFEDELETE( m_pBlueBack );
	SAFEDELETE( m_pBlueGauge );
	SAFEDELETE( m_pRedDead );
	SAFEDELETE( m_pAddGauge );
	SAFEDELETE( m_pTargetMark );
	SAFEDELETE( m_pGuildBack );

	for( int i=0; i < MAX_TEAM_PLAYER; ++i )
	{
		SAFEDELETE( m_pRedKeyText[i] );
		SAFEDELETE( m_pBlueKeyText[i] );
	}

	m_pPlayStage = NULL;

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "ObserverRedWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "ObserverRedWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void ioObserverRedWnd::InitObserverWnd( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

void ioObserverRedWnd::ClearInfo()
{
	m_RoundInfoList.erase( m_RoundInfoList.begin(), m_RoundInfoList.end() );
}

void ioObserverRedWnd::SetInfo( RoundInfo &rkInfo )
{
	if( rkInfo.m_szName.IsEmpty() )
		return;

	m_RoundInfoList.push_back( rkInfo );
}

void ioObserverRedWnd::UpdateWndSize()
{
	int iCnt = m_RoundInfoList.size();

	int iSizeX = DEFAULT_WIDTH;
	int iSizeY = DEFAULT_HEIGHT * iCnt;

	SetSize( iSizeX, iSizeY );

	RECT  rc = { 0, 0, iSizeX, iSizeY };
	SetDragRect( rc );

	int iGap = GetYPos() - ((Setting::Height() - GetHeight() ) + 1);
	if( iGap > 0 )
	{
		int iNewYPos = GetYPos() - iGap;

		m_iDefaultX = GetXPos();
		m_iDefaultY = iNewYPos;

		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void ioObserverRedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueBack" )
	{
		SAFEDELETE( m_pBlueBack );
		m_pBlueBack = pImage;
	}
	else if( szType == "RedBack" )
	{
		SAFEDELETE( m_pRedBack );
		m_pRedBack = pImage;
	}
	else if( szType == "BlueGauge" )
	{
		SAFEDELETE( m_pBlueGauge );
		m_pBlueGauge = pImage;
	}
	else if( szType == "RedGauge" )
	{
		SAFEDELETE( m_pRedGauge );
		m_pRedGauge = pImage;
	}
	else if( szType == "RedDead" )
	{
		SAFEDELETE( m_pRedDead );
		m_pRedDead = pImage;
	}
	else if( szType == "AddGauge" )
	{
		SAFEDELETE( m_pAddGauge );
		m_pAddGauge = pImage;
	}
	else if( szType == "Target" )
	{
		SAFEDELETE( m_pTargetMark );
		m_pTargetMark = pImage;
	}
	else if( szType == "GuildBack" )
	{
		SAFEDELETE( m_pGuildBack );
		m_pGuildBack = pImage;
	}
	else if( szType == "RedKeyTextQ" )
	{
		SAFEDELETE( m_pRedKeyText[0] );
		m_pRedKeyText[0] = pImage;
	}
	else if( szType == "RedKeyTextW" )
	{
		SAFEDELETE( m_pRedKeyText[1] );
		m_pRedKeyText[1] = pImage;
	}
	else if( szType == "RedKeyTextE" )
	{
		SAFEDELETE( m_pRedKeyText[2] );
		m_pRedKeyText[2] = pImage;
	}
	else if( szType == "RedKeyTextR" )
	{
		SAFEDELETE( m_pRedKeyText[3] );
		m_pRedKeyText[3] = pImage;
	}
	else if( szType == "RedKeyTextT" )
	{
		SAFEDELETE( m_pRedKeyText[4] );
		m_pRedKeyText[4] = pImage;
	}
	else if( szType == "RedKeyTextY" )
	{
		SAFEDELETE( m_pRedKeyText[5] );
		m_pRedKeyText[5] = pImage;
	}
	else if( szType == "RedKeyTextU" )
	{
		SAFEDELETE( m_pRedKeyText[6] );
		m_pRedKeyText[6] = pImage;
	}
	else if( szType == "RedKeyTextI" )
	{
		SAFEDELETE( m_pRedKeyText[7] );
		m_pRedKeyText[7] = pImage;
	}
	else if( szType == "BlueKeyTextQ" )
	{
		SAFEDELETE( m_pBlueKeyText[0] );
		m_pBlueKeyText[0] = pImage;
	}
	else if( szType == "BlueKeyTextW" )
	{
		SAFEDELETE( m_pBlueKeyText[1] );
		m_pBlueKeyText[1] = pImage;
	}
	else if( szType == "BlueKeyTextE" )
	{
		SAFEDELETE( m_pBlueKeyText[2] );
		m_pBlueKeyText[2] = pImage;
	}
	else if( szType == "BlueKeyTextR" )
	{
		SAFEDELETE( m_pBlueKeyText[3] );
		m_pBlueKeyText[3] = pImage;
	}
	else if( szType == "BlueKeyTextT" )
	{
		SAFEDELETE( m_pBlueKeyText[4] );
		m_pBlueKeyText[4] = pImage;
	}
	else if( szType == "BlueKeyTextY" )
	{
		SAFEDELETE( m_pBlueKeyText[5] );
		m_pBlueKeyText[5] = pImage;
	}
	else if( szType == "BlueKeyTextU" )
	{
		SAFEDELETE( m_pBlueKeyText[6] );
		m_pBlueKeyText[6] = pImage;
	}
	else if( szType == "BlueKeyTextI" )
	{
		SAFEDELETE( m_pBlueKeyText[7] );
		m_pBlueKeyText[7] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioObserverRedWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwMarkChangeTime = xElement.GetIntAttribute_e( "ChangeTime" );
}

void ioObserverRedWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "ObserverRedWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "ObserverRedWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void ioObserverRedWnd::iwm_show()
{
	UpdateWndSize();

	m_bCheckMark = false;
	m_dwCheckTime = FRAMEGETTIME();
}

void ioObserverRedWnd::iwm_hide()
{
	m_RoundInfoList.erase( m_RoundInfoList.begin(), m_RoundInfoList.end() );
}

void ioObserverRedWnd::OnRender()
{
	ioMovingWnd::OnRender();

	if( m_RoundInfoList.empty() )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	for( int i=0; i < (int)m_RoundInfoList.size(); ++i )
	{
		const RoundInfo &rkInfo = m_RoundInfoList[i];
		int iNewYPos = iYPos + DEFAULT_HEIGHT * i;

		if( rkInfo.m_Team == TEAM_BLUE )
		{
			OnOtherRender( iXPos, iNewYPos, i, rkInfo );
		}
		else
		{
			OnRedRender( iXPos, iNewYPos, i, rkInfo );
		}


	}
}

void ioObserverRedWnd::OnRedRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo )
{
	if( !m_pRedBack || !m_pRedGauge || !m_pRedDead || !m_pAddGauge )
		return;

	enum
	{
		MARK_XPOS	= 5,
		MARK_YPOS	= 5,
		GAUGE_XPOS	= 30,
		GAUGE_YPOS	= 2,
		NAME_XPOS	= 86,
		NAME_YPOS	= 4,
		KEY_XPOS	= 146,
		KEY_YPOS	= 12,
	};

	//Back
	m_pRedBack->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	m_pRedBack->Render( iXPos, iYPos );

	//Gauge or Dead
	if( IsDieChar( rkInfo.m_szName ) )
	{
		m_pRedDead->Render( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS );
	}
	else
	{
		float fHPRate = GetCurHPRate( rkInfo.m_szName );
		int iSize = m_pRedGauge->GetWidth();
		int iStart = (float)iSize * ( FLOAT1 - fHPRate );

		m_pRedGauge->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pRedGauge->RenderWidthCut( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS, iStart, iSize );
	}

	//Key
	if( IsTarget( rkInfo.m_szName ) && m_pTargetMark )
	{
		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pTargetMark->Render( iXPos+146, iYPos+8 );

		if( m_bCheckMark )
			m_pTargetMark->Render( iXPos+146, iYPos+8, UI_RENDER_SCREEN );

		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}
	else
	{
		if( m_pRedKeyText[iSlot] )
		{
			if( IsDieChar( rkInfo.m_szName ) )
				m_pRedKeyText[iSlot]->Render( iXPos+KEY_XPOS, iYPos+KEY_YPOS, UI_RENDER_GRAY );
			else
				m_pRedKeyText[iSlot]->Render( iXPos+KEY_XPOS, iYPos+KEY_YPOS );
		}
	}

	// Guild Mark
	if( m_pGuildBack )
	{
		m_pGuildBack->SetSize( 23, 23 );
		m_pGuildBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGuildBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGuildBack->Render( iXPos + 4, iYPos + 4, UI_RENDER_MULTIPLY );
	}

	if( IsDieChar( rkInfo.m_szName ) )
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS, UI_RENDER_GRAY );
	else
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS );

	//Name
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( IsDieChar( rkInfo.m_szName ) )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.PrintTextWidthCut( iXPos+NAME_XPOS, iYPos+NAME_YPOS, FONT_SIZE_15, 111.0f, rkInfo.m_szName.c_str() );


	//Add
	m_pAddGauge->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	m_pAddGauge->Render( iXPos, iYPos, UI_RENDER_ADD );
}

void ioObserverRedWnd::OnOtherRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo )
{
	enum
	{
		MARK_XPOS	= 5,
		MARK_YPOS	= 5,
		GAUGE_XPOS	= 30,
		GAUGE_YPOS	= 2,
		NAME_XPOS	= 86,
		NAME_YPOS	= 4,
		KEY_XPOS	= 146,
		KEY_YPOS	= 12,
	};

	//Back
	m_pBlueBack->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	m_pBlueBack->Render( iXPos, iYPos );
	m_pBlueBack->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );

	//Gauge or Dead
	if( IsDieChar( rkInfo.m_szName ) )
	{
		m_pRedDead->Render( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS );
	}
	else
	{
		float fHPRate = GetCurHPRate( rkInfo.m_szName );
		int iSize = m_pRedGauge->GetWidth();
		int iEnd = (float)iSize * fHPRate;

		m_pBlueGauge->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pBlueGauge->RenderWidthCut( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS, 0, iEnd );
		m_pBlueGauge->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}

	//Key
	if( IsTarget( rkInfo.m_szName ) && m_pTargetMark )
	{
		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pTargetMark->Render( iXPos+146, iYPos+8 );

		if( m_bCheckMark )
			m_pTargetMark->Render( iXPos+146, iYPos+8, UI_RENDER_SCREEN );

		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}
	else
	{
		if( m_pBlueKeyText[iSlot] )
		{
			if( IsDieChar( rkInfo.m_szName ) )
				m_pBlueKeyText[iSlot]->Render( iXPos+KEY_XPOS, iYPos+KEY_YPOS, UI_RENDER_GRAY );
			else
				m_pBlueKeyText[iSlot]->Render( iXPos+KEY_XPOS, iYPos+KEY_YPOS );
		}
	}

	// Guild Mark
	if( m_pGuildBack )
	{
		m_pGuildBack->SetSize( 23, 23 );
		m_pGuildBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGuildBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGuildBack->Render( iXPos + 4, iYPos + 4, UI_RENDER_MULTIPLY );
	}

	if( IsDieChar( rkInfo.m_szName ) )
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS, UI_RENDER_GRAY );
	else
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS );

	//Name
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( IsDieChar( rkInfo.m_szName ) )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.PrintTextWidthCut( iXPos+NAME_XPOS, iYPos+NAME_YPOS, FONT_SIZE_15, 111.0f, rkInfo.m_szName.c_str() );

	//Add
	m_pAddGauge->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	m_pAddGauge->Render( iXPos, iYPos, UI_RENDER_ADD );
}

void ioObserverRedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	UpdateWndSize();
	UpdateDefaultPos();
	ProcessCameraClick();
	UpdateMark();

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioObserverRedWnd::ProcessCameraClick()
{
	if( !g_GUIMgr.IsCurrentOver( GetID() ) )
		return;

	if( !g_App.IsVisibleMouse() )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	int iInfoCount = m_RoundInfoList.size();
	for( int i=0; i < iInfoCount; i++ )
	{
		RoundInfo kInfo = m_RoundInfoList[i];

		RECT rcNameRect;
		SetRectEmpty( &rcNameRect );

		rcNameRect.left += iXPos;
		rcNameRect.top  += iYPos + i * DEFAULT_HEIGHT;
		rcNameRect.right  = rcNameRect.left + DEFAULT_WIDTH;
		rcNameRect.bottom = rcNameRect.top  + DEFAULT_HEIGHT;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( pMouse->IsLBtnUp() && m_pPlayStage )
			{
				m_pPlayStage->CheckObserverCameraTarget( kInfo.m_szName );
			}
			break;
		}
	}
}

bool ioObserverRedWnd::IsDieChar( const ioHashString &szName )
{
	if( !m_pPlayStage ) return true;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser ) return true;

	if( pUser->GetState() == CS_DIE ||
		pUser->GetState() == CS_VIEW ||
		pUser->GetState() == CS_READY ||
		pUser->GetState() == CS_LOADING ||
		pUser->IsPrisonerMode() )
		return true;

	return false;
}

bool ioObserverRedWnd::IsTarget( const ioHashString &szName )
{
	if( !m_pPlayStage ) return false;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser ) return false;

	if( m_pPlayStage->GetCameraTargetChar()->GetCharName() == szName )
		return true;

	return false;
}

void ioObserverRedWnd::UpdateMark()
{
	if( m_dwMarkChangeTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime - m_dwCheckTime > m_dwMarkChangeTime )
	{
		m_dwCheckTime = dwCurTime;
		m_bCheckMark = !m_bCheckMark;
	}
}

float ioObserverRedWnd::GetCurHPRate( const ioHashString &szName )
{
	if( !m_pPlayStage )
		return FLOAT1;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser )
		return FLOAT1;

	StatusValue rkHP = pUser->GetHP();
	if( rkHP.m_fMaxValue <= 0.0f )
		return 0.0f;

	float fRate = rkHP.m_fCurValue / rkHP.m_fMaxValue;

	return fRate;
}

void ioObserverRedWnd::UpdateDefaultPos()
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
			if( COMPARE( GetYPos(), 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = GetYPos();
		}
	}
}

void ioObserverRedWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	UpdateMainDefaultPos();

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	float fDefCenterX = (float)( m_iDefXPos + GetWidth() / 2 );
	float fDefCenterXRate = fDefCenterX / 800.0f;

	float fDefCenterY = (float)(pTimeScoreWnd->GetMainDefaultYPos() + pTimeScoreWnd->GetHeight() / 2) ;
	float fDefCenterYRate = fDefCenterY / 600.0f;

	m_iDefXPos = (int)(Setting::Width() * fDefCenterXRate) - GetWidth() / 2;
	m_iDefYPos = (int)(Setting::Height() * fDefCenterYRate) - (pTimeScoreWnd->GetHeight() / 2) + pTimeScoreWnd->GetHeight();

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void ioObserverRedWnd::UpdateMainDefaultPos()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	int iScoreWndXPos = pTimeScoreWnd->GetMainDefaultXPos();
	int iScoreWndYPos = pTimeScoreWnd->GetMainDefaultYPos();

	iScoreWndXPos += pTimeScoreWnd->GetWidth()/2;
	iScoreWndYPos += pTimeScoreWnd->GetHeight();

	int iCenterXPos = 800 / 2;
	int iGap = abs(iCenterXPos - iScoreWndXPos);

	int iNewXPos = iCenterXPos + iGap;
	iNewXPos -= DEFAULT_WIDTH / 2;

	if( COMPARE( iNewXPos, 0, (800 - GetWidth() ) + 1 ))
		m_iDefXPos = iNewXPos;

	if( COMPARE( iScoreWndYPos, 0, (600 - GetHeight() ) + 1 ))
		m_iDefYPos = iScoreWndYPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioObserverBlueWnd::m_bPosToDefualt = false;

ioObserverBlueWnd::ioObserverBlueWnd()
{
	m_pBlueBack = NULL;
	m_pRedBack = NULL;
	m_pBlueGauge = NULL;
	m_pRedGauge = NULL;

	m_pBlueDead = NULL;
	m_pAddGauge = NULL;

	m_pBlueNumKey = NULL;
	m_pRedNumKey = NULL;

	m_pTargetMark = NULL;
	m_pGuildBack = NULL;

	m_pPlayStage = NULL;

	m_bCheckMark = false;
	m_dwCheckTime = 0;

	m_iDefaultX = 0;
	m_iDefaultY = 0;
}

ioObserverBlueWnd::~ioObserverBlueWnd()
{
	m_RoundInfoList.clear();

	SAFEDELETE( m_pBlueBack );
	SAFEDELETE( m_pRedBack );
	SAFEDELETE( m_pBlueGauge );
	SAFEDELETE( m_pRedGauge );

	SAFEDELETE( m_pBlueDead );
	SAFEDELETE( m_pAddGauge );

	SAFEDELETE( m_pBlueNumKey );
	SAFEDELETE( m_pRedNumKey );

	SAFEDELETE( m_pTargetMark );
	SAFEDELETE( m_pGuildBack );

	m_pPlayStage = NULL;

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "ObserverBlueWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "ObserverBlueWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void ioObserverBlueWnd::InitObserverWnd( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

void ioObserverBlueWnd::ClearInfo()
{
	m_RoundInfoList.erase( m_RoundInfoList.begin(), m_RoundInfoList.end() );
}

void ioObserverBlueWnd::SetInfo( RoundInfo &rkInfo )
{
	if( rkInfo.m_szName.IsEmpty() ) 
		return;

	m_RoundInfoList.push_back( rkInfo );
}

void ioObserverBlueWnd::UpdateWndSize()
{
	int iCnt = m_RoundInfoList.size();

	int iSizeX = DEFAULT_WIDTH;
	int iSizeY = DEFAULT_HEIGHT * iCnt;

	SetSize( iSizeX, iSizeY );

	RECT  rc = { 0, 0, iSizeX, iSizeY };
	SetDragRect( rc );

	int iGap = GetYPos() - ((Setting::Height() - GetHeight() ) + 1);
	if( iGap > 0 )
	{
		int iNewYPos = GetYPos() - iGap;

		m_iDefaultX = GetXPos();
		m_iDefaultY = iNewYPos;

		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void ioObserverBlueWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueBack" )
	{
		SAFEDELETE( m_pBlueBack );
		m_pBlueBack = pImage;
	}
	else if( szType == "RedBack" )
	{
		SAFEDELETE( m_pRedBack );
		m_pRedBack = pImage;
	}
	else if( szType == "BlueGauge" )
	{
		SAFEDELETE( m_pBlueGauge );
		m_pBlueGauge = pImage;
	}
	else if( szType == "RedGauge" )
	{
		SAFEDELETE( m_pRedGauge );
		m_pRedGauge = pImage;
	}
	else if( szType == "BlueDead" )
	{
		SAFEDELETE( m_pBlueDead );
		m_pBlueDead = pImage;
	}
	else if( szType == "AddGauge" )
	{
		SAFEDELETE( m_pAddGauge );
		m_pAddGauge = pImage;
	}
	else if( szType == "BlueNumKey" )
	{
		SAFEDELETE( m_pBlueNumKey );
		m_pBlueNumKey = pImage;
	}
	else if( szType == "RedNumKey" )
	{
		SAFEDELETE( m_pRedNumKey );
		m_pRedNumKey = pImage;
	}
	else if( szType == "Target" )
	{
		SAFEDELETE( m_pTargetMark );
		m_pTargetMark = pImage;
	}
	else if( szType == "GuildBack" )
	{
		SAFEDELETE( m_pGuildBack );
		m_pGuildBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioObserverBlueWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwMarkChangeTime = xElement.GetIntAttribute_e( "ChangeTime" );
}

void ioObserverBlueWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "ObserverBlueWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "ObserverBlueWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void ioObserverBlueWnd::iwm_show()
{
	UpdateWndSize();

	m_bCheckMark = false;
	m_dwCheckTime = FRAMEGETTIME();
}

void ioObserverBlueWnd::iwm_hide()
{
	m_RoundInfoList.erase( m_RoundInfoList.begin(), m_RoundInfoList.end() );
}

void ioObserverBlueWnd::OnRender()
{
	ioMovingWnd::OnRender();

	if( m_RoundInfoList.empty() )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	for( int i=0; i < (int)m_RoundInfoList.size(); ++i )
	{
		const RoundInfo &rkInfo = m_RoundInfoList[i];
		int iNewYPos = iYPos + DEFAULT_HEIGHT * i;

		if( rkInfo.m_Team == TEAM_BLUE )
		{
			OnBlueRender( iXPos, iNewYPos, i, rkInfo );
		}
		else
		{
			OnOtherRender( iXPos, iNewYPos, i, rkInfo );
		}
	}
}

void ioObserverBlueWnd::OnBlueRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo )
{
	if( !m_pBlueBack || !m_pBlueGauge || !m_pBlueDead || !m_pAddGauge || !m_pBlueNumKey )
		return;

	enum
	{
		MARK_XPOS	= 134,
		MARK_YPOS	= 5,
		GAUGE_XPOS	= 17,
		GAUGE_YPOS	= 2,
		NAME_XPOS	= 73,
		NAME_YPOS	= 4,
		KEY_XPOS	= 5,
		KEY_YPOS	= 12,
	};

	//Back
	m_pBlueBack->Render( iXPos, iYPos );

	//Gauge or Dead
	if( IsDieChar( rkInfo.m_szName ) )
	{
		m_pBlueDead->Render( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS );
	}
	else
	{
		float fHPRate = GetCurHPRate( rkInfo.m_szName );
		int iSize = m_pRedGauge->GetWidth();
		int iEnd = (float)iSize * fHPRate;

		m_pBlueGauge->RenderWidthCut( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS, 0, iEnd );
	}

	//Key
	if( IsTarget( rkInfo.m_szName ) && m_pTargetMark )
	{
		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pTargetMark->Render( iXPos+4, iYPos+8 );

		if( m_bCheckMark )
			m_pTargetMark->Render( iXPos+4, iYPos+8, UI_RENDER_SCREEN );
	}
	else
	{
		int iNum = iSlot+1;

		if( IsDieChar( rkInfo.m_szName ) )
			m_pBlueNumKey->RenderNum( iXPos+KEY_XPOS, iYPos+KEY_YPOS, iNum, 0.0f, FLOAT1, UI_RENDER_GRAY );
		else
			m_pBlueNumKey->RenderNum( iXPos+KEY_XPOS, iYPos+KEY_YPOS, iNum );
	}

	// Guild Mark
	if( m_pGuildBack )
	{
		m_pGuildBack->SetSize( 23, 23 );
		m_pGuildBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGuildBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGuildBack->Render( iXPos + 133, iYPos + 4, UI_RENDER_MULTIPLY );
	}

	if( IsDieChar( rkInfo.m_szName ) )
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS, UI_RENDER_GRAY );
	else
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS );


	//Name
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( IsDieChar( rkInfo.m_szName ) )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.PrintTextWidthCut( iXPos+NAME_XPOS, iYPos+NAME_YPOS, FONT_SIZE_15, 111.0f, rkInfo.m_szName.c_str() );

	//Add
	m_pAddGauge->Render( iXPos, iYPos, UI_RENDER_ADD );

	//Contribute
	/*
	//g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	//g_FontMgr.PrintText( iXPos+CONT_XPOS, iYPos+2, FONT_SIZE_13, STR(1), rkInfo.m_iContributePer );
	*/
}

void ioObserverBlueWnd::OnOtherRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo )
{
	if( !m_pRedBack || !m_pRedGauge || !m_pBlueDead || !m_pAddGauge || !m_pRedNumKey )
		return;

	enum
	{
		MARK_XPOS	= 134,
		MARK_YPOS	= 5,
		GAUGE_XPOS	= 17,
		GAUGE_YPOS	= 2,
		NAME_XPOS	= 73,
		NAME_YPOS	= 4,
		KEY_XPOS	= 5,
		KEY_YPOS	= 12,
	};

	//Back
	m_pRedBack->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	m_pRedBack->Render( iXPos, iYPos );
	m_pRedBack->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );

	//Gauge or Dead
	if( IsDieChar( rkInfo.m_szName ) )
	{
		m_pBlueDead->Render( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS );
	}
	else
	{
		float fHPRate = GetCurHPRate( rkInfo.m_szName );
		int iSize = m_pRedGauge->GetWidth();
		int iEnd = (float)iSize * fHPRate;

		m_pRedGauge->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pRedGauge->RenderWidthCut( iXPos+GAUGE_XPOS, iYPos+GAUGE_YPOS, 0, iEnd );
		m_pRedGauge->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}

	//Key
	if( IsTarget( rkInfo.m_szName ) && m_pTargetMark )
	{
		m_pTargetMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pTargetMark->Render( iXPos+4, iYPos+8 );

		if( m_bCheckMark )
			m_pTargetMark->Render( iXPos+4, iYPos+8, UI_RENDER_SCREEN );
	}
	else
	{
		int iNum = iSlot+1;

		if( IsDieChar( rkInfo.m_szName ) )
			m_pRedNumKey->RenderNum( iXPos+KEY_XPOS, iYPos+KEY_YPOS, iNum, 0.0f, FLOAT1, UI_RENDER_GRAY );
		else
			m_pRedNumKey->RenderNum( iXPos+KEY_XPOS, iYPos+KEY_YPOS, iNum );
	}

	// Guild Mark
	if( m_pGuildBack )
	{
		m_pGuildBack->SetSize( 23, 23 );
		m_pGuildBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGuildBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGuildBack->Render( iXPos + 133, iYPos + 4, UI_RENDER_MULTIPLY );
	}

	if( IsDieChar( rkInfo.m_szName ) )
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS, UI_RENDER_GRAY );
	else
		g_GuildMarkMgr.RenderMarkSize( 21, 21, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark, iXPos+MARK_XPOS, iYPos+MARK_YPOS );


	//Name
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( IsDieChar( rkInfo.m_szName ) )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );


	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.PrintTextWidthCut( iXPos+NAME_XPOS, iYPos+NAME_YPOS, FONT_SIZE_15, 111.0f, rkInfo.m_szName.c_str() );

	//Add
	m_pAddGauge->Render( iXPos, iYPos, UI_RENDER_ADD );

	//Contribute
	/*
	//g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	//g_FontMgr.PrintText( iXPos+129, iYPos+2, FONT_SIZE_13, STR(1), rkInfo.m_iContributePer );
	*/
}

void ioObserverBlueWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	UpdateWndSize();
	UpdateDefaultPos();
	ProcessCameraClick();
	UpdateMark();

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioObserverBlueWnd::ProcessCameraClick()
{
	if( !g_GUIMgr.IsCurrentOver( GetID() ) )
		return;

	if( !g_App.IsVisibleMouse() )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	int iInfoCount = m_RoundInfoList.size();
	for( int i=0; i < iInfoCount; i++ )
	{
		RoundInfo kInfo = m_RoundInfoList[i];

		RECT rcNameRect;
		SetRectEmpty( &rcNameRect );

		rcNameRect.left += iXPos;
		rcNameRect.top  += iYPos + i * DEFAULT_HEIGHT;
		rcNameRect.right  = rcNameRect.left + DEFAULT_WIDTH;
		rcNameRect.bottom = rcNameRect.top  + DEFAULT_HEIGHT;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( pMouse->IsLBtnUp() && m_pPlayStage )
			{
				m_pPlayStage->CheckObserverCameraTarget( kInfo.m_szName );
			}
			break;
		}
	}
}

bool ioObserverBlueWnd::IsDieChar( const ioHashString &szName )
{
	if( !m_pPlayStage ) return true;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser ) return true;

	if( pUser->GetState() == CS_DIE ||
		pUser->GetState() == CS_VIEW ||
		pUser->GetState() == CS_READY ||
		pUser->GetState() == CS_LOADING ||
		pUser->IsPrisonerMode() )
		return true;

	return false;
}

bool ioObserverBlueWnd::IsTarget( const ioHashString &szName )
{
	if( !m_pPlayStage ) return false;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser ) return false;

	if( m_pPlayStage->GetCameraTargetChar()->GetCharName() == szName )
		return true;

	return false;
}

void ioObserverBlueWnd::UpdateMark()
{
	if( m_dwMarkChangeTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime - m_dwCheckTime > m_dwMarkChangeTime )
	{
		m_dwCheckTime = dwCurTime;
		m_bCheckMark = !m_bCheckMark;
	}
}

float ioObserverBlueWnd::GetCurHPRate( const ioHashString &szName )
{
	if( !m_pPlayStage )
		return FLOAT1;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser )
		return FLOAT1;

	StatusValue rkHP = pUser->GetHP();
	if( rkHP.m_fMaxValue <= 0.0f )
		return 0.0f;

	float fRate = rkHP.m_fCurValue / rkHP.m_fMaxValue;

	return fRate;
}

void ioObserverBlueWnd::UpdateDefaultPos()
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
			if( COMPARE( GetYPos(), 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = GetYPos();
		}
	}
}

void ioObserverBlueWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	UpdateMainDefaultPos();

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	float fDefCenterX = (float)( m_iDefXPos + GetWidth() / 2 );
	float fDefCenterXRate = fDefCenterX / 800.0f;

	float fDefCenterY = (float)(pTimeScoreWnd->GetMainDefaultYPos() + pTimeScoreWnd->GetHeight() / 2);
	float fDefCenterYRate = fDefCenterY / 600.0f;

	m_iDefXPos = (int)(Setting::Width() * fDefCenterXRate) - GetWidth() / 2;
	m_iDefYPos = (int)(Setting::Height() * fDefCenterYRate) - (pTimeScoreWnd->GetHeight() / 2) + pTimeScoreWnd->GetHeight();

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void ioObserverBlueWnd::UpdateMainDefaultPos()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	int iScoreWndXPos = pTimeScoreWnd->GetMainDefaultXPos();
	int iScoreWndYPos = pTimeScoreWnd->GetMainDefaultYPos();

	iScoreWndXPos += pTimeScoreWnd->GetWidth()/2;
	iScoreWndYPos += pTimeScoreWnd->GetHeight();

	iScoreWndXPos -= DEFAULT_WIDTH / 2;

	if( COMPARE( iScoreWndXPos, 0, (800 - GetWidth() ) + 1 ))
		m_iDefXPos = iScoreWndXPos;

	if( COMPARE( iScoreWndYPos, 0, (600 - GetHeight() ) + 1 ))
		m_iDefYPos = iScoreWndYPos;
}
/////////////////////////////////////////////////////////////////////////////////////////////
ioObserverInfoWnd::ioObserverInfoWnd()
{
	m_pBack = NULL;

	m_dwShowStartTime = 0;
	m_bSpecialObserver = false;
}

ioObserverInfoWnd::~ioObserverInfoWnd()
{
	SAFEDELETE( m_pBack );
}

void ioObserverInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
		m_pBack->SetScale( 9.f, 1.5f );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioObserverInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iShowTime = xElement.GetIntAttribute_e( "ShowTime" );
	m_iBlinkTime = xElement.GetIntAttribute_e( "BlinkTime" );
	m_iBlinkCount = xElement.GetIntAttribute_e( "BlinkCount" );

	ioHashString szText1 = xElement.GetStringAttribute_e( "Desc1" );
	ioHashString szText2 = xElement.GetStringAttribute_e( "Desc2" );
	ioHashString szText3 = xElement.GetStringAttribute_e( "Desc3" );
	ioHashString szText4 = xElement.GetStringAttribute_e( "Desc4" );
	m_szDesc[0].Set(0, 0, szText1.c_str(), FONT_SIZE_18, -1, TS_OUTLINE_FULL_2X, 0xFFFDC103, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_TOP, 4);
	m_szDesc[1].Set(0, 0, szText2.c_str(), FONT_SIZE_16, -1, TS_OUTLINE_FULL_2X, 0xFF0199FE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_TOP, 4);
	m_szDesc[2].Set(0, 0, szText3.c_str(), FONT_SIZE_18, -1, TS_OUTLINE_FULL_2X, 0xFFFDC103, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_TOP, 4);
	m_szDesc[3].Set(0, 0, szText4.c_str(), FONT_SIZE_16, -1, TS_OUTLINE_FULL_2X, 0xFF0199FE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_TOP, 4);
}

void ioObserverInfoWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_dwShowStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bShow = ( dwCurTime < m_dwShowStartTime + m_iShowTime );
	for( int i = 0; i < m_iBlinkCount; i++ )
	{
		if( bShow )
			break;

		if( dwCurTime > m_dwShowStartTime + m_iShowTime + m_iBlinkTime * (i * 2 + 1) && dwCurTime < m_dwShowStartTime + m_iShowTime + m_iBlinkTime * (i * 2 + 2) )
			bShow = true;
	}
	if( bShow )
	{
		int iXPos, iYPos;
		iXPos = GetDerivedPosX();
		iYPos = GetDerivedPosY();

		if( m_pBack )
			m_pBack->Render( iXPos, iYPos );

		int iIndex = 0;
		if( !m_bSpecialObserver )
			iIndex = 2;

		m_szDesc[iIndex].OnRender( iXPos, iYPos - 20 );
		m_szDesc[iIndex + 1].OnRender( iXPos, iYPos + 4 );
	}
	else if( dwCurTime > m_dwShowStartTime + m_iShowTime + m_iBlinkTime * 2 * m_iBlinkCount )
		m_dwShowStartTime = 0;
}

void ioObserverInfoWnd::SetShow( bool bSpecial )
{
	m_bSpecialObserver = bSpecial;
	m_dwShowStartTime = FRAMEGETTIME();
}