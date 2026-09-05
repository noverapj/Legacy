#include <StdAfx.h>

#include "CountUpWnd.h"
#include "StartingSignalWnd.h"
#include "../GameStage/Mode/Practice/IoPracticeManager.h"

CountUpWnd::CountUpWnd()
{

	m_pTextLine		= NULL;
	m_pNum_Blue		= NULL;
	m_pNum_Gray		= NULL;

	m_pColon_Blue = NULL;
	m_pColon_Gray = NULL;
	m_dwCurrentTime = 0;
	m_iRank = -1;

	m_bCountDown = false;

	m_bShow = true;
}

CountUpWnd::~CountUpWnd()
{
	SAFEDELETE( m_pNum_Blue );
	SAFEDELETE( m_pNum_Gray );
	SAFEDELETE( m_pColon_Blue );
	SAFEDELETE( m_pColon_Gray );
	SAFEDELETE( m_pTextLine );
}

void CountUpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{	
	if( szType == "CountNum_Blue" )
	{
		SAFEDELETE( m_pNum_Blue );
		m_pNum_Blue = pImage;
	}
	else if( szType == "CountNum_Gray" )
	{
		SAFEDELETE( m_pNum_Gray );
		m_pNum_Gray = pImage;
	}
	else if( szType == "BlueColon" )
	{
		SAFEDELETE( m_pColon_Blue );
		m_pColon_Blue = pImage;
	}
	else if( szType == "GrayColon" )
	{
		SAFEDELETE( m_pColon_Gray );
		m_pColon_Gray = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void CountUpWnd::OnRender()
{
	StartingSignalWnd *pSignalWnd = static_cast<StartingSignalWnd *>( g_GUIMgr.FindWnd( STARTING_SIGNAL_WND ) );
	if(pSignalWnd->IsShow())
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	DWORD dwMinute = ( m_dwCurrentTime / 1000 ) / 60;
	DWORD dwSecond = ( m_dwCurrentTime / 1000 ) % 60;
	DWORD dwCentisecond = (m_dwCurrentTime % 1000) / 10;


	if(m_pNum_Blue)
	{
		m_pNum_Blue->RenderNum( iXPos + 35, iYPos, dwMinute / 10);
		m_pNum_Blue->RenderNum( iXPos + 63, iYPos, dwMinute % 10);
		m_pColon_Blue->Render( iXPos + 86, iYPos );
		m_pNum_Blue->RenderNum( iXPos + 110 , iYPos, dwSecond/10);
		m_pNum_Blue->RenderNum( iXPos + 138 , iYPos, dwSecond%10);
		m_pColon_Gray->Render( iXPos + 160, iYPos );
		m_pNum_Gray->RenderNum( iXPos + 182, iYPos, dwCentisecond/10);
		m_pNum_Gray->RenderNum( iXPos + 210, iYPos, dwCentisecond%10);
	}
	
	m_ProgressText.OnRender( iXPos, iYPos + 60 );


}

void CountUpWnd::OnRenderRank( int iXPos, int iYPos )
{
	if( m_iRank == -1 )
		return;

	char szRank[128] = {0,};
	{
		ioComplexStringPrinter kPrinter;	
		kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );

		kPrinter.SetBkColor( TCT_DEFAULT_BLACK );

		kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );
		kPrinter.AddTextPiece( FONT_SIZE_13, m_strRank[3].c_str() );

		kPrinter.PrintFullText( iXPos+ 142, iYPos + 71, TAT_CENTER );
		return;
	}
	char szRecord[32];
	memset(szRecord, 0, sizeof(szRecord));


	sprintf(szRecord, "99:99.99");
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );

	kPrinter.SetBkColor( TCT_DEFAULT_BLACK );

	kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );
	kPrinter.AddTextPiece( FONT_SIZE_13, szRank );

	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );//silver
	kPrinter.AddTextPiece( FONT_SIZE_13, m_strRank[2].c_str() );

	kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );
	kPrinter.AddTextPiece( FONT_SIZE_13, szRecord );

	kPrinter.PrintFullText( iXPos+ 142, iYPos + 71, TAT_CENTER );
}

void CountUpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;
}

void CountUpWnd::iwm_create()
{
	m_iX = GetXPos();
	m_iY = GetYPos();

	if( COMPARE( m_iX, 0, g_App.GetCurWidth() ) && 
		COMPARE( m_iY, 0, g_App.GetCurHeight() ) )
	{
		SetWndPos( m_iX, m_iY );
	}
}

void CountUpWnd::UpdateTime( DWORD dwTime ) 
{ 
	m_dwCurrentTime = dwTime;

	return;
}

void CountUpWnd::UpdateProgress_Practice( int iProgress, int iMaxProgress )
{
	IoUIText* pText = GetText( "ProgressTitle" );

	if( pText )
	{
		if( iProgress == -1 || iMaxProgress == -1 )
		{
			pText->SetText( g_PracticeMgr.GetPracticeTypeDesc().c_str() );
		}
		else
		{
			char szText[MAX_PATH] = {0,};
			sprintf_s( szText, "%s (%d/%d)", g_PracticeMgr.GetPracticeTypeDesc().c_str(), iProgress, iMaxProgress );

			pText->SetText( szText );
		}
		pText->SetSize(13);
		pText->SetStyle(TS_OUTLINE_2X);
		pText->SetAllColor( TCT_DEFAULT_WHITE, TCT_DEFAULT_BLACK );
		m_ProgressText.SetByUIText(pText, 0, 0 );
	}
}