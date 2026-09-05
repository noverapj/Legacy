

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "TutorialWnd.h"

#include "../TextColorTable.h"
#include "../ioComplexStringPrinter.h"
#include "../ioMyInfo.h"
#include "../WndID.h"
#include "../DirectInput.h"
#include "../ioPlayStage.h"
#include "../ioBattleRoomMgr.h"
#include "../ioQuestManager.h"

#include "ioSP2GUIManager.h"
#include "SoldierManualWnd.h"
#include "PartyMenuWnd.h"
#include "ioTutorialManager.h"
#include "HelpWnd.h"

TutorialBtn::TutorialBtn()
{
	m_dwScreenTimer = 0;
	m_dwCurTimer    = 0;
	m_bScreen       = false;
}

TutorialBtn::~TutorialBtn()
{

}

void TutorialBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwScreenTimer = xElement.GetIntAttribute_e( "ScreenTime" );
	m_dwCurTimer    = FRAMEGETTIME();
}

void TutorialBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwCurTimer > m_dwScreenTimer )
	{
		m_dwCurTimer = FRAMEGETTIME();
		m_bScreen = !m_bScreen;
	}
}

void TutorialBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen )
		ioButton::OnDrawOvered( iXPos, iYPos );
	else
		ioButton::OnDrawNormal( iXPos, iYPos );
}

void TutorialBtn::OnRender()
{
//	if( !g_GUIMgr.IsSkipUIByBlind() )
	{
		ioButton::OnRender();
	}
}

//////////////////////////////////////////////////////////////////////////
TutorialWnd::TutorialWnd()
{
	m_pHelpBackFrm= NULL;
	m_pDevK		  = NULL;
	m_pDevKBack	  = NULL;

	m_iCurStep	  = 0;
	m_iMaxStep    = 0;
	m_iCurValue   = 0;
	m_iMaxValue	  = 0;
	m_fScale	  = FLOAT1;
	m_fFontYGap   = 0.0f;
	m_dwCurTimer  = 0;
	m_dwTextTickTime = 0;	
	m_dwCurTextTickTime = FRAMEGETTIME();
	m_iCurTextTickCount = 0;
	m_iCurTextLineCount = 0;
	m_iShowBtnID		= -1;
	m_dwSkipBtnShowTime    = 0;
	m_dwCurSkipBtnShowTime = 0;
	m_dwAutoNextTime       = 0;
	m_dwCurAutoNextTime    = 0;
	m_iCurUseSkipBtnID     = 0;
}

TutorialWnd::~TutorialWnd()
{
	SAFEDELETE( m_pHelpBackFrm );
	SAFEDELETE( m_pDevK );
	SAFEDELETE( m_pDevKBack );
}

void TutorialWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwTextTickTime = xElement.GetIntAttribute_e( "TextTickTime" );
	m_szTextTickSnd  = xElement.GetStringAttribute_e( "TextTickSnd" );
	g_TutorialMgr.SetSound( xElement.GetStringAttribute_e( "CountUpSnd" ), xElement.GetStringAttribute_e( "StartSnd" ),
							xElement.GetStringAttribute_e( "NextSnd" ), xElement.GetStringAttribute_e( "ClearSnd" ) );

	m_dwSkipBtnShowTime = xElement.GetIntAttribute_e( "SkipBtnShowTime" );
	m_dwAutoNextTime    = xElement.GetIntAttribute_e( "AutoNextTime" );
}

void TutorialWnd::ShowButtonID( int iBtnID )
{
	m_iShowBtnID = -1;
	HideChildWnd( ID_NEXT );
	HideChildWnd( ID_CLEAR );
	HideChildWnd( ID_START );
	if( IsTextTick() )
		m_iShowBtnID = iBtnID;
	else if( iBtnID != -1 )
	{
		ShowChildWnd( iBtnID );
		m_dwCurAutoNextTime    = FRAMEGETTIME();
	}
}

void TutorialWnd::SetTutorialStep( int iCurStep, int iCurValue, int iMaxStep, int iMaxValue,
								   ioHashString &szTitle, ioHashString &szHelp1, ioHashString &szHelp2, ioHashString &szComplete )
{
	if( iCurValue != 0 )
		SetCountUp();
	else
	{
		SetSkipBtn( iCurStep, iMaxStep );
		m_dwCurAutoNextTime = 0;
	}

	m_iCurStep  = iCurStep;
	m_iCurValue = iCurValue;
	m_iMaxStep  = iMaxStep;	
	m_iMaxValue = iMaxValue;

	SetTitleText( szTitle.c_str() );

	if( m_iCurValue == m_iMaxValue )
	{
		if( m_iCurValue == 0 )
		{
			SplitDescription( szComplete );
			SetTextTick();
			m_szHelp.Clear();

			if( m_iCurStep == 0 )
			{
				ShowButtonID( ID_START );
			}
			else if( m_iCurStep == m_iMaxStep - 1 )
			{
				ShowButtonID( ID_CLEAR );
			}
			else
				ShowButtonID( ID_NEXT );
		}		
		else
		{
			if( m_iCurStep == m_iMaxStep - 1 )
			{
				ShowButtonID( ID_CLEAR );
			}
			else
			{
				ShowButtonID( ID_NEXT );
			}						
			SplitDescription( szHelp1 );
			m_szHelp = szHelp2;
		}
	}
	else
	{
		ShowButtonID( -1 );
		SplitDescription( szHelp1 );
		m_szHelp = szHelp2;

		if( m_iCurValue == 0 )
			SetTextTick();
	}
}

void TutorialWnd::SplitDescription( const ioHashString &szDesc )
{
	m_DescList.clear();
	m_DescList = ioStringConverter::Split( szDesc.c_str(), "#" );
}

bool TutorialWnd::IsTextTick()
{
	if( m_dwCurTextTickTime == 0 )
		return false;

	return true;
}

void TutorialWnd::SetTextTick()
{
	if( m_DescList.empty() ) return;

	m_vStringTickList.clear();

	m_dwCurTextTickTime = FRAMEGETTIME();
	m_iCurTextTickCount = 0;
	m_iCurTextLineCount = 0;
	int iSize = m_DescList.size();
	for(int i = 0;i < iSize;i++)
	{
		StringTick kTick;
		m_vStringTickList.push_back( kTick );
	}
}

void TutorialWnd::ProcessTextTick()
{
	if( !IsTextTick() ) return;
	if( !COMPARE( m_iCurTextLineCount, 0, (int)m_DescList.size() ) ) return;
	if( FRAMEGETTIME() - m_dwCurTextTickTime < m_dwTextTickTime ) return;

	m_dwCurTextTickTime = FRAMEGETTIME();

	if( !m_szTextTickSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szTextTickSnd );

	char szHelp[MAX_PATH] = "";
	memset( szHelp, 0, sizeof( szHelp ) );
	sprintf( szHelp, "%s", m_DescList[m_iCurTextLineCount].c_str() );

	int iMaxLen = strlen( szHelp );

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
	if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szHelp[m_iCurTextTickCount] ) )
#else
	if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szHelp[m_iCurTextTickCount] ) )
#endif

#else
	if( IsDBCSLeadByte ( (BYTE)szHelp[m_iCurTextTickCount] ) )
#endif

		m_iCurTextTickCount+=2;
	else 
		m_iCurTextTickCount+=1;

	// Exception Check
	if( (m_iCurTextLineCount >= (int)m_vStringTickList.size()) ||
		(m_iCurTextTickCount >= MAX_PATH) )
	{
		m_dwCurTextTickTime = 0;
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount = 0;
		ShowButtonID( m_iShowBtnID );
		g_TutorialMgr.ActionTextTickEnd();
		m_dwCurSkipBtnShowTime = FRAMEGETTIME();
		return;
	}

	memcpy( m_vStringTickList[m_iCurTextLineCount].m_szCopyDesc, szHelp, m_iCurTextTickCount );

	if( m_iCurTextTickCount >= iMaxLen )
	{
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount++;
	}

	if( m_iCurTextLineCount >= (int)m_DescList.size() )
	{
		m_dwCurTextTickTime = 0;
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount = 0;
		ShowButtonID( m_iShowBtnID );
		g_TutorialMgr.ActionTextTickEnd();
		m_dwCurSkipBtnShowTime = FRAMEGETTIME();
	}
}

void TutorialWnd::SetCountUp()
{
	m_fFontYGap		= -20.0f;
	m_fScale		= 3.0f;
	m_dwCurTimer	= FRAMEGETTIME();
}

void TutorialWnd::iwm_show()
{	
	g_GUIMgr.ShowWnd( TUTORIAL_KEYBOARD_WND );
}

void TutorialWnd::iwm_hide()
{	
	
}

bool TutorialWnd::iwm_spacebar()
{
	return IsTutorialEsc();
}

void TutorialWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(1) );
		}
		break;
	case ID_START:
		if( cmd == IOBN_BTNUP )
		{
			g_TutorialMgr.OnStart();
		}
		break;
	case ID_NEXT:
		if( cmd == IOBN_BTNUP )
		{
			g_TutorialMgr.OnNext();
		}
		break;
	case ID_CLEAR:
		if( cmd == IOBN_BTNUP )
		{
			g_TutorialMgr.OnClear();
			HideWnd();
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if(  m_iCurUseSkipBtnID == ID_START )
					g_TutorialMgr.OnAllSkip();
				else if( m_iCurUseSkipBtnID == ID_NEXT )
					g_TutorialMgr.OnAllSkip();
				else if( m_iCurUseSkipBtnID == ID_CLEAR )
				{
					g_TutorialMgr.OnClear();
					HideWnd();
				}
			}
		}
		break;
	}
}

void TutorialWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "HelpBackFrm" )
	{
		SAFEDELETE( m_pHelpBackFrm );
		m_pHelpBackFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void TutorialWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevK" )
	{
		SAFEDELETE( m_pDevK );
		m_pDevK = pImage;
	}
	else if( szType == "DevKBack" )
	{
		SAFEDELETE( m_pDevKBack );
		m_pDevKBack = pImage;
	}
	else
        ioWnd::AddRenderImage( szType, pImage );
}

void TutorialWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	
	{
		if( g_MyInfo.GetActiveCharCount() == 0 ) return;
		if( !g_TutorialMgr.IsTutorial() ) return;
		
		ShowWnd();
		return;
	}

	if( m_dwCurTimer != 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwCurTimer;
		if( dwGap < 150 )
		{
			float fGap = (float) dwGap / 150;
			m_fScale    = 3.0f - ( 2.0f * fGap );
			m_fFontYGap = 20 - ( 20.0f * fGap );
		}
		else if( dwGap < 2000 )
		{
			m_fScale     = FLOAT1;
			m_fFontYGap  = 0.0f;
		}
		else
			m_dwCurTimer = 0;
	}

	if( g_GUIMgr.IsSkipUIByBlind() )
	{
		HideChildWnd( ID_EXIT );
	}
	else
	{
		ShowChildWnd( ID_EXIT );
	}

	if( m_dwCurSkipBtnShowTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwCurSkipBtnShowTime > m_dwSkipBtnShowTime )
		{
			SetChildActive( ID_EXIT );
			m_dwCurSkipBtnShowTime = 0;
		}
	}

	ProcessTextTick();

	// auto next
	if( m_dwCurAutoNextTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwCurAutoNextTime > m_dwAutoNextTime )
		{
			IsTutorialEsc();
			m_dwCurAutoNextTime = 0;
		}
	}

}

bool TutorialWnd::IsTutorialEsc()
{
	if( !IsShow() ) return false;
	if( IsTextTick() ) return false;

	ioWnd *pButton = FindChildWnd( ID_START );
	if( pButton && pButton->IsShow() )
	{
		g_TutorialMgr.OnStart();
		return true;
	}

	pButton = FindChildWnd( ID_NEXT );
	if( pButton && pButton->IsShow() )
	{
		g_TutorialMgr.OnNext();
		return true;
	}

	pButton = FindChildWnd( ID_CLEAR );
	if( pButton && pButton->IsShow() )
	{
		g_TutorialMgr.OnClear();
		HideWnd(); 
		return true;
	}
	
	return false;
}

void TutorialWnd::OnRender()
{
//	if( g_GUIMgr.IsSkipUIByBlind() ) return;

	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	if( m_pDevKBack && m_pDevK )
	{
		m_pDevKBack->SetScale( 0.85f );
		m_pDevKBack->Render( iXPos + 40, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		
		m_pDevK->SetScale( 0.85f );
		m_pDevK->Render( iXPos + 40, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	RenderTutorialInfo();
}

void TutorialWnd::RenderTutorialInfo()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	//////////////////////////////////////////////////////////////////////////	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	int i = 0;
	int iInfoSize = m_vStringTickList.size();      
	for(;i < iInfoSize;i++)
		g_FontMgr.PrintText( iXPos + 77, iYPos + 47 + ( 17 * i ), FONT_SIZE_12, m_vStringTickList[i].m_szCopyDesc );

	enum { MAX_NEXT_BTN = 3 };
	for (int i = 0; i < MAX_NEXT_BTN ; i++)
	{
		ioWnd *pButton = FindChildWnd( ID_START + i );
		if( pButton && pButton->IsShow() )
			return;	
	}

	if( m_pHelpBackFrm )
		m_pHelpBackFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	if( !m_szHelp.IsEmpty() )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), m_szHelp.c_str() );
		kPrinter.AddTextPieceWithYOffset( FONT_SIZE_13 * m_fScale, 7 - ((14 * m_fScale)/2), STR(1), m_iCurValue );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), m_iMaxValue );	
		kPrinter.PrintFullText( iXPos + 188, iYPos + 117, TAT_CENTER );
	}
}

void TutorialWnd::SetSkipBtn( int iCurStep, int iMaxStep )
{
	SetChildInActive( ID_EXIT );
	m_dwCurSkipBtnShowTime = 0;
	
	if( iCurStep == 0 )
		m_iCurUseSkipBtnID = ID_START;
	else if( iCurStep == iMaxStep - 1 )
		m_iCurUseSkipBtnID = ID_CLEAR;
	else
		m_iCurUseSkipBtnID = ID_NEXT;
}

void TutorialWnd::SetBattleProcess()
{
	// 전투방 생성으로 변경
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	PartyCreateWnd *pPartyCreate = dynamic_cast<PartyCreateWnd*>( g_GUIMgr.FindWnd( PARTY_CREATE_WND ) );
	if( pPartyCreate )
	{
		pPartyCreate->CreateBattleRoom( BMT_MONSTER_SURVIVAL_EASY, g_BattleRoomMgr.GetDefMaxPlayer() / 2, g_BattleRoomMgr.GetDefMaxPlayer() / 2 );
	}
}
//////////////////////////////////////////////////////////////////////////
TutorialKeyboardWnd::TutorialKeyboardWnd()
{
	m_pButtonNormal = NULL;
	m_pButtonOver	= NULL;
	m_pArrowLeft	= NULL;
	m_pArrowUp		= NULL;

	m_bShowProcess  = false;
	m_fProcessRate  = 0.0f;
	m_dwProcessTimer= 0;

	m_bKeyA = m_bKeyS = m_bKeyD = m_bKeyLeft = false;
	m_bKeyRight = m_bKeyUp = m_bKeyDown = false;
}

TutorialKeyboardWnd::~TutorialKeyboardWnd()
{
	SAFEDELETE( m_pButtonNormal );
	SAFEDELETE( m_pButtonOver );
	SAFEDELETE( m_pArrowLeft );
	SAFEDELETE( m_pArrowUp );
}

void TutorialKeyboardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ButtonNormal" )
	{
		SAFEDELETE( m_pButtonNormal );
		m_pButtonNormal = pImage;
	}
	else if( szType == "ButtonOver" )
	{
		SAFEDELETE( m_pButtonOver );
		m_pButtonOver = pImage;
	}
	else if( szType == "ArrowLeft" )
	{
		SAFEDELETE( m_pArrowLeft );
		m_pArrowLeft = pImage;
	}
	else if( szType == "ArrowUp" )
	{
		SAFEDELETE( m_pArrowUp );
		m_pArrowUp = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TutorialKeyboardWnd::iwm_show()
{
	m_bShowProcess  = true;
	m_fProcessRate  = 0.0f;
	m_dwProcessTimer= FRAMEGETTIME();

	m_bKeyA = m_bKeyS = m_bKeyD = m_bKeyLeft = false;
	m_bKeyRight = m_bKeyUp = m_bKeyDown = false;
}

void TutorialKeyboardWnd::iwm_hide()
{

}

void TutorialKeyboardWnd::SetHideProcess()
{
	m_bShowProcess  = false;
	m_fProcessRate  = FLOAT1;
	m_dwProcessTimer= FRAMEGETTIME();
}

void TutorialKeyboardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	OnProcessKeyCheck();
	OnProcessShow();
}

void TutorialKeyboardWnd::OnProcessKeyCheck()
{
	m_bKeyA = m_bKeyS = m_bKeyD = m_bKeyLeft = false;
	m_bKeyRight = m_bKeyUp = m_bKeyDown = false;

	// A
	if( GetAsyncKeyState( 'A' ) & 0x8000 )
	{
		m_bKeyA = true;
	}

	// S
	if( GetAsyncKeyState( 'S' ) & 0x8000 )
	{
		m_bKeyS = true;
	}

	// D
	if( GetAsyncKeyState( 'D' ) & 0x8000 )
	{
		m_bKeyD = true;
	}

	// UP
	if( GetAsyncKeyState( VK_UP ) & 0x8000 )
	{
		m_bKeyUp = true;
	}

	// LEFT
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 )
	{
		m_bKeyLeft = true;
	}

	// DOWN
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 )
	{
		m_bKeyDown = true;
	}

	// RIGHT
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 )
	{
		m_bKeyRight = true;
	}
}

void TutorialKeyboardWnd::OnProcessShow()
{
	if( m_dwProcessTimer == 0 )
		return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwProcessTimer;
	if( m_bShowProcess )
	{
		if( dwGapTime >= 1000 )
		{
			m_fProcessRate	= FLOAT1;
			m_dwProcessTimer= 0;
		}
		else
		{
			m_fProcessRate  = sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / 1000 );
		}
	}
	else
	{
		if( dwGapTime >= 1000 )
		{
			HideWnd();
		}
		else
		{
			m_fProcessRate  = FLOAT1 - sinf( ( D3DX_PI / 2 ) * (float)( dwGapTime ) / 1000 );
		}
	}
}

void TutorialKeyboardWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderASD( ( iXPos - 375 ) - ( 400 * ( FLOAT1 - m_fProcessRate ) ), iYPos - 49 );
	OnRenderArrow( ( iXPos + 228 ) + ( 400 * ( FLOAT1 - m_fProcessRate ) ), iYPos - 49 );
}

void TutorialKeyboardWnd::OnRenderASD( int iXPos, int iYPos )
{
	if( m_pButtonNormal )
	{
		m_pButtonNormal->Render( iXPos + 2, iYPos + 36 );
		m_pButtonNormal->Render( iXPos + 50, iYPos + 36 );
		m_pButtonNormal->Render( iXPos + 98, iYPos + 36 );
	}

	if( m_pButtonOver )
	{
		if( m_bKeyA )
			m_pButtonOver->Render( iXPos + 2, iYPos + 36, UI_RENDER_SCREEN );
		if( m_bKeyS )
			m_pButtonOver->Render( iXPos + 50, iYPos + 36, UI_RENDER_SCREEN );
		if( m_bKeyD )
			m_pButtonOver->Render( iXPos + 98, iYPos + 36, UI_RENDER_SCREEN );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 34, 34, 34 );
	g_FontMgr.SetTextColor( 204, 204, 204 );
	g_FontMgr.PrintText( iXPos + 25, iYPos + 18, FONT_SIZE_13, "점프" );
	g_FontMgr.PrintText( iXPos + 73, iYPos + 18, FONT_SIZE_13, "방어" );
	g_FontMgr.PrintText( iXPos + 121, iYPos + 18, FONT_SIZE_13, "공격" );

	g_FontMgr.PrintText( iXPos + 26, iYPos + 46, FONT_SIZE_19, "A" );
	g_FontMgr.PrintText( iXPos + 75, iYPos + 46, FONT_SIZE_19, "S" );
	g_FontMgr.PrintText( iXPos + 124, iYPos + 46, FONT_SIZE_19, "D" );
}

void TutorialKeyboardWnd::OnRenderArrow( int iXPos, int iYPos )
{
	if( m_pButtonNormal )
	{
		m_pButtonNormal->Render( iXPos + 50, iYPos + 2 );
		m_pButtonNormal->Render( iXPos + 50, iYPos + 50 );
		m_pButtonNormal->Render( iXPos + 2, iYPos + 50 );
		m_pButtonNormal->Render( iXPos + 98, iYPos + 50 );
	}

	if( m_pButtonOver )
	{
		if( m_bKeyUp )
			m_pButtonOver->Render( iXPos + 50, iYPos + 2, UI_RENDER_SCREEN );
		if( m_bKeyDown )
			m_pButtonOver->Render( iXPos + 50, iYPos + 50, UI_RENDER_SCREEN );
		if( m_bKeyLeft )
			m_pButtonOver->Render( iXPos + 2, iYPos + 50, UI_RENDER_SCREEN );
		if( m_bKeyRight )
			m_pButtonOver->Render( iXPos + 98, iYPos + 50, UI_RENDER_SCREEN );
	}

	if( m_pArrowUp )
	{
		m_pArrowUp->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pArrowUp->Render( iXPos + 68, iYPos + 13 );

		m_pArrowUp->SetReverseFormatting( ioUIRenderImage::RF_VERT_REVERSE );
		m_pArrowUp->Render( iXPos + 68, iYPos + 61 );
	}

	if( m_pArrowLeft )
	{
		m_pArrowLeft->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pArrowLeft->Render( iXPos + 14, iYPos + 67 );

		m_pArrowLeft->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pArrowLeft->Render( iXPos + 110, iYPos + 67 );
	}
}