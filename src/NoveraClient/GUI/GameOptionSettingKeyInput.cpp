#include "StdAfx.h"

#include "..\DirectInput.h"
#include "../TextColorTable.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderFrame.h"

#include "ioSP2GUIManager.h"

#include ".\gameoptionsettingkeyinput.h"
#include "ioMessageBox.h"
#include "..\io3DEngine\ioStringManager.h"


GameOptionSettingKeyInput::GameOptionSettingKeyInput()
{
	m_wPressKey   = KEY_NULL;
	m_pTextBoxFrm = NULL;
	m_pReturnWnd  = NULL;
}

GameOptionSettingKeyInput::~GameOptionSettingKeyInput()
{
	SAFEDELETE( m_pTextBoxFrm );
}

void GameOptionSettingKeyInput::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioWnd *pWnd = FindChildWnd( ID_USE );
	if( pWnd && pWnd->IsShow() ) return;

	// 조이패드 값은 받지 않는다
	for( WORD i=0; i<KEY_LSTICKLEFT; i++)
	{
		if( g_Input.IsKeyPress( i ) )
		{
			HideWnd();
			m_wPressKey = i;
			if( m_pReturnWnd )
				m_pReturnWnd->iwm_command( this, IOBN_BTNUP , i );
			break;
		}
	}
}

void GameOptionSettingKeyInput::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{	
	case ID_DONT_SETTING:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pReturnWnd )
				m_pReturnWnd->iwm_command( this, IOBN_BTNUP , ID_BTNUP_DONT_SETTING );
			HideWnd();
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pReturnWnd )
				m_pReturnWnd->iwm_command( this, IOBN_BTNUP , m_wPressKey );
			m_wPressKey = KEY_NULL;
			HideWnd();
		}
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

bool GameOptionSettingKeyInput::iwm_spacebar()
{
	ioWnd *pBtn = FindChildWnd( ID_USE );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

bool GameOptionSettingKeyInput::iwm_esc()
{
	ioWnd *pBtn = FindChildWnd( ID_CANCEL );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

void GameOptionSettingKeyInput::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		MAX_COMPLEXSTRING = 2,

		UP_X = 161,
		UP_Y = 53, 
		UP_Y_GAP = 20,

		BOX_X = 17,
		BOX_Y = 119,

		BOX_TEXT_X = 161,
		BOX_TEXT_Y = 126,
	};

	ioWnd *pWnd = FindChildWnd( ID_USE );
	if( pWnd && pWnd->IsShow() ) 
	{
		ioComplexStringPrinter kPrinter[MAX_COMPLEXSTRING];
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), m_sExistKeyTitle.c_str() );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );
		kPrinter[0].PrintFullText( iXPos + UP_X, iYPos + UP_Y, TAT_CENTER );

		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(3), m_sCurKeyTitle.c_str() );
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );
		kPrinter[1].PrintFullText( iXPos + UP_X, iYPos + UP_Y + UP_Y_GAP, TAT_CENTER );
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + UP_X, iYPos + UP_Y, FONT_SIZE_13, STR(5), m_sCurKeyTitle.c_str() );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + UP_X, iYPos + UP_Y + UP_Y_GAP, FONT_SIZE_13, STR(6) );

		if( m_pTextBoxFrm )
			m_pTextBoxFrm->Render( iXPos + BOX_X, iYPos + BOX_Y, UI_RENDER_MULTIPLY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + BOX_TEXT_X, iYPos + BOX_TEXT_Y, FONT_SIZE_13, STR(7) );
	}
}

void GameOptionSettingKeyInput::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "TextBoxFrm" )
		{
			SAFEDELETE( m_pTextBoxFrm );
			m_pTextBoxFrm = pFrame;

			enum { WIDTH = 288, HEIGHT = 31, };
			if( m_pTextBoxFrm )
				m_pTextBoxFrm->SetSize( WIDTH, HEIGHT );
		}
		else
		{	
			ioWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void GameOptionSettingKeyInput::iwm_create()
{
	enum { MAX_BTN = 2, };
	DWORD dwBtnArray[MAX_BTN]={ ID_USE, ID_CANCEL };

	for (int i = 0; i < MAX_BTN ; i++)
	{
		ioMessageBoxBtn *pBtn = dynamic_cast<ioMessageBoxBtn*> ( FindChildWnd( dwBtnArray[i] ) );
		if( pBtn )
			pBtn->AttachTitle();	
	}
}