

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
#include "ioExtraItemReleaseWnd.h"

#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////
ioExtraItemReleaseWnd::ioExtraItemReleaseWnd()
{
	m_pIconBack		 = NULL;
	m_pExtraItemIcon = NULL;
	m_pExtraItemNum  = NULL;
	m_pExtraItemPlus = NULL;
	
	m_iReinforce = 0;
	m_dwHideDelayTime = 0;
	m_nGradeType = 0;
}

ioExtraItemReleaseWnd::~ioExtraItemReleaseWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pExtraItemIcon );
	SAFEDELETE( m_pExtraItemNum );
	SAFEDELETE( m_pExtraItemPlus );
}

void ioExtraItemReleaseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "ExtraItemNum" )
	{
		SAFEDELETE( m_pExtraItemNum );
		m_pExtraItemNum = pImage;
	}
	else if( szType == "ExtraItemPlus" )
	{
		SAFEDELETE( m_pExtraItemPlus );
		m_pExtraItemPlus = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioExtraItemReleaseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_MyInfo.IsReserveDeleteExtraItem() )
		{
			ShowWnd();
		}
		return;
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioExtraItemReleaseWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

	if( m_pExtraItemIcon )
	{
		m_pExtraItemIcon->Render( iXPos + 103, iYPos + 98 );
		// 성장값 표시
		if( m_pExtraItemNum && m_pExtraItemPlus )
		{
			enum
			{
				TEXT_SIZE	= 15,
				TEXT_GAP	= 5,
				PLUS_SIZE   = 14,
			};
			int iCurPos = 0;
			int iDefaultPos = iXPos + 103;

			int iTotalSize = PLUS_SIZE;      
			if( m_iReinforce < 10 )
				iTotalSize += TEXT_SIZE;
			else if( m_iReinforce < 100 )
				iTotalSize += TEXT_SIZE * 2;
			else
				iTotalSize += TEXT_SIZE * 3;

			iCurPos = iDefaultPos - (iTotalSize / 2);
			m_pExtraItemNum->RenderNum( iCurPos + ( PLUS_SIZE - ( TEXT_GAP - 1 ) ), iYPos + 112, m_iReinforce, -TEXT_GAP );
			m_pExtraItemPlus->Render( iCurPos, iYPos + 112 );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, STR(1), m_szClassName.c_str() ); 
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, STR(2), m_szItemName.c_str(), m_iReinforce ); 
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_13, STR(3) ); 
}

void ioExtraItemReleaseWnd::iwm_show()
{
	SAFEDELETE( m_pExtraItemIcon );
	m_szClassName.Clear();
	m_szItemName.Clear();
	m_szIconName.Clear();
	m_iReinforce = 0;

	int nItemCode = 0;
	g_MyInfo.GetFirstDeleteExtraItem( m_szClassName, m_szItemName, m_szIconName, m_iReinforce, nItemCode );
	m_pExtraItemIcon = g_UIImageSetMgr.CreateImageByFullName( m_szIconName );

	const ioItem *pItem = g_ItemMaker.GetItemConst( nItemCode, __FUNCTION__ );
	if( pItem )
		m_nGradeType = pItem->GetGradeType();
}

void ioExtraItemReleaseWnd::iwm_hide()
{
	m_szClassName.Clear();
	m_szItemName.Clear();
	m_szIconName.Clear();
	m_iReinforce = 0;

	m_dwHideDelayTime = FRAMEGETTIME();
}

bool ioExtraItemReleaseWnd::iwm_esc()
{
	if( !IsShow() ) return false;
	if( ioWnd::iwm_esc() )
	{
		g_MyInfo.EraseDeleteExtraItem( false );
		return true;
	}
	return false;
}

void ioExtraItemReleaseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ALL_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteExtraItem( true );
			HideWnd();
		}
		break;
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteExtraItem( false );
			HideWnd();
		}
		break;
	}
}