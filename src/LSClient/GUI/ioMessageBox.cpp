
#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include "../DirectInput.h"
#include "../ioKeyManager.h"
#include "../TextColorTable.h"
#include "../ioFishingManager.h"
#include "../NetworkWrappingFunc.h"

#include "../Setting.h"
#include "../WndID.h"

#include "ioSP2GUIManager.h"
#include "ioMessageBox.h"
#include "GameChatInput.h"
#include "FishingInventoryWnd.h"

#include <strsafe.h>


ioMessageBoxBtn::ioMessageBoxBtn()
{
}

ioMessageBoxBtn::~ioMessageBoxBtn()
{
}

void ioMessageBoxBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szTitleName  = xElement.GetStringAttribute_e( "TitleName" );
	m_szHotKey     = xElement.GetStringAttribute_e( "HotKey" );

	ioButton::ParseExtraInfo( xElement );
}

void ioMessageBoxBtn::SetTitleName( const char *szTitleName )
{
	m_szTitleName = szTitleName;
}

void ioMessageBoxBtn::SetHotKey( const char *szHotKey )
{
	m_szHotKey = szHotKey;
}

void ioMessageBoxBtn::AttachTitle()
{	
	ioUITitle *pTitle = GetTitle();
	if( pTitle )
	{
		char szTitle[MAX_PATH] = "";
		if( !m_szHotKey.IsEmpty() )
			sprintf_e( szTitle, "%s (%s)", m_szTitleName.c_str(), m_szHotKey.c_str() );
		else
			sprintf_e( szTitle, "%s" );
		SetTitleText( szTitle );
		pTitle->SetOffset( GetWidth() / 2, pTitle->GetTextYOffSet() );	
	}
}
//////////////////////////////////////////////////////////////////////////
ioMessageBox::ioMessageBox()
{
	m_iBoxType = MB_YESNO;
	m_pPrevWnd = NULL;
	m_pReturnWnd = NULL;
	m_pBottomFrm = NULL;
	m_pioString = NULL;
}

ioMessageBox::~ioMessageBox()
{
	SAFEDELETE( m_pBottomFrm );
}

void ioMessageBox::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "BottomFrm" )
		{
			SAFEDELETE( m_pBottomFrm );
			m_pBottomFrm = pFrame;
		}
		else
			ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioMessageBox::SetMessage( int iType, const char *szBuf, ioWnd *pPrevWnd, ioWnd *pReturnWnd )
{
	SAFEDELETE( m_pioString );

	m_pPrevWnd		= pPrevWnd;
	m_pReturnWnd = pReturnWnd;

	m_MessageList.clear();
	m_MessageList = ioStringConverter::Split( szBuf, "#" );

	int iMinWidth  = MIN_WIDTH;
	int iMinHeight = MIN_HEIGHT;

	StringVector::iterator iter;
	for( iter = m_MessageList.begin() ; iter!=m_MessageList.end() ; ++iter )
	{
		int iCurLineWidth = g_FontMgr.GetTextWidth( iter->c_str(), TS_NORMAL, FONT_SIZE_13  ) + SIDE_TEXT_GAP;
		if( iMinWidth < iCurLineWidth )
			iMinWidth = iCurLineWidth;
	}
	iMinHeight += ( TEXT_LINE_GAP * (m_MessageList.size() - 1) );

	SetSize( iMinWidth, iMinHeight );
	SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );

	m_iBoxType = iType;
	ReBatchButton( iType );

	if( !IsShow() )
		ShowWnd();
}

void ioMessageBox::SetIoStringMessage( int iType, IoString* pStr, ioWnd *pPrevWnd, ioWnd *pReturnWnd )
{
	if( IsShow() )
		HideWnd();

	int iMinWidth  = MIN_WIDTH;
	int iMinHeight = MIN_HEIGHT;

	m_pPrevWnd = pPrevWnd;
	m_pReturnWnd = pReturnWnd;
	m_MessageList.clear();

	SAFEDELETE( m_pioString );
	m_pioString = new IoString;
	m_pioString->Set(0, 0, pStr->GetText(), pStr->GetTextSize(), pStr->GetLineWidth(), pStr->GetStyle(),
					 pStr->GetTextColor(), pStr->GetBackColor(), TAT_CENTER, pStr->GetVartAlign(), pStr->GetLineGap());

	int iCurLineWidth = m_pioString->GetMaxLineWidth() + SIDE_TEXT_GAP;
	iMinWidth = max( iCurLineWidth, MIN_WIDTH );
	iMinHeight = max( m_pioString->GetMaxHeight() + 123, MIN_HEIGHT );

	m_iBoxType = iType;
	SetSize( iMinWidth, iMinHeight );

	SetWndPos( (g_App.GetCurWidth() - GetWidth() ) / 2, ( (g_App.GetCurHeight() - GetHeight() ) / 2 ) - 20 );
	ReBatchButton( iType );

	ShowWnd();
}

void ioMessageBox::ReBatchButton( int iType )
{
	if( iType == MB_OK )
	{
		ioMessageBoxBtn *pOkBtn = (ioMessageBoxBtn*)FindChildWnd( ID_OK );
		ioWnd *pXBtn = FindChildWnd( ID_X );
		if( pOkBtn && pXBtn )
		{
			pOkBtn->SetSize( GetWidth() - SIDE_TEXT_GAP, pOkBtn->GetHeight() );
			pOkBtn->SetWndPos( BTN_LEFT_XPOS, GetHeight() - BTN_BOTTOM_YPOS );
			pOkBtn->AttachTitle();

			pXBtn->SetWndPos( GetWidth() - XBTN_RIGHT_XPOS, pXBtn->GetYPos() );
		}

		ShowChildWnd( ID_OK );
		HideChildWnd( ID_YES );
		HideChildWnd( ID_NO );
		ShowChildWnd( ID_X );
	}
	else if( iType == MB_YESNO )
	{
		ioMessageBoxBtn *pYesBtn = (ioMessageBoxBtn*)FindChildWnd( ID_YES );
		ioMessageBoxBtn *pNoBtn  = (ioMessageBoxBtn*)FindChildWnd( ID_NO );
		ioWnd *pXBtn = FindChildWnd( ID_X );
		if( pYesBtn && pNoBtn && pXBtn )
		{
			pYesBtn->SetSize( ( GetWidth()/2 - 18 ), pYesBtn->GetHeight() );
			pYesBtn->SetWndPos( BTN_LEFT_XPOS, GetHeight() - BTN_BOTTOM_YPOS );
			pYesBtn->AttachTitle();
			
			pNoBtn->SetSize( ( GetWidth()/2 - 18 ), pNoBtn->GetHeight() );
			pNoBtn->SetWndPos( pYesBtn->GetWidth() + 19, GetHeight() - BTN_BOTTOM_YPOS );
			pNoBtn->AttachTitle();

			pXBtn->SetWndPos( GetWidth() - XBTN_RIGHT_XPOS, pXBtn->GetYPos() );
		}

		HideChildWnd( ID_OK );
		ShowChildWnd( ID_YES );
		ShowChildWnd( ID_NO );
		ShowChildWnd( ID_X );
	}
}

void ioMessageBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_YES:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDYES );
			}
		}
		break;
	case ID_NO:
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDNO );
			}
		}
		break;
	}
}

void ioMessageBox::iwm_hide()
{
	SAFEDELETE( m_pioString );

	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();
}

bool ioMessageBox::iwm_esc()
{
	ioMessageBoxBtn *pXBtn = (ioMessageBoxBtn*)FindChildWnd( ID_X );
	if( !pXBtn ) return false;
	if( !pXBtn->IsShow() ) return false;

	iwm_command( pXBtn, IOBN_BTNUP, 0 );
	return true;
}

bool ioMessageBox::iwm_spacebar()
{
	ioMessageBoxBtn *pYesBtn = (ioMessageBoxBtn*)FindChildWnd( ID_YES );
	if( !pYesBtn ) return false;
	if( !pYesBtn->IsShow() ) return false;

	iwm_command( pYesBtn, IOBN_BTNUP, 0 );
	return true;
}

void ioMessageBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY() + 53;

	if( m_pioString )
	{
		m_pioString->OnRender( iXPos, iYPos );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetAlignType( TAT_CENTER );

		int iLineCnt = m_MessageList.size();
		for( int i=0 ; i<iLineCnt ; i++ )
		{
			if( !m_MessageList[i].empty() )
			{
				g_FontMgr.PrintText( iXPos, iYPos + TEXT_LINE_GAP * i, FONT_SIZE_13, m_MessageList[i].c_str() );
			}
		}	
	}
	if( m_pBottomFrm )
	{
		m_pBottomFrm->SetSize( GetWidth(), BOTTOM_MULTIPLY_H );
		m_pBottomFrm->Render( GetDerivedPosX(), GetDerivedPosY() + ( GetHeight() - BOTTOM_MULTIPLY_H ), UI_RENDER_MULTIPLY );
	}
}

void ioMessageBox::SetBtnTitle( const char *szOKTitle, const char *szYesTitle, const char *szNoTitle )
{
	ioMessageBoxBtn *pBtn = dynamic_cast< ioMessageBoxBtn* > ( FindChildWnd( ID_OK ) );
	if( pBtn )
	{
		pBtn->SetTitleName( szOKTitle );
		pBtn->AttachTitle();
	}

	pBtn = dynamic_cast< ioMessageBoxBtn* > ( FindChildWnd( ID_YES ) );
	if( pBtn )
	{
		pBtn->SetTitleName( szYesTitle );
		pBtn->AttachTitle();
	}

	pBtn = dynamic_cast< ioMessageBoxBtn* > ( FindChildWnd( ID_NO ) );
	if( pBtn )
	{
		pBtn->SetTitleName( szNoTitle );
		pBtn->AttachTitle();
	}
}
//////////////////////////////////////////////////////////////////////////
ioMessageBox3::ioMessageBox3()
{
	m_AlignType = TAT_CENTER;
}

ioMessageBox3::~ioMessageBox3()
{
	ClearList();
}

void ioMessageBox3::ClearList()
{
	for( int i=0; i<MAX_MSG_LIST_SIZE; i++ )
	{
		m_PrinterList[i].ClearList();
	}
}

void ioMessageBox3::SetPrintMessage( int iType, ioComplexStringPrinter *MsgPrinter, ioWnd *pPrevWnd, ioWnd *pReturnWnd )
{
	m_pPrevWnd = pPrevWnd;
	m_pReturnWnd = pReturnWnd;

	ClearList();
	int i = 0;
	for(i = 0;i < MAX_MSG_LIST_SIZE;i++)
	{
		if( MsgPrinter[i].IsEmpty() )
			break;
		
		MsgPrinter[i].CopyFullText( m_PrinterList[i] );
	}	

	int iMinWidth  = MIN_WIDTH;
	int iMinHeight = MIN_HEIGHT;
	int iMaxLine   = 0;
	for(i = 0;i < MAX_MSG_LIST_SIZE;i++)
	{
		if( m_PrinterList[i].IsEmpty() )
			break;
		
		int iCurLineWidth = m_PrinterList[i].GetFullWidth() + SIDE_TEXT_GAP;
		if( iMinWidth < iCurLineWidth )
			iMinWidth = iCurLineWidth;
		iMaxLine++;
	}
	iMinHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMinWidth, iMinHeight );
	SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );

	m_iBoxType = iType;
	ReBatchButton( iType );

	if( !IsShow() )
		ShowWnd();
}

void ioMessageBox3::OnRender()
{
	ioWnd::OnRender();

	int iXPos = 0; 
	
	if( m_AlignType == TAT_CENTER )
		iXPos = GetDerivedPosX() + GetWidth() / 2;
	else
		iXPos = GetDerivedPosX() + SIDE_TEXT_GAP / 2;

	int iYPos = GetDerivedPosY() + 53;

	for( int i=0 ; i<MAX_MSG_LIST_SIZE ; i++ )
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		m_PrinterList[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP * i, m_AlignType );
	}

	if( m_pBottomFrm )
	{
		m_pBottomFrm->SetSize( GetWidth(), BOTTOM_MULTIPLY_H );
		m_pBottomFrm->Render( GetDerivedPosX(), GetDerivedPosY() + ( GetHeight() - BOTTOM_MULTIPLY_H ), UI_RENDER_MULTIPLY );
	}
}

//////////////////////////////////////////////////////////////////////////
