

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/Safesprintf.h"

#include "../TextColorTable.h"

#include "../Setting.h"
#include "../WndID.h"
#include "../ioMyInfo.h"

#include "ioSP2GUIManager.h"

#include "ioMessageBox.h"
#include "ioExMedalSlotReleaseWnd.h"

#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////
ioExMedalSlotReleaseWnd::ioExMedalSlotReleaseWnd()
{
	m_pIconBack		 = NULL;
	m_pExtraItemIcon = NULL;
	
	m_dwHideDelayTime = 0;
}

ioExMedalSlotReleaseWnd::~ioExMedalSlotReleaseWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pExtraItemIcon );
}

void ioExMedalSlotReleaseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioExMedalSlotReleaseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_MyInfo.IsReserveDeleteExMedalSlot() )
		{
			ShowWnd();
		}
		return;
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioExMedalSlotReleaseWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	if( m_pExtraItemIcon )
	{
		m_pExtraItemIcon->Render( iXPos + 103, iYPos + 98 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, STR(1), m_szClassName.c_str() ); 
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, STR(2), m_iSlotLevel );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_13, STR(3) );
}

void ioExMedalSlotReleaseWnd::iwm_show()
{
	SAFEDELETE( m_pExtraItemIcon );
	m_szClassName.Clear();
	m_szIconName.Clear();
	m_szSubIconName.Clear();
	m_iSlotLevel = 0;

	g_MyInfo.GetFirstDeleteExMedalSlot( m_szIconName, m_szSubIconName, m_szClassName, m_iSlotLevel );
	m_pExtraItemIcon = g_UIImageSetMgr.CreateImageByFullName( m_szIconName );
}

void ioExMedalSlotReleaseWnd::iwm_hide()
{
	m_szClassName.Clear();
	m_szIconName.Clear();
	m_iSlotLevel = 0;

	m_dwHideDelayTime = FRAMEGETTIME();
}

bool ioExMedalSlotReleaseWnd::iwm_esc()
{
	if( !IsShow() ) return false;
	if( ioWnd::iwm_esc() )
	{
		g_MyInfo.EraseDeleteExMedalSlot( false );
		return true;
	}
	return false;
}

void ioExMedalSlotReleaseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ALL_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteExMedalSlot( true );
			HideWnd();
		}
		break;
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteExMedalSlot( false );
			HideWnd();
		}
		break;
	}
}