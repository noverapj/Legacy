
#include "StdAfx.h"

#include "BingoRightWnd.h"
#include "BingoHelpToolTip.h"

BingoHelpToolTip::BingoHelpToolTip()
{
	m_dwParentID = 0;
}

BingoHelpToolTip::~BingoHelpToolTip()
{

}

void BingoHelpToolTip::iwm_show()
{
}

void BingoHelpToolTip::iwm_hide()
{
	m_dwParentID = 0;
}

void BingoHelpToolTip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	pMouse->SetToolTipCursor();
	POINT pt = pMouse->GetMousePos();
	int iXPos = pt.x + 8;
	int iYPos = pt.y + 8;
	
	int iXOffset = pt.x + GetWidth() + 8;
	if( Setting::Width() < iXOffset )
	{
		iXPos -= ( iXOffset - Setting::Width() ) + 8;
	}

	int iYOffset = pt.y + GetHeight() + 8;
	if( Setting::Height() < iYOffset )
	{
		iYPos -= ( iYOffset - Setting::Height() ) + 8;
	}

	SetScreenWndPos( iXPos, iYPos );
}

void BingoHelpToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_dwParentID )
	{
	case BingoRightWnd::ID_HELP_BTN_1:
		{
			int iWidth1 = 0;
			int iWidth2 = 0;

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( TCT_DEFAULT_DARKGRAY );	
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS, TAT_LEFT );
			iWidth1 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_12, STR(2) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP, TAT_LEFT );
			iWidth2 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			int iWidth = max( iWidth1, iWidth2 );
			SetSize( iWidth + SIDE_TEXT_GAP, WND_HEIGHT + TEXT_LINE_GAP );
		}
		break;
	case BingoRightWnd::ID_HELP_BTN_2:
		{
			int iWidth1 = 0;
			int iWidth2 = 0;

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( TCT_DEFAULT_DARKGRAY );	
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS, TAT_LEFT );
			iWidth1 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP, TAT_LEFT );
			iWidth2 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			int iWidth = max( iWidth1, iWidth2 );
			SetSize( iWidth + SIDE_TEXT_GAP, WND_HEIGHT + TEXT_LINE_GAP );
		}
		break;
	case BingoRightWnd::ID_HELP_BTN_3:
		{
			int iWidth1 = 0;
			int iWidth2 = 0;
			int iWidth3 = 0;

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( TCT_DEFAULT_DARKGRAY );	
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(5) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS, TAT_LEFT );
			iWidth1 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_12, STR(6) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP, TAT_LEFT );
			iWidth2 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_12, STR(7) );
			kPrinter.PrintFullText( iXPos + START_TEXT_XPOS, iYPos + START_TEXT_YPOS + TEXT_LINE_GAP * 2, TAT_LEFT );
			iWidth3 = static_cast<int>( kPrinter.GetFullWidth() );
			kPrinter.ClearList();

			int iWidth = max( iWidth1, iWidth2 );
			iWidth = max( iWidth, iWidth3 );
			SetSize( iWidth + SIDE_TEXT_GAP, WND_HEIGHT + TEXT_LINE_GAP * 2 );
		}
		break;
	}
}

void BingoHelpToolTip::SetInfo( DWORD dwParentID )
{
	if( !IsShow() )
		ShowWnd();

	m_dwParentID = dwParentID;
}