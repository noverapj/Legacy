#include "StdAfx.h"

#include "../ioDateHelp.h"

#include "ChatMsgWnd.h"
#include "ItemCustomWnd.h"
#include "GlobalWnd.h"
#include "MessengerWnd.h"
#include "../GameStage/Mode/Practice/ioPracticeMode.h"

InviteProgressWnd::InviteProgressWnd() 
{
	m_pPhoneIcon = NULL;
	m_pBackType1 = NULL;
	m_pBackType2 = NULL;
	m_pGauge	 = NULL;

	m_pOpenWnd     = NULL;
	m_iInviteCount = 0;
	m_dwStartTimer = 0;
	m_bComplete    = false;
}

InviteProgressWnd::~InviteProgressWnd()
{
	SAFEDELETE( m_pPhoneIcon );
	SAFEDELETE( m_pBackType1 );
	SAFEDELETE( m_pBackType2 );
	SAFEDELETE( m_pGauge );
}

void InviteProgressWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "PhoneIcon" )
	{
		SAFEDELETE( m_pPhoneIcon );
		m_pPhoneIcon = pImage;
	}
	else if( szType == "BackType1" )
	{
		SAFEDELETE( m_pBackType1 );
		m_pBackType1 = pImage;
	}
	else if( szType == "BackType2" )
	{
		SAFEDELETE( m_pBackType2 );
		m_pBackType2 = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void InviteProgressWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void InviteProgressWnd::StartInivteWait( ioWnd *pOpenWnd, int iValue, const ioHashString &szName )
{
	m_pOpenWnd     = pOpenWnd;
	m_iInviteCount = iValue;
	m_szName       = szName;
	m_bComplete    = false;
	m_dwStartTimer = FRAMEGETTIME();

	ShowWnd();
}

void InviteProgressWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
   
	ioMovingWnd::OnProcess( fTimePerSec );

	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTimer;
	if( dwGapTime >= INVITE_WAIT_SEC )
	{
		HideWnd();
		if( m_pOpenWnd )
		{
			m_pOpenWnd->ShowWnd();
		}
	}
	else if( !m_bComplete )
	{
		DWORD dwRemainTime = INVITE_WAIT_SEC - dwGapTime;
		if( dwRemainTime <= 500 )
			m_bComplete = true;
	}
}

void InviteProgressWnd::OnRender()
{
	ioWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	if( m_szName.IsEmpty() )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, STR(1), m_iInviteCount );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		if( m_bComplete )
			g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(2) );
		else
			g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(3) );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, m_szName.c_str() );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		if( m_bComplete )
			g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(4) );
		else
			g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(5) );
	}

	if( m_bComplete && m_pBackType2 )
		m_pBackType2->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );
	else if( m_pBackType1 )
		m_pBackType1->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pPhoneIcon )
		m_pPhoneIcon->Render( iXPos + 103, iYPos + 98 );

	if( m_pGauge )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTimer;
		float fGapRate = (float)dwGapTime / (float)INVITE_WAIT_SEC;
		m_pGauge->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGauge->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGauge->RenderWidthCut( iXPos + 19, iYPos + 212, 0, (float)m_pGauge->GetWidth() * fGapRate );
	}
}
//////////////////////////////////////////////////////////////////////////
JoinRequestProgressWnd::JoinRequestProgressWnd() 
{
	m_pIcon			= NULL;
	m_pIconBack1	= NULL;
	m_pIconBack2	= NULL;
	m_pGauge		= NULL;

	m_dwStartTimer	= 0;
	m_bComplete		= false;

	m_eModeType		= MT_HEADQUARTERS;
}

JoinRequestProgressWnd::~JoinRequestProgressWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack1 );
	SAFEDELETE( m_pIconBack2 );
	SAFEDELETE( m_pGauge );
}

void JoinRequestProgressWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack1" )
	{
		SAFEDELETE( m_pIconBack1 );
		m_pIconBack1 = pImage;
	}
	else if( szType == "IconBack2" )
	{
		SAFEDELETE( m_pIconBack2 );
		m_pIconBack2 = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void JoinRequestProgressWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void JoinRequestProgressWnd::iwm_show()
{
	m_bComplete    = false;
	m_dwStartTimer = FRAMEGETTIME();
	g_SoundMgr.PlaySound( "interface/team_invite.wav" );

	SetChildInActive( ID_EXIT );
}

void JoinRequestProgressWnd::SetModeType( int eModeType )
{
	m_eModeType = eModeType;
}

void JoinRequestProgressWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTimer;
	if( dwGapTime >= INVITE_WAIT_SEC )
	{
		HideWnd();
	}
	else if( !m_bComplete )
	{
		DWORD dwRemainTime = INVITE_WAIT_SEC - dwGapTime;
		if( dwRemainTime <= 1500 )
			m_bComplete = true;
	}
}

void JoinRequestProgressWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	switch( m_eModeType )
	{
	case MT_HOUSE:
		g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, "개인본부" );
		break;
	case MT_HEADQUARTERS:
		g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, "훈련소" );
		break;
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_bComplete )
		g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(2) );
	else
		g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(3) );

	if( m_bComplete && m_pIconBack2 )
		m_pIconBack2->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );
	else if( m_pIconBack1 )
		m_pIconBack1->Render( iXPos + 103, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 103, iYPos + 98 );

	if( m_pGauge )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTimer;
		float fGapRate = (float)dwGapTime / (float)INVITE_WAIT_SEC;
		m_pGauge->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGauge->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGauge->RenderWidthCut( iXPos + 19, iYPos + 212, 0, (float)m_pGauge->GetWidth() * fGapRate );
	}
}
//////////////////////////////////////////////////////////////////////////
UppderAlphaWnd::UppderAlphaWnd()
{
	m_dwAlphaTime = m_dwShowTimer = 0;
	m_bSetSizeFromParent = true;
}

UppderAlphaWnd::~UppderAlphaWnd()
{
}

void UppderAlphaWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwAlphaTime = xElement.GetIntAttribute_e( "AlphaTime" );
	m_bSetSizeFromParent = xElement.GetBoolAttribute_e( "SizeFromParent" );
}

void UppderAlphaWnd::iwm_show()
{
	m_dwShowTimer = FRAMEGETTIME();

	if( m_bSetSizeFromParent )
	{
		if( GetParent() )
		{
			SetSize( GetParent()->GetWidth(), GetParent()->GetHeight() );
		}
	}

	if( m_pSelectedFrm )
	{
		m_pSelectedFrm->SetAlpha( MAX_ALPHA_RATE );
	}
}

void UppderAlphaWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_dwShowTimer > 0 )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwShowTimer;
		if( dwGapTime > m_dwAlphaTime + 50 )
		{
			HideWnd();
		}
		else if( dwGapTime > 50 )
		{
			dwGapTime -= 50;
			float fRate = max( 0.0f, cosf( ( D3DX_PI / 2 ) * (float)dwGapTime / m_dwAlphaTime ) );
			if( m_pSelectedFrm )
			{
				int iAlphaRate = (float)MAX_ALPHA_RATE * fRate;
				m_pSelectedFrm->SetAlpha( (BYTE)iAlphaRate );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
GradeUPWnd::GradeUPWnd()
{
	m_pGradeBack = NULL;
	m_pMajorMark = NULL;
	m_pMinorGrade= NULL;
}

GradeUPWnd::~GradeUPWnd()
{
	SAFEDELETE( m_pGradeBack );
	SAFEDELETE( m_pMajorMark );
	SAFEDELETE( m_pMinorGrade );
}

void GradeUPWnd::iwm_show()
{
	ShowChildWnd( ID_ALPHA_WND );

	SAFEDELETE( m_pMajorMark );
	m_pMajorMark = g_LevelMgr.GetGradeBigIcon( g_MyInfo.GetGradeLevel() );
}

void GradeUPWnd::iwm_hide()
{
	HideChildWnd( ID_ALPHA_WND );
}

void GradeUPWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

void GradeUPWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GradeBack" )
	{
		SAFEDELETE( m_pGradeBack );
		m_pGradeBack = pImage;
	}
	else if( szType == "MinorGrade" )
	{
		SAFEDELETE( m_pMinorGrade );
		m_pMinorGrade = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void GradeUPWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void GradeUPWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pGradeBack )
		m_pGradeBack->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pMajorMark )
		m_pMajorMark->Render( iXPos + 125, iYPos + 98 );

	// 하사 ~ 대령
	int iGradeLevel = g_MyInfo.GetGradeLevel();;
	if( m_pMinorGrade && COMPARE( iGradeLevel, 5, 49 ) )
	{
		int iMinorGrade = iGradeLevel % 5;
		switch( iMinorGrade )
		{
		case 0:
			m_pMinorGrade->Render( iXPos + 121, iYPos + 125 );
			break;
		case 1:
			m_pMinorGrade->Render( iXPos + 116, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 126, iYPos + 125 );
			break;
		case 2:
			m_pMinorGrade->Render( iXPos + 111, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 121, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 131, iYPos + 125 );
			break;
		case 3:
			m_pMinorGrade->Render( iXPos + 106, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 116, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 126, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 136, iYPos + 125 );
			break;
		case 4:
			m_pMinorGrade->Render( iXPos + 101, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 111, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 121, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 131, iYPos + 125 );
			m_pMinorGrade->Render( iXPos + 141, iYPos + 125 );
			break;
		}
	}

	char szGradeName[MAX_PATH] = "";
	g_LevelMgr.GetGradeName( iGradeLevel, szGradeName, sizeof( szGradeName ), true, true );
	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 125, iYPos + 146, FONT_SIZE_17, STR(1), szGradeName );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( g_MyInfo.GetUserRanking() == 0 )
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, STR(2) );
	else
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, STR(3), g_MyInfo.GetUserRanking() );
}
//////////////////////////////////////////////////////////////////////////
SoldierLevelUPWnd::SoldierLevelUPWnd()
{
	m_pSoldierIcon = NULL;
	m_pSoldierSubIcon = NULL;

	m_iClassType = m_iPesoBonus = 0;
}

SoldierLevelUPWnd::~SoldierLevelUPWnd()
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
}

void SoldierLevelUPWnd::SetLevelUPData( int iClassType, int iPesoBonus )
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );

	m_iClassType = iClassType;
	m_iPesoBonus = iPesoBonus;

	int iClassLevel = g_MyInfo.GetClassLevel( m_iClassType, true );
	if( iClassLevel < 10 )
	{
		m_pSoldierIcon = g_MyInfo.GetSoldierIcon( m_iClassType, true );
		m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );
	}
	else if( m_iClassType < 190 || CheckHaveResource(m_iClassType)  )
	{
		char szIconName[MAX_PATH] = "";
		sprintf( szIconName, "Quest_Soldier_Level_%.2d#%.3d_%.2d", 1 + ( ( m_iClassType - 1 ) / 3 ), m_iClassType, iClassLevel / 10 ); //Except Extracting Hangeul
		m_pSoldierIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}
	else
	{
		m_pSoldierIcon = g_MyInfo.GetSoldierIcon( m_iClassType, true );
		
		char szIconName[MAX_PATH] = "";
		sprintf( szIconName, "Quest_Soldier_Level_00#icon_lv%d", iClassLevel / 10 ); //Except Extracting Hangeul
		m_pSoldierSubIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}

	ShowWnd();
}

bool SoldierLevelUPWnd::CheckHaveResource( int iClassType )
{
	switch( iClassType )
	{
	case 505:	// R-사이버 메딕
	case 507:	// R-로빈후드
	case 509:	// R-아이스 메이지
	case 514:	// R-청룡관우
	case 807:	// 소원
	case 885:	// 유주
	case 902:	// 신비
	case 920:	// 예린
	case 926:	// 엄지
	case 938:	// 은하
		return true;
	}
	return false;
}

void SoldierLevelUPWnd::iwm_show()
{
	ShowChildWnd( ID_ALPHA_WND );
}

void SoldierLevelUPWnd::iwm_hide()
{
	HideChildWnd( ID_ALPHA_WND );
}

void SoldierLevelUPWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

void SoldierLevelUPWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void SoldierLevelUPWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pSoldierIcon )
		m_pSoldierIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pSoldierSubIcon )
		m_pSoldierSubIcon->Render( iXPos + 125, iYPos + 98 );

	ioComplexStringPrinter kSoldierPrinter;
	kSoldierPrinter.SetTextStyle( TS_NORMAL );
	kSoldierPrinter.SetBkColor( 0, 0, 0 );
	kSoldierPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kSoldierPrinter.AddTextPiece( FONT_SIZE_17, STR(1), g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) ) );
	kSoldierPrinter.PrintFullText( iXPos + 128, iYPos + 147, TAT_CENTER );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(2), g_MyInfo.GetClassLevel( m_iClassType, true ) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(3) );
	kPrinter.PrintFullText( iXPos + 126, iYPos + 168, TAT_CENTER );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	char szConvertText[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iPesoBonus, szConvertText, sizeof( szConvertText ) );
	g_FontMgr.PrintText( iXPos + 125, iYPos + 190, FONT_SIZE_13, STR(4), szConvertText );
}
//////////////////////////////////////////////////////////////////////////
GlobalChatWnd::GlobalChatWnd()
{
	m_pPlayStage = NULL;
	m_iDefaultX  = 0;
	m_iDefaultY  = 0;

	m_dwCurSelectTab = 0;

	m_pBackImage1 = NULL;
	m_pBackImage2 = NULL;

	m_pCurBackImage = NULL;

	m_pDotImage = NULL;

	SetWndName( "GLOBAL_CHAT_WND");
}

GlobalChatWnd::~GlobalChatWnd()
{
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "GlobalChatWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "GlobalChatWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );

	SAFEDELETE( m_pBackImage1 );
	SAFEDELETE( m_pBackImage2 );

	SAFEDELETE( m_pDotImage );
}

void GlobalChatWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetPlayStage( pPlayStage );
		pGameChatInput->SetChatTypeToBtn( CT_ALL );
	}
}

void GlobalChatWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos - fPosGap;

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

void GlobalChatWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "GlobalChatWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "GlobalChatWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void GlobalChatWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ChatBack1" )
	{
		SAFEDELETE( m_pBackImage1 );
		m_pBackImage1 = pImage;
	}
	else if( szType == "ChatBack2" )
	{
		SAFEDELETE( m_pBackImage2 );
		m_pBackImage2 = pImage;
	}
	else if( szType == "DotImage" )
	{
		SAFEDELETE( m_pDotImage );
		m_pDotImage = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GlobalChatWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szLoadInfo = xElement.GetStringAttribute_e( "LoadInfo" );	
}

void GlobalChatWnd::SetInitState()
{
	m_CurrentOption = Setting::GetSettingOption();

	CheckRoomTitle();

	if( CheckTabButton( ID_CHAT_ALL_TAB ) )
	{
		CheckCurFeature();
	}

	int iBackType = m_CurrentOption.m_iChatBackType;
	m_pCurBackImage = NULL;
	m_iAlphaRate = 0;

	switch( iBackType )
	{
	case 0:
		m_pCurBackImage = NULL;
		break;
	case 1:
		{
			float fRate = 255 * 0.4f;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage1;
		}
		break;
	case 2:
		{
			float fRate = 255 * FLOAT08;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage1;
		}
		break;
	case 3:
		{
			float fRate = 255 * 0.4f;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage2;
		}
		break;
	case 4:
		{
			float fRate = 255 * FLOAT08;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage2;
		}
		break;
	}
}

void GlobalChatWnd::iwm_show()
{
	if( g_GUIMgr.IsShow( TOTAL_RESULT_MAIN_WND ) ||
		g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) ||
		g_GUIMgr.IsShow( LADDER_BATTLE_WND ) )
	{
		HideWnd();
	}
	else
	{
		g_ChatMgr.LoadChatInfo( m_szLoadInfo );

		ShowChildWnd( ID_CHAT_MSG_WND );
		HideChildWnd( ID_CHAT_INPUT_WND );

		GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
		if( pGameChatInput )
		{
			pGameChatInput->SetFirstChatTypeToBtn( CT_ALL );		
		}
		SetCheckParentOutSide( true );

		SetInitState();

		ShowChildWnd( ID_CHAT_ALL_TAB );
		ShowChildWnd( ID_CHAT_ROOM_TAB );
		ShowChildWnd( ID_CHAT_SYSTEM_TAB );
		ShowChildWnd( ID_CHAT_CUSTOM_TAB );
	}

	//ID_CHAT_MSG_WND
	ioWnd *pChatWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_CHAT_MSG_WND ) );
	if( pChatWnd )
		pChatWnd->SetFocusRectTypeIsParent( true );
}

void GlobalChatWnd::iwm_hide()
{
	HideChildWnd( ID_CHAT_MSG_WND );
	HideChildWnd( ID_CHAT_INPUT_WND );

	HideChildWnd( ID_CHAT_ALL_TAB );
	HideChildWnd( ID_CHAT_ROOM_TAB );
	HideChildWnd( ID_CHAT_SYSTEM_TAB );
	HideChildWnd( ID_CHAT_CUSTOM_TAB );

	SetCheckParentOutSide( false );

	m_dwCurSelectTab = 0;

	g_GUIMgr.HideWnd( CHAT_OPTION_WND );
}

bool GlobalChatWnd::iwm_esc()
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput && pGameChatInput->IsShow() )
	{
		pGameChatInput->HideWnd();
	}
	return false;
}

void GlobalChatWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ChatMsgWnd *pChatWnd = dynamic_cast<ChatMsgWnd*>( FindChildWnd( ID_CHAT_MSG_WND ) );
		if( pChatWnd && pChatWnd->IsShow() )
			pChatWnd->iwm_wheel( zDelta );
	}
}

void GlobalChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			pWnd->HideWnd();
		}
		break;
	case ID_CHAT_ALL_TAB:
	case ID_CHAT_ROOM_TAB:
	case ID_CHAT_SYSTEM_TAB:
	case ID_CHAT_CUSTOM_TAB:
		if( cmd == IOBN_BTNDOWN )
		{
			if( CheckTabButton( dwID ) )
				CheckCurFeature();
		}
		break;
	case ID_CUSTOM_OPTION:
		if( cmd == IOBN_BTNUP )
		{
			if( !g_GUIMgr.IsShow( CHAT_OPTION_WND ) )
				g_GUIMgr.ShowWnd( CHAT_OPTION_WND );
		}
		break;
	}
}

void GlobalChatWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	
	ioMovingWnd::OnProcess( fTimePerSec );

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
	
	if( !m_pPlayStage ) return;	
	if( ioWnd::m_pTopModalWnd ) return;
	
	GameChatInput *pChatInput = dynamic_cast<GameChatInput*>( FindChildWnd( ID_CHAT_INPUT_WND ) );
	if( !pChatInput ) return;

	if( !pChatInput->IsEnableEnter() )
		return;

	// scroll show hide
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	ioWnd *pChatMsg = FindChildWnd( ID_CHAT_MSG_WND );
	if( !pChatMsg ) return;

	RECT rcRect = { GetDerivedPosX(), GetDerivedPosY(), GetDerivedPosX() + 300, GetDerivedPosY() + 244 };
	if( g_App.IsMouseRender() && ::PtInRect( &rcRect, pMouse->GetMousePos() ) || pChatInput->IsShow() )
	{
		pChatMsg->ShowChildWnd( ChatMsgWnd::ID_VERT_SCROLL );

		ShowChildWnd( ID_CHAT_ALL_TAB );
		ShowChildWnd( ID_CHAT_ROOM_TAB );
		ShowChildWnd( ID_CHAT_SYSTEM_TAB );
		ShowChildWnd( ID_CHAT_CUSTOM_TAB );

		ShowChildWnd( ID_CUSTOM_OPTION );
	}
	else
	{
		pChatMsg->HideChildWnd( ChatMsgWnd::ID_VERT_SCROLL );

		HideChildWnd( ID_CHAT_ALL_TAB );
		HideChildWnd( ID_CHAT_ROOM_TAB );
		HideChildWnd( ID_CHAT_SYSTEM_TAB );
		HideChildWnd( ID_CHAT_CUSTOM_TAB );

		HideChildWnd( ID_CUSTOM_OPTION );
	}
		
	if( pChatInput->IsShow() ) return;
    
	MessengerWnd *pMessenger = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
	if( pMessenger && pMessenger->IsChatBlock() )
		return;

	bool bPressEnter = g_InputBox.IsEnter();
	bool bShowSettingKeyWnd = g_GUIMgr.IsShowSettingKeyWnd();
	if( bPressEnter && !bShowSettingKeyWnd && !g_GUIMgr.IsShow( SOLDIER_SELECT_WND ) && !g_GUIMgr.IsShow( FIRST_IDCHANGE_WND ) )
	{
		if( g_Input.IsKeyDown( KEY_LEFTSHIFT ) || g_Input.IsKeyDown( KEY_RIGHTSHIFT ) )
		{
			pChatInput->ShowChatConv( g_MyInfo.GetPublicID(), m_pPlayStage->GetOwnerTeam(), m_pPlayStage );
		}
		else
		{
			pChatInput->ShowChat( g_MyInfo.GetPublicID(), m_pPlayStage->GetOwnerTeam(), m_pPlayStage );
		}
	}	
}

void GlobalChatWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCurBackImage )
	{
		m_pCurBackImage->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCurBackImage->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCurBackImage->SetSize( 360, 194 );
		m_pCurBackImage->SetAlpha( m_iAlphaRate );
		m_pCurBackImage->Render( iXPos+14, iYPos+19, UI_RENDER_NORMAL );
	}

	ioMovingWnd::OnRender();

	ioWnd *pChildWnd =  FindChildWnd( ID_CHAT_ALL_TAB );
	if( pChildWnd && pChildWnd->IsShow() && m_pDotImage )
	{
		m_pDotImage->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pDotImage->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pDotImage->SetSize( 1, 2 );

		m_pDotImage->Render( iXPos+83, iYPos+16, UI_RENDER_NORMAL );
		m_pDotImage->Render( iXPos+153, iYPos+16, UI_RENDER_NORMAL );
		m_pDotImage->Render( iXPos+223, iYPos+16, UI_RENDER_NORMAL );
	}
}

void GlobalChatWnd::CheckRoomTitle()
{
	ioHashString szRoomText;

	switch( m_pPlayStage->GetModeType() )
	{
	case MT_MYROOM:
	case MT_HEADQUARTERS:
		{
			switch( ioMyRoomMode::GetMyRoomType() )
			{
			case MRT_PRACTICE:
				{
					szRoomText = STR(1);
				}
				break;
			}
		}
		break;
	case MT_TRAINING:
		{
			szRoomText = STR(2);
		}
		break;
	case MT_SYMBOL:
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_KING:
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_MONSTER_SURVIVAL:
	case MT_FOOTBALL:
	case MT_DUNGEON_A:
	case MT_BOSS:
	case MT_GANGSI:
	case MT_CATCH_RUNNINGMAN:
	case MT_DOUBLE_CROWN:
	case MT_SHUFFLE_BONUS:
	case MT_FACTORY:
	case MT_RAID:
	case MT_FLAG_CAPTURE:
	case MT_ARENA:
	case MT_BATTLE:
	case MT_FARMING:
		{
			switch( m_pPlayStage->GetRoomStyle() )
			{
			case RSTYLE_BATTLEROOM:
				szRoomText = STR(3);
				break;
			case RSTYLE_LADDERBATTLE:
				szRoomText = STR(4);
				break;
			case RSTYLE_SHUFFLEROOM:
				szRoomText = "전투";
				break;
			case RSTYLE_MATCHROOM:
				szRoomText = "전투";
				break;
			}
		}
		break;
	case MT_HEROMATCH:
		{
			szRoomText = STR(5);
		}
		break;
	case MT_HOUSE:
		{
			szRoomText = "개인본부";
		}
		break;
	}

	if( !szRoomText.IsEmpty() )
	{
		ioWnd *pReasonBtn = FindChildWnd( ID_CHAT_ROOM_TAB );
		if( pReasonBtn )
		{
			ioUITitle *pTitle = pReasonBtn->GetTitle();
			if( pTitle )
			{
				pTitle->SetText( szRoomText.c_str(), "" );
			}
		}
	}
}

bool GlobalChatWnd::CheckTabButton( DWORD dwID )
{
	if( m_dwCurSelectTab == dwID )
		return false;

	m_dwCurSelectTab = dwID;
	CheckRadioButton( ID_CHAT_ALL_TAB, ID_CHAT_CUSTOM_TAB, m_dwCurSelectTab );

	return true;
}

void GlobalChatWnd::CheckCurFeature()
{
	switch( m_dwCurSelectTab )
	{
	case ID_CHAT_ALL_TAB:
		g_ChatMgr.SetMsgWndFeature( 0 );
		break;
	case ID_CHAT_ROOM_TAB:
		g_ChatMgr.SetMsgWndFeature( MSF_NORMAL_CHAT | MSF_WHOLE_CHAT );
		break;
	case ID_CHAT_SYSTEM_TAB:
		g_ChatMgr.SetMsgWndFeature( MSF_SYSTEM_CHAT );
		break;
	case ID_CHAT_CUSTOM_TAB:
		{
			DWORD dwFeature = 0;

			if( m_CurrentOption.m_bNormalChat )
				dwFeature |= MSF_NORMAL_CHAT;

			if( m_CurrentOption.m_bWholeChat )
				dwFeature |= MSF_WHOLE_CHAT;

			if( m_CurrentOption.m_bSystemChat )
				dwFeature |= MSF_SYSTEM_CHAT;

			if( dwFeature == 0 )
				dwFeature = MSF_NO_CHAT;

			g_ChatMgr.SetMsgWndFeature( dwFeature );
		}
		break;
	}
}

void GlobalChatWnd::RefreshChatOption()
{
	m_CurrentOption = Setting::GetSettingOption();

	CheckCurFeature();

	int iBackType = m_CurrentOption.m_iChatBackType;
	m_pCurBackImage = NULL;
	m_iAlphaRate = 0;

	switch( iBackType )
	{
	case 0:
		m_pCurBackImage = NULL;
		break;
	case 1:
		{
			float fRate = 255 * 0.4f;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage1;
		}
		break;
	case 2:
		{
			float fRate = 255 * FLOAT08;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage1;
		}
		break;
	case 3:
		{
			float fRate = 255 * 0.4f;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage2;
		}
		break;
	case 4:
		{
			float fRate = 255 * FLOAT08;
			m_iAlphaRate = (BYTE)fRate;

			m_pCurBackImage = m_pBackImage2;
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ChatOptionWnd::ChatOptionWnd()
{
	m_pPlayStage = NULL;
	m_iDefaultX  = 0;
	m_iDefaultY  = 0;
}

ChatOptionWnd::~ChatOptionWnd()
{
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "ChatOptionWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "ChatOptionWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void ChatOptionWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void ChatOptionWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos - fPosGap;

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

void ChatOptionWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "ChatOptionWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "ChatOptionWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void ChatOptionWnd::iwm_show()
{
	if( g_GUIMgr.IsShow( TOTAL_RESULT_MAIN_WND ) ||
		g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) ||
		g_GUIMgr.IsShow( LADDER_BATTLE_WND ) )
	{

		HideWnd();
	}
	else
	{
		SetCheckParentOutSide( true );

		m_CurrentOption = Setting::GetSettingOption();
		UpdateByCurrentOption();
	}
}

void ChatOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void ChatOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ApplyOption();
			HideWnd();
		}
		break;
	case ID_EXIT_BTN:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_INIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_iChatBackType = 0;
			m_CurrentOption.m_bNormalChat = true;
			m_CurrentOption.m_bWholeChat = true;
			m_CurrentOption.m_bSystemChat = true;

			UpdateByCurrentOption();
		}
		break;
	case ID_RADIO_BTN1:
	case ID_RADIO_BTN2:
	case ID_RADIO_BTN3:
	case ID_RADIO_BTN4:
	case ID_RADIO_BTN5:
		if( cmd == IOBN_BTNDOWN )
		{
			CheckChatBack( dwID );
		}
		break;
	case ID_NORMAL_CHAT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bNormalChat = !m_CurrentOption.m_bNormalChat;
			UpdateByCurrentOption();
		}
		break;
	case ID_WHOLE_CHAT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bWholeChat = !m_CurrentOption.m_bWholeChat;
			UpdateByCurrentOption();
		}
		break;
	case ID_SYSTEM_CHAT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bSystemChat = !m_CurrentOption.m_bSystemChat;
			UpdateByCurrentOption();
		}
		break;
	}
}

void ChatOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );

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

	if( !m_pPlayStage ) return;	
	if( ioWnd::m_pTopModalWnd ) return;
}

void ChatOptionWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	g_FontMgr.PrintText( iXPos + 21, iYPos + 58, FONT_SIZE_13, STR(1) );
	g_FontMgr.PrintText( iXPos + 21, iYPos + 178, FONT_SIZE_13, STR(2) );
}

void ChatOptionWnd::CheckChatBack( DWORD dwID )
{
	CheckRadioButton( ID_RADIO_BTN1, ID_RADIO_BTN5, dwID );

	switch( dwID )
	{
	case ID_RADIO_BTN1:
		m_CurrentOption.m_iChatBackType = 0;
		break;
	case ID_RADIO_BTN2:
		m_CurrentOption.m_iChatBackType = 1;
		break;
	case ID_RADIO_BTN3:
		m_CurrentOption.m_iChatBackType = 2;
		break;
	case ID_RADIO_BTN4:
		m_CurrentOption.m_iChatBackType = 3;
		break;
	case ID_RADIO_BTN5:
		m_CurrentOption.m_iChatBackType = 4;
		break;
	}
}

void ChatOptionWnd::SetButtonCheck( DWORD dwID, bool bCheck )
{
	ioWnd *pWnd = FindChildWnd( dwID );
	if( pWnd && pWnd->GetWndType() == IWT_CHECK_BOX )
	{
		ioCheckButton *pCheck = dynamic_cast< ioCheckButton* >( pWnd );
		pCheck->SetCheck( bCheck );
	}	
}

void ChatOptionWnd::UpdateByCurrentOption()
{
	CheckRadioButton( ID_RADIO_BTN1, ID_RADIO_BTN5, m_CurrentOption.m_iChatBackType+ID_RADIO_BTN1 );

	SetButtonCheck( ID_NORMAL_CHAT, m_CurrentOption.m_bNormalChat );
	SetButtonCheck( ID_WHOLE_CHAT, m_CurrentOption.m_bWholeChat );
	SetButtonCheck( ID_SYSTEM_CHAT, m_CurrentOption.m_bSystemChat );
}

void ChatOptionWnd::ApplyOption()
{
	bool bExceptResolution = false;

	if( m_CurrentOption.IsOptionChanged( Setting::GetSettingOption() ) )
	{
		SettingOption kBackUpOption = Setting::GetSettingOption();

		if( !g_App.ApplyOptionChange( m_CurrentOption ) )
		{
			// 디바이스리셋이 실패했기때문에 리셋관련 옵션은 기존 옵션으로 되돌림

			if( !g_App.ApplyOptionChange( kBackUpOption ) )
			{
				// 되돌림조차 실패한다면...어쩔수 없을듯..
			}
		}

		m_CurrentOption = Setting::GetSettingOption();
		UpdateByCurrentOption();

#ifdef SHIPPING
		Setting::SaveSetting( g_MyInfo.GetPublicID(), bExceptResolution );
#else
		if( !P2PNetwork::IsNetworkPlaying() )
			Setting::SaveSetting( "character1", bExceptResolution );
		else
			Setting::SaveSetting( g_MyInfo.GetPublicID(), bExceptResolution );
#endif

		GlobalChatWnd *pChatWnd = dynamic_cast<GlobalChatWnd*>( g_GUIMgr.FindWnd( GLOBAL_CHAT_WND ) );
		if( pChatWnd )
		{
			pChatWnd->RefreshChatOption();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
GlobalTextTooltip::GlobalTextTooltip()
{
}

GlobalTextTooltip::~GlobalTextTooltip()
{
}

void GlobalTextTooltip::ClearList()
{
	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; ++i )
	{
		m_Printer[i].ClearList();
	}
}

void GlobalTextTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter )
{
	ClearList();

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}

	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = 0;
	int iMaxHeight = MIN_HEIGHT;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( m_Printer[i].IsEmpty() )
			break;

		int iCurLineWidth = m_Printer[i].GetFullWidth() + SIDE_TEXT_GAP;
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMaxWidth, iMaxHeight );
	ProcessPosition();
}

void GlobalTextTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void GlobalTextTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessPosition();
}

void GlobalTextTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;;

	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP*i, TAT_LEFT );
	}
}
//////////////////////////////////////////////////////////////////////////
ModeHelpTooltip::ModeHelpTooltip()
{
	m_iQuickstartModeHelpIndex = -1;
	m_pCallWnd = NULL;
}

ModeHelpTooltip::~ModeHelpTooltip()
{
}

void ModeHelpTooltip::SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex )
{
	m_pCallWnd = pCallWnd;
	m_szModeTitle = szTitle;
	m_iQuickstartModeHelpIndex = iQuickstartModeHelpIndex;

	if( m_iQuickstartModeHelpIndex == -1 )
	{
		HideWnd();
		return;
	}
	int iMaxLine = g_ModeSelectInfo.GetQuickStartMaxHelpMent( m_iQuickstartModeHelpIndex ) - g_ModeSelectInfo.GetQuickStartMaxPassHelpMent( m_iQuickstartModeHelpIndex );
	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	QuickStartHelp &rkHelpData = g_ModeSelectInfo.GetQuickStartHelpData( m_iQuickstartModeHelpIndex );

	int iMaxWidth  = DEFAULT_WIDTH;
	int iMaxHeight = DEFAULT_HEIGHT + ( ( iMaxLine - 1 ) * LINE_GAP_HEIGHT );
	for(int i = 0;i < (int)rkHelpData.m_vHelpList.size();i++)
	{
		QuickStartHelpData &rkData = rkHelpData.m_vHelpList[i];
		if( rkData.m_bModeTooltipHelpPass ) continue;

		int iCurLineWidth = g_FontMgr.GetTextWidth( rkData.m_szHelp.c_str(), TS_NORMAL, FONT_SIZE_13 ) + 14;
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	SetSize( iMaxWidth, iMaxHeight );
	ProcessPosition();
}

void ModeHelpTooltip::ProcessPosition()
{
	if( m_pCallWnd )
	{
		if( !m_pCallWnd->IsShow() )
		{
			SetModeHelpTooltip( NULL, "", -1 );
			return;
		}
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void ModeHelpTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessPosition();
}

void ModeHelpTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + ( GetWidth() / 2 );
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	// 모드명
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	g_FontMgr.PrintTextWidthCut( iXPos, iYPos + START_TITLE_Y, FONT_SIZE_12, 170.f, m_szModeTitle.c_str() );	

	// 멘트
	int iLine = 0;
	QuickStartHelp &rkHelpData = g_ModeSelectInfo.GetQuickStartHelpData( m_iQuickstartModeHelpIndex );
	int iMaxLine = rkHelpData.m_vHelpList.size();
	for(int i = 0;i < iMaxLine;i++)
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		
		QuickStartHelpData &rkData = rkHelpData.m_vHelpList[i];
		if( rkData.m_bModeTooltipHelpPass ) continue;
		
		g_FontMgr.PrintText( iXPos, iYPos + START_HELP_Y + (iLine*LINE_GAP_HEIGHT), FONT_SIZE_12, rkData.m_szHelp.c_str() );	
		iLine++;
	}
}
//////////////////////////////////////////////////////////////////////////
UserBlockTypeAlarm::UserBlockTypeAlarm()
{
}

UserBlockTypeAlarm::~UserBlockTypeAlarm()
{
}

void UserBlockTypeAlarm::iwm_show()
{
	if( g_MyInfo.GetBlockType() == BKT_NORMAL )
	{
		HideWnd();
	}
	else if( g_MyInfo.GetBlockType() == BKT_WARNNING )
	{
		SetSize( GetWidth(), 247 );
		
		ioWnd *pCloseBtn = FindChildWnd( ID_CLOSE );
		if( pCloseBtn )
			pCloseBtn->SetWndPos( 17, 199 );

		if( m_pMultiplyFrm )
			m_pMultiplyFrm->SetPosition( 0, 182 );
	}
	else if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		SetSize( GetWidth(), 267 );

		ioWnd *pCloseBtn = FindChildWnd( ID_CLOSE );
		if( pCloseBtn )
			pCloseBtn->SetWndPos( 17, 219 );

		if( m_pMultiplyFrm )
			m_pMultiplyFrm->SetPosition( 0, 202 );
	}
}

void UserBlockTypeAlarm::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			iwm_show();
		}
	}
}

void UserBlockTypeAlarm::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void UserBlockTypeAlarm::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iYear   = 0;
	int iMonth  = 0;
	int iDay    = 0;
	int iHour   = 0;
	int iMinute = 0;
	if( g_MyInfo.GetBlockType() != BKT_NORMAL )
	{
		DWORD dwYearMonthDay = g_MyInfo.GetBlockYearMonthDay();
		DWORD dwHourMinute   = g_MyInfo.GetBlockHourMinute();
		iYear  =   ( dwYearMonthDay / 10000 );
		iMonth = ( ( dwYearMonthDay / 100 ) % 100 );
		iDay   =   ( dwYearMonthDay % 100 );
		iHour  =   ( dwHourMinute / 100 );
		iMinute=   ( dwHourMinute % 100 );
	}

	ioComplexStringPrinter kPrinter;

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), iYear, iMonth, iDay, iHour, iMinute );
	kPrinter.PrintFullText( iXPos + 18, iYPos + 73, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.PrintFullText( iXPos + 18, iYPos + 113, TAT_LEFT );
	kPrinter.ClearList();

	if( g_MyInfo.GetBlockType() == BKT_WARNNING )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(12) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 53, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 93, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 133, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 153, TAT_LEFT );
		kPrinter.ClearList();
	}
	else if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 53, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 93, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 133, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(11) );
		kPrinter.PrintFullText( iXPos + 18, iYPos + 173, TAT_LEFT );
		kPrinter.ClearList();
	}
}
//////////////////////////////////////////////////////////////////////////
ChatBubbleWnd::ChatBubbleWnd()
{
	m_pTooltipTail = NULL;

	m_iDefaultW  = 0;
	m_iDefaultH  = 0;
	m_iAddHeight = 0;
	m_iStartXPos = 0;
	m_iStartYPos = 0;
	m_iWidthCut  = 0;
	m_iMaxLine   = 0;
	m_dwShowTime = 0;
	m_dwStartTime= 0;
	m_dwShowEventTime = 0;
	m_dwShowAniTime = 0;
}

ChatBubbleWnd::~ChatBubbleWnd()
{
	SAFEDELETE( m_pTooltipTail );
}

void ChatBubbleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TooltipTail" )
	{
		SAFEDELETE( m_pTooltipTail );
		m_pTooltipTail = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ChatBubbleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iDefaultW  = xElement.GetIntAttribute_e( "DefaultW" );
	m_iDefaultH  = xElement.GetIntAttribute_e( "DefaultH" );
	m_iAddHeight = xElement.GetIntAttribute_e( "AddHeight" );
	m_iStartXPos = xElement.GetIntAttribute_e( "StartXPos" );
	m_iStartYPos = xElement.GetIntAttribute_e( "StartYPos" );
	m_iWidthCut  = xElement.GetIntAttribute_e( "WidthCut" );
	m_iMaxLine   = xElement.GetIntAttribute_e( "MaxLine" );
	m_dwShowTime = xElement.GetIntAttribute_e( "ShowTime" );
	m_dwShowEventTime = xElement.GetIntAttribute_e( "ShowEventTime" );
}

void ChatBubbleWnd::StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat )
{
	if( szChat == NULL ) return;
	if( strlen( szChat ) == 0 ) return;
	
	HideWnd();
	m_BubbleList.clear();
	DWORD dwTextColor = 0xFF3A3A3A;
	if( bWholeChat )
	{	
		dwTextColor = 0xFF005A53;
	}

	int iLen = (int)strlen( szChat );
	int iResult = 0;
	for(int i = 0;i < m_iMaxLine;i++)
	{
		ChatBubble kBubble;
		kBubble.m_dwTextColor = dwTextColor;
		
		char szDst[MAX_PATH] = "";
		memset( szDst, 0, sizeof( szDst ) );
		if( i == m_iMaxLine - 1 )
		{
			sprintf( szDst, "%s", &szChat[iResult] ); // 마지막 채팅은 전부 넣는다.			
			iResult = iLen;
		}
		else
		{
			iResult += Help::StringCutFun( FONT_SIZE_12, m_iWidthCut, TS_NORMAL, szDst, sizeof( szDst ), &szChat[iResult] );
		}
		kBubble.m_szText = szDst;
		m_BubbleList.push_back( kBubble );

		if( iResult >= iLen ) break;
	}

	m_szBubbleName  = szBubbleName;
	m_dwShowAniTime = FRAMEGETTIME();
	m_dwStartTime   = FRAMEGETTIME();
    SetSize( m_iDefaultW, m_iDefaultH + (m_BubbleList.size() * m_iAddHeight) );
	ShowWnd();
}

void ChatBubbleWnd::ChangeChatBubble( ChatBubbleWnd *pChatBubbleWnd )
{
	if( pChatBubbleWnd == NULL ) return;

	int i = 0;

	// Temp < - this
	vChatBubble vTempList;
	DWORD dwTempStartTime = 0;
	bool  bTempShow = false;
	ioHashString szTempBubbleName;
	for(i = 0;i < (int)m_BubbleList.size();i++)
	{
		vTempList.push_back( m_BubbleList[i] );
	}
	dwTempStartTime = m_dwStartTime;
	szTempBubbleName = m_szBubbleName;
	bTempShow = IsShow();

	// this < - pChatBubbleWnd
	m_BubbleList.clear();
	for(i = 0;i < (int)pChatBubbleWnd->m_BubbleList.size();i++)
	{
		m_BubbleList.push_back( pChatBubbleWnd->m_BubbleList[i] );
	}
	m_dwStartTime = pChatBubbleWnd->m_dwStartTime;
	m_szBubbleName = pChatBubbleWnd->m_szBubbleName;
	if( pChatBubbleWnd->IsShow() )
	{
		SetSize( m_iDefaultW, m_iDefaultH + (m_BubbleList.size() * m_iAddHeight) );
		ShowWnd();
	}
	else
	{
		HideWnd();
	}

	// pChatBubbleWnd < - Temp
	pChatBubbleWnd->m_BubbleList.clear();
	for(i = 0;i < (int)vTempList.size();i++)
	{
		pChatBubbleWnd->m_BubbleList.push_back( vTempList[i] );
	}
	pChatBubbleWnd->m_dwStartTime = dwTempStartTime;
	pChatBubbleWnd->m_szBubbleName = szTempBubbleName;
	if( bTempShow )
	{
		pChatBubbleWnd->SetSize( m_iDefaultW, m_iDefaultH + (pChatBubbleWnd->m_BubbleList.size() * m_iAddHeight) );
		pChatBubbleWnd->ShowWnd();
	}
	else
	{
		pChatBubbleWnd->HideWnd();
	}
}

void ChatBubbleWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwStartTime > m_dwShowTime )
	{
		HideWnd();
	}
}

bool ChatBubbleWnd::OnRenderShowAndHide()
{
	if( m_dwShowAniTime == 0 || m_dwShowEventTime == 0 ) return false;
	if( FRAMEGETTIME() - m_dwShowAniTime > m_dwShowEventTime )
	{
		m_dwShowAniTime = 0;
		if( m_pSelectedFrm )
		{
			m_pSelectedFrm->SetSize( GetWidth(), GetHeight() );
		}
		return false;
	}

	// 확대 & 축소를 20% ~ 100% 사이만 진행 시킨다.
	DWORD dwGapTime = FRAMEGETTIME() - m_dwShowAniTime;
	float fTimeRate = ( (float)dwGapTime / m_dwShowEventTime ) * FLOAT08;
	if( IsShow() )
	{
		fTimeRate = min( FLOAT08, fTimeRate ) + 0.2f;
	}
	else
	{
		fTimeRate = max( 0.0f, FLOAT08 - fTimeRate ) + 0.2f;
	}

	if( m_pSelectedFrm )
	{
		int iWidth = GetWidth() * fTimeRate;
		int iHeight= GetHeight() * fTimeRate;
		int iXPos  = GetDerivedPosX() + ( ( GetWidth() - iWidth ) / 2 );
		int iYPos  = GetDerivedPosY() + ( ( GetHeight() - iHeight ) / 2 );
		m_pSelectedFrm->SetSize( iWidth, iHeight );
		m_pSelectedFrm->Render( iXPos, iYPos );
	}		
	return true;
}

void ChatBubbleWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	for(int i = 0;i < (int)m_BubbleList.size();i++)
	{
		ChatBubble &rkBubble = m_BubbleList[i];
		
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( rkBubble.m_dwTextColor );
		g_FontMgr.PrintTextWidthCut( iXPos + m_iStartXPos, iYPos + m_iStartYPos + (i * m_iAddHeight), FONT_SIZE_12, m_iWidthCut, rkBubble.m_szText.c_str() );
	}

	if( m_pTooltipTail )
	{
		m_pTooltipTail->Render( iXPos, ( iYPos + GetHeight() ) - 2 );
	}
}
//////////////////////////////////////////////////////////////////////////
MovieModeHelpWnd::MovieModeHelpWnd()
{
	m_pLeftRes  = NULL;
	m_pRightRes = NULL;
	m_pMouseRes = NULL;
}

MovieModeHelpWnd::~MovieModeHelpWnd()
{
	SAFEDELETE( m_pLeftRes );
	SAFEDELETE( m_pRightRes );
	SAFEDELETE( m_pMouseRes );
}

void MovieModeHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "LeftRes" )
	{
		SAFEDELETE( m_pLeftRes );
		m_pLeftRes  = pImage;
	}
	else if( szType == "RightRes" )
	{
		SAFEDELETE( m_pRightRes );
		m_pRightRes = pImage;
	}
	else if( szType == "MouseRes" )
	{
		SAFEDELETE( m_pMouseRes );
		m_pMouseRes = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MovieModeHelpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_MOVIEMODE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			g_App.CheckMouseControlMode( true );
		}
		break;
	}
}

bool MovieModeHelpWnd::iwm_spacebar()
{
	ioWnd *pMovieBtn = FindChildWnd( ID_MOVIEMODE );
	if( !pMovieBtn ) return false;
	if( !pMovieBtn->IsShow() ) return false;

	iwm_command( pMovieBtn, IOBN_BTNUP, 0 );
	return true;
}

void MovieModeHelpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pLeftRes )
	{
		m_pLeftRes->SetSize( 134, 116 );
		m_pLeftRes->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pLeftRes->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pLeftRes->Render( iXPos + 16, iYPos + 56, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pRightRes )
	{
		m_pRightRes->SetSize( 134, 116 );
		m_pRightRes->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pRightRes->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pRightRes->Render( iXPos + 152, iYPos + 56, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pMouseRes )
	{
		m_pMouseRes->Render( iXPos + 17, iYPos + 252 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 187, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 207, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 227, FONT_SIZE_13, STR(3) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos + 88, iYPos + 247, FONT_SIZE_13, STR(4) );
	g_FontMgr.PrintText( iXPos + 88, iYPos + 267, FONT_SIZE_13, STR(5) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos + 88, iYPos + 287, FONT_SIZE_13, STR(6) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 88, iYPos + 307, FONT_SIZE_13, STR(7) );
}
//////////////////////////////////////////////////////////////////////////
ClauseViewWnd::ClauseViewWnd()
{
	m_iCurPos = 0;
}

ClauseViewWnd::~ClauseViewWnd()
{
	m_ClauseText.clear();
}

void ClauseViewWnd::iwm_show()
{
	if( m_ClauseText.empty() )
	{
		ioINILoader_e kLoader( "config/sp2_mgame_clause.ini" );
		kLoader.SetTitle_e( "common" );
		int iMaxClause = kLoader.LoadInt_e( "max_clause", 0 );

		char szBuf[MAX_PATH] = "";
		char szTitle[MAX_PATH] = "";
		for(int i = 0;i < iMaxClause;i++)
		{			
			sprintf_e( szTitle, "clause%d", i + 1 );
			kLoader.SetTitle( szTitle );

			ClauseText kClauseText;
			kLoader.LoadString_e( "text", "", szBuf, MAX_PATH );
			kClauseText.m_szText = szBuf;
			kClauseText.m_iOffSet = kLoader.LoadInt_e( "offset", 0 );
			m_ClauseText.push_back( kClauseText );
		}
	}

	m_iCurPos = 0;
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, (int)m_ClauseText.size() - ( MAX_LINE - 1 ) );
		pScroll->SetScrollPos( m_iCurPos );
	}
}

void ClauseViewWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPos = curPos;
}

void ClauseViewWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( !pScroll ) return;
		if( !pScroll->IsShow() ) return;

		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos + 1 );
		}
	}
}

void ClauseViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void ClauseViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	int iLineCnt = 0;
	for(int i = m_iCurPos;i < (int)m_ClauseText.size();i++, iLineCnt++)
	{
		if( iLineCnt >= MAX_LINE ) break;

		ClauseText &rkText = m_ClauseText[i];

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 23 + rkText.m_iOffSet, iYPos + 57 + iLineCnt * 20, FONT_SIZE_13, rkText.m_szText.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
FileWriteUploadWnd::FileWriteUploadWnd()
{
	m_pGaugeBar = NULL;

	m_iMaxFile = 0;
	m_iCurFile = 0;
	m_iMaxFileSize = 0;
	m_iCurFileSize = 0;
}

FileWriteUploadWnd::~FileWriteUploadWnd()
{
	SAFEDELETE( m_pGaugeBar );
}

void FileWriteUploadWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGaugeBar );
		m_pGaugeBar = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FileWriteUploadWnd::InitUploadData( int iMaxFile )
{
	m_iMaxFile = iMaxFile;
	m_iCurFile = m_iCurFileSize = m_iMaxFileSize = 0;
	m_iUploadFullRate = 0;
}

void FileWriteUploadWnd::UploadRemainFile( int iRemailFile )
{
	m_iCurFile = max( 0, m_iMaxFile - iRemailFile );
	m_iCurFileSize = m_iMaxFileSize = 0;
}

void FileWriteUploadWnd::UploadFileSize( int iCurFileSize, int iMaxFileSize )
{
	m_iCurFileSize = iCurFileSize;
	m_iMaxFileSize = iMaxFileSize;

	// 전체 진행 률
	float fPieceRate = FLOAT100 / m_iMaxFile;
	float fFileSizeRate = (float)m_iCurFileSize / (float)max( 1, m_iMaxFileSize );
	m_iUploadFullRate = min( 100, ( fPieceRate * m_iCurFile ) + ( fPieceRate * fFileSizeRate ) );
}

void FileWriteUploadWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.PrintText( iXPos + 43, iYPos + 81, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 43, iYPos + 100, FONT_SIZE_12, STR(2), m_iCurFile + 1, m_iMaxFile );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + (GetWidth() / 2), iYPos + 141, FONT_SIZE_13, STR(3) );
	g_FontMgr.PrintText( iXPos + (GetWidth() / 2), iYPos + 158, FONT_SIZE_13, STR(4) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + 125, iYPos + 53, FONT_SIZE_17, STR(5) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(6), m_iUploadFullRate );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 11, TAT_CENTER );
	kPrinter.ClearList();

	OnRenderGauge( iXPos, iYPos );
}

void FileWriteUploadWnd::OnRenderGauge( int iXPos, int iYPos )
{
	if( !m_pGaugeBar ) return;

	float fFileSizeRate = (float)m_iCurFileSize / max( 1, m_iMaxFileSize );
	float fGaugeSize = m_pGaugeBar->GetWidth();
	float fStepGauge = fGaugeSize / max( 1, m_iMaxFile );

	int iAllFileSize = min( fGaugeSize, ( fStepGauge * m_iCurFile ) + ( fStepGauge * fFileSizeRate ) ); 
	m_pGaugeBar->RenderWidthCut( iXPos + 50, iYPos + 82, 0, iAllFileSize );
	
	int iPieceFileSize = min( fGaugeSize, fGaugeSize * fFileSizeRate );
	m_pGaugeBar->RenderWidthCut( iXPos + 50, iYPos + 101, 0, iPieceFileSize );
}
//////////////////////////////////////////////////////////////////////////
ItemCustomThumbnailTooltip::ItemCustomThumbnailTooltip()
{
}

ItemCustomThumbnailTooltip::~ItemCustomThumbnailTooltip()
{
}

void ItemCustomThumbnailTooltip::ClearList()
{
	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; ++i )
	{
		m_Printer[i].ClearList();
	}
	m_ExtraItem.Init();
}

void ItemCustomThumbnailTooltip::iwm_hide()
{
	HideChildWnd( ID_CUSTOM_1 );	
	HideChildWnd( ID_CUSTOM_2 );
}

void ItemCustomThumbnailTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter, EXTRAITEMSLOT &rkExtraItem )
{
	ClearList();
	
	m_ExtraItem = rkExtraItem;
	if( m_ExtraItem.m_dwMaleCustom == 0 && m_ExtraItem.m_dwFemaleCustom == 0 )
	{
		HideWnd();
		return;
	}

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}

	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = THUMBNAIL_WIDTH;
	if( m_ExtraItem.m_dwMaleCustom > 0 && m_ExtraItem.m_dwFemaleCustom > 0 )
	{
		iMaxWidth += THUMBNAIL_WIDTH + 4;
	}
	else
	{
		iMaxWidth = THUMBNAIL_WIDTH + 42;
	}

	int iMaxHeight = MIN_HEIGHT + 8;
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMaxWidth, THUMBNAIL_HEIGHT + iMaxHeight );
	ProcessPosition();

	if( m_ExtraItem.m_dwMaleCustom > 0 && m_ExtraItem.m_dwFemaleCustom > 0 )
	{
		ItemCustomThumbnailWnd *pCustom1 = dynamic_cast< ItemCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 182, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_ExtraItem, true );
		}

		ItemCustomThumbnailWnd *pCustom2 = dynamic_cast< ItemCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_2 ) );
		if( pCustom2 )
			pCustom2->CreateCharacter( m_ExtraItem, false );
	}
	else if( m_ExtraItem.m_dwMaleCustom > 0 )
	{
		ItemCustomThumbnailWnd *pCustom1 = dynamic_cast< ItemCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 217, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_ExtraItem, true );
		}
	}
	else if( m_ExtraItem.m_dwFemaleCustom > 0 )
	{
		ItemCustomThumbnailWnd *pCustom1 = dynamic_cast< ItemCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 217, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_ExtraItem, false );
		}
	}
}

void ItemCustomThumbnailTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void ItemCustomThumbnailTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessPosition();
}

void ItemCustomThumbnailTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;;

	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP*i, TAT_LEFT );
	}
}
//////////////////////////////////////////////////////////////////////////
EtcMotionSelectWnd::EtcMotionSelectWnd()
{
	m_pCallWnd = NULL;
	m_dwEtcItem= 0;
	m_dwBeforeSelectID = 0;
	m_dwAfterSelectID  = 0;
}

EtcMotionSelectWnd::~EtcMotionSelectWnd()
{
}

void EtcMotionSelectWnd::ShowSelectWnd( ioWnd *pCallWnd )
{
	if( !pCallWnd )
		return;

	m_pCallWnd     = pCallWnd;
	m_dwEtcItem    = 0;
	m_dwBeforeSelectID = 0;
	m_dwAfterSelectID  = 0;

	InventoryEtcMotionBtn *pEtcMotionBtn = dynamic_cast< InventoryEtcMotionBtn * >( m_pCallWnd );
	{
		m_dwEtcItem = pEtcMotionBtn->GetEtcCode();

		if( !COMPARE( m_dwEtcItem, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) &&
			!COMPARE( m_dwEtcItem, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
			return;

		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( !pUserEtcItem->GetEtcItem( m_dwEtcItem, kSlot ) )
				return;

			ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( m_dwEtcItem ) );
			if( pEtcMotion )
			{					
				SetTitleText( pEtcMotion->GetName().c_str() );

				m_dwBeforeSelectID = m_dwAfterSelectID = kSlot.GetUse();
				ioHashString kTitle = pEtcMotion->GetMotionOptionUITitle( m_dwBeforeSelectID );
				if( kTitle.IsEmpty() )
				{
					ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
					if( pSelectBtn )
					{
						pSelectBtn->SetTitleText( STR(1) );
					}
				}
				else
				{
					ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
					if( pSelectBtn )
					{
						pSelectBtn->SetTitleText( kTitle.c_str() );
					}
				}
			}			
		}
	}
	ShowWnd();
}

void EtcMotionSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 리스트 오픈
			EtcMotionSelectListWnd *pEtcMotionSelectList = dynamic_cast< EtcMotionSelectListWnd * >( FindChildWnd( ID_SELECT_LIST ) );
			if( pEtcMotionSelectList )
			{
				pEtcMotionSelectList->ShowSelectListWnd( m_dwEtcItem );
			}	
		}
		break;
	case ID_COMPLETE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			// 설정 완료
			HideWnd();

			if( m_dwBeforeSelectID != m_dwAfterSelectID )
			{
				if( !COMPARE( m_dwEtcItem, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) &&
					!COMPARE( m_dwEtcItem, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
					return;

				//
				ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( m_dwEtcItem ) );
				if( pEtcMotion )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_ETCITEM_MOTION_OPTION );
					kPacket << m_dwEtcItem << m_dwAfterSelectID << pEtcMotion->IsMotionOptionOnlyOne( m_dwAfterSelectID );
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	case ID_SELECT_LIST:
		if( cmd == IOBN_BTNUP )
		{
			// 리스트 선택
			if( param != 0 )
			{
				ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( m_dwEtcItem ) );
				if( pEtcMotion )
				{										
					m_dwAfterSelectID = param;
					ioHashString kTitle = pEtcMotion->GetMotionOptionUITitle( m_dwAfterSelectID );
					if( kTitle.IsEmpty() )
					{
						ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
						if( pSelectBtn )
						{
							pSelectBtn->SetTitleText( STR(1) );
						}
					}
					else
					{
						ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
						if( pSelectBtn )
						{
							pSelectBtn->SetTitleText( kTitle.c_str() );
						}
					}
				}			
			}
		}
		break;
	}
}

void EtcMotionSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
EtcMotionSelectListWnd::EtcMotionSelectListWnd()
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
	m_iCutWidth = 0;
	m_iCurPos   = 0;
	m_dwEtcItem = 0;
}

EtcMotionSelectListWnd::~EtcMotionSelectListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void EtcMotionSelectListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void EtcMotionSelectListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void EtcMotionSelectListWnd::ShowSelectListWnd( DWORD dwEtcItem )
{
	ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
	if( !pEtcMotion ) return;

	m_SelectList.clear();

	// 설정 안함
	SelectData kSelectData;
	kSelectData.m_dwSelectID = 1;
	kSelectData.m_szTitle    = STR(1);
	m_SelectList.push_back( kSelectData );

	int iMaxOption = pEtcMotion->GetMaxMotionOption();
	for(int i = 0;i < iMaxOption;i++)
	{
		kSelectData.m_dwSelectID = pEtcMotion->GetMotionOptionArrayToID( i );
		if( kSelectData.m_dwSelectID == 0 ) continue;

		kSelectData.m_szTitle = pEtcMotion->GetMotionOptionUITitle( kSelectData.m_dwSelectID );
		m_SelectList.push_back( kSelectData );
	}
	m_dwSelectID = 0;
	ShowWnd();
}

void EtcMotionSelectListWnd::iwm_show()
{
	m_iCurPos = 0;
	if( (int)m_SelectList.size() <= MAX_PAGE_LIST )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_SelectList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	// 윈도우 사이즈 결정	
	SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_SelectList.size() ) * m_iHeightGap) + m_iAddHeight );
	SetCheckParentOutSide( true );
}

void EtcMotionSelectListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void EtcMotionSelectListWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void EtcMotionSelectListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void EtcMotionSelectListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_dwSelectID = 0;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( EtcMotionSelectWnd::ID_SELECT_BTN ) &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_SelectList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_SelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - m_iAddWidth ) - iCutWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_dwSelectID  = m_SelectList[iListPos].m_dwSelectID;
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( this, IOBN_BTNUP, m_dwSelectID );
				HideWnd();
			}
			break;
		}
	}
}

void EtcMotionSelectListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_SelectList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_SelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		SelectData &rkData = m_SelectList[iListPos];
		bDarkRender = !bDarkRender;
		if( m_dwSelectID == rkData.m_dwSelectID )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 244.0f - iCutWidth, 
									 rkData.m_szTitle.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
PingStepWnd::PingStepWnd()
{
	for(int i = 0;i < MAX_PING_STEP;i++)
		m_pPingStep[i] = NULL;
	m_pServerRelay = NULL;
}

PingStepWnd::~PingStepWnd()
{
	for(int i = 0;i < MAX_PING_STEP;i++)
		SAFEDELETE( m_pPingStep[i] );
	SAFEDELETE( m_pServerRelay );
}

void PingStepWnd::Create()
{
	for(int i = 0;i < MAX_PING_STEP;i++)
		SAFEDELETE( m_pPingStep[i] );
	SAFEDELETE( m_pServerRelay );

	m_pPingStep[0] = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_line5" );
	m_pPingStep[1] = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_line4" );
	m_pPingStep[2] = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_line3" );
	m_pPingStep[3] = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_line2" );
	m_pPingStep[4] = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_line1" );
	m_pServerRelay = g_UIImageSetMgr.CreateImageByFullName( "BaseImage008#icon0_nping_r" );
}

void PingStepWnd::RenderPing( int iXPos, int iYPos, int iPingStep, bool bServerRelay, UIRenderType eRenderType )
{
	iPingStep = max( 0, min( MAX_PING_STEP - 1, iPingStep ) );
	if( m_pPingStep[iPingStep] )
	{
		m_pPingStep[iPingStep]->Render( iXPos, iYPos, eRenderType );

		if( bServerRelay && m_pServerRelay )
		{
			m_pServerRelay->Render( iXPos, iYPos );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
SoldierExperienceWnd::SoldierExperienceWnd()
{
}

SoldierExperienceWnd::~SoldierExperienceWnd()
{
}

void SoldierExperienceWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void SoldierExperienceWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void SoldierExperienceWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 184, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 204, TAT_CENTER );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
StringScrollListWnd::StringScrollListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_pOpenBtn  = NULL;

	m_eStyle = DOWN_STYLE;

	m_iDefaultXPos = 0;
	m_iDefaultYPos = 0;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iCutWidth = 0;
	m_iCurPos   = 0;
}

StringScrollListWnd::~StringScrollListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void StringScrollListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void StringScrollListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void StringScrollListWnd::SetList( ioWnd *pOpenBtn, int iStartXPos, int iStartYPos, OpenStyle eStyle )
{
	m_pOpenBtn = pOpenBtn;
	m_eStyle = eStyle;

	m_ListData.clear();		

	SetWndPos( iStartXPos, iStartYPos );
}

void StringScrollListWnd::AddList( int iNotifyCommand, int iNotifyCommandCode, const ioHashString &szTitle, ioUIRenderImage *pMarkRes )
{
	ListData kListData;
	kListData.m_iNotifyCommand		= iNotifyCommand;
	kListData.m_iNotifyCommandCode	= iNotifyCommandCode;
	kListData.m_szTitle				= szTitle;
	kListData.m_pMarkRes			= pMarkRes;

	m_ListData.push_back( kListData );
}

void StringScrollListWnd::ShowListWnd()
{
	if( IsShow() )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();
	}
}

void StringScrollListWnd::iwm_create()
{
	m_iDefaultXPos = GetXPos();
	m_iDefaultYPos = GetYPos();
}

void StringScrollListWnd::iwm_show()
{	
	if( m_ListData.empty() )
		HideWnd();
	else
	{
		m_iCurPos = 0;
		if( (int)m_ListData.size() <= MAX_PAGE_LIST )
		{
			HideChildWnd( ID_VERT_SCROLL );
		}
		else
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollRange( 0, m_ListData.size() );
				pScroll->SetScrollPos( m_iCurPos );
				pScroll->ShowWnd();
			}
		}

		// 윈도우 사이즈 결정	
		SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_ListData.size() ) * m_iHeightGap ) + m_iAddHeight );
		SetCheckParentOutSide( true );

		if( m_eStyle == UP_STYLE )
		{
			SetWndPos( m_iDefaultXPos, m_iDefaultYPos - (( min( MAX_PAGE_LIST, m_ListData.size() ) * m_iHeightGap ) + m_iAddHeight) );
		}
	}
}

void StringScrollListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void StringScrollListWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );

	}
}

void StringScrollListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void StringScrollListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_OverListItem.m_szTitle.Clear();
	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != m_pOpenBtn &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_ListData.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_ListData.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - m_iAddWidth ) - iCutWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_OverListItem  = m_ListData[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
				{
					GetParent()->iwm_command( m_pOpenBtn, GetID(), m_OverListItem.m_iNotifyCommandCode );
				}
				HideWnd();
			}
			break;
		}
	}
}

void StringScrollListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_ListData.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_ListData.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_OverListItem.m_szTitle == m_ListData[iListPos].m_szTitle )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 181.0f - iCutWidth, 
			m_ListData[iListPos].m_szTitle.c_str() );

		if( m_ListData[iListPos].m_pMarkRes )
		{
			int iTitleSize = g_FontMgr.GetTextWidthCutSize( m_ListData[iListPos].m_szTitle.c_str(), TS_NORMAL, FONT_SIZE_12, 181.0f - iCutWidth );
			m_ListData[iListPos].m_pMarkRes->Render( iXPos + m_iListXPos + 7 + iTitleSize, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 5 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ioCalendarSet CalendarButton::m_StartEnableSet;
int CalendarButton::m_iPeriodEnableTime  = 0;
CalendarButton::CalendarButton()
{
	m_pSelectNor	 = NULL;
	m_pSelectOver	 = NULL;
	m_pSelectPush	 = NULL;

	m_pCurrentNor	 = NULL;
	m_pCurrentPush	 = NULL;
	m_pCurrentOver	 = NULL;

	m_bSelect = false;
	m_bPeriod = false;	
}

CalendarButton::~CalendarButton()
{
	SAFEDELETE( m_pSelectNor );
	SAFEDELETE( m_pSelectOver );
	SAFEDELETE( m_pSelectPush );
}

void CalendarButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SelectNor" )
	{
		SAFEDELETE( m_pSelectNor );
		m_pSelectNor = pFrame;
	}
	else if( szType == "SelectOver" )
	{
		SAFEDELETE( m_pSelectOver );
		m_pSelectOver = pFrame;
	}
	else if( szType == "SelectPush" )
	{
		SAFEDELETE( m_pSelectPush );
		m_pSelectPush = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void CalendarButton::SetDate( int iYear, int iMonth, int iDay, int iDayOfWeek )
{
	m_CalendarSet.SetClear();
	m_CalendarSet.m_iYear  = iYear;
	m_CalendarSet.m_iMonth = iMonth;
	m_CalendarSet.m_iDay   = iDay;
	m_CalendarSet.m_iDayOfWeek = iDayOfWeek;
	m_CalendarSet.SetCalendarCompareType( DT_LAST_DAY );

	if( m_StartEnableSet.isEmpty() )
	{
		m_bPeriod = true;
		SetActive();
	}
	else if( m_iPeriodEnableTime == 0 )
	{
		m_bPeriod = true;
		SetActive();
	}
	else if( m_StartEnableSet <= m_CalendarSet )
	{
		if( m_CalendarSet <= m_StartEnableSet + m_iPeriodEnableTime * 24 * 60 )
		{
			m_bPeriod = true;
			SetActive();
		}
		else
		{
			m_bPeriod = false;
			SetInActive();
		}
	}
 	else
 	{
		m_bPeriod = false;
		SetInActive();
 	}
}

void CalendarButton::SetDateOption( ioCalendarSet StartEnableSet, int iPeriodEnableTime )
{
	m_StartEnableSet     = StartEnableSet;
	m_iPeriodEnableTime  = iPeriodEnableTime;

	m_StartEnableSet.SetCalendarCompareType( DT_LAST_DAY );
}

void CalendarButton::SetSelect( bool bSelect )
{
	m_bSelect = bSelect;

	if( bSelect )
	{
		m_pCurrentNor  = m_pSelectNor;
		m_pCurrentPush = m_pSelectPush;
		m_pCurrentOver = m_pSelectOver;
	}
	else
	{
		m_pCurrentNor  = NULL;
		m_pCurrentPush = NULL;
		m_pCurrentOver = NULL;
	}
}

ioCalendarSet& CalendarButton::GetDate()
{
	return m_CalendarSet;
}

void CalendarButton::OnRender()
{
	if( !IsShow() ) return;

	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_bSelect )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	}
	else
	{
		if( m_bPeriod )
		{
			if( m_CalendarSet.m_iDayOfWeek == 0 )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		}
	}
	
	g_FontMgr.PrintText( iXPos + 22, iYPos + 12, FONT_SIZE_18, "%d", m_CalendarSet.m_iDay );
}

void CalendarButton::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	if( m_pCurrentNor )
		m_pCurrentNor->Render( iXPos, iYPos );
}

void CalendarButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	if( m_pCurrentOver )	
	{
		m_pCurrentOver->Render( iXPos, iYPos );
		OnDrawOveredAdd( iXPos, iYPos );
	}	
}

void CalendarButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	if( m_pCurrentPush )
		m_pCurrentPush->Render( iXPos, iYPos );

}
//////////////////////////////////////////////////////////////////////////
CalendarWnd::CalendarWnd()
{
	m_szExplainList.clear();
	
	m_dwNotifyParam = 0;

	m_iPeriodEnableTime = 0;
	m_dwWheelStartTime = 0;
	m_dwWheelTime = 0;

	m_pPreEdit = NULL;
}

CalendarWnd::~CalendarWnd()
{
}

void CalendarWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
}

void CalendarWnd::iwm_hide()
{
	ioEdit* pEditWnd = (ioEdit*)FindChildWnd( ID_HOUR_INPUT );
	if( pEditWnd )
		pEditWnd->KillKeyFocus();

	pEditWnd = (ioEdit*)FindChildWnd( ID_MINITUE_INPUT );
	if( pEditWnd )
		pEditWnd->KillKeyFocus();

	if( m_pPreEdit )
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}
		m_pPreEdit = NULL;
	}
}

void CalendarWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwWheelTime = xElement.GetIntAttribute_e( "WheelTime" );
}

void CalendarWnd::InitCalendar( ioWnd* pDateRequestWnd,
								DWORD dwNotifyParam,
								ioHashString& szTtitle,
								ioHashStringVec& szExplainList,
								ioCalendarSet& StartCalendarSet,
								int iPeriodEnableTime )
{
	m_pDateRequestWnd   = pDateRequestWnd;
	m_szExplainList	    = szExplainList;
	m_dwNotifyParam     = dwNotifyParam;
	m_iPeriodEnableTime = iPeriodEnableTime;

	SetTitleText( szTtitle.c_str() );

	//서버기준 시작날짜 세팅
	m_CurCalendarSet.SetTodayServerDate();

	//시작날짜 달의 1일의 값을 구함
	int iCurYear  = m_CurCalendarSet.m_iYear;
	int iCurMonth = m_CurCalendarSet.m_iMonth;
	int iCurDay   = 1;

	CalendarButton::SetDateOption( StartCalendarSet, iPeriodEnableTime );

	SetSelectHour( m_CurCalendarSet.m_iHour );
	SetSelectMin( m_CurCalendarSet.m_iMin );
		
	m_SelectDate.SetClear();

	InitCalendarButton( iCurYear, iCurMonth, iCurDay );
}

void CalendarWnd::ModifyCalendar( ioWnd* pDateRequestWnd,
								  DWORD dwNotifyParam,
								  ioHashString& szTtitle,
								  ioHashStringVec& szExplainList,
								  ioCalendarSet& CurCalendarSet )
{
	m_pDateRequestWnd   = pDateRequestWnd;
	m_dwNotifyParam     = dwNotifyParam;
	m_szExplainList	    = szExplainList;
	
	SetTitleText( szTtitle.c_str() );

	m_SelectDate = CurCalendarSet;
	m_SelectDate.SetCalendarCompareType( DT_LAST_DAY );

	SetSelectHour( m_SelectDate.m_iHour );
	SetSelectMin( m_SelectDate.m_iMin );
	
	CheckSelectButton( m_SelectDate );
}

void CalendarWnd::InitCalendarButton( int iYear, int iMonth, int iDay )
{
	char szBuff[MAX_PATH];
	sprintf_s_e( szBuff, "%d년 %d월", iYear, iMonth );
	m_szDate = szBuff;

	int iWeekOfDay = DateHelp::GetWeekDay( iYear, iMonth, iDay );

	//시작날짜를 기준으로 현 달력페이지의 첫 날짜를 구한다.
	for( int i = 0; i < iWeekOfDay; ++i )
		DateHelp::DecreaseDay( iYear, iMonth, iDay );

	//위에서 구한 첫 날짜를 기준으로 순차적으로 날짜 세팅
	for( int iWeek = WEEK_LINE1; iWeek <= WEEK_LINE6; iWeek+=100 )
	{
		for( int iDayOfWeek = 0; iDayOfWeek < WEEK; ++iDayOfWeek )
		{
			int iID = iWeek + iDayOfWeek;
			CalendarButton* pWnd = dynamic_cast<CalendarButton*>( FindChildWnd( iID ) );
			if( pWnd )
			{
				pWnd->SetDate( iYear, iMonth, iDay, iDayOfWeek );
				DateHelp::IncreaseDay( iYear, iMonth, iDay );
			}
		}
	}

	CheckSelectButton( m_SelectDate );
}

void CalendarWnd::CreateHourList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_HOUR_LIST ) );
	if( pList )
	{
		for( int i = 0; i < ONEDAYTIME; ++i )
		{
			char szHour[MAX_PATH];
			if( 1 < GetNumberCount(i) )
				sprintf_s_e( szHour, "%d", i );
			else
				sprintf_s( szHour, "0%d", i );
			
			pList->AddList( ID_HOUR_LIST, i, szHour, NULL );
		}
	}
}

void CalendarWnd::CreateMinitueList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_MINITUE_LIST ) );
	if( pList )
	{
		for( int i = 0; i < ONEHOUR; i+=10 )
		{
			char szMin[MAX_PATH];
			sprintf_s_e( szMin, "%d", i );

			pList->AddList( ID_HOUR_LIST, i, szMin, NULL );			
		}
	}
}

void CalendarWnd::OpenHourList( ioWnd *pOpenWnd )
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_HOUR_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() - 24, StringScrollListWnd::UP_STYLE );
		CreateHourList();
		pList->ShowListWnd();
	}
}

void CalendarWnd::OpenMinitueList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_MINITUE_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() - 24, StringScrollListWnd::UP_STYLE );
		CreateMinitueList();
		pList->ShowListWnd();
	}
}

void CalendarWnd::SetSelectHour( int iValue )
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_HOUR_INPUT ) );
	if( pEdit )
	{
		char szBuff[MAX_PATH];
		sprintf_s_e( szBuff, "%d", iValue );
		pEdit->SetText( szBuff );
	}
}

void CalendarWnd::SetSelectMin( int iValue )
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( CalendarWnd::ID_MINITUE_INPUT ) );
	if( pEdit )
	{
		char szBuff[MAX_PATH];
		sprintf_s_e( szBuff, "%d", iValue );
		pEdit->SetText( szBuff );
	}
}

int CalendarWnd::GetSelectHour()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd(ID_HOUR_INPUT) );
	if( pEdit )
	{
		int iHour = atoi( pEdit->GetText() );
		return iHour;
	}

	return 0;
}

int CalendarWnd::GetSelectMin()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd(ID_MINITUE_INPUT) );
	if( pEdit )
	{
		int iMin = atoi( pEdit->GetText() );
		return iMin;
	}

	return 0;
}

void CalendarWnd::GetSelectDate( ioCalendarSet& SelectSet )
{	
	if( !m_SelectDate.isEmpty() )
	{		
		SelectSet = m_SelectDate;
		SelectSet.m_iHour = GetSelectHour();
		SelectSet.m_iMin  = GetSelectMin();
	}
	else
	{
		SelectSet.SetClear();
	}
}

bool CalendarWnd::isEnableHour()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd(ID_HOUR_INPUT) );
	if( pEdit )
	{
		int iHour = atoi( pEdit->GetText() );
		if( 23 < iHour )
			return false;
		else if( iHour < 0 )
			return false;
	}

	return true;
}
bool CalendarWnd::isEnableMin()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd(ID_MINITUE_INPUT) );
	if( pEdit )
	{
		int iMin = atoi( pEdit->GetText() );
		if( 59 < iMin )
			return false;
		else if( iMin < 0 )
			return false;
	}

	return true;
}

ioWnd* CalendarWnd::FindCalendarButton( ioCalendarSet& FindSet )
{
	DWORD dwCurID = 0;
	for( int iWeek = WEEK_LINE1; iWeek <= WEEK_LINE6; iWeek+=100 )
	{
		for(int iWDay = 0; iWDay < WEEK; ++iWDay )
		{
			dwCurID = iWeek + iWDay;
			CalendarButton* pWnd = dynamic_cast<CalendarButton*>( FindChildWnd( dwCurID ) );
			if( pWnd )
			{
				if( FindSet == pWnd->GetDate() )
				{
					return pWnd;
				}
			}
		}
	}

	return NULL;
}

void CalendarWnd::CheckSelectButton( ioCalendarSet& SelectSet )
{
	DWORD dwCurID;
	for( int iWeek = WEEK_LINE1; iWeek <= WEEK_LINE6; iWeek+=100 )
	{
		for(int iWDay = 0; iWDay <  WEEK; ++iWDay )
		{
			dwCurID = iWeek + iWDay;
			CalendarButton* pWnd = dynamic_cast<CalendarButton*>( FindChildWnd( dwCurID ) );
			if( pWnd )
			{
				if( !SelectSet.isEmpty() && SelectSet == pWnd->GetDate() )				
				{
					m_SelectDate = SelectSet;
					pWnd->SetSelect( true );
				}
				else
				{
					pWnd->SetSelect( false );
				}
			}
		}
	}
}

void CalendarWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CLOSE:
	case ID_CANCEL:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	case ID_CONFIRM:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_SelectDate.isEmpty() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "날짜와 시간을 선택 해주세요");
				}
				else if( !isEnableHour() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "올바른 시간을 입력 해주세요");
					SetSelectHour( 0 );
				}
				else if( !isEnableMin() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "올바른 분을 입력 해주세요");
					SetSelectMin( 0 );
				}
				else
				{
					if( m_pDateRequestWnd )
						m_pDateRequestWnd->iwm_command( this, GetID(), m_dwNotifyParam );
				}
			}
		}
		break;
	case ID_HOUR_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				OpenHourList( pWnd );
			}
			else if( cmd == ID_HOUR_LIST )
			{
				SetSelectHour( (int)param );
			}
		}
		break;
	case ID_MINITUE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				OpenMinitueList( pWnd );
			}
			else if( cmd == ID_MINITUE_LIST )
			{
				SetSelectMin( (int)param );
			}
		}
		break;
	case ID_PREV_BTN : 
		{
			if( cmd == IOBN_BTNUP )
			{
				DateHelp::DecreaseMonth( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay );
				InitCalendarButton( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay ); 
			}
		}
		break;
	case ID_NEXT_BTN :
		{
			if( cmd == IOBN_BTNUP )
			{
				DateHelp::IncreaseMonth( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay );	
				InitCalendarButton( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay ); 
			}
		}
		break;
	case ID_CALENDAR_SUN1 :
	case ID_CALENDAR_MON1 :
	case ID_CALENDAR_TUE1 :
	case ID_CALENDAR_WEN1 :
	case ID_CALENDAR_THU1 :
	case ID_CALENDAR_FRI1 :
	case ID_CALENDAR_SAT1 :
	case ID_CALENDAR_SUN2 :
	case ID_CALENDAR_MON2 :
	case ID_CALENDAR_TUE2 :
	case ID_CALENDAR_WEN2 :
	case ID_CALENDAR_THU2 :
	case ID_CALENDAR_FRI2 :
	case ID_CALENDAR_SAT2 :
	case ID_CALENDAR_SUN3 :
	case ID_CALENDAR_MON3 :
	case ID_CALENDAR_TUE3 :
	case ID_CALENDAR_WEN3 :
	case ID_CALENDAR_THU3 :
	case ID_CALENDAR_FRI3 :
	case ID_CALENDAR_SAT3 :
	case ID_CALENDAR_SUN4 :
	case ID_CALENDAR_MON4 :
	case ID_CALENDAR_TUE4 :
	case ID_CALENDAR_WEN4 :
	case ID_CALENDAR_THU4 :
	case ID_CALENDAR_FRI4 :
	case ID_CALENDAR_SAT4 :
	case ID_CALENDAR_SUN5 :
	case ID_CALENDAR_MON5 :
	case ID_CALENDAR_TUE5 :
	case ID_CALENDAR_WEN5 :
	case ID_CALENDAR_THU5 :
	case ID_CALENDAR_FRI5 :
	case ID_CALENDAR_SAT5 :
	case ID_CALENDAR_SUN6 :
	case ID_CALENDAR_MON6 :
	case ID_CALENDAR_TUE6 :
	case ID_CALENDAR_WEN6 :
	case ID_CALENDAR_THU6 :
	case ID_CALENDAR_FRI6 :
	case ID_CALENDAR_SAT6 :
		{
			if( cmd == IOBN_BTNUP )
			{
				CalendarButton* pCalendarBtn = dynamic_cast<CalendarButton*>( pWnd );
				if( pCalendarBtn && pCalendarBtn->GetPeriod() )				
				{					
					CheckSelectButton( pCalendarBtn->GetDate() );
				}
				else
				{
					m_pDateRequestWnd->iwm_command( this, IOCM_USER_DEFINED, 0 );
				}
			}
		}
		break;
	}
}

void CalendarWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;
		DWORD dwCurTime = FRAMEGETTIME();

		if( m_dwWheelStartTime + m_dwWheelTime < dwCurTime )
		{
			if( zDelta == WHEEL_DELTA )
			{
				DateHelp::DecreaseMonth( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay );
				InitCalendarButton( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay );			
			}	
			else if( zDelta == -WHEEL_DELTA )
			{
				DateHelp::IncreaseMonth( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay );	
				InitCalendarButton( m_CurCalendarSet.m_iYear, m_CurCalendarSet.m_iMonth, m_CurCalendarSet.m_iDay ); 
			}

			m_dwWheelStartTime = dwCurTime;
		}
	}
}

void CalendarWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnTitleRender( iXPos, iYPos );
	OnExplainRender( iXPos, iYPos );
}

void CalendarWnd::OnTitleRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 180, iYPos + 55, FONT_SIZE_22, "%s", m_szDate.c_str());

	int iOffsetX = 0;
	for( int i = 0; i < WEEK; ++i )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 39 + iOffsetX, iYPos + 92, FONT_SIZE_12, "%s", DateHelp::GetWeekDayString( i ) );
		iOffsetX += 47;
	}
}

void CalendarWnd::OnExplainRender( int iXPos, int iYPos )
{
	int iOffset = 0;
	ioHashStringVec::const_iterator iter = m_szExplainList.begin();
	for( ; iter != m_szExplainList.end(); ++iter )
	{
		ioHashString szMent = (*iter);
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 180, iYPos + 430 + iOffset, FONT_SIZE_12, "%s", szMent.c_str() );
		iOffset +=18;
	}
}


void NetworkWnd::SetModeShow( int iModeType )
{
	if( IsShow() )
		HideWnd();

	m_iCurStep = 0;
	m_iYOffSet = 0;

	//m_iCurStep = Help::GetNetworkStep( g_MyInfo.GetPublicID() );

	switch( iModeType )
	{
		//case MT_MY_HOME:
	case MT_TRAINING:
	case MT_HEADQUARTERS:
	case MT_PRACTICE:
		m_iYOffSet = 55;
		break;
	}

	ResetWndPos();

	ioWnd *pStepWnd = FindChildWnd( ID_STEP_WND );
	if( pStepWnd && !pStepWnd->IsShow() )
	{
		pStepWnd->ShowWnd();
	}

	ClearUITextList();
	GetUITextList();
	ChangeTextState();

	ShowWnd();
}

void NetworkWnd::ChangeTextState()
{
	switch( m_iCurStep )
	{
	case 0:
	case 1:
		if( m_NetText[0] )
			m_NetText[0]->SetVisible( true );

		if( m_NetText[1] )
			m_NetText[1]->SetVisible( false );

		if( m_NetText[2] )
			m_NetText[2]->SetVisible( false );
		break;
	case 2:
	case 3:
		if( m_NetText[0] )
			m_NetText[0]->SetVisible( false );

		if( m_NetText[1] )
			m_NetText[1]->SetVisible( true );

		if( m_NetText[2] )
			m_NetText[2]->SetVisible( false );
		break;
	case 4:
	default:
		if( m_NetText[0] )
			m_NetText[0]->SetVisible( false );

		if( m_NetText[1] )
			m_NetText[1]->SetVisible( false );

		if( m_NetText[2] )
			m_NetText[2]->SetVisible( true );
		break;
	}
}

void NetworkWnd::GetUITextList()
{
	char szBuf[MAX_PATH];
	for( int i=0; i < MAX_TEXT; ++i )
	{
		wsprintf_e( szBuf, "NetText%d", i+1 );
		m_NetText[i] = GetText( szBuf );

		if( m_NetText[i] )
			m_NetText[i]->SetVisible( false );
	}
}

void NetworkWnd::ClearUITextList()
{
	for( int i=0; i < MAX_TEXT; ++i )
	{
		m_NetText[i] = NULL;
	}
}

void NetworkWnd::ResetWndPos()
{
	SetWndPos( GetXPos(), GetYPos() - m_iYOffSet );
	m_iPlayPosY = GetYPos();
}

void RetryModeWnd::iwm_command(ioWnd *pWnd, int cmd, DWORD param)
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();

	switch(cmd)
	{
	case IOBN_BTNUP:
		{
			switch(dwID)
			{
			case ID_RETRY_MODE_BTN:
				{
					if(m_pPlayMode)
						m_pPlayMode->RetryMode();
				}
				break;
			}
		}
		break;
	}
}

void RetryModeWnd::OnProcess( float fTimePerSec )
{
	if( g_Input.IsKeyPress( KEY_BACKSPACE ) )
	{
		if(m_pPlayMode)
			m_pPlayMode->RetryMode();
	}
}

RetryModeWnd::RetryModeWnd()
{
	m_pPlayMode = NULL;
}

RetryModeWnd::~RetryModeWnd()
{
	m_pPlayMode = NULL;
}
